"""Compile actual deep-one death/growth code and compare native growth baseline.

Run inside a VS developer shell. All generated files stay in the given output.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)

def function(source, name):
    return re.search(r'(?m)^(?:staticfn )?[\w *]+\n' + name + r'\([\s\S]*?^\}', source)[0]

mondata = (repo / 'src/mondata.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^static const short grownups\[\]\[2\] = \{[\s\S]*?^\};', mondata)[0],
         function(mondata, 'little_to_big')]
source = (repo / 'src/makemon.c').read_text(encoding='utf8')
parts.append(function(source, 'grow_up'))
baseline = subprocess.check_output(['git', 'show',
    '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/makemon.c'], cwd=repo).decode('utf8')
parts.append(function(baseline, 'grow_up').replace('grow_up(', 'baseline_grow_up('))
source = (repo / 'src/mon.c').read_text(encoding='utf8')
parts.append(function(source, 'mith_deep_soul'))
tail = function(source,'corpse_chance').split('    if (((bigmonst',1)[1]
parts.append('static boolean corpse_tail(struct monst *mon) { struct permonst *mdat=mon->data; int tmp; if (((bigmonst'+tail)
oldmon = subprocess.check_output(['git','show',
    '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/mon.c'],cwd=repo).decode('utf8')
oldtail = function(oldmon,'corpse_chance').split('    if (((bigmonst',1)[1]
assert tail.replace('\n        || mdat == &mons[PM_ALABASTER_MUMMY]','')==oldtail
parts.append('static boolean baseline_corpse_tail(struct monst *mon) { struct permonst *mdat=mon->data; int tmp; if (((bigmonst'+oldtail)
(out / 'step9c_souls.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_souls.exe'),
                str(repo / 'test/test_step9c_souls.c'), str(repo / 'src/monst.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_souls.exe')], cwd=out, check=True)
