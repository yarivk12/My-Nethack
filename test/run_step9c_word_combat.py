"""Exercise real Word combat/terrain helpers with native world APIs stubbed."""
from pathlib import Path
import re
import subprocess
import sys
repo=Path(__file__).resolve().parents[1]
donor,out=map(Path,sys.argv[1:3]);out.mkdir(parents=True,exist_ok=True)
source=(repo/'src/spell.c').read_text(encoding='utf8')
parts=[re.search(r'(?m)^staticfn (?:void|boolean)\n'+name+r'\([\s\S]*?^\}',source)[0]
       for name in ['mith_blessed_light','mith_word_effect']]
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/spell.c'],cwd=donor).decode('utf8')
light=re.search(r'(?m)^blessedlight\([\s\S]*?^\{([\s\S]*?)^\}',original)[1]
light=light.replace('is_undead_mon(mon)','is_undead(mon->data)')
light=light.replace('is_primordial(mon->data)','(mon->data==&mons[PM_ASPECT_OF_THE_SILENCE])')
light=light.replace(' && !dmgtype(mon->data, AD_ACFR)','')
light=light.replace('mon->data->mlet == S_SHADE','shadelike(mon->data)')
light=light.replace('xkilled(mon, 1)','killed(mon)')
parts.append('static void donor_light(coordxy x,coordxy y) {'+light+'}')
(out/'step9c_word_combat.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_word_combat.exe'),
 str(repo/'test/test_step9c_word_combat.c'),str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_word_combat.exe')],cwd=out,check=True)
