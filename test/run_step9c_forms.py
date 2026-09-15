"""Compare selected transformation decisions with pinned were.c; test shifts."""
from pathlib import Path
import re
import subprocess
import sys

PIN = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
repo = Path(__file__).resolve().parents[1]
donor, out = map(lambda p: Path(p).resolve(), sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/were.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^staticfn \w+\n' + name + r'\([\s\S]*?^\}', source)[0]
         for name in ['mith_fey_counter', 'mith_fey_change_ready', 'mith_fey_shift']]
# NEED_WEAPON is a deferred request, not a valid immediate wield operation.
# Check the native callee's accepted states rather than trusting a test stub.
wield = re.search(r'(?m)^int\nmon_wield_item\([\s\S]*?^\}',
                  (repo/'src/weapon.c').read_text(encoding='utf8'))[0]
states = re.findall(r'case (NEED_\w+):', wield)
assert 'NEED_HTH_WEAPON' in states and 'NEED_WEAPON' not in states
source = subprocess.check_output(['git', 'show', PIN + ':dnethack-3.4.3/src/were.c'], cwd=donor).decode('utf8')
body = re.search(r'(?m)^were_change\(mon\)[\s\S]*?^\}', source)[0]
body = 'static void donor_check(struct monst *mon)\n' + body[body.index('{'):]
body = body.replace('new_were(mon)', 'donor_changed++')
body = body.replace('is_pool(mon->mx, mon->my, FALSE)', 'is_pool(mon->mx, mon->my)')
body = body.replace('flags.soundok', '0').replace('mon->mfaction', 'mon->mspare1')
parts.append(body)
(out / 'step9c_forms.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_forms.exe'),
                str(repo / 'test/test_step9c_forms.c'),
                str(repo / 'src/monst.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_forms.exe')], cwd=out, check=True)
