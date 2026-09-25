"""Step 16A exact delta and integration guards complement native fixtures."""
from pathlib import Path
import subprocess
from step16b_source_projection import project as step16b_project
from step16a_source_projection import CHANGES, project
R=Path(__file__).resolve().parents[1]
BASE="62ff68b0abccb85f489795af03e3164f57ccb8fb"
mutations=0
for path,hunks in CHANGES.items():
    before=subprocess.check_output(['git','show',BASE+':'+path],cwd=R).decode().replace('\r\n','\n')
    current=step16b_project(path,(R/path).read_text())
    assert project(path,current)==before,(path,'unreviewed delta')
    for hunk in hunks:
        altered=current.replace(hunk['after'],'@'+hunk['after'][1:],1)
        assert project(path,altered)!=before,(path,'masked mutation')
        mutations+=1
    assert project(path,current+'\n/* mutation */\n')!=before
h=(R/'include/enhance.h').read_text()
for absent in ('ACID_IV','ANARCHIC_III','ANARCHIC_IV','AXIOMATIC_III','AXIOMATIC_IV','CONCORDANT','SLIMING','SLAYER'):
    assert 'OEP_'+absent not in h
c=(R/'src/enhance.c').read_text()
assert 'mon_aligntyp(' in c and 'alignment == A_NONE' in c
assert 'nonliving(target->data)' in c
assert 'minstapetrify(target,' in c and 'do_stone_u(attacker)' in c
assert 'gm.migrating_objs' not in c and 'gm.migrating_mons' not in c
assert (R/'src/allmain.c').read_text().count('enhancement_tick();')==1
print(f'PASS Step 16A {len(CHANGES)} production files, {mutations} rejected mutations, canonical alignment/living/petrification and active-level tick')
