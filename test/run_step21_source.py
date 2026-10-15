# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-10.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Current Step 21 contracts and unchanged historical source/mutation gates.

The frozen manifest records reviewed Step 21 changes against the completed
Step 20 baseline. Exact hashes and mutation probes prevent the projection
from hiding later edits. Native gameplay tests remain a separate requirement.
"""
import hashlib
import json
from pathlib import Path
import runpy
import subprocess
import sys
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = json.loads((ROOT / 'test/step21_historical_changes.json').read_text())

def digest(text):
    return hashlib.sha256(text.encode()).hexdigest()

def project(path, text):
    replacements = []
    for hunk in MANIFEST['files'].get(path, {}).get('hunks', []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 21 hunk')
            start = text.index(hunk['after'])
            replacements.append((start, start + len(hunk['after']), hunk['before']))
    # Match against the original text: renumbered tile hunks can otherwise
    # accidentally match text created by reversing an earlier hunk.
    boundary = len(text)
    for start, end, before in sorted(replacements, reverse=True):
        assert end <= boundary, (path, 'overlapping Step 21 hunks')
        text = text[:start] + before + text[end:]
        boundary = start
    return text

rejections = 0
for path, record in MANIFEST['files'].items():
    current = (ROOT / path).read_text(encoding='utf8')
    assert digest(current) == record['accepted_sha256'], (path, 'unreviewed Step 21 source')
    assert digest(project(path, current)) == record['baseline_sha256'], (path, 'unreviewed Step 21 delta')
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1, (path, 'missing accepted Step 21 delta')
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project(path, changed)) != record['baseline_sha256'], (path, 'masked Step 21 mutation')
        rejections += 1
    assert digest(project(path, current + '\n/* unreviewed */')) != record['baseline_sha256']
    rejections += 1
print(f'PASS Step 21 exact production deltas: {len(MANIFEST["files"])} files, {rejections} rejected mutations', flush=True)

for suite in ('test_step19_source.py', 'test_step20_source.py', 'test_step21_source.py'):
    subprocess.run([sys.executable, str(ROOT / 'test' / suite)], cwd=ROOT, check=True)

original_read = Path.read_text
def historical_read(path, *args, **kwargs):
    text = original_read(path, *args, **kwargs)
    try:
        key = path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return text
    return project(key, text)

# Preserve earlier suites, including their epoch/catalogue assertions.
# Only exact reviewed Step 21 hunks are reversed before they read source.
with patch.object(Path, 'read_text', historical_read):
    runpy.run_path(str(ROOT / 'test/run_step19_source.py'), run_name='__main__')
