"""Exact audit scope and mutation checks for historical source projections."""
from pathlib import Path
import subprocess
from phase1_audit_projection import CHANGES, project

R = Path(__file__).resolve().parents[1]
BASE = '17d070c1290cafe64c7f40d52a2888521477ecd0'
mutations = 0
for path, hunks in CHANGES.items():
    original = subprocess.check_output(['git', 'show', BASE + ':' + path], cwd=R).decode().replace('\r\n', '\n')
    current = (R / path).read_text(encoding='utf8')
    assert project(path, current) == original, (path, 'unreviewed change')
    assert project(path, original) == original, (path, 'changed baseline')
    for hunk in hunks:
        assert current.count(hunk['after']) == 1, (path, 'missing frozen hunk')
        # A change inside an accepted hunk must remain visible to the caller's
        # equality gate; so must an unrelated addition outside every hunk.
        altered = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert project(path, altered) != original, (path, 'masked mutation')
        mutations += 1
    assert project(path, current + '\n/* unreviewed */\n') != original
    mutations += 1
print(f'PASS exact audit scope for {len(CHANGES)} files; {mutations} rejected source mutations')
