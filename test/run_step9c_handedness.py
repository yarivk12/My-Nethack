"""Compare sized branch weapons with pinned dNetHack's actual bimanual rule."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out = out.resolve(); out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'

def read(path):
    return subprocess.check_output(['git', 'show', pin + ':dnethack-3.4.3/' + path],
                                   cwd=donor).decode('utf8')

source = (repo / 'src/weapon.c').read_text(encoding='utf8')
local = re.search(r'(?m)^boolean\nmith_bimanual\([\s\S]*?^\}', source)[0]
original = re.search(r'(?m)^boolean\nbimanual\([\s\S]*?^\}', read('src/wield.c'))[0]
original = original.replace('bimanual(otmp, ptr)\nstruct obj * otmp;\nstruct permonst * ptr;',
                            'donor_bimanual(struct obj *otmp, struct permonst *ptr)')
original = original.replace('objects[otmp->otyp].oc_size', 'donor_sizes[otmp->otyp]')
original = original.replace('otmp->objsize', '(otmp->obranch_size - 1)')
# The selected ordinary weapons/species never use these unrelated exceptions.
for old, new in {'PM_THRONE_ARCHON': 'PM_ORACLE', 'PM_LUNGORTHIN': 'PM_MEDUSA',
                 'PM_BASTARD_OF_THE_BOREAL_VALLEY': 'PM_WIZARD_OF_YENDOR',
                 'ART_HOLY_MOONLIGHT_SWORD': '65535', 'ART_FRIEDE_S_SCYTHE': '65534',
                 'DOUBLE_LIGHTSABER': 'STRANGE_OBJECT', 'MZ_MEDIUM': 'MZ_HUMAN'}.items():
    original = original.replace(old, new)
catalog = dict(re.findall(r'(?:WEAPON|WEPTOOL)\("([^"]+)",[^\n]*\n[^\n]*?\b(MZ_\w+)',
                          read('src/objects.c')))
catalog.update(re.findall(r'BOW\("([^"]+)",[^\n]*?\b(MZ_\w+)', read('src/objects.c')))
catalog.update((name, 'MZ_TINY') for name in re.findall(
    r'PROJECTILE(?:_MATSPEC)?\("([^"]+)"', read('src/objects.c')))
array = re.search(r'short hwep\[\] = \{([\s\S]*?)\};', source)[1]
types = re.findall(r'\b[A-Z][A-Z_]+\b', re.sub(r'/\*[\s\S]*?\*/', '', array))
types += ['BOW', 'ELVEN_BOW', 'ORCISH_BOW', 'YUMI', 'CROSSBOW', 'SLING',
          'ARROW', 'ELVEN_ARROW', 'ORCISH_ARROW', 'SILVER_ARROW', 'YA',
          'CROSSBOW_BOLT', 'DART', 'SHURIKEN', 'SPIKE']
aliases = {'SILVER_SABER': 'saber', 'TWO_HANDED_SWORD': 'two-handed sword',
           'BATTLE_AXE': 'battle-axe', 'HIGH_ELVEN_WARSWORD': 'high-elven warsword',
           'PICK_AXE': 'pick-axe', 'SILVER_DAGGER': 'dagger', 'SILVER_MACE': 'mace',
           'SILVER_SPEAR': 'spear', 'SILVER_ARROW': 'arrow'}
pairs = []
for typ in types:
    if typ in ('CORPSE', 'RUBBER_HOSE'):
        continue
    name = aliases.get(typ, typ.lower().replace('_', ' '))
    assert name in catalog, (typ, name)
    pairs.append((typ, catalog[name].replace('MZ_MEDIUM', 'MZ_HUMAN')))
header = 'static int donor_sizes[NUM_OBJECTS] = {\n' + ''.join(
    f'[{typ}] = {size},\n' for typ, size in pairs) + '};\n'
header += 'static int tested_types[] = {' + ','.join(t for t, _ in pairs) + '};\n'
(out / 'step9c_handedness.h').write_text(header + original + '\n' + local, encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_handedness.exe'),
                str(repo / 'test/test_step9c_handedness.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_handedness.exe')], cwd=out, check=True)
