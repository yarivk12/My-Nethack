"""Pin the mirage disguise and retain native mimic/protection contracts."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
donor=Path(sys.argv[1])
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
base='5ce8b8193e4c581dd293ccac2bd0cafb4da89e96'
def fn(text,name):
    return re.search(r'(?m)^(?:staticfn )?void\n'+name+r'\([\s\S]*?^\}',text)[0]
source=(repo/'src/makemon.c').read_text(encoding='utf8')
original=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/makemon.c'],cwd=donor).decode('utf8')
assert re.search(r'else if \(mtmp->data == &mons\[PM_LIVING_MIRAGE\]\) \{\s*ap_type = M_AP_FURNITURE;\s*appear = S_puddle;',original)
old=subprocess.check_output(['git','show',base+':src/makemon.c'],cwd=repo).decode('utf8')
local=fn(source,'set_mimic_sym')
addition='''    if (mtmp->data == &mons[PM_LIVING_MIRAGE]) {
        ap_type = M_AP_FURNITURE;
        appear = S_puddle;
    } else if (OBJ_AT(mx, my)) {'''
assert local.replace(addition,'    if (OBJ_AT(mx, my)) {')==fn(old,'set_mimic_sym')
assert 'if (!mtmp || Protection_from_shape_changers)' in local
assert 'if (mndx == PM_LIVING_MIRAGE) {\n        set_mimic_sym(mtmp);' in source
source=(repo/'src/mon.c').read_text(encoding='utf8')
old=subprocess.check_output(['git','show',base+':src/mon.c'],cwd=repo).decode('utf8')
addition='''              || (mptr == &mons[PM_LIVING_MIRAGE]
                  && M_AP_TYPE(mtmp) == M_AP_FURNITURE
                  && mtmp->mappearance == S_puddle)
'''
assert fn(source,'sanity_check_single_mon').replace(addition,'')==fn(old,'sanity_check_single_mon')
for name in ('normal_shape','restore_cham'):
    assert fn(source,name)==fn(old,name)
print('PASS pinned puddle identity, native creation/protection, narrow sanity exception and unchanged restoration')
