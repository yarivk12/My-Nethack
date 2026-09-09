"""Validate imported difficulty with NetHack 5.0's actual mstrength function.

Run in a VS developer shell. Optional --report prints values without asserting
the table, for recalibration after changing an imported attack definition.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/mondata.c').read_text(encoding='utf8')
functions = [re.search(r'(?m)^(?:staticfn )?\w+\n' + n + r'\([\s\S]*?^\}', source)[0]
             for n in ['mstrength_ranged_attk', 'mstrength']]
(out / 'step9c_difficulty.h').write_text('\n'.join(functions), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_difficulty.exe'),
                str(repo / 'test/test_step9c_difficulty.c'),
                str(repo / 'src/monst.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_difficulty.exe')] + sys.argv[2:], cwd=out, check=True)
