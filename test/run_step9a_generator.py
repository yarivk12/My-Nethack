"""VS shell: run_step9a_generator.py PINNED_UNNETHACK_CLONE NEW_OUTPUT_DIR.

Read the immutable revision with git show; compile and compare donor and actual
production generator output over 192 seeded upper/middle/lower-floor cases.
"""
from pathlib import Path
import re
import subprocess
import sys

repo=Path(__file__).resolve().parents[1]
donor,out=map(Path,sys.argv[1:3])
pin='439b8d63d3d1ca78fb08588dd43f61874114b21a'
local=(repo/'src/mkmap.c').read_text().split('/* Step 9A: UnNetHack ',1)[1]
local=local[local.index('*/')+2:]
upstream=subprocess.check_output(['git','-C',str(donor),'show',pin+':src/mksheol.c'],text=True)
upstream=upstream[upstream.index('/* Minimum distance'):]
upstream=re.sub(r'mksheol\(void \*init_lev_par\)\n\{[\s\S]*?    int i1, i2;',
                'mksheol(void)\n{\n    int i1, i2;',upstream,count=1)
upstream=upstream.replace('level.flags.','svl.level.flags.').replace('IS_ROCK(', 'IS_OBSTRUCTED(')
outputs=[]
for label,code in [('donor',upstream),('local',local)]:
    dest=out/label
    dest.mkdir(parents=True)
    (dest/'step9a_generator.h').write_text(code)
    exe=dest/'generator.exe'
    subprocess.run(['cl','/nologo','/std:c11','/W4','/DWIN32','/DWIN32CON',
                    '/D_CRT_SECURE_NO_WARNINGS','/I'+str(repo/'include'),
                    '/I'+str(repo/'submodules/lua'),'/I'+str(dest),
                    '/Fe:'+str(exe),str(repo/'test/test_step9a_generator.c')],
                   cwd=dest,check=True)
    result=subprocess.check_output([str(exe)],text=True,timeout=240)
    (dest/'hashes.txt').write_text(result)
    outputs.append(result)
assert outputs[0]==outputs[1], 'Donor/production generator mismatch'
assert len(outputs[0].splitlines())==192
print('PASS: 192 donor/production Sheol generation comparisons, stair regions and terrain')
