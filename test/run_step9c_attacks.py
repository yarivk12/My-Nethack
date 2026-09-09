"""Compile actual Mithardir attack handlers with deterministic combat fixtures."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
parts = []
for file, names in {
    'src/mondata.c': ['mith_anhydrous', 'mith_watery', 'resists_drli'],
    'src/uhitm.c': ['mith_attack_kill', 'mith_drain_attack', 'mith_desiccate',
                   'mith_disintegrate'],
    'src/mon.c': ['mith_cold_heal'],
}.items():
    source = (repo / file).read_text(encoding='utf8')
    for name in names:
        parts.append(re.search(r'(?m)^(?:staticfn )?\w+\n' + name
                               + r'\([\s\S]*?^\}', source)[0])
(out / 'step9c_attacks.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_attacks.exe'),
                str(repo / 'test/test_step9c_attacks.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_attacks.exe')], cwd=out, check=True)
