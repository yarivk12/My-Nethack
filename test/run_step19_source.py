"""Current Step 19 contracts and unchanged historical mutation guards."""
import hashlib
from pathlib import Path
import runpy
from unittest.mock import patch
from step19_source_projection import MANIFEST, project
from step20_source_projection import MANIFEST as STEP20, project as project_step20, project_validation

ROOT = Path(__file__).resolve().parents[1]
digest = lambda text: hashlib.sha256(text.encode()).hexdigest()
rejections = 0
for path, record in STEP20.get('validation_fixes', {}).get('files', {}).items():
    current = (ROOT / path).read_text()
    assert digest(project_validation(path, current)) == record['baseline_sha256'], (path, 'unreviewed validation fix')
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project_validation(path, changed)) != record['baseline_sha256']
        rejections += 1
    assert digest(project_validation(path, current+'\n/* unreviewed */')) != record['baseline_sha256']
    rejections += 1
print(f'PASS exact Step 20 validation fixes and {rejections} rejected mutations')
rejections = 0
for path, record in STEP20['files'].items():
    current = project_validation(path, (ROOT / path).read_text())
    assert digest(project_step20(path, current)) == record['baseline_sha256'], (path, 'unreviewed Step 20 delta')
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1, (path, 'missing accepted Step 20 delta')
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project_step20(path, changed)) != record['baseline_sha256'], (path, 'masked Step 20 mutation')
        rejections += 1
    assert digest(project_step20(path, current + '\n/* unreviewed */')) != record['baseline_sha256']
    rejections += 1
print(f'PASS Step 20 exact production deltas and {rejections} rejected mutations')
rejections = 0
for path, record in MANIFEST.items():
    current = project_step20(path, (ROOT / path).read_text())
    assert digest(project(path, current)) == record['baseline_sha256'], path
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1, (path, 'missing accepted delta')
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project(path, changed)) != record['baseline_sha256'], path
        rejections += 1
    assert digest(project(path, current + '\n/* unreviewed */')) != record['baseline_sha256']
    rejections += 1
assert '#define EDITLEVEL 15' in project_step20('include/patchlevel.h', (ROOT / 'include/patchlevel.h').read_text())
print(f'PASS Step 19 exact production deltas and {rejections} rejected mutations')

original_read = Path.read_text
def historical_read(path, *args, **kwargs):
    text = original_read(path, *args, **kwargs)
    try:
        key = path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return text
    return project(key, text)

# The earlier suites still assert their own catalogue and persistence versions.
# Only exact accepted deltas are reversed; mutations remain visible to them.
with patch.object(Path, 'read_text', historical_read):
    runpy.run_path(str(ROOT / 'test/run_step17_source.py'), run_name='__main__')
