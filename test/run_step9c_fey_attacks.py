"""Exercise actual Eladrin weapon dispatch and post-hit sleep helpers."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve(); out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/uhitm.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^(?:boolean|void)\n' + name + r'\([\s\S]*?^\}', source)[0]
         for name in ['mith_fey_weapon_attack', 'mith_coure_sleep']]
dispatch = re.search(r'(?m)^void\nmhitm_adtyping\([\s\S]*?^\}', source)[0]
# Preserve the complete new dispatch prefix. The native switch is a fixture
# boundary; its mature handlers are not reimplemented in this focused test.
prefix, native = dispatch.split('    switch (mattk->adtyp) {', 1)
assert native.startswith('\n    case AD_STUN:')
parts.append(prefix + '    ++native_calls;\n}')
(out / 'step9c_fey_attacks.h').write_text('\n'.join(parts), encoding='utf8')
for file, required in {
    'src/mhitu.c': ['u.umortality == mortality', 'mith_coure_sleep(mtmp, &gy.youmonst)'],
    'src/mhitm.c': ['return mhm.hitflags; /* mdef lifesaved */', 'mith_coure_sleep(magr, mdef)'],
}.items():
    text = (repo / file).read_text(encoding='utf8')
    assert all(line in text for line in required)
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_fey_attacks.exe'),
                str(repo / 'test/test_step9c_fey_attacks.c'), str(repo / 'src/monst.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_fey_attacks.exe')], cwd=out, check=True)
