"""Step 14 structural contracts supplement the native gameplay fixtures."""
from pathlib import Path
import re
R = Path(__file__).resolve().parents[1]
def read(p): return (R / p).read_text()
h = read('include/enhance.h')
c = read('src/enhance.c')
assert re.search(r'#define EDITLEVEL\s+9\b', read('include/patchlevel.h'))
assert 'OEP_CUMBERSOME' not in h + c
for name in ('FIRE_II', 'COLD_II', 'SHOCK_II', 'FIRE_III', 'COLD_III',
             'SHOCK_III', 'PRIMORDIAL', 'REFLECTION', 'MAGIC_RES'):
    assert 'OEP_' + name in h
assert 'depth(&u.uz)' in c
assert 'enhancement_generate' in c
assert 'enhancement_suffix' in read('src/objnam.c')
print('PASS Step 14 catalog, epoch, depth and naming structure')
