"""VS developer shell: compile actual Sheol functions and monster/object tables."""
from pathlib import Path
import re
import subprocess
import sys

repo=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]).resolve()
subprocess.run([sys.executable,str(repo/'test/run_step7.py'),str(out)],check=True)
parts=[]
for path,names in {
    'src/uhitm.c':['sheol_freeze'],
    'src/makemon.c':['golemhp','mbirth_limit','sheol_mon_boosted'],
    'src/monmove.c':['sheol_share_hp','sheol_chillbug_turn'],
    'src/trap.c':['trapeffect_ice_trap'],
}.items():
    source=(repo/path).read_text()
    for name in names:
        matches=re.findall(r'(?m)^(?:staticfn )?\w+\n'+name+r'\([\s\S]*?^\}',source)
        assert len(matches)==1,(path,name)
        parts.append(matches[0])
(out/'step9a_functions.h').write_text('\n\n'.join(parts))
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9a_runtime.exe'),
 str(repo/'test/test_step9a_runtime.c'),str(repo/'src/monst.c'),
 str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9a_runtime.exe')],cwd=out,check=True)
