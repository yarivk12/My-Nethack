"""Locked formulas/order and production integration, alongside native tests."""
from pathlib import Path
import re

R = Path(__file__).resolve().parents[1]
c = (R / 'src/apply.c').read_text()
table = c.split('static const struct forge_recipe forge_recipes[] = {', 1)[1].split('\n};', 1)[0]
actual = re.findall(r'\{ (\w+), \{ \{ (\w+), (\d+) \}, \{ (\w+), (\d+) \} \} \}', table)
expected = [
    ('KATANA', 'LONG_SWORD', 'LONG_SWORD'),
    ('TWO_HANDED_SWORD', 'LONG_SWORD', 'BROADSWORD'),
    ('TSURUGI', 'KATANA', 'TWO_HANDED_SWORD'),
    ('BATTLE_AXE', 'AXE', 'BROADSWORD'),
    ('DWARVISH_MATTOCK', 'PICK_AXE', 'DWARVISH_SHORT_SWORD'),
    ('TRIDENT', 'SCIMITAR', 'SPEAR'),
    ('ATHAME', 'DAGGER', 'STILETTO'),
    ('RUNESWORD', 'BROADSWORD', 'DAGGER'),
    ('CHAIN_MAIL', 'RING_MAIL', 'RING_MAIL'),
    ('SPLINT_MAIL', 'SCALE_MAIL', 'CHAIN_MAIL'),
    ('PLATE_MAIL', 'SPLINT_MAIL', 'CHAIN_MAIL'),
    ('ELVEN_SHIELD', 'ELVEN_DAGGER', 'SMALL_SHIELD'),
]
assert actual == [(o, a, '1', b, '1') for o, a, b in expected]
engine = c.split('/* Step 15B:', 1)[1].split('/* the #apply command', 1)[0]
assert 'hold_another_object' not in engine and 'dropy(' not in engine
assert 'splitobj(' not in engine
assert 'ENH_CONTEXT_NONE' in engine and 'enhancement_context_set(old)' in engine
assert 'invlet_basic' in engine and 'mergable(obj, output)' in engine
print('PASS Step 15B locked 12 formulas/curated order and native transaction integration')
