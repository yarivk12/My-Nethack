"""Production-path and phase-boundary gate for Step 10B3-3."""
from pathlib import Path
import re


repo = Path(__file__).resolve().parents[1]
read = lambda p: (repo / p).read_text(encoding="utf8")
artilist = read("include/artilist.h")
artifact_h = read("include/artifact.h")
artifact = read("src/artifact.c")
read_c = read("src/read.c")
allmain = read("src/allmain.c")
obj_h = read("include/obj.h")
extern_h = read("include/extern.h")
bones = read("src/bones.c")
save_c = read("src/save.c")
restore_c = read("src/restore.c")
hack_h = read("include/hack.h")

names = ["The Necronomicon", "The Silver Key"]
positions = [artilist.index('A("' + name + '"') for name in names]
assert positions == sorted(positions)
assert artilist.index("Soulmirror") < positions[0] < positions[1]
assert positions[1] < artilist.index("A(0, 0, 0, 0, 0")
assert artilist.count('A("The Necronomicon"') == 1
assert artilist.count('A("The Silver Key"') == 1

necro_record = artilist[positions[0]:positions[1]]
silver_record = artilist[positions[1]:artilist.index("A(0, 0, 0, 0, 0")]
for fragment in ("SPE_SECRETS", "SPFX_NOGEN | SPFX_RESTR",
                 "NECRONOMICON"):
    assert fragment in necro_record
for fragment in ("UNIVERSAL_KEY", "SPFX_NOGEN | SPFX_RESTR",
                 "SPFX_EREGEN", "SPFX_TCTRL", "SPFX_PCTRL",
                 "CREATE_PORTAL", "SILVER_KEY"):
    assert fragment in silver_record

assert "case ART_SILVER_KEY" in artifact
assert "obranch_material = mod ? SILVER" in artifact[
    artifact.index("case ART_SILVER_KEY"):]
assert "silver_key_destination_valid" in artifact
assert "silver_key_choose_destination" in artifact
silver_invoke = artifact[artifact.index("invoke_silver_key_portal(struct obj *obj)"):]
assert "step10c_silver_key_domain" in silver_invoke
assert "silver_key_choose_destination" in silver_invoke
assert "goto_level(&target" in silver_invoke
assert "ART_SILVER_KEY" in artifact[artifact.index("arti_invoke("):]
assert artifact.index("invoke_silver_key_portal(obj)") < artifact.index(
    "arti_invoke_cost(obj)", artifact.index("arti_invoke("))

assert "SPFX_PCTRL" in artifact_h
assert "EPolymorph_control" in artifact
assert "&EPolymorph_control, SPFX_PCTRL" in artifact
assert "EEnergy_regeneration" in artifact
assert "ETeleport_control" in artifact
assert "Energy_regeneration" in allmain

assert "is_art(scroll, ART_NECRONOMICON)" in read_c
assert "read_necronomicon(scroll)" in read_c
for operation in (
    "NECRONOMICON_SUMMON_BYAKHEE",
    "NECRONOMICON_SUMMON_NIGHTGAUNT",
    "NECRONOMICON_DETECT_MONSTERS",
    "NECRONOMICON_HEALTH_RECOVERY",
):
    assert operation in read_c
assert "PM_BYAKHEE" in read_c and "PM_NIGHTGAUNT" in read_c
assert "SPE_DETECT_MONSTERS" in read_c
assert "use_unicorn_horn" in read_c
assert "tamedog" in read_c
assert "discover_artifact(ART_NECRONOMICON)" in read_c
assert "fully_identify_obj(book)" in read_c
necro_reader = read_c[read_c.index("read_necronomicon(struct obj *book)"):
                      read_c.index("/* max spe is", read_c.index(
                          "read_necronomicon(struct obj *book)"))]
assert "set_occupation" not in necro_reader

# No donor persistent page/occult frameworks are introduced.
for forbidden in ("uread_necronomicon", "wardsknown", "sealsKnown",
                  "usanity", "uinsight", "umadness", "LAST_PAGE"):
    assert forbidden not in read_c
assert "obj_art_uses_ovar1" not in obj_h

# Native artifact arrays and the complete struct-obj codec own persistence;
# generic bones restoration re-registers or de-artifacts duplicate uniques.
assert "for (i = 0; i < (NROFARTIFACTS + 1); ++i)" in artifact
assert "for (i = 0; i < NROFARTIFACTS; ++i)" in artifact
assert "save_artifacts(nhfp)" in save_c
assert "restore_artifacts(nhfp)" in restore_c
assert "artifact_exists(otmp, safe_oname(otmp), TRUE" in bones
assert "ONAME_BONES" in bones
assert "char oartifact" in obj_h
assert "artifact_id_must_fit_saved_char" in hack_h

# The helper is topology-independent until Step 10C supplies real identities.
portal_core = artifact[artifact.index("silver_key_destination_valid"):
                       artifact.index("silver_key_choose_destination")]
for forbidden in ("neulev", "neutral_dungeon_number",
                  "lost_cities_dungeon_number", "dungeon.lua", "DL111"):
    assert forbidden not in portal_core
assert "extern boolean silver_key_destination_valid" in extern_h

for protected in ("README.md", "dat/dungeon.lua"):
    assert "B3-3" not in read(protected)

print("PASS Step 10B3-3 declarations, passive/read/portal paths, and phase boundaries")
