"""Cross-B3 integration and phase-boundary checks."""
from pathlib import Path
import subprocess


repo = Path(__file__).resolve().parents[1]
read = lambda path: (repo / path).read_text(encoding="utf8")

objects = read("include/objects.h")
artifacts = read("include/artilist.h")
monsters = read("include/monsters.h")
makemon = read("src/makemon.c")
mon = read("src/mon.c")
mkobj = read("src/mkobj.c")
artifact = read("src/artifact.c")
obj = read("include/obj.h")
save = read("src/save.c")
restore = read("src/restore.c")
bones = read("src/bones.c")


object_start = objects.index("/* Step 10B3-1 append-only object extension")
object_tail = objects[object_start:
                     objects.index("#if defined(OBJECTS_DESCR_INIT)", object_start)]
assert object_tail.count("OBJECT(OBJ(") == 21
assert "MARKER(FIRST_STEP10B_OBJECT, SICKLE)" in object_tail
assert "MARKER(LAST_STEP10B_OBJECT, LIFELESS_DOLL)" in object_tail
assert all(name in object_tail for name in (
    "SICKLE", "SCYTHE", "MIRRORBLADE", "KAMEREL_VAJRA", "VIPERWHIP",
    "RAKUYO", "KHAKKHARA", "ROUNDSHIELD", "WITCH_HAT",
    "WHITE_FACELESS_ROBE", "BLACK_FACELESS_ROBE",
    "SMOKY_VIOLET_FACELESS_ROBE", "UNIVERSAL_KEY", "TORCH",
    "SHADOWLANDER_S_TORCH", "DOUBLE_LIGHTSABER", "EYEBALL",
    "POT_AMNESIA", "POT_SPACE_MEAD", "SPE_SECRETS", "LIFELESS_DOLL"))

artifact_tail = artifacts[artifacts.index("/* Step 10B3-2: neutral keys"):
                          artifacts.index("A(0, 0, 0, 0, 0")]
assert artifact_tail.count('A("') == 10
assert all(name in artifact_tail for name in (
    "The First Key of Neutrality", "The Second Key of Neutrality",
    "The Third Key of Neutrality", "Infinity's Mirrored Arc",
    "The Staff of Twelve Mirrors", "The Sansara Mirror", "Mirror Brand",
    "Soulmirror", "The Necronomicon", "The Silver Key"))

monster_tail = monsters[monsters.index("/* Step 10B append-only branch content"):
                       monsters.index("/*\n     * mons_init()")]
assert monster_tail.count("    MON(") == 73

# These are the B3-dependent birth and death paths that must remain connected
# after all appended object declarations are present.
plumach = makemon[makemon.index("if (mm == PM_PLUMACH_RILMANI)"):]
assert "SICKLE" in plumach and "SCYTHE" in plumach
amm = makemon[makemon.index("if (mm == PM_AMM_KAMEREL)"):]
assert "mksobj(MIRRORBLADE, TRUE, FALSE)" in amm
assert "mksobj(ROUNDSHIELD, TRUE, FALSE)" in amm
ara = makemon[makemon.index("if (mm == PM_ARA_KAMEREL)"):]
assert "mksobj(KAMEREL_VAJRA, FALSE, FALSE)" in ara
ziggurat = makemon[makemon.index(
    "if (mm == PM_SHATTERED_ZIGGURAT_CULTIST)"):]
for fragment in ("mksobj(TORCH, FALSE, FALSE)",
                 "mksobj(SHADOWLANDER_S_TORCH, FALSE, FALSE)",
                 "PM_SHATTERED_ZIGGURAT_WIZARD"):
    assert fragment in ziggurat, fragment
illurien = makemon[makemon.index(
    "if (mm == PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES)"):]
assert "mksobj(SPE_SECRETS, TRUE, FALSE)" in illurien
center = makemon[makemon.index("if (mm == PM_CENTER_OF_ALL)"):]
assert "m_give_step10b_key(mtmp, ART_FIRST_KEY_OF_NEUTRALITY)" in center
for fragment in (
    "mndx == PM_LIVING_DOLL", "mksobj_at(LIFELESS_DOLL",
    "mndx == PM_PARASITIZED_DOLL", "mksobj_at(EYEBALL",
    "set_corpsenm(doll, mndx)"):
    assert fragment in mon, fragment

# Representative metadata and lifecycle must stay on the existing saved
# object/artifact codecs; B3 adds no parallel persistence path.
for fragment in ("obranch_material", "obranch_size", "obranch_props",
                 "save_artifacts(nhfp)", "restore_artifacts(nhfp)",
                 "artifact_exists(otmp, safe_oname(otmp), TRUE",
                 "ONAME_BONES"):
    assert fragment in (obj + artifact + save + restore + bones), fragment
assert "case SICKLE:" in mkobj and "case TORCH:" in mkobj
assert "OBP_DEEP" in obj and "OBP_CONCORDANT" in obj

# Current serialized identity/capacity and the phase boundary are fixed by
# the focused C gates; this cross-check makes the closeout runner fail fast if
# the production tables drift while those gates are being composed.
assert "#define EDITLEVEL 7" in read("include/patchlevel.h")
assert "#define MAXDUNGEON 18" in read("include/global.h")
assert subprocess.run(["git", "diff", "--quiet", "--", "README.md"],
                      cwd=repo).returncode == 0
# Later Step 10C topology is allowed after this historical B3 closeout; retain
# the fixed-parent prohibition here and leave its full contract to C-B gates.
for path in ("dat/dungeon.lua", "src/dungeon.c"):
    text = read(path)
    assert "DL111" not in text

print("PASS Step 10B3-4 cross-B3 source integration and phase boundaries")
