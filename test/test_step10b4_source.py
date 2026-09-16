"""Production-path, lifecycle, reuse, and Step 10C boundary gate for B4."""
from pathlib import Path
import re


repo = Path(__file__).resolve().parents[1]
read = lambda path: (repo / path).read_text(encoding="utf8")

global_h = read("include/global.h")
extern_h = read("include/extern.h")
trap = read("src/trap.c")
hack = read("src/hack.c")
spell = read("src/spell.c")
mcastu = read("src/mcastu.c")
dog = read("src/dog.c")
dig = read("src/dig.c")
dokick = read("src/dokick.c")
display = read("src/display.c")
pager = read("src/pager.c")
shknam = read("src/shknam.c")
priest = read("src/priest.c")
sp_lev = read("src/sp_lev.c")
sp_lev_h = read("include/sp_lev.h")
obj_h = read("include/obj.h")
save = read("src/save.c")
restore = read("src/restore.c")

# Explicit, nonserialized context seam.  B4 hooks are live but dormant until
# Step 10C replaces the local NONE argument with its identity resolver.
for name in ("STEP10B_CTX_NONE", "STEP10B_CTX_GATE",
             "STEP10B_CTX_OUTLANDS_1", "STEP10B_CTX_OUTLANDS_4",
             "STEP10B_CTX_SPIRE", "STEP10B_CTX_SUM",
             "STEP10B_CTX_LOST_CITIES", "STEP10B_CTX_RLYEH"):
    assert name in global_h
assert "step10b_level_context" not in read("include/you.h")
assert "step10b_level_context" not in read("include/rm.h")

# Lethe immersion uses the existing flag and native arrival path.  It invokes
# memory loss and the Lethe chain before native gremlin/golem/drowning logic.
drown = trap[trap.index("drown(void)"):trap.index("if (u.umonnum == PM_GREMLIN",
                                                   trap.index("drown(void)"))]
assert "svl.level.flags.lethe" in drown
assert "step10b_forget_memories(10)" in drown
assert "step10b_lethe_damage_chain(gi.invent, FALSE)" in drown
assert drown.index("step10b_forget_memories") < drown.index(
    "step10b_lethe_damage_chain")
assert "otmp = here ? obj->nexthere : obj->nobj" in trap
assert "step10b_lethe_damage_chain(obj->cobj, FALSE)" in trap
for fragment in ("obj->blessed = obj->cursed = 0",
                 "STEP10B_SCR_RESISTANCE", "SPE_BOOK_OF_THE_DEAD",
                 "obj->owt = weight(obj)", "drain_item(obj, FALSE)",
                 "step10b_lethe_marker_spe"):
    assert fragment in trap, fragment
assert "sparkling " in pager and "svl.level.flags.lethe" in pager
assert "spoteffects(TRUE)" in read("src/teleport.c")
assert "drown()" in hack

# Spell gradients are inserted into the actual hero and monster cast paths.
assert "step10b_hero_spell_chance(step10c_level_context(&u.uz)" in spell
assert spell.index("step10b_hero_spell_chance(step10c_level_context(&u.uz)") < spell.index(
    "/* Clamp to percentile */")
cast = mcastu[mcastu.index("nomul(0);"):mcastu.index(
    "if (canspotmon", mcastu.index("nomul(0);"))]
assert "enum step10b_level_context context = step10c_level_context(&u.uz)" in cast
assert "step10b_mon_spell_fumble_threshold(" in cast
assert "step10b_mon_spell_always_fumbles(context)" in cast

# Outlands trees, mirror-shard pits, and projectile materials are hooked at
# their native behavior points; ordinary contexts preserve native behavior.
assert "step10b_tree_kick_has_loot(step10c_level_context(&u.uz))" in dokick
assert "enum step10b_level_context context = step10c_level_context(&u.uz)" in dig
assert "step10b_tree_cut_sticks(" in dig
assert re.search(r"mksobj_at\(\s*rn2\(2\) \? QUARTERSTAFF : CLUB", dig)
assert "mirror-shards" in trap
assert "enum step10b_level_context context = step10c_level_context(&u.uz)" in trap
assert "step10b_mirror_pit_damage(" in trap
assert "mon_hates_silver(mtmp)" in trap and "Hate_silver" in trap
assert "step10b_trap_projectile_material(" in trap
assert "otmp->obranch_material" in trap and "otmp->owt = weight(otmp)" in trap

# Presentation colors are applied late enough to preserve no-color mode.
assert re.search(r"step10b_terrain_color\(\s*step10c_level_context\(&u\.uz\)", display)
assert "if (glyph_is_cmap(glyph))" in display

# Gate Town catch-up is isolated in the native pet separation path.
pet = dog[dog.index("step10b_pet_separation_catchup"):
          dog.index("if (!mtmp->mtame && mtmp->mleashed")]
assert "STEP10B_CTX_GATE" in pet
assert "edog->hungrytime = svm.moves + 500" in pet
assert "step10b_pet_separation_catchup(mtmp, imv,\n                                   step10c_level_context(&u.uz))" in dog

# Species morph helpers require native auxiliary roles first, then preserve
# their shop/temple ownership while changing only species/hostility.
plumach = shknam[shknam.index("step10b_designate_plumach_shopkeeper"):]
assert "isshk" in plumach and "has_eshk" in plumach
assert "PM_PLUMACH_RILMANI" in plumach and "newcham" in plumach
bridge = priest[priest.index("step10b_designate_bridge_priest"):]
assert "ispriest" in bridge and "has_epri" in bridge
assert "PM_BLASPHEMOUS_LURKER" in bridge and "mpeaceful = 0" in bridge
assert "set_malign" in bridge
for fragment in ("neweshk", "free_eshk", "Sfo_eshk", "Sfi_eshk",
                 "newepri", "free_epri", "Sfo_epri", "Sfi_epri"):
    assert fragment in (shknam + priest + save + restore), fragment

# Portal seen, ordinary room kinds, and object metadata are native reuse.
assert "boolean seen" in sp_lev_h
assert "MKTRAP_SEEN" in sp_lev
assert "get_table_boolean_opt(L, \"seen\", FALSE)" in sp_lev
for room in ("SHOPBASE", "TEMPLE", "BARRACKS", "COURT"):
    assert room in read("include/mkroom.h") + read("src/mkroom.c")
for field in ("obranch_material", "obranch_size", "obranch_props"):
    assert field in obj_h
assert "Sfo_obj" in save and "Sfi_obj" in restore

# B4 owns no topology, scheduler, ID, or edit-level change.
assert "#define EDITLEVEL 6" in read("include/patchlevel.h")
assert "#define MAXDUNGEON 18" in global_h
assert "DL111" not in read("dat/dungeon.lua")
assert "step10c_level_context" in read("src/dungeon.c")

print("PASS Step 10B4 production hooks, native lifecycle/reuse, and Step 10C boundary")
