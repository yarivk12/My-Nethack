"""Production-path and phase-boundary gate for Step 10B3-2."""
from pathlib import Path
import re

repo = Path(__file__).resolve().parents[1]
read = lambda p: (repo / p).read_text(encoding="utf8")
artilist = read("include/artilist.h")
artifact_h = read("include/artifact.h")
artifact = read("src/artifact.c")
makemon = read("src/makemon.c")
quest = read("src/questpgr.c")
muse = read("src/muse.c")
mondata = read("src/mondata.c")
do_wear = read("src/do_wear.c")
worn = read("src/worn.c")
weapon = read("src/weapon.c")
hack = read("include/hack.h")
objects = read("include/objects.h")

names = [
    "The First Key of Neutrality", "The Second Key of Neutrality",
    "The Third Key of Neutrality", "Infinity's Mirrored Arc",
    "The Staff of Twelve Mirrors", "The Sansara Mirror", "Mirror Brand",
    "Soulmirror",
]
positions = [artilist.index('A("' + name + '"') for name in names]
assert positions == sorted(positions) and len(set(positions)) == 8
tail = artilist[positions[0]:artilist.index("A(0, 0, 0, 0, 0")]
assert sum(tail.count('A("' + name + '"') for name in names) == 8
assert artilist.index("The Third Key of Neutrality") < artilist.index(
    "A(0, 0, 0, 0, 0")

for key, bn in (("The First Key of Neutrality", "FIRST_KEY_OF_NEUTRALITY"),
                ("The Second Key of Neutrality", "SECOND_KEY_OF_NEUTRALITY"),
                ("The Third Key of Neutrality", "THIRD_KEY_OF_NEUTRALITY")):
    start = artilist.index('A("' + key + '"')
    end = artilist.index(bn + "),", start) + len(bn)
    record = artilist[start:end]
    assert "UNIVERSAL_KEY" in record
    assert "SPFX_NOGEN | SPFX_RESTR" in record
    assert "NO_ATTK" in record and "NO_DFNS" in record
    assert "A_NEUTRAL" in record and "1500L" in record

records = {
    "Infinity's Mirrored Arc": ("DOUBLE_LIGHTSABER", "SPFX_REFLECT",
                                 "ALTMODE", "3000L", "INFINITY_S_MIRRORED_ARC"),
    "The Staff of Twelve Mirrors": ("KHAKKHARA", "SPFX_REFLECT",
                                     "SPFX_DISPL", "PHYS(5,6)",
                                     "STAFF_OF_TWELVE_MIRRORS"),
    "The Sansara Mirror": ("MIRRORBLADE", "GOLD", "SPFX_HSPDAM",
                           "PHYS(8,8)", "SANSARA_MIRROR"),
    "Mirror Brand": ("LONG_SWORD", "SILVER", "SPFX_DALIGN", "STUN(1,0)",
                     "MIRROR_BRAND"),
    "Soulmirror": ("PLATE_MAIL", "SPFX_REFLECT", "9000L", "A_NEUTRAL",
                    "SOULMIRROR"),
}
for name, values in records.items():
    start = artilist.index('A("' + name + '"')
    end = artilist.index(values[-1] + "),", start) + len(values[-1])
    record = artilist[start:end]
    for fragment in values[:-1]:
        if fragment in ("GOLD", "SILVER") and name in (
                "The Sansara Mirror", "Mirror Brand"):
            continue
        assert fragment in record, (name, fragment)

assert "ART_FIRST_KEY_OF_NEUTRALITY" in read("src/makemon.c")
assert artilist.index("Soulmirror") < artilist.index("The Necronomicon")
assert artilist.index("The Necronomicon") < artilist.index("The Silver Key")
assert artilist.index("The Silver Key") < artilist.index("A(0, 0, 0, 0, 0")
assert "SPFX_DISPL" in artifact_h
assert "ALTMODE" in artifact_h
assert "AFTER_LAST_ARTIFACT" in hack
assert "artifact_id_must_fit_saved_char" in hack
assert "NUM_OBJECTS" not in objects[objects.index("Step 10B3-1 append-only"):
                                    objects.index("#ifdef") if "#ifdef" in objects else len(objects)]

center = makemon[makemon.index("PM_CENTER_OF_ALL"):]
assert "m_give_step10b_key(mtmp, ART_FIRST_KEY_OF_NEUTRALITY)" in center[:3500]
key_helper = makemon[makemon.index("staticfn void\nm_give_step10b_key") :]
assert "mksobj(UNIVERSAL_KEY" in key_helper[:1200]
assert "oname(" in key_helper[:1200]
assert "exist_artifact" in key_helper[:1200]

assert "step10b_alhoon_key_choice" in quest
alhoon = makemon[makemon.index("staticfn void\nm_initinv("):]
assert "PM_ALHOON" in alhoon[:5000]
assert "ART_SECOND_KEY_OF_NEUTRALITY" in alhoon[:5000]
assert "ART_THIRD_KEY_OF_NEUTRALITY" in alhoon[:5000]
assert alhoon.index("ART_SECOND_KEY_OF_NEUTRALITY") < alhoon.index(
    "ART_THIRD_KEY_OF_NEUTRALITY")
assert "UNIVERSAL_KEY" in alhoon[:5000]

assert "obranch_material = mod ? SILVER" in artifact
assert "obranch_material = mod ? GOLD" in artifact
assert "obranch_material = mod ? MITHRIL" in artifact
assert "artifact_exists" in artifact
assert "usecount" in artifact[artifact.index("ALTMODE"):]
assert "ART_INFINITY_S_MIRRORED_ARC" in artifact
assert "SPFX_DISPL" in artifact[artifact.index("set_artifact_intrinsic"):
                                artifact.index("/* touch_artifact")]
assert "W_ARMOR" in artifact[artifact.index("set_artifact_intrinsic"):
                              artifact.index("/* touch_artifact")]

assert "arti_reflects(orefl)" in muse
assert "SPFX_DISPL" in mondata
assert "ART_SOULMIRROR" in do_wear or "artifact_arm_bonus" in do_wear
assert "artifact_arm_bonus" in worn
assert "ART_MIRROR_BRAND" in artifact
assert "dnum" in artifact and "dsize" in artifact
assert "rnd(4)" in artifact  # Magicbane's original bounded dice remain
assert "PHYS(8,8)" in artilist and "PHYS(5,6)" in artilist
assert "DOUBLE_LIGHTSABER" in objects
assert "bimanual(otmp)" in read("include/obj.h")

for protected in ("README.md", "dat/dungeon.lua"):
    assert "B3-2" not in read(protected)
print("PASS Step 10B3-2 artifact records, integration paths, and boundaries")
