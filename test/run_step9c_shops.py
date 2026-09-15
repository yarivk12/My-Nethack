"""Check actual imported stock code and arrays against immutable donor data.

VS shell: runner PINNED_DNET_HACK_CLONE OUTPUT_DIRECTORY
Object allocation, fruit registration and RNG are fixture adapters. Full
shopkeeper creation, billing and services require separate wizard tests.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
source = (repo / 'src/shknam.c').read_text(encoding='utf8')
upstream = subprocess.check_output(
    ['git', '-C', str(donor), 'show', pin + ':dnethack-3.4.3/src/shknam.c'],
    text=True, encoding='utf8')
parts = ['#define VEGETARIAN_CLASS (MAXOCLASSES + 1)',
         '#define MITH_TILE_CLASS (MAXOCLASSES + 2)']
parts += re.findall(r'(?m)^static const [^\n]+\[\] = \{[\s\S]*?^\};', source)
parts.append(re.search(r'(?m)^const struct shclass shtypes\[\] = \{[\s\S]*?^\};', source)[0])
for name in ['mith_shop_stock', 'get_shop_item']:
    parts.append(re.search(r'(?m)^(?:staticfn )?\w+\n' + name
                           + r'\([\s\S]*?^\}', source)[0])
checks = []
for name in ['garden_armors', 'garden_weapons', 'sand_armors', 'sand_weapons',
             'fancy_clothes']:
    array = re.search(r'(?m)^const int ' + name + r'\[\] = \{[\s\S]*?^\};', upstream)[0]
    array = re.sub(r'\bCLOAK\b', 'LEATHER_CLOAK', array)
    array = re.sub(r'\bGLOVES\b', 'LEATHER_GLOVES', array)
    array = re.sub(r'\bELVEN_HELM\b', 'ELVEN_LEATHER_HELM', array)
    parts.append(array.replace(name, 'donor_' + name))
    checks.append('assert(sizeof(%s)==sizeof(donor_%s));' % (name, name))
    checks.append('assert(!memcmp(%s,donor_%s,sizeof(%s)));' % (name, name, name))
parts.append('static void check_donor_lists(void) {\n' + '\n'.join(checks) + '\n}')
(out / 'step9c_shops.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_shops.exe'),
                str(repo / 'test/test_step9c_shops.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_shops.exe')], cwd=out, check=True)
