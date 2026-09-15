"""Compare explicit armor sizing with pinned donor wear conditions."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
donor,out=map(lambda p:Path(p).resolve(),sys.argv[1:3]);out.mkdir(parents=True,exist_ok=True)
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
source=(repo/'src/worn.c').read_text(encoding='utf8')
parts=[re.search(r'(?m)^boolean\nmith_armor_size_fits\([\s\S]*?^\}',source)[0]]
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/do_wear.c'],cwd=donor).decode('utf8')
conditions=list(dict.fromkeys(re.findall(r'else if\(([^\n]*objsize[^\n]*)\)\{',original)))
assert len(conditions)==3,conditions
strict,cloak,suit=[c.replace('youracedata','ptr').replace('otmp->objsize','(otmp->obranch_size-1)') for c in conditions]
header=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/include/obj.h'],cwd=donor).decode('utf8')
elven=re.search(r'#define is_elven_armor\(otmp\)([\s\S]*?)(?=\n#define)',header)[1]
elven=elven.replace('ELVEN_HELM','ELVEN_LEATHER_HELM').replace('HIGH_ELVEN_LEATHER_HELM','HIGH_ELVEN_HELM')
parts.append('#define donor_elven(otmp) '+elven)
parts.append('''static boolean donor_fits(struct obj *otmp,const struct permonst *ptr) {
if (!otmp->obranch_size || otmp->oclass!=ARMOR_CLASS || is_shield(otmp)) return TRUE;
if (is_helmet(otmp) && obj_material(otmp)<=LEATHER) return TRUE;
if (is_cloak(otmp)) return !('''+cloak+''');
if (is_suit(otmp)) return !('''+suit.replace('is_elven_armor','donor_elven')+''');
return !('''+strict+''');
}''')
assert 'if (!mith_armor_size_fits(otmp, gy.youmonst.data))' in (repo/'src/do_wear.c').read_text(encoding='utf8')
assert '!mith_armor_size_fits(obj, mon->data)' in source
(out/'step9c_armor_size.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_armor_size.exe'),str(repo/'test/test_step9c_armor_size.c'),
 str(repo/'src/monst.c'),str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_armor_size.exe')],cwd=out,check=True)
