"""Compare service pricing to pinned donor and test actual payment/init helpers.

VS shell: runner PINNED_DNET_HACK_CLONE OUTPUT_DIRECTORY
Menu/effect integration and save/reload are separate wizard checks.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
parts = []
for file, names in {'shk.c': ['mith_service_price', 'mith_service_pay'],
                    'shknam.c': ['mith_init_services']}.items():
    source = (repo / 'src' / file).read_text(encoding='utf8')
    for name in names:
        parts.append(re.search(r'(?m)^staticfn \w+\n' + name
                               + r'\([\s\S]*?^\}', source)[0])
upstream = subprocess.check_output(
    ['git', '-C', str(donor), 'show', pin + ':dnethack-3.4.3/src/shk.c'],
    text=True, encoding='utf8')
body = re.search(r'(?m)^shk_smooth_charge\([\s\S]*?^\}', upstream)[0]
body = body[body.index('{'):]
parts.append('#define NOBOUND (-1)\nstatic void donor_price(int *pcharge, int lower, int upper)\n' + body)
(out / 'step9c_services.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_services.exe'),
                str(repo / 'test/test_step9c_services.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_services.exe')], cwd=out, check=True)
