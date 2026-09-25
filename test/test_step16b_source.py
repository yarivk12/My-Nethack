"""Step 16B catalogue and damage integration guards."""
from pathlib import Path
R = Path(__file__).resolve().parents[1]
c = (R/'src/enhance.c').read_text()
for prefix, suffix in [('Sanctified','of Warding'),('Mystic','of Casting'),('Sorcerous','of the Archmage'),('Lightweight','of Lightness'),('Feathered','of Featherweight'),('Weightless','of Weightlessness'),('Barbed','of Thorns'),('Spiked','of Retribution'),('Hardened','of Resilience'),('Stalwart','of Preservation'),('Aegis-Bound','of Aegis'),('Bulwarked','of Safeguarding')]:
    assert f'"{prefix}", "{suffix}"' in c, prefix
print('PASS Step 16B exact catalogue names')

import hashlib, json
from step16b_source_projection import CHANGES, project
hashes=json.loads((R/'test/step16b_baseline_hashes.json').read_text())
mutations=0
for path,hunks in CHANGES.items():
    current=(R/path).read_text()
    expected=hashes[path]
    digest=lambda text: hashlib.sha256(project(path,text).encode()).hexdigest()
    assert digest(current)==expected,(path,'unreviewed production delta')
    for hunk in hunks:
        altered=current.replace(hunk['after'],'@'+hunk['after'][1:],1)
        assert digest(altered)!=expected,(path,'masked mutation')
        mutations+=1
    assert digest(current+'\n/* mutation */\n')!=expected
assert '#define EDITLEVEL 12' in (R/'include/patchlevel.h').read_text()
assert 'ENH_FATAL' in (R/'src/uhitm.c').read_text()
assert 'enhancement_weight(obj,' in (R/'src/mkobj.c').read_text()
assert 'enhancement_curse_protected(otmp)' in (R/'src/mkobj.c').read_text()
print(f'PASS Step 16B {len(CHANGES)} production files, {mutations} rejected mutations, epoch and integration guards')
