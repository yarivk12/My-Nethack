"""Alabaster iron damage and safe handling, using actual production functions."""
from pathlib import Path
import re, subprocess, sys
repo=Path(__file__).resolve().parents[1]
donor,out=map(lambda p:Path(p).resolve(),sys.argv[1:3])
out.mkdir(parents=True,exist_ok=True)
def function(source,name):
    return re.search(r'(?m)^(?:staticfn )?[\w *]+\n'+name+r'\([\s\S]*?^\}',source)[0]
weapon=(repo/'src/weapon.c').read_text(encoding='utf8')
mon=(repo/'src/mon.c').read_text(encoding='utf8')
parts=[function(weapon,'mith_iron_damage'),function(weapon,'mith_iron_contact'),
       function(mon,'can_touch_safely')]
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/xhityhelpers.c'],cwd=donor).decode('utf8')
body=function(original,'hatesobjdmg').split('if (hates_iron(pd)',1)[1].split('if (hates_holy_mon',1)[0]
body='if (hates_iron(pd)'+body
# Khakkhara is an unrelated donor item, not in the imported branch stock.
body=re.sub(r'\s*if \(otmp->otyp == KHAKKHARA\)\s*ndice = rnd\(3\);','',body)
body=body.replace('hates_iron(pd)','mith_hates_iron(pd)').replace('otmp->obj_material','obj_material(otmp)')
parts.append('''static int donor_iron(struct monst *mdef,struct obj *otmp) {
struct permonst *pd=mdef->data; int ndice,diesize,dmg=0;
#define mlev(m) ((m)==&gy.youmonst ? (Upolyd ? mons[u.umonnum].mlevel : u.ulevel) : (m)->m_lev)
#define vd(n,x) d(n,x)
'''+body+'return dmg;\n#undef vd\n#undef mlev\n}')
old=subprocess.check_output(['git','show','5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/mon.c'],cwd=repo).decode('utf8')
guard='    if (mith_hates_iron(mdat) && obj_material(otmp) == IRON\n        && !(mtmp->misc_worn_check & W_ARMG))\n        return FALSE;\n'
assert function(mon,'can_touch_safely').replace(guard,'')==function(old,'can_touch_safely')
parts.append(function(old,'can_touch_safely').replace('can_touch_safely(','native_touch('))
assert 'bonus += mith_iron_damage(mon, obj_material(otmp));' in function(weapon,'dmgval')
special=function(weapon,'special_dmgval')
for item in ('obj','uleft','uright'):
    assert 'bonus += mith_iron_damage(mdef, obj_material('+item+'));' in special
hero=(repo/'src/uhitm.c').read_text(encoding='utf8')
for name in ('hmon_hitmon_weapon_ranged','hmon_hitmon_misc_obj'):
    assert 'hmd->dmg += mith_iron_damage(mon, hmd->material);' in function(hero,name)
(out/'step9c_iron.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_iron.exe'),str(repo/'test/test_step9c_iron.c'),
 str(repo/'src/monst.c'),str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_iron.exe')],cwd=out,check=True)
