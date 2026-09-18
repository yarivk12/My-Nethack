"""Compile actual scoped defenses and vision predicate with native databases."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
parts = []
for file, names in {
    'src/worn.c': ['mith_armor_base', 'mith_material_defense', 'mith_armor_dr',
                   'mith_worn', 'step10b_natural_dr', 'mith_roll_dr',
                   'mith_physical_damage'],
    'src/hacklib.c': ['isqrt'],
    'src/weapon.c': ['mith_aesh_bonus'],
}.items():
    source = (repo / file).read_text(encoding='utf8')
    for name in names:
        parts.append(re.search(r'(?m)^(?:staticfn )?(?:struct obj \*|\w+)\n'
                               + name + r'\([\s\S]*?^\}', source)[0])
missiles = (repo/'src/mthrowu.c').read_text(encoding='utf8')
hero = re.search(r'if \(!is_acid\)\n +dam = mith_physical_damage\(&gy.youmonst, obj, AT_WEAP, dam\);', missiles)[0]
monster = re.search(r'if \(otmp->otyp != ACID_VENOM\)\n +damage = mith_physical_damage\(mtmp, otmp, AT_WEAP, damage\);', missiles)[0]
parts.append('static int missile_hero(struct obj *obj, int dam, boolean is_acid) {'+hero+'return dam;}')
parts.append('static int missile_mon(struct monst *mtmp, struct obj *otmp, int damage) {'+monster+'return damage;}')
hits=(repo/'src/uhitm.c').read_text(encoding='utf8')
guard=hits.split('    if (hmd.use_weapon_skill && !hmd.already_killed)',1)[1].split('    if (!hmd.already_killed && hmd.dmg > 0)',1)[0]
parts.append('''static int hero_hit(struct monst *mon,struct obj *obj,int damage,
boolean skilled,boolean physical,boolean dead) {
struct {int dmg;boolean use_weapon_skill,get_dmg_bonus,already_killed;} hmd;
hmd.dmg=damage;hmd.use_weapon_skill=skilled;hmd.get_dmg_bonus=physical;hmd.already_killed=dead;
if (hmd.use_weapon_skill && !hmd.already_killed)'''+guard+'return hmd.dmg;}')
assert missiles.index(monster) < missiles.index('if (otmp->opoisoned && is_poisonable(otmp))')
kick=(repo/'src/dokick.c').read_text()
baseline=subprocess.check_output(['git','show','5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/dokick.c'],cwd=repo).decode()
addition='    dmg += mith_aesh_bonus();\n    dmg = mith_physical_damage(mon, uarmf, AT_KICK, dmg);\n'
tree_helper=re.search(r'(?m)^boolean\nstep10b_tree_kick_has_loot\([\s\S]*?^\}\n\n',kick)[0]
tree_hook='''        /* Outlands trees yield neither fruit nor a swarm.  Step 10C-D
         * supplies the real production level identity. */
        if (!step10b_tree_kick_has_loot(step10c_level_context(&u.uz))) {
            kick_ouch(x, y, "");
            return ECMD_TIME;
        }

'''
assert kick.count(addition)==1 and kick.count(tree_hook)==1
assert kick.replace(tree_helper,'',1).replace(tree_hook,'',1).replace(addition,'')==baseline
guard=kick.split('    dmg += u.udaminc; /* add ring(s) of increase damage */',1)[1].split('    if (!DEADMONSTER(mon)',1)[0]
parts.append('static void kick_commit(struct monst *mon,int dmg) {'+guard+'}')
(out / 'step9c_defense.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_defense.exe'),
                str(repo / 'test/test_step9c_defense.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')],
               cwd=out, check=True)
subprocess.run([str(out / 'step9c_defense.exe')], cwd=out, check=True)
