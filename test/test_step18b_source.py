"""Current catalogue/native charge seam and exact historical-delta guards."""
import hashlib
import re
from pathlib import Path
from step18b_source_projection import MANIFEST, project

R = Path(__file__).resolve().parents[1]
def read(path):
    return (R / path).read_text()

def body(text, name):
    return re.search(r'(?m)^' + name + r'\([^\n]*[\s\S]*?^\}', text)[0]

rejections = 0
for path, record in MANIFEST.items():
    text = read(path)
    digest = lambda s: hashlib.sha256(s.encode()).hexdigest()
    assert digest(project(path, text)) == record['baseline_sha256'], path
    for hunk in record['hunks']:
        assert text.count(hunk['after']) == 1
        altered = text.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project(path, altered)) != record['baseline_sha256']
        rejections += 1
    assert digest(project(path, text + '\n/* unreviewed */')) != record['baseline_sha256']

apply = read('src/apply.c')
mkobj = read('src/mkobj.c')
catalogue = apply.split('forge_recipes[] = {')[1].split('\n};')[0]
expected = read('test/test_step18b.c').split('step18b_expected[] = {')[1].split('\n};')[0]
formula = r'\{ (\w+), \{ \{ (\w+), 1 \}, \{ (\w+), 1 \} \} \}'
actual = re.findall(formula, catalogue)
approved = re.findall(formula, expected)
assert len(actual) == 47 and len(approved) == 24
assert len({out for out, _, _ in approved}) == 24
assert all(actual.count(recipe) == 1 for recipe in approved)
assert len({tuple(sorted((a, b))) for _, a, b in actual}) == 47
for output, a, b in actual:
    if output == 'SPEED_BOOTS': assert (a, b) == ('ELVEN_BOOTS', 'SPE_HASTE_SELF')
    if output == 'LEVITATION_BOOTS': assert (a, b) == ('LOW_BOOTS', 'RIN_LEVITATION')
output = body(apply, 'forge_output')
assert 'init_obj_charges(obj)' in output
assert 'mksobj(typ, TRUE' not in output
assert not any(name in output for name in ('MAGIC_FLUTE', 'MAGIC_HARP'))
charges = body(mkobj, 'init_obj_charges')
assert 'obj->spe = rn1(5, 4)' in charges
assert 'init_obj_charges(otmp)' in body(mkobj, 'mksobj_init')
assert 'rn1(5, 4)' not in body(mkobj, 'mksobj_init')
enchantment = body(apply, 'forge_enchantment_supported')
assert 'RING_CLASS && objects[obj->otyp].oc_charged' in enchantment
assert 'WAND_CLASS' not in enchantment and 'TOOL_CLASS' not in enchantment
assert 'is_weptool(obj)' in enchantment
assert '#define EDITLEVEL 14' in read('include/patchlevel.h')
print(f'PASS Step 18B catalogue/native charge contracts and {rejections} exact-delta mutation rejections')
