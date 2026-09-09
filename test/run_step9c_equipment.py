"""Compile the actual branch equipment selector; enumerate its random choices."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/makemon.c').read_text(encoding='utf8')
body = re.search(r'(?m)^staticfn boolean\nmith_deep_equipment\([\s\S]*?^\}', source)[0]
(out / 'step9c_equipment.h').write_text(body, encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_equipment.exe'),
                str(repo / 'test/test_step9c_equipment.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_equipment.exe')], cwd=out, check=True)
