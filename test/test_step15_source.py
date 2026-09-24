"""Focused forge integration wiring guard; runtime tests prove behavior."""
from pathlib import Path
from phase1_audit_projection import project as audit_project
R=Path(__file__).resolve().parents[1]
def read(p): return (R/p).read_text()
assert 'FORGE' in read('include/rm.h'), 'forge terrain missing'
assert 'S_forge' in read('include/defsym.h'), 'forge display missing'
assert 'forge_interact(obj)' in read('src/apply.c'), 'apply dispatch missing'
assert 'forge_generate();' in read('src/mklev.c'), 'generation integration missing'
print('PASS Step 15 integration wiring')

import subprocess,re
BASE='b9d04d0d6e159713f01a30a9863cb4b2cd900cf5'
for path in ['include/enhance.h','include/obj.h','src/enhance.c','src/makemon.c',
             'src/mkobj.c','src/mkroom.c','src/shknam.c','src/shk.c','src/mhitm.c',
             'src/mhitu.c','src/mthrowu.c','src/uhitm.c','src/worn.c','src/objnam.c',
             'src/save.c','src/restore.c','src/bones.c','util/recover.c']:
    old=subprocess.check_output(['git','show',BASE+':'+path],cwd=R).decode().replace('\r\n','\n')
    assert audit_project(path, read(path))==old,path
symbols=[int(n) for n in re.findall(r'PCHAR2?\(\s*(\d+)',read('include/defsym.h'))]
assert symbols==list(range(len(symbols))), 'contiguous symbol ids'
assert re.search(r'#define EDITLEVEL\s+10\b',read('include/patchlevel.h'))
print('PASS protected enhancement/codec source identity outside exact audited corrections, contiguous symbols and epoch 10')
