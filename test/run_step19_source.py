"""Current Step 19 contracts and unchanged historical mutation guards."""
import hashlib
from pathlib import Path
import runpy
from unittest.mock import patch
from step19_source_projection import MANIFEST, project

ROOT = Path(__file__).resolve().parents[1]
digest = lambda text: hashlib.sha256(text.encode()).hexdigest()
rejections = 0
for path, record in MANIFEST.items():
    current = (ROOT / path).read_text()
    assert digest(project(path, current)) == record['baseline_sha256'], path
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1, (path, 'missing accepted delta')
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project(path, changed)) != record['baseline_sha256'], path
        rejections += 1
    assert digest(project(path, current + '\n/* unreviewed */')) != record['baseline_sha256']
    rejections += 1
assert '#define EDITLEVEL 15' in (ROOT / 'include/patchlevel.h').read_text()
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
