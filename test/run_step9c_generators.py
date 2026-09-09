"""Compare production river paths and monster pool against immutable donor C.

VS shell: runner PINNED_DNET_HACK_CLONE NEW_OUTPUT_DIRECTORY
The adapters replace terrain side effects/class choice, preserving RNG order.
Room packing and full floor accessibility are checked by wizard traversal.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'

def upstream(file):
    return subprocess.check_output(['git', '-C', str(donor), 'show',
                                    pin + ':dnethack-3.4.3/src/' + file],
                                   text=True, encoding='utf8')

def function(source, name):
    return re.search(r'(?m)^(?:(?:staticfn|STATIC_OVL) )?(?:struct permonst \*|\w+)\n'
                     + name + r'\([\s\S]*?^\}', source)[0]

parts = []
source = (repo / 'src/mklev.c').read_text(encoding='utf8')
parts.append(function(source, 'mith_river'))
source = (repo / 'src/makemon.c').read_text(encoding='utf8')
parts += [function(source, n) for n in ['mith_eladrin_fallback', 'mith_rndmonst']]
river = function(upstream('mkroom.c'), 'mkriver')
river = river.replace('STATIC_OVL', 'staticfn').replace('mkriver()', 'donor_river(void)')
river = river.replace('level.flags.has_river = 1;', '')
river = river.replace('liquify(', 'mith_liquify(')
parts.append(river)
pool = function(upstream('questpgr.c'), 'chaos2_montype')
pool = pool.replace('chaos2_montype()', 'donor_pool(void)')
pool = pool.replace('mvitals[', 'svm.mvitals[')
pool = pool.replace('on_level(&elshava_level,&u.uz)', '(u.uz.dlevel == 1)')
pool = pool.replace('In_mithardir_terminus(&u.uz)', '(u.uz.dlevel == 10)')
pool = pool.replace('S_CHA_ANGEL', 'S_ANGEL')
parts.append(pool)
(out / 'step9c_generators.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_generators.exe'),
                str(repo / 'test/test_step9c_generators.c'),
                str(repo / 'src/monst.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_generators.exe')], cwd=out, check=True)
