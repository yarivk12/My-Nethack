"""Exercise actual monster weapon selection and preserve native preference order."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/weapon.c').read_text(encoding='utf8')
baseline = subprocess.check_output(['git', 'show',
    '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/weapon.c'], cwd=repo).decode('utf8')
new_types = {'CRYSTAL_SWORD', 'MOON_AXE', 'HIGH_ELVEN_WARSWORD', 'RAPIER', 'ELVEN_SICKLE', 'SPIKE'}

def array(text, name):
    return re.search(r'(?m)^static[^\n]*\b' + name + r'\[\] = \{[\s\S]*?^\};', text)[0]

for name in ['hwep', 'rwep']:
    def entries(text):
        text = re.sub(r'/\*[\s\S]*?\*/', '', array(text, name))
        return re.findall(r'\b[A-Z][A-Z_0-9]*\b', text[text.index('{'):])
    assert [x for x in entries(source) if x not in new_types] == entries(baseline)
parts = [array(source, 'hwep'), array(source, 'rwep')]
parts.append('#define Oselect(x) do { if ((otmp = oselect(mtmp, x)) != 0) return otmp; } while (0)')
for name in ['mith_bimanual', 'mith_offhand_attack', 'oselect', 'select_hwep', 'mith_select_offhand']:
    parts.append(re.search(r'(?m)^(?:staticfn )?(?:struct obj \*|boolean)\n' + name + r'\([\s\S]*?^\}', source)[0])
(out / 'step9c_weapon_selection.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_weapon_selection.exe'),
                str(repo / 'test/test_step9c_weapon_selection.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_weapon_selection.exe')], cwd=out, check=True)
print('PASS native melee/ranged preference order unchanged after removing new types')
