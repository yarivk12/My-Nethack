"""Exact Step 15C scope, mutation rejection, and transaction wiring guard."""
from pathlib import Path
import re
import subprocess
from step15d_source_projection import project as step15d_project
from step15c_source_projection import CHANGES, project

R = Path(__file__).resolve().parents[1]
BASE = '439f459b1afbe42fa7118874d4e22f772229d7e9'
mutations = 0
assert set(CHANGES) == {'src/apply.c'}
for path, hunks in CHANGES.items():
    original = subprocess.check_output(['git', 'show', BASE + ':' + path], cwd=R).decode().replace('\r\n', '\n')
    current = step15d_project(path, (R / path).read_text(encoding='utf8'))
    assert project(path, current) == original, (path, 'unreviewed change')
    assert project(path, original) == original, (path, 'changed baseline')
    for hunk in hunks:
        assert current.count(hunk['after']) == 1
        altered = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert project(path, altered) != original, (path, 'masked mutation')
        mutations += 1
    assert project(path, current + '\n/* unreviewed */\n') != original
    mutations += 1

c = (R / 'src/apply.c').read_text()
inherit = c.split('struct forge_state {', 1)[1].split('forge_output(int typ)', 1)[0]
assert not re.search(r'\b(rn2|rnd|rn1|enhancement_generate|splitobj|mksobj)\(', inherit)
assert 'struct obj *' not in inherit.split('};', 1)[0], 'no donor pointers'
commit = c.split('forge_commit(const ', 1)[1].split('forge_category(int typ)', 1)[0]
assert commit.index('forge_gather(') < commit.index('forge_output(') < commit.index('forge_inherit(')
assert commit.index('forge_inherit(') < commit.index('mergable(') < commit.index('useupall(') < commit.index('addinv(output)')
assert 'forge_gather(' not in commit.split('useupall(', 1)[1]
print(f'PASS exact Step 15C production scope; {mutations} rejected mutations; value capture/finalization/preflight ordering and RNG boundary')
