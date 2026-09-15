"""Source contract for Step 10C-D real identity and content wiring."""
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


global_h = read("include/global.h")
extern_h = read("include/extern.h")
dungeon = read("src/dungeon.c")

# One native resolver owns all production Step 10 contexts, including the
# same-dnum Dispensary and the scheduled DoD neulev special level.
for token in ("STEP10B_CTX_APPROACH", "STEP10B_CTX_DISPENSARY"):
    assert token in global_h, token
assert "step10c_level_context" in extern_h
resolver = dungeon[dungeon.index("step10c_level_context("):]
for token in ('dname_to_dnum("Neutral Quest")',
              'dname_to_dnum("The Lost Cities")', 'find_level("neulev")',
              "STEP10C_DISPENSARY_LEVEL"):
    assert token in resolver, token
assert "depth(" not in resolver[:resolver.index("\n}")]
assert "ledger_no(" not in resolver[:resolver.index("\n}")]

# Every formerly dormant B4 production hook must use the resolver.
owners = {
    "src/spell.c": 1,
    "src/mcastu.c": 1,
    "src/dokick.c": 1,
    "src/dig.c": 1,
    "src/display.c": 1,
    "src/dog.c": 1,
}
for path, minimum in owners.items():
    text = read(path)
    assert text.count("step10c_level_context(&u.uz)") >= minimum, path
    assert "STEP10B_CTX_NONE" not in text, path

trap = read("src/trap.c")
assert trap.count("step10c_level_context(&u.uz)") >= 3
assert "STEP10B_CTX_NONE" not in trap

# Creation lifecycle wiring is identity-owned and uses the existing B4 state.
mklev = read("src/mklev.c")
mkmaze = read("src/mkmaze.c")
mkroom = read("src/mkroom.c")
assert "step10c_set_level_flags(&u.uz)" in mklev
assert "step10c_post_load_content(&u.uz)" in mkmaze
assert "svl.level.flags.lethe" in dungeon
assert "step10b_designate_plumach_shopkeeper" in mkroom
for token in ("MAGIC_PORTAL", "tseen", "step10b_designate_bridge_priest"):
    assert token in dungeon, token

# The two deferred map-owned details use existing Lua construction fields.
sp_lev = read("src/sp_lev.c")
leth_c = read("dat/leth-c-1.lua")
lethe_z = read("dat/lethe-z.lua")
assert 'get_table_boolean_opt(L, "deep", FALSE)' in sp_lev
assert 'appear_as="ter:staircase down"' in leth_c
for token in ('id="long sword"', 'name="The Sword of the Deeps"',
              'buc="cursed"', "spe=12", "deep=true"):
    assert token in lethe_z, token

for path in (ROOT / "dat").glob("*.lua"):
    assert "DEFERRED_10C_D" not in path.read_text(encoding="utf-8"), path.name

# Silver Key candidates come from the real topology, are progression-filtered,
# pass through the B3 validator, and use the native menu and transition APIs.
artifact = read("src/artifact.c")
for token in ("step10c_silver_key_domain", "VISITED",
              "silver_key_destination_valid", "create_nhwindow(NHW_MENU)",
              "select_menu", "goto_level(&target"):
    assert token in artifact, token
assert "silver_key_choose_destination((const d_level *) 0" not in artifact

print("PASS Step 10C-D identity resolver and B4 source wiring")
