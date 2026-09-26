"""Two-word closure and native integration gates, paired with runtime tests."""
from pathlib import Path
import re

R = Path(__file__).resolve().parents[1]
h = (R / 'include/enhance.h').read_text()
c = (R / 'src/enhance.c').read_text()
obj = (R / 'include/obj.h').read_text()

def body(path, name):
    text = (R / path).read_text()
    match = re.search(r'(?m)^' + name + r'\([^\n]*[\s\S]*?^\}', text)
    assert match, (path, name)
    return match[0]

assert '#define EDITLEVEL 13' in (R / 'include/patchlevel.h').read_text()
assert '__int128' not in h + c + obj
for field in ('o_enh_props', 'o_enh_props2', 'o_enh_known', 'o_enh_known2'):
    assert re.search(r'uint64\s+' + field + r'\s*;', obj), field
    assert field in body('src/invent.c', 'mergable'), field
assert 'o_purification_remaining' in obj
assert 'o_purification_sampled' in obj
assert 'purification_snapshot_turn' in (R / 'include/context.h').read_text()
assert 'sizeof *d_##dt' in (R / 'src/sfstruct.c').read_text()
assert 'utility_purification_interval(obj)' in body('src/invent.c', 'mergable')
assert 'utility_purification_interval(otmp)' in body('src/invent.c', 'mergable')
for name in ('enhancement_mask_property', 'enhancement_mask_union',
             'enhancement_mask_count', 'enhancement_actual', 'enhancement_known',
             'enhancement_set_mask', 'enhancement_mask_allowed'):
    assert name in h and name in c, name
for path in ('src/enhance.c', 'src/utility.c', 'src/apply.c', 'src/wizcmds.c', 'src/inspect.c'):
    if not (R / path).exists():
        continue
    text = (R / path).read_text()
    assert not re.search(r'1(?:ULL|UL|LL)?\s*<<\s*\(?\s*(?:property_id|prop_id|id)\b', text), path
    assert not re.search(r'\b(?:property_id|prop_id|id)\s*<\s*64\b', text), path
assert 'enhancement_mask' in body('src/enhance.c', 'enhancement_generate')
assert 'enhancement_mask' in body('src/apply.c', 'forge_inherit')
assert 'enhancement_mask' in body('src/enhance.c', 'enhancement_names')
recipient = body('src/enhance.c', 'utility_recipient')
assert 'return object_origin(obj->otyp) == OBJ_ORIGIN_VANILLA;' in recipient
assert 'return TRUE' not in recipient
assert 'obj->oartifact' in recipient and 'objects[obj->otyp].oc_unique' in recipient
exclusions = '''SADDLE CHEST LARGE_BOX ICE_BOX IRON_SAFE
CANDELABRUM_OF_INVOCATION BELL_OF_OPENING BEARTRAP LAND_MINE BAG_OF_TRICKS
CRYSTAL_BALL MIRROR TINNING_KIT FIGURINE TALLOW_CANDLE WAX_CANDLE MAGIC_CANDLE
CAN_OF_GREASE CRYSTAL_PICK'''.split()
assert set(re.findall(r'case (\w+):', recipient)) == set(exclusions)
assert 'obj->otyp == PICK_AXE' in body('src/enhance.c', 'utility_property_allowed')
for name in ('enhancement_eligible', 'utility_property_allowed'):
    assert 'utility_recipient(obj)' in body('src/enhance.c', name)
for path, name, call in (
    ('src/enhance.c', 'enhancement_generate', 'enhancement_eligible(obj)'),
    ('src/apply.c', 'forge_inherit', 'enhancement_mask_allowed(obj,'),
    ('src/enhance.c', 'enhancement_set_mask', 'enhancement_mask_allowed(obj,'),
    ('src/enhance.c', 'socket_normalize', 'enhancement_mask_allowed(&ordinary,'),
    ('src/enhance.c', 'enhancement_normalize', 'socket_normalize(obj)'),
    ('src/enhance.c', 'enhancement_set', 'enhancement_set_mask(obj,'),
    ('src/enhance.c', 'enhancement_change_type', 'enhancement_normalize(obj)'),
    ('src/restore.c', 'restobj', 'socket_normalize(otmp)'),
    ('src/zap.c', 'poly_obj', 'socket_normalize(otmp)'),
):
    assert call in body(path, name), (path, name)
assert 'o_enh_props2 =' not in (R / 'src/wizcmds.c').read_text()
import test_object_provenance
print('PASS Step 16C provenance policy and shared generation/forge/setter/normalization/transform/restore validation')
print('PASS Step 16C two-word declarations, persistence fields, merge comparison and generic raw-ID audit')

# Exact accepted deltas preserve every earlier mutation gate. The baseline
# remains the actual preflight HEAD, not a hash generated during this test.
import hashlib
import json
from step16c_source_projection import CHANGES, project
hashes = json.loads((R / 'test/step16c_baseline_hashes.json').read_text())
mutations = 0
for path, hunks in CHANGES.items():
    current = (R / path).read_text()
    digest = lambda value: hashlib.sha256(project(path, value).encode()).hexdigest()
    assert digest(current) == hashes[path], (path, 'unreviewed Step 16C delta')
    for hunk in hunks:
        assert current.count(hunk['after']) == 1, (path, 'missing frozen hunk')
        changed = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(changed) != hashes[path], (path, 'masked mutation')
        mutations += 1
    assert digest(current + '\n/* mutation */\n') != hashes[path]
    mutations += 1
print(f'PASS Step 16C {len(CHANGES)} production files, {mutations} rejected mutations against preflight HEAD')
