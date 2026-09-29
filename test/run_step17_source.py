"""Current Step 17 contracts plus unchanged historical source/mutation gates."""
import hashlib
from pathlib import Path
import re
import runpy
from unittest.mock import patch
from step17_source_projection import MANIFEST, project

R = Path(__file__).resolve().parents[1]
def read(path):
    return (R / path).read_text(encoding='utf8')
def body(path, name):
    match = re.search(r'(?m)^' + name + r'\([^\n]*[\s\S]*?^\}', read(path))
    assert match, (path, name)
    return match[0]
def digest(text):
    return hashlib.sha256(text.encode()).hexdigest()

mutations = 0
for path, record in MANIFEST['files'].items():
    current = read(path)
    assert digest(project(path, current)) == record['baseline_sha256'], (path, 'unreviewed delta')
    for hunk in record['hunks']:
        assert current.count(hunk['after']) == 1, (path, 'missing accepted hunk')
        altered = current.replace(hunk['after'], '@' + hunk['after'][1:], 1)
        assert digest(project(path, altered)) != record['baseline_sha256'], (path, 'masked mutation')
        mutations += 1
    assert digest(project(path, current + '\n/* unreviewed */\n')) != record['baseline_sha256']
    mutations += 1
for path, expected in MANIFEST['added'].items():
    assert digest(read(path)) == expected, (path, 'changed persistent definition')
print(f'PASS Step 17 exact production scope: {len(MANIFEST["files"])} files, {mutations} rejected mutations')

assert '#define EDITLEVEL 14' in read('include/patchlevel.h')
assert 'struct affix_slot o_affixes[ENHANCEMENT_MAX_SLOTS]' in read('include/obj.h')
assert 'uint64 affix_essence[EP_COUNT]' in read('include/context.h')
assert 'sizeof *d_##dt' in read('src/sfstruct.c')
assert 'enhancement_slots_valid(otmp)' in body('src/restore.c', 'restobj')
assert 'affix_essence' in body('src/restore.c', 'restgamestate')
for function in ('mergable',):
    assert 'enhancement_slot_count' in body('src/invent.c', function)
assert 'enhancement_slot_count' in body('src/mkobj.c', 'splitobj')
assert 'o_affixes' in body('src/zap.c', 'poly_obj')
assert 'enhancement_finalize_stack' in read('src/u_init.c')
for path in ('src/makemon.c','src/mplayer.c','src/sp_lev.c'):
    assert 'enhancement_finalize_stack' in read(path)
assert 'is_ammo(obj)' in body('src/enhance.c', 'enhancement_eligible')
wish = body('src/zap.c', 'makewish')
wish_roll = 'enhancement_generate(otmp, depth(&u.uz));'
assert wish.count(wish_roll) == 1
assert wish.index('if (otmp == &hands_obj)') < wish.index(wish_roll) < wish.index('doname(otmp)')
assert wish.index(wish_roll) < wish.index('hold_another_object(')
assert 'enhancement_eligible(' not in wish and 'enhancement_context_set(' not in wish
read_wish = body('src/objnam.c', 'readobjnam')
assert 'enhancement_generate(' not in read_wish
assert 'GENERIC_ESSENCE' in body('src/shk.c', 'billable')
assert 'GENERIC_ESSENCE' in body('src/shknam.c', 'saleable')
assert 'GENERIC_ESSENCE' in body('src/shk.c', 'get_cost')
# The 1% hook must not widen to incidental callers of the core constructors.
callers = {p.relative_to(R).as_posix() for p in (R/'src').glob('*.c')
           if re.search(r'\bordinary_loot_at\(', p.read_text())}
assert callers == {'src/enhance.c','src/mklev.c','src/mkmaze.c'}, callers
context_callers = {p.relative_to(R).as_posix() for p in (R/'src').glob('*.c')
                  if re.search(r'\bessence_loot_context\(', p.read_text())}
assert context_callers == {'src/enhance.c','src/mkobj.c'}, context_callers
commit = body('src/apply.c','forge_affix_commit')
assert commit.index('forge_spend(') < commit.index('rn2(')
assert 'enhancement_slot_history' in commit and 'return ECMD_TIME' in commit
salvage = body('src/apply.c','forge_salvage_commit')
assert salvage.index('useupall(obj)') < salvage.index('d(2, 10)')
assert 'enhancement_forge_known' in body('src/apply.c','forge_affix_target')
assert 'return forge_interact(obj);' in body('src/apply.c','doapply')
for name in ('forge_affix_commit','forge_salvage_commit','forge_affix_attempt','forge_interact'):
    assert not re.search(r'(?:svm\.moves|multi)\s*(?:=|\+\+)',body('src/apply.c',name))
print('PASS Step 17 canonical storage, lifecycle, scope, shop exclusion and native command commitment contracts')

# Existing assertions retain their original expectations. Only these exact
# reviewed production deltas are projected back to the completed Step 16C state.
original_read = Path.read_text
def historical_read(path, *args, **kwargs):
    text = original_read(path, *args, **kwargs)
    try:
        key = path.resolve().relative_to(R).as_posix()
    except ValueError:
        return text
    return project(key, text)
with patch.object(Path, 'read_text', historical_read):
    for name in ('step13','step14','step15','step15b','step15c','step15d',
                 'step15_overview','step16a','step16b','step16c','phase1_audit'):
        runpy.run_path(str(R / 'test' / f'test_{name}_source.py'), run_name='__main__')
print('PASS Step 13 through 16C historical source and mutation gates with exact Step 17 projection')
