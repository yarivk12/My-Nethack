"""Exercise real scoped weapon effects and projectile hit dispatch."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
source=(repo/'src/weapon.c').read_text(encoding='utf8')
parts=[re.search(r'(?m)^int\nmith_weapon_effects\([\s\S]*?^\}',source)[0]]
missiles=(repo/'src/mthrowu.c').read_text(encoding='utf8')
hero=re.search(r'if \(obj && \(obj->oclass == WEAPON_CLASS \|\| is_weptool\(obj\)\)\)\n +dam \+= mith_weapon_effects\(obj, &gy.youmonst, dam\);',missiles)[0]
monster=re.search(r'if \(otmp->oclass == WEAPON_CLASS \|\| is_weptool\(otmp\)\)\n +damage \+= mith_weapon_effects\(otmp, mtmp, damage\);',missiles)[0]
parts+=['static int missile_hero(struct obj *obj,int dam) {'+hero+'return dam;}',
        'static int missile_mon(struct obj *otmp,struct monst *mtmp,int damage) {'+monster+'return damage;}']
assert missiles.index(monster)>missiles.index('if (!harmless && !DEADMONSTER(mtmp))')
(out/'step9c_coatings.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_coatings.exe'),str(repo/'test/test_step9c_coatings.c'),
 str(repo/'src/monst.c'),str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_coatings.exe')],cwd=out,check=True)
