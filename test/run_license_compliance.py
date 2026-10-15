# My-NetHack modification/provenance validation, created 2026-10-15.
# NetHack license: see LICENSE and dat/license. Existing authors retain credits.
"""Validate exact notice-only insertions, then unchanged historical gates.

Native tests and builds always use the real annotated source. Only historical
source readers receive the exact validated Step 22 bytes after all current
files and notice syntax have been checked against the immutable Git commit.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import runpy
import subprocess
import sys
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
BASE = 'cbef52d28b24e177c875e816397b6ed3e4cb834a'
RECORD = ROOT / 'doc/my-nethack-modifications.json'
NEW = {'LICENSE', 'doc/license-compliance.md', 'doc/license-donor-audit.md',
       'doc/my-nethack-modifications.json', 'test/run_license_compliance.py'}


def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT)


def lf(data):
    return data.replace(b'\r\n', b'\n')


def digest(data):
    return hashlib.sha256(data).hexdigest()


def baseline_blobs():
    entries, links = [], {}
    for line in git('ls-tree','-r',BASE).decode().splitlines():
        metadata, name = line.split('\t',1)
        mode, kind, oid = metadata.split()
        if mode == '160000':
            links[name] = oid
        else:
            assert kind == 'blob'
            entries.append((name,oid))
    result = subprocess.run(['git','cat-file','--batch'], cwd=ROOT,
                            input=''.join(oid+'\n' for name,oid in entries).encode(),
                            stdout=subprocess.PIPE, check=True).stdout
    blobs, cursor = {}, 0
    for name, oid in entries:
        end = result.index(b'\n',cursor)
        actual_oid, kind, length = result[cursor:end].decode().split()
        assert actual_oid == oid and kind == 'blob'
        cursor = end+1
        length = int(length)
        blobs[name] = result[cursor:cursor+length]
        cursor += length+1
    assert cursor == len(result)
    return blobs, links


def raw_offset(raw, position):
    """Map an LF-normalized byte offset without changing mixed checkout EOLs."""
    raw_cursor = logical = 0
    for line in raw.splitlines(keepends=True):
        size = len(lf(line))
        if position < logical+size:
            return raw_cursor+position-logical
        if position == logical+size:
            return raw_cursor+len(line)
        raw_cursor += len(line)
        logical += size
    assert position == logical
    return raw_cursor


def metadata_removed(value):
    if isinstance(value, dict):
        return {k:metadata_removed(v) for k,v in value.items()
                if k != '_modification_notice'}
    if isinstance(value, list):
        return list(map(metadata_removed, value))
    return value


def notice_syntax(style, value):
    if style == 'c-block':
        assert re.fullmatch(rb'/\*(?:(?!\*/).)*\*/\n', value, re.S)
    elif style in ('hash-line', 'lua-line'):
        marker = b'#' if style == 'hash-line' else b'--'
        assert all(not line or line.startswith(marker) for line in value.splitlines())
    elif style == 'xml-comment-after-declaration':
        assert re.fullmatch(rb'<!--(?:(?!--).)*-->\n', value, re.S)
    elif style == 'json-object-metadata-review-consumers':
        assert re.fullmatch(rb'\n +"_modification_notice": "(?:[^"\\]|\\.)*",', value)
    else:
        assert style in ('markdown-prose', 'plain-prose')


def verify():
    record = json.loads(RECORD.read_text(encoding='utf8'))
    assert record['validated_step22_commit'] == BASE
    blobs, tree_links = baseline_blobs()
    baseline = sorted(set(blobs) | set(tree_links))
    links = record['unchanged_dependency_gitlinks']
    assert links == tree_links
    assert len(baseline) == record['tracked_files_audited'] == 1696
    assert len(record['files']) == record['project_files_corrected'] == 561
    assert set(record['files']) | set(record['unchanged_files_sha256_lf']) | set(links) == set(baseline)
    canonical = {}
    probes = 0
    for name in baseline:
        if name in links:
            assert git('rev-parse', BASE+':'+name).decode().strip() == links[name]
            assert git('ls-files','-s','--',name).decode().split()[1] == links[name]
            continue
        before = lf(blobs[name])
        raw_actual = (ROOT/name).read_bytes()
        actual = lf(raw_actual)
        if name not in record['files']:
            assert digest(before) == record['unchanged_files_sha256_lf'][name]
            assert actual == before, (name, 'unreviewed change outside notice audit')
            continue
        row = record['files'][name]
        assert digest(before) == row['validated_sha256_lf'], name
        expected = before
        offsets = [v['original_offset_lf'] for v in row['insertions']]
        assert offsets == sorted(set(offsets)), name
        for insertion in reversed(row['insertions']):
            offset, value = insertion['original_offset_lf'], insertion['text_lf'].encode('utf8')
            assert 0 <= offset <= len(before), name
            notice_syntax(row['format'], value)
            assert offset == 0 or before[offset-1:offset] in (b'\n', b'{'), name
            expected = expected[:offset]+value+expected[offset:]
        assert b'My-NetHack modification notice, added 2026-10-15.' in expected, name
        assert actual == expected and digest(actual) == row['compliance_sha256_lf'], (name, 'not an approved notice-only insertion')
        if name.endswith('.json'):
            assert metadata_removed(json.loads(actual)) == json.loads(before), name
        for mutated in (actual+b'\nUNREVIEWED', b'!'+actual[1:]):
            assert mutated != expected and digest(mutated) != row['compliance_sha256_lf']
            probes += 1
        positions, total = [], 0
        for insertion in row['insertions']:
            start = insertion['original_offset_lf']+total
            length = len(insertion['text_lf'].encode('utf8'))
            positions.append((start,start+length))
            total += length
        raw_before = raw_actual
        for start,end in reversed(positions):
            a,b = raw_offset(raw_before,start),raw_offset(raw_before,end)
            raw_before = raw_before[:a]+raw_before[b:]
        assert lf(raw_before) == before, name
        canonical[name] = raw_before
    current_names = set(git('ls-files','--cached','--others','--exclude-standard').decode().splitlines())
    assert current_names - set(baseline) - NEW == {'.codegraph/.gitignore'}
    assert all((ROOT/name).is_file() for name in NEW)
    assert lf((ROOT/'LICENSE').read_bytes()) == lf(git('show',BASE+':dat/license'))
    assert (ROOT/'dat/license').read_bytes() == git('show',BASE+':dat/license') or lf((ROOT/'dat/license').read_bytes()) == lf(git('show',BASE+':dat/license'))
    print(f'PASS license audit: {len(baseline)-len(links)} regular files and {len(links)} unchanged dependency pins; 561 notice-only corrections, {probes} mutation rejections', flush=True)
    return canonical


def historical(canonical, target, arguments):
    original_text, original_bytes = Path.read_text, Path.read_bytes
    def key(path):
        try:
            return path.resolve().relative_to(ROOT).as_posix()
        except ValueError:
            return None
    def read_bytes(path):
        name = key(path)
        return canonical[name] if name in canonical else original_bytes(path)
    def read_text(path, *args, **kwargs):
        name = key(path)
        if name not in canonical:
            return original_text(path, *args, **kwargs)
        encoding = kwargs.get('encoding') or (args[0] if args else None) or 'utf8'
        errors = kwargs.get('errors') or (args[1] if len(args)>1 else None) or 'strict'
        return lf(canonical[name]).decode(encoding, errors)
    with patch.object(Path,'read_text',read_text), patch.object(Path,'read_bytes',read_bytes), patch.object(sys,'argv',[str(target)]+arguments):
        runpy.run_path(str(target), run_name='__main__')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--verify-only', action='store_true')
    parser.add_argument('--historical-script', help='Run an additional existing source checker with exact notice projection')
    parser.add_argument('--historical-arg', action='append', default=[], help='Argument forwarded to the historical script')
    args = parser.parse_args()
    canonical = verify()
    if not args.verify_only:
        target = ROOT/'test'/(args.historical_script or 'run_step22_source.py')
        assert target.parent == ROOT/'test' and target.is_file()
        historical(canonical, target, args.historical_arg)
        print('PASS notice reconciliation and unchanged historical source/mutation expectations', flush=True)


if __name__ == '__main__':
    main()
