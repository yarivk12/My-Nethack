# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-18, 2026-09-19,
# 2026-09-24, 2026-09-25, 2026-09-26.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Step 14 structural contracts supplement the native gameplay fixtures."""
from pathlib import Path
import re
R = Path(__file__).resolve().parents[1]
def read(p): return (R / p).read_text()
h = read('include/enhance.h')
c = read('src/enhance.c')
assert re.search(r'#define EDITLEVEL\s+13\b', read('include/patchlevel.h'))
assert 'OEP_CUMBERSOME' not in h + c
for name in ('FIRE_II', 'COLD_II', 'SHOCK_II', 'FIRE_III', 'COLD_III',
             'SHOCK_III', 'PRIMORDIAL', 'REFLECTION', 'MAGIC_RES'):
    assert 'OEP_' + name in h
assert 'depth(&u.uz)' in c
assert 'enhancement_generate' in c
assert 'enhancement_suffix' in read('src/objnam.c')
print('PASS Step 14 catalog, epoch, depth and naming structure')
