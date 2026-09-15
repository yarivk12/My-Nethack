"""Compile actual dust callbacks and compare pinned damage/drift behavior."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original = subprocess.check_output(['git', 'show', pin+':dnethack-3.4.3/src/region.c'],
                                   cwd=donor).decode('utf8')
local = (repo/'src/region.c').read_text(encoding='utf8')
anatomy = (repo/'src/mondata.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^boolean\nmith_anhydrous\([\s\S]*?^\}', anatomy)[0]]
parts.append(re.search(r'(?m)^int\ndistmin\([\s\S]*?^\}',
                      (repo/'src/hacklib.c').read_text(encoding='utf8'))[0])
for name in ['create_dust_cloud', 'expire_dust_cloud', 'inside_dust_cloud']:
    part = re.search(r'(?m)^(?:NhRegion \*|boolean)\n'+name+r'\([\s\S]*?^\}', local)[0]
    if name == 'create_dust_cloud':
        part = part.replace('create_dust_cloud(', 'actual_create_dust_cloud(', 1)
    parts.append(part)
parts.append(re.search(r'(?m)^void\nmith_dust_storm\([\s\S]*?^\}',local)[0])
for name in ['expire_dust_cloud', 'inside_dust_cloud']:
    body = re.search(r'(?m)^'+name+r'\([\s\S]*?^\{([\s\S]*?)^\}', original)[1]
    body = body.replace('(int)(intptr_t)reg->arg', 'reg->arg.a_int')
    body = body.replace('reg->rx', 'donor_x').replace('reg->ry', 'donor_y')
    body = body.replace('youracedata', 'gy.youmonst.data')
    body = body.replace('is_anhydrous', 'mith_anhydrous')
    body = body.replace('breathless_mon(mtmp)', 'breathless(mtmp->data)')
    body = body.replace('resists_sickness(mtmp)',
                        '(mtmp->data->mlet == S_FUNGUS || mtmp->data == &mons[PM_GHOUL])')
    body = body.replace('nomul(0, NULL)', 'nomul(0)')
    parts.append('static boolean donor_'+name+'(genericptr_t p1, genericptr_t p2) {'+body+'}')
(out/'step9c_dust.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_dust.exe'),
 str(repo/'test/test_step9c_dust.c'),str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_dust.exe')],cwd=out,check=True)
