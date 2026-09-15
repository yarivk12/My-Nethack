"""Compare reused cellular map generation and Wastes cleanup to pinned donor.

VS shell: runner PINNED_DNET_HACK_CLONE OUTPUT_DIRECTORY
Relocation and final wallification are callbacks; the actual native cellular
passes and cleanup function execute on real terrain arrays.
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

source = (repo / 'src/mkmap.c').read_text(encoding='utf8')
names = ['init_map', 'init_fill', 'get_map', 'pass_one', 'pass_two']
parts = ['#define HEIGHT (ROWNO - 1)\n#define WIDTH (COLNO - 2)',
         re.search(r'(?m)^staticfn const int dirs\[16\] = \{[\s\S]*?^\};', source)[0],
         '#define new_loc(i,j) *(gn.new_locations + (j)*(WIDTH+1)+(i))']
for name in names:
    parts.append(re.search(r'(?m)^staticfn \w+\n' + name + r'\([\s\S]*?^\}', source)[0])
parts.append('#undef new_loc\n#define new_loc(i,j) donor_locations[(j)*(WIDTH+1)+(i)]')
source = upstream('mkmap.c')
for name in names:
    body = re.search(r'(?m)^' + name + r'\([\s\S]*?^\}', source)[0]
    body = body[body.index('{'):]
    body = body.replace('levl[', 'donor_map[')
    for n in names:
        body = re.sub(r'\b' + n + r'\(', 'donor_' + n + '(', body)
    signature = ('static schar donor_get_map(int col, int row, schar bg_typ)'
                 if name == 'get_map' else 'static void donor_' + name
                 + ('(schar bg_typ)' if name == 'init_map' else '(schar bg_typ, schar fg_typ)'))
    parts.append(signature + '\n' + body)
source = (repo / 'src/mkmaze.c').read_text(encoding='utf8')
parts.append(re.search(r'(?m)^staticfn void\nmith_finish_wastes\([\s\S]*?^\}', source)[0])
source = upstream('mkmaze.c')
block = source[source.index('\t/* CHAOS QUEST 2:'):source.index('\t/* CHAOS QUEST 3:')]
block = block[block.index('\tif (In_mithardir_quest'):]
block = block.replace('In_mithardir_quest', 'In_mithardir')
block = block.replace('on_level(&u.uz, &elshava_level)', '(u.uz.dlevel == 1)')
block = block.replace('rloc(m_at(x, y), FALSE)', 'rloc(m_at(x, y), RLOC_NOMSG)')
parts.append('static void donor_cleanup(void) { int x,y;\n' + block + '\n}')
(out / 'step9c_wastes.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_wastes.exe'),
                str(repo / 'test/test_step9c_wastes.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_wastes.exe')], cwd=out, check=True)
