"""Production integration and phase-boundary checks for Step 10B3-1."""
from pathlib import Path
import re

repo = Path(__file__).resolve().parents[1]
read = lambda p: (repo / p).read_text(encoding="utf8")
objects = read("include/objects.h")
oinit = read("src/o_init.c")
makemon = read("src/makemon.c")
mkobj = read("src/mkobj.c")
mon = read("src/mon.c")
potion = read("src/potion.c")
spell = read("src/spell.c")
apply = read("src/apply.c")
lock = read("src/lock.c")
objh = read("include/obj.h")

tokens = [
    "SICKLE", "SCYTHE", "MIRRORBLADE", "KAMEREL_VAJRA", "VIPERWHIP",
    "RAKUYO", "KHAKKHARA", "ROUNDSHIELD", "WITCH_HAT",
    "WHITE_FACELESS_ROBE", "BLACK_FACELESS_ROBE",
    "SMOKY_VIOLET_FACELESS_ROBE", "UNIVERSAL_KEY", "TORCH",
    "SHADOWLANDER_S_TORCH", "DOUBLE_LIGHTSABER", "EYEBALL",
    "POT_AMNESIA", "POT_SPACE_MEAD", "SPE_SECRETS", "LIFELESS_DOLL",
]
tail = objects[objects.index("/* weapons */", objects.index(
    "Step 10B3-1 append-only object extension")):]
positions = [tail.index(token) for token in tokens]
assert positions == sorted(positions) and len(set(positions)) == 21
assert "MARKER(LAST_STEP10B_OBJECT, LIFELESS_DOLL)" in tail
assert "MARKER(FIRST_STEP10B_OBJECT, SICKLE)" in tail
assert "while (first < FIRST_STEP10B_OBJECT)" in oinit
assert "NUM_OBJECTS" not in re.search(
    r"(?m)^boolean\nstep10b_extension_otyp[\s\S]*?^\}", oinit)[0]

for token in tokens:
    pos = tail.index(token)
    declaration = tail[max(0, pos - 420):pos + 80]
    assert re.search(r"(?:CLASS|, )0,", declaration), token

for token in ("SICKLE", "SCYTHE", "MIRRORBLADE", "ROUNDSHIELD",
              "KAMEREL_VAJRA", "BLACK_DRESS", "WITCH_HAT", "TORCH",
              "SHADOWLANDER_S_TORCH", "WHITE_FACELESS_ROBE",
              "BLACK_FACELESS_ROBE", "SMOKY_VIOLET_FACELESS_ROBE",
              "SPE_SECRETS", "BARDICHE", "WAR_HAT", "ARCHAIC_GAUNTLETS"):
    assert token in makemon

plumach = makemon[makemon.index("mm == PM_PLUMACH_RILMANI"):]
assert plumach.index("rn2(3)") < plumach.index("SICKLE") < plumach.index("SCYTHE")
amm = makemon[makemon.index("mm == PM_AMM_KAMEREL"):]
assert "MIRRORBLADE" in amm[:9000] and "ROUNDSHIELD" in amm[:9000]
assert "mksobj(MIRROR, FALSE, FALSE)" in amm[:9000]
ara = makemon[makemon.index("mm == PM_ARA_KAMEREL"):]
assert "KAMEREL_VAJRA" in ara[:1800] and "MIRROR" in ara[:1800]
assert "mksobj(KAMEREL_VAJRA, FALSE, FALSE)" in ara[:1800]

assert "mndx == PM_LIVING_DOLL" in mon and "LIFELESS_DOLL" in mon
parasitized = mon[mon.index("mndx == PM_PARASITIZED_DOLL"):]
assert "EYEBALL" in parasitized[:2200] and "LIFELESS_DOLL" in parasitized[:2200]
assert "POT_AMNESIA" in potion
assert "step10b_forget_memories(" in potion and "step10b_amnesia_percent" in potion
assert "POT_SPACE_MEAD" in potion
assert "booktype == SPE_SECRETS" in spell
assert "case UNIVERSAL_KEY:" in apply
assert lock.count("case UNIVERSAL_KEY:") >= 2
assert "TORCH" in re.search(r"#define ignitable[\s\S]*?\n\n", objh)[0]
assert "SHADOWLANDER_S_TORCH" in re.search(r"#define ignitable[\s\S]*?\n\n", objh)[0]
assert "OBP_DEEP" in objh
for token in ("TORCH", "SHADOWLANDER_S_TORCH"):
    ziggurat = makemon[makemon.index("mm == PM_SHATTERED_ZIGGURAT_CULTIST"):]
    assert "mksobj(" + token + ", FALSE, FALSE)" in ziggurat[:5000]
assert "mksobj(SICKLE, FALSE, FALSE)" in makemon[
    makemon.index("mm == PM_HMNYW_PHARAOH"):][:1800]

for token, size in {
    "SICKLE": "MZ_SMALL", "MIRRORBLADE": "MZ_SMALL",
    "DOUBLE_LIGHTSABER": "MZ_SMALL", "TORCH": "MZ_SMALL",
    "SHADOWLANDER_S_TORCH": "MZ_SMALL", "SCYTHE": "MZ_HUGE",
    "KHAKKHARA": "MZ_HUGE", "LIFELESS_DOLL": "MZ_HUGE",
    "ROUNDSHIELD": "MZ_LARGE", "SPE_SECRETS": "MZ_LARGE",
    "UNIVERSAL_KEY": "MZ_TINY", "EYEBALL": "MZ_TINY",
}.items():
    assert re.search(r"case " + token + r":\n(?:\s*case [A-Z0-9_]+:\n)*"
                     r"\s*otmp->obranch_size = " + size + r" \+ 1;",
                     mkobj), (token, size)
for token in ("TORCH", "SHADOWLANDER_S_TORCH"):
    init_case = mkobj[mkobj.index("case " + token + ":"):]
    assert re.search(r"if \(init\)\n\s*otmp->age = rn1\(500, 1000\);",
                     init_case[:260]), token

assert "ART_FIRST_KEY_OF_NEUTRALITY" in makemon
assert "ART_NECRONOMICON" not in spell
assert "UPGRADE_KIT" not in objects
assert "EDITLEVEL 8" in read("include/patchlevel.h")
for protected in ("README.md", "dat/dungeon.lua"):
    assert "STEP 10B3-1" not in read(protected)
print("PASS Step 10B3-1 production paths and phase boundaries")
