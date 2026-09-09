"""Compile the worn living-armor turn handler and exercise native boundaries."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
source=(repo/'src/uhitm.c').read_text(encoding='utf8')
body=re.search(r'(?m)^void\nmith_living_armor_turn\([\s\S]*?^\}',source)[0]
(out/'step9c_living.h').write_text(body,encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
 '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
 '/I'+str(out),'/Fe:'+str(out/'step9c_living.exe'),
 str(repo/'test/test_step9c_living.c'),str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_living.exe')],cwd=out,check=True)
