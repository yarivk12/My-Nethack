"""Compile actual Mithardir terrain/type definitions and reward selectors.

Run in a VS developer shell; output belongs outside the checkout.
This is a foundation check, not the Step9C completion gate.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
parts = []
for file, name in [('src/botl.c', 'terrain_descr'),
                   ('src/cmd.c', 'levltyp'),
                   ('src/display.c', 'type_names')]:
    source = (repo / file).read_text(encoding='utf8')
    body = re.search(r'(?m)^[^\n]*\b' + name + r'\[[^\]]*\] = \{[\s\S]*?^\};', source)[0]
    parts.append(body.replace('c_Wall', '"Wall"').replace('static ', ''))
for file, names in {
    'src/mkobj.c': ['mith_tile_type', 'mith_slab_type', 'weight'],
    'src/mondata.c': ['mith_anhydrous', 'mith_watery'],
    'src/weapon.c': ['mith_aesh_bonus'],
    'src/allmain.c': ['mith_regen_increment'],
    'src/mkmaze.c': ['bad_location'],
}.items():
    source = (repo / file).read_text(encoding='utf8')
    for name in names:
        parts.append(re.search(r'(?m)^(?:staticfn )?\w+\n' + name + r'\([\s\S]*?^\}', source)[0])
(out / 'step9c_foundation.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_foundation.exe'),
                str(repo / 'test/test_step9c_foundation.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_foundation.exe')], cwd=out, check=True)
