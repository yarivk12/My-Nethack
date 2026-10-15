# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-15.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Frozen Step 22 source scope and unchanged historical source/mutation gates.

Run --freeze only after reviewing the final production changes. Normal runs
never update the manifest. This checks source contracts, not native gameplay.
"""
import argparse
import difflib
import hashlib
import json
from pathlib import Path
import re
import runpy
import subprocess
import sys
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
BASELINE = 'e61a290dece24a4dbaaaa2f05747da15dc96e36f'
MANIFEST_PATH = ROOT / 'test/step22_historical_changes.json'
PRODUCTION = ('include/', 'src/', 'sys/', 'win/', 'dat/', 'util/')


def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def matches_frozen(data, expected):
    # Git's checkout EOL conversion must not invalidate reviewed text. Binary
    # resources remain byte-exact; text accepts only LF/CRLF normalization.
    if digest(data) == expected:
        return True
    if b'\0' in data:
        return False
    normalized = data.replace(b'\r\n', b'\n')
    return expected in (digest(normalized), digest(normalized.replace(b'\n', b'\r\n')))


def text(data):
    return data.decode('utf8').replace('\r\n', '\n').replace('\r', '\n')


def baseline_files():
    return git('ls-tree', '-r', '--name-only', BASELINE).decode().splitlines()


def production_files():
    names = git('ls-files', '--cached', '--others', '--exclude-standard').decode().splitlines()
    return sorted({name for name in names if name.startswith(PRODUCTION)})


def hunks(before, after):
    """Unique context makes each reverse operation unambiguous."""
    old, new = before.splitlines(keepends=True), after.splitlines(keepends=True)
    for context in (4, 12, 40, max(len(old), len(new)) + 1):
        result = []
        for group in difflib.SequenceMatcher(None, old, new, autojunk=False).get_grouped_opcodes(context):
            first, last = group[0], group[-1]
            left = ''.join(old[first[1]:last[2]])
            right = ''.join(new[first[3]:last[4]])
            result.append({'before': left, 'after': right})
        if all(h['after'] and after.count(h['after']) == 1 for h in result):
            return result
    raise AssertionError('Cannot construct unique reverse delta (deleted files are unsupported)')


def project(manifest, path, current):
    edits = []
    for hunk in manifest['files'].get(path, {}).get('hunks', []):
        if hunk['after'] in current:
            assert current.count(hunk['after']) == 1, (path, 'ambiguous Step 22 delta')
            start = current.index(hunk['after'])
            edits.append((start, start + len(hunk['after']), hunk['before']))
    boundary = len(current)
    for start, end, before in sorted(edits, reverse=True):
        assert end <= boundary, (path, 'overlapping Step 22 delta')
        current = current[:start] + before + current[end:]
        boundary = start
    return current


def current_contracts():
    read = lambda name: (ROOT / name).read_text(encoding='utf8', errors='replace')
    assert re.search(r'#define EDITLEVEL 18\b', read('include/patchlevel.h'))
    for name in ('include/artilist.h', 'include/artifact.h', 'src/artilist.c'):
        if name in baseline_files():
            assert text((ROOT / name).read_bytes()) == text(git('show', f'{BASELINE}:{name}')), (name, 'production catalogue changed')
    header, engine = read('include/exotic.h'), read('src/exotic.c')
    props = header.split('enum ex_property {', 1)[1].split('}', 1)[0]
    assert len(re.findall(r'\bEX_[A-Z_]+\b', props)) == 19  # 18 properties plus sentinel
    assert props.rstrip().endswith('EX_COUNT')
    setter = engine.index('void\nex_test_grant(')
    guard = engine.rfind('#ifdef STEP15_TEST', 0, setter)
    assert guard >= 0 and '#endif' not in engine[guard:setter]
    assert '#endif' in engine[setter:]
    assert engine.count('ex_test_grant(') == 1
    # No production caller may fabricate a fixture grant.
    for name in production_files():
        if name.endswith(('.c', '.inc')) and name != 'src/exotic.c':
            assert 'ex_test_grant(' not in read(name), name
    for name, tokens in {
        'include/obj.h': ('struct ex_object_state exotic;',),
        'include/you.h': ('ex_teleport_disarmed', 'ex_guardians'),
        'include/monst.h': ('ex_guardian_until', 'ex_guardian_source'),
        'src/allmain.c': ('ex_world_turn();', 'ex_health_changed();'),
        'src/mhitu.c': ('ex_melee_begin(', 'ex_melee_end(', 'ex_native_gulpmu('),
        'src/mcastu.c': ('ex_spell_absorb(', 'ex_area_spell_begin('),
        'src/dog.c': ('ex_guardian_expire(',),
        'src/makemon.c': ('ex_guardian_clone(',),
        'src/mon.c': ('ex_guardian_detach(',),
        'src/teleport.c': ('ex_reactive_teleport(',),
    }.items():
        for token in tokens:
            assert token in read(name), (name, token)


def freeze():
    current_contracts()
    assert not MANIFEST_PATH.exists(), 'Manifest already exists; review and explicitly remove it before refreezing'
    baseline = set(baseline_files())
    names = sorted(set(production_files()) | {n for n in baseline if n.startswith(PRODUCTION)})
    manifest = {'baseline_commit': BASELINE, 'production_sha256': {}, 'historical_guards_sha256': {}, 'files': {}}
    for name in names:
        actual = (ROOT / name).read_bytes()
        prior = git('show', f'{BASELINE}:{name}') if name in baseline else b''
        manifest['production_sha256'][name] = digest(actual)
        normalized = lambda data: data.replace(b'\r\n', b'\n').replace(b'\r', b'\n')
        if normalized(actual) != normalized(prior):
            old, new = text(prior), text(actual)
            manifest['files'][name] = {'baseline_sha256': digest(old.encode()),
                                     'accepted_sha256': digest(new.encode()),
                                     'hunks': hunks(old, new)}
    for name in sorted(baseline):
        if name.startswith('test/') and name.endswith(('.py', '.json')):
            prior = git('show', f'{BASELINE}:{name}')
            assert text((ROOT / name).read_bytes()) == text(prior), (name, 'historical guard modified')
            manifest['historical_guards_sha256'][name] = digest((ROOT / name).read_bytes())
    MANIFEST_PATH.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf8')
    print(f'FROZEN {len(manifest["files"])} reviewed deltas; run the gate separately')


def verify(manifest):
    assert manifest['baseline_commit'] == BASELINE
    assert set(production_files()) == set(manifest['production_sha256']), 'Unreviewed production file addition/removal'
    for name, expected in {**manifest['production_sha256'], **manifest['historical_guards_sha256']}.items():
        assert matches_frozen((ROOT / name).read_bytes(), expected), (name, 'unreviewed source change')
    rejected = 0
    for name, record in manifest['files'].items():
        current = (ROOT / name).read_text(encoding='utf8')
        assert digest(current.encode()) == record['accepted_sha256'], name
        expected = record['baseline_sha256']
        assert digest(project(manifest, name, current).encode()) == expected, (name, 'projection mismatch')
        assert digest(text(git('show', f'{BASELINE}:{name}')).encode()) == expected if name in baseline_files() else expected == digest(b''), name
        for hunk in record['hunks']:
            assert current.count(hunk['after']) == 1
            altered = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
            assert digest(project(manifest, name, altered).encode()) != expected, (name, 'masked mutation')
            rejected += 1
        assert digest(project(manifest, name, current + '\n/* unreviewed */').encode()) != expected
        rejected += 1
    print(f'PASS Step 22 frozen scope: {len(manifest["files"])} deltas, {rejected} rejected mutations', flush=True)


def historical(manifest):
    original_read = Path.read_text
    original_run = subprocess.run

    def historical_read(path, *args, **kwargs):
        value = original_read(path, *args, **kwargs)
        try:
            key = path.resolve().relative_to(ROOT).as_posix()
        except ValueError:
            return value
        return project(manifest, key, value)

    def nested_python(command, **kwargs):
        # Step 21 launches its current source suites as subprocesses. Keep
        # those unchanged suites in this process so they inherit projection.
        if command[0] != sys.executable:
            # Historical mutation guards also query pinned Git blobs.
            assert command[0] == 'git', command
            return original_run(command, **kwargs)
        assert len(command) == 2, command
        target = Path(command[1]).resolve()
        assert target.parent == ROOT / 'test' and target.name.endswith('_source.py'), command
        with patch.object(sys, 'argv', [str(target)]):
            try:
                runpy.run_path(str(target), run_name='__main__')
            except SystemExit as error:
                assert error.code in (None, 0), (target, error.code)
        return subprocess.CompletedProcess(command, 0)

    with patch.object(Path, 'read_text', historical_read), patch.object(subprocess, 'run', nested_python), patch.object(sys, 'argv', [str(ROOT / 'test/run_step21_source.py')]):
        runpy.run_path(str(ROOT / 'test/run_step21_source.py'), run_name='__main__')
    print('PASS unchanged Step 21 and inherited historical source/mutation gates', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--freeze', action='store_true', help='Explicitly freeze reviewed final sources once; does not run historical gates')
    args = parser.parse_args()
    if args.freeze:
        freeze()
        return
    assert MANIFEST_PATH.exists(), 'No reviewed manifest. Run --freeze only after final source review.'
    manifest = json.loads(MANIFEST_PATH.read_text(encoding='utf8'))
    verify(manifest)
    current_contracts()
    historical(manifest)


if __name__ == '__main__':
    main()
