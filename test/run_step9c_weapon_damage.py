"""Compare branch weapon dice with the pinned donor's actual core rules."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out = out.resolve(); out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original = subprocess.check_output(['git', 'show', pin + ':dnethack-3.4.3/src/weapon.c'],
                                   cwd=donor).decode('utf8')
source = (repo / 'src/weapon.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^staticfn int\n' + name + r'\([\s\S]*?^\}', source)[0]
         for name in ['mith_weapon_material_bonus', 'mith_weapon_dice']]
core = original.split('dmgval_core(wdice, large, obj, otyp)', 1)[1]
core = core.split('int otyp;\n{', 1)[1].split('/* SPECIAL CASES', 1)[0]
core = core.replace('obj->objsize', '(obj->obranch_size ? obj->obranch_size-1 : MZ_MEDIUM)')
core = core.replace('obj->obj_material', 'obj_material(obj)')
for name, value in {'ART_FRIEDE_S_SCYTHE': '65530', 'ART_HOLY_MOONLIGHT_SWORD': '65531',
                    'PLATINUM': '101', 'OBSIDIAN_MT': '102', 'SHADOWSTEEL': '103'}.items():
    core = core.replace(name, value)
# Null-object setup is outside this comparison; every case has an actual object.
core = re.sub(r'\tif \(!otyp\) \{[\s\S]*?\n\t\}', '', core, count=1)
moon = original.split('else if (otyp == MOON_AXE)', 1)[1].split('else if (otyp == HEAVY_IRON_BALL)', 1)[0]
moon = moon.replace('obj->ovar1', 'obj->usecount').replace('ECLIPSE_MOON', '0')
core += '\nif (otyp == MOON_AXE)' + moon
macros = original.split('#define plus_base', 1)[1].split('/* bonus dice */', 1)[0]
core += '\n#define plus_base' + macros + '\nswitch (otyp) {\n'
types = re.findall(r'\b[A-Z][A-Z_]+\b', re.search(
    r'short hwep\[\] = \{([\s\S]*?)\};', source)[1])
types = list(dict.fromkeys(types + ['CROSSBOW_BOLT', 'ARROW', 'DART', 'SHURIKEN', 'SPIKE']))
types = [t for t in types if t not in ('CORPSE', 'RUBBER_HOSE')]
bonus = original.split('/* bonus dice */', 1)[1].split('#undef plus_base', 1)[0]
for typ in types:
    mapped = 'MACE' if typ == 'SILVER_MACE' else typ
    case = re.search(r'(?m)^\tcase ' + mapped + r':[^\n]*', bonus)
    if case:
        assert 'break;' in case[0], case[0]
        core += case[0].replace('case ' + mapped + ':', 'case ' + typ + ':') + '\n'
core += '}\n#undef plus_base\n#undef plus\n#undef pls\n#undef add\n#undef chrgd\n'
core += original.split('/* safety checks */', 1)[1].split('/* plug everything into wdice */', 1)[0]
crystal = original.split('case CRYSTAL_SWORD:', 2)[2].split('break;', 1)[0]
assert 'wdice.flat += otmp->spe/3;' in crystal
core += 'if(otyp==CRYSTAL_SWORD) flat += obj->spe/3;\n'
core += 'flat += d(ocn,ocd); if(bonn) flat += d(bonn,bond); return flat;\n}'
parts.append('static int donor_dice(struct obj *obj, boolean large) { int otyp=obj->otyp;' + core)
parts.append('static int tested_types[] = {' + ','.join(types) + '};')
(out / 'step9c_weapon_damage.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_weapon_damage.exe'),
                str(repo / 'test/test_step9c_weapon_damage.c'),
                str(repo / 'src/objects.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_weapon_damage.exe')], cwd=out, check=True)

# The whole native base-damage implementation remains byte-identical.
baseline = subprocess.check_output(['git', 'show',
    '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/weapon.c'], cwd=repo).decode('utf8')
native = baseline.split('int\ndmgval(', 1)[1].split('    if (Is_weapon) {', 1)[0]
native = native[native.index('    if (bigmonst(ptr))'):]
local = source.split('tmp = mith_weapon_dice(otmp, bigmonst(ptr));', 1)[1]
local = local.split('    if (Is_weapon) {', 1)[0]
local = local.replace('        case SCYTHE:\n', '')
local = local.replace('    if (otyp == VIPERWHIP)\n'
                      '        tmp *= max(1, otmp->usecount);\n', '')
assert local.replace('\n    } else if', '    if', 1) == native
print('PASS native base-damage dice unchanged outside scoped scythe additions')
