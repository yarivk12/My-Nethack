"""Whole-Step-10B cross-phase ownership and Step 10C boundary checks."""
from pathlib import Path
import re
import subprocess


repo = Path(__file__).resolve().parents[1]
read = lambda path: (repo / path).read_text(encoding="utf8")

global_h = read("include/global.h")
you_h = read("include/you.h")
rm_h = read("include/rm.h")
objects = read("include/objects.h")
artifacts = read("include/artilist.h")
monsters = read("include/monsters.h")
o_init = read("src/o_init.c")
trap = read("src/trap.c")
read_c = read("src/read.c")
spell = read("src/spell.c")
mcastu = read("src/mcastu.c")
artifact = read("src/artifact.c")
makemon = read("src/makemon.c")
mon = read("src/mon.c")
save = read("src/save.c")
restore = read("src/restore.c")
bones = read("src/bones.c")

# Final append-only identity blocks and the one Step 10 save epoch.
monster_tail = monsters[monsters.index("/* Step 10B append-only branch content"):
                        monsters.index("/*\n     * mons_init()")]
object_tail = objects[objects.index("/* Step 10B3-1 append-only object extension"):
                      objects.index("#if defined(OBJECTS_DESCR_INIT)",
                                    objects.index("/* Step 10B3-1 append-only object extension"))]
artifact_tail = artifacts[artifacts.index("/* Step 10B3-2: neutral keys"):
                          artifacts.index("A(0, 0, 0, 0, 0")]
assert monster_tail.count("    MON(") == 73
assert object_tail.count("OBJECT(OBJ(") == 21
assert artifact_tail.count('A("') == 10
assert "#define EDITLEVEL 5" in read("include/patchlevel.h")
assert "#define MAXDUNGEON 18" in global_h
assert "FIRST_STEP10B_OBJECT" in o_init
assert re.search(r"svb\.bases\[MAXOCLASSES\]\s*=\s*"
                 r"svb\.bases\[MAXOCLASSES \+ 1\]\s*"
                 r"= FIRST_STEP10B_OBJECT", o_init)

# B1 persistence remains on native codecs.  B4 context is deliberately
# transient and must never become saved state before Step 10C identity wiring.
assert "sum_entered" in you_h
assert "lethe" in rm_h
assert "Sfo_levelflags" in save and "Sfi_levelflags" in restore
assert "SF_C(struct, you)" in read("include/sfmacros.h")
assert "step10b_level_context" not in you_h
assert "step10b_level_context" not in rm_h
assert "step10b_level_context" not in save
assert "step10b_level_context" not in restore

# B3 objects/artifacts and B4 Lethe behavior meet at the native object state:
# artifacts and the Book survive, water becomes amnesia, existing amnesia is
# stable, and transformations refresh weight without introducing a side codec.
lethe_type = trap[trap.index("step10b_lethe_otyp("):
                  trap.index("step10b_lethe_marker_spe(")]
for fragment in (
    "if (artifact || otyp == SPE_BOOK_OF_THE_DEAD)",
    "if (otyp == POT_WATER)",
    "return POT_AMNESIA",
    "if (otyp == POT_AMNESIA)",
):
    assert fragment in lethe_type, fragment
lethe_damage = trap[trap.index("step10b_lethe_damage(struct obj"):
                     trap.index("water_damage(", trap.index(
                         "step10b_lethe_damage(struct obj"))]
assert "obj->oartifact != 0" in lethe_damage
assert "obj->owt = weight(obj)" in lethe_damage
for field in ("obranch_material", "obranch_size", "obranch_props"):
    assert field in read("include/obj.h")
assert "SF_C(struct, obj)" in read("include/sfmacros.h")
assert "Sfo_obj(nhfp, otmp, \"obj\")" in save
assert "Sfi_obj(nhfp, otmp, \"obj\")" in restore

# The ordinary secrets spellbook cannot enter the Necronomicon menu, while
# the artifact does.  Step 10C-D supplies only validated discovered travel.
assert "if (is_art(scroll, ART_NECRONOMICON))" in read_c
assert "if (booktype == SPE_SECRETS)" in spell
assert "The ragged pages hint at secrets beyond mortal spellcraft." in spell
silver_invoke = artifact[artifact.index("invoke_silver_key_portal(struct"):
                           artifact.index("invoke_create_portal", artifact.index(
                               "invoke_silver_key_portal(struct"))]
assert "step10c_silver_key_domain" in silver_invoke
assert "silver_key_destination_valid" in silver_invoke
assert "svl.level_info[ledger].flags & VISITED" in silver_invoke
assert "goto_level(&target" in silver_invoke
assert "find no door it can open" in silver_invoke

# B2 equipment/casting and B4 spell context share the real production paths.
for fragment in (
    "PM_PLUMACH_RILMANI", "PM_AMM_KAMEREL", "PM_ARA_KAMEREL",
    "PM_WITCH_S_FAMILIAR", "PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES",
    "PM_CENTER_OF_ALL",
):
    assert fragment in makemon, fragment
assert "step10b_mon_spell_fumble_threshold(" in mcastu
assert "step10b_mon_spell_always_fumbles(context)" in mcastu
assert "enum step10b_level_context context = step10c_level_context(&u.uz)" in mcastu

# Native lifecycle owners remain connected for familiars, artifacts, bones,
# shopkeepers, and priests.  The focused/runtime gates exercise their state.
for fragment in ("witch_familiar", "PM_WITCH_S_FAMILIAR"):
    assert fragment in makemon + mon, fragment
for fragment in ("save_artifacts(nhfp)", "restore_artifacts(nhfp)"):
    assert fragment in save + restore, fragment
assert "ONAME_BONES" in bones
assert "Sfo_eshk" in save and "Sfi_eshk" in restore
assert "Sfo_epri" in save and "Sfi_epri" in restore

# B5 itself must not alter the README.  Step 10C-B owns the authorized
# dungeon/topology/scheduler changes; Step 10C-C now owns only the audited
# Outlands generator definitions and post-load bridge.
for protected in ("README.md",):
    assert subprocess.run(["git", "diff", "--quiet", "--", protected],
                          cwd=repo).returncode == 0, protected
assert "DL111" not in read("dat/dungeon.lua")
assert "step10c_level_context" in read("src/dungeon.c")
generator_names = (
    "place_neutral_features", "mkkamereltowers", "mkminorspire",
    "mkfishingvillage", "mkwell", "mkpluhomestead", "mkpluvillage",
    "mkferrutower", "mkinvertzigg", "mkneuriver", "neuliquify",
)
production_c = "\n".join(path.read_text(encoding="utf8", errors="replace")
                         for path in (repo / "src").glob("*.c"))
for name in generator_names:
    definitions = re.findall(
        r"\n(?:void|boolean)\s*\n" + re.escape(name) + r"\s*\(",
        production_c,
    )
    assert len(definitions) == 1, (name, len(definitions))
assert "mkferrufort(" not in production_c

untracked = subprocess.check_output(
    ["git", "ls-files", "--others", "--exclude-standard", "-z"],
    cwd=repo).decode("utf8").split("\0")
untracked = [path for path in untracked if path]
step10c_a_resources = {
    "dat/" + name + ".lua" for name in (
        "neulev", "gatetwn", "out1", "out2", "out3", "out4", "spire",
        "sumall", "leth-a-1", "leth-a-2", "lethe-b", "leth-c-1",
        "leth-c-2", "leth-d-1", "leth-d-2", "lethe-e", "lethe-f",
        "lethe-g", "lethe-z", "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c",
        "nkai-z", "rlyeh", "lbyrnth",
    )
}
assert all(path in {"doc/step10.md", "doc/step10-playtest.md"}
           or path.startswith("test/")
           or path.startswith("_qa/")
           or path in step10c_a_resources
           for path in untracked), untracked

print("PASS Step 10B5 cross-phase ownership, lifecycle, isolation, and Step 10C boundary")
