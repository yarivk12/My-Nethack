"""Compare the exact branch-local hostility rule with pinned dNetHack."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
donor,out=map(Path,sys.argv[1:3]);out.mkdir(parents=True,exist_ok=True)
source=(repo/'src/mon.c').read_text(encoding='utf8')
body=source.split('/* Mithardir\'s Alabaster defenders and oozes are mutual enemies. */',1)[1].split('/* The Moria captors',1)[0]
parts=['static long local_rule(struct monst *magr,struct monst *mdef) {'+body+'return 0;}']
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/mon.c'],cwd=donor).decode('utf8')
body=original.split('/* Alabaster elves vs. oozes */',1)[1].split('/* drow vs. other drow */',1)[0]
body=body.replace('is_undead_mon(magr)','is_undead(magr->data)').replace('is_undead_mon(mdef)','is_undead(mdef->data)')
parts.append('static long donor_rule(struct monst *magr,struct monst *mdef) {struct permonst *ma=magr->data,*md=mdef->data;'+body+'return 0;}')
old=subprocess.check_output(['git','show','5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/mon.c'],cwd=repo).decode('utf8')
pattern=r'(?m)^staticfn long\nmm_aggression\([\s\S]*?^\}'
assert re.search(pattern,source)[0]==re.search(pattern,old)[0], 'native pet guard/dispatch changed'
(out/'step9c_aggression.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_aggression.exe'),
 str(repo/'test/test_step9c_aggression.c'),str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_aggression.exe')],cwd=out,check=True)
