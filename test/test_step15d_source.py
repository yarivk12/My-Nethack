"""Frozen reviewed delta, mutation rejection and integration boundaries."""
from pathlib import Path
from step16a_source_projection import project as step16a_project
import subprocess
from step15d_source_projection import CHANGES, project

R = Path(__file__).resolve().parents[1]
BASE = 'a4585f85f1acc56f489cf9dbc3eecc3c6a2679d5'
mutations = 0
for path, hunks in CHANGES.items():
    old = subprocess.check_output(['git','show',BASE+':'+path],cwd=R).decode().replace('\r\n','\n')
    current = step16a_project(path, (R/path).read_text())
    assert project(path,current) == old, (path,'unreviewed change')
    assert project(path,old) == old, (path,'baseline changed')
    for hunk in hunks:
        assert current.count(hunk['after']) == 1
        altered = current.replace(hunk['after'],'@'+hunk['after'][1:],1)
        assert project(path,altered) != old, (path,'masked mutation')
        mutations += 1
    assert project(path,current+'\n/* unreviewed */\n') != old
    mutations += 1
c=(R/'src/apply.c').read_text()
preflight=c.split('affix_target(',1)[1].split('affix_commit(',1)[0]
assert not any(x in preflight for x in ('rn2(', 'rnd(', 'splitobj(', 'useup('))
commit=c.split('affix_commit(',1)[1].split('affix_menu(',1)[0]
assert commit.index('affix_room(') < commit.index('/* COMMIT:') < commit.index('rn2(100)')
assert commit.index('rn2(100)') < commit.index('pool[rn2(n)]') < commit.index('d(e->dice')
assert 'return ECMD_TIME;' in commit and commit.count('useup(gem)')==1
assert 'doinspect, IFBURIED | AUTOCOMPLETE | GENERALCMD' in (R/'src/cmd.c').read_text()
assert '#define EDITLEVEL 12' in (R/'include/patchlevel.h').read_text()
print(f'PASS Step 15D {len(CHANGES)} production files, {mutations} rejected mutations, preflight/RNG and command/version boundaries')
