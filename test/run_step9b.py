"""VS shell: compile actual Dragon Caves mechanics and all prior fixtures."""
from pathlib import Path
import re
import subprocess
import sys
repo=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]).resolve()
subprocess.run([sys.executable,'-B',str(repo/'test/run_step9a.py'),str(out)],check=True)
parts=[]
for path,names in {
 'src/dungeon.c':['In_dragon_caves'],
 'src/mon.c':['cave_dragon_scale_chance','make_corpse'],
 'src/mondata.c':['get_atkdam_type','cave_breath_type','dmgtype_fromattack','dmgtype'],
 'src/zap.c':['lava_jet_obstacle','montraits'],
 'src/polyself.c':['armor_to_dragon'],
 'src/do_wear.c':['dragon_armor_handling'],
 'src/worn.c':['update_mon_extrinsics'],
}.items():
 source=(repo/path).read_text(encoding='utf8')
 if path=='src/mon.c':
  parts.append(re.search(r'#define KEEPTRAITS[\s\S]*?\n\n',source)[0])
 for name in names:
  matches=re.findall(r'(?m)^(?:staticfn )?(?:struct \w+ \*|\w+)\n'+name+r'\([\s\S]*?^\}',source)
  assert len(matches)==1,(path,name)
  parts.append(matches[0])
(out/'step9b_functions.h').write_text('\n\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9b_runtime.exe'),
 str(repo/'test/test_step9b_runtime.c'),str(repo/'src/monst.c'),
 str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9b_runtime.exe')],cwd=out,check=True)
