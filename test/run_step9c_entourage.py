"""Pinned donor leader entourage creation order, RNG and failure handling."""
from pathlib import Path
import re
import subprocess
import sys

repo=Path(__file__).resolve().parents[1]
donor,out=map(Path,sys.argv[1:3]);out.mkdir(parents=True,exist_ok=True)
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/makemon.c'],cwd=donor).decode('utf8')
source=(repo/'src/makemon.c').read_text(encoding='utf8')
local=re.search(r'(?m)^staticfn void\nmith_entourage\([\s\S]*?^\}',source)[0]

def block(marker):
    text=original.split(marker,1)[1]; start=text.index('{'); nesting=0
    for i in range(start,len(text)):
        nesting += (text[i]=='{')-(text[i]=='}')
        if nesting==0:
            return text[start:i+1]
    raise AssertionError(marker)

body='static void donor_entourage(struct monst *mtmp,boolean anymon,mmflags_nht flags) {'
body+='int num,mndx=monsndx(mtmp->data);struct monst *tmpm;'
body+='if(!anymon || (flags & (MM_EDOG|MM_NOGRP))) return;'
for i,(typ,marker) in enumerate([
    ('PM_DEEPEST_ONE','if (anymon && mndx == PM_DEEPEST_ONE)'),
    ('PM_DEEPER_ONE','if (mndx == PM_DEEPER_ONE)'),
    ('PM_ALABASTER_ELF_ELDER','else if (mndx == PM_ALABASTER_ELF_ELDER)')]):
    selected=block(marker)
    selected=re.sub(r'(m_init[sl]grp\(tmpm, mtmp->mx, mtmp->my)\)',r'\1, flags)',selected)
    body+=('else ' if i else '')+'if(mndx=='+typ+')'+selected
body+='}'
(out/'step9c_entourage.h').write_text(local+'\n'+body,encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS','/DWIN32','/DWIN32CON',
                '/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),'/I'+str(out),
                '/Fe:'+str(out/'step9c_entourage.exe'),str(repo/'test/test_step9c_entourage.c'),
                str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_entourage.exe')],cwd=out,check=True)
