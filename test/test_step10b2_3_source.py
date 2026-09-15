"""Source, selector-fidelity, topology, and tile gate for Step 10B2-3."""
from pathlib import Path
import re

repo = Path(__file__).resolve().parents[1]
monsters = (repo / "include/monsters.h").read_text(encoding="utf8")
quest = (repo / "src/questpgr.c").read_text(encoding="utf8")
tiles = (repo / "win/share/monsters.txt").read_text(encoding="utf8")

tokens = [
    "SMALL_GOAT_SPAWN", "GOAT_SPAWN", "GIANT_GOAT_SPAWN", "BLESSED",
    "MOUTH_OF_THE_GOAT", "APPRENTICE_WITCH", "WITCH", "COVEN_LEADER",
    "THE_GOOD_NEIGHBOR", "HMNYW_PHARAOH", "MIGO_WORKER", "MIGO_SOLDIER",
    "MIGO_PHILOSOPHER", "MIGO_QUEEN", "BYAKHEE", "DARK_YOUNG",
    "DEEP_DWELLER", "DEMINYMPH", "GNOLL_GHOUL", "GUG",
    "ILLURIEN_OF_THE_MYRIAD_GLIMPSES", "NIGHTGAUNT", "OREAD",
    "MINOTAUR_PRIESTESS", "PRIEST_OF_AN_UNKNOWN_GOD", "SHOGGOTH",
    "STAR_SPAWN", "SHATTERED_ZIGGURAT_CULTIST",
    "SHATTERED_ZIGGURAT_KNIGHT", "SHATTERED_ZIGGURAT_WIZARD",
    "HUNTING_HORROR", "BLASPHEMOUS_LURKER",
]
positions = [re.search(r"\b" + token + r"\),", monsters).start()
             for token in tokens]
assert positions == sorted(positions) and len(set(positions)) == 32
assert "STEP10B_DEFER_SHATTERED_ZIGGURAT" not in quest
assert ": PM_SHATTERED_ZIGGURAT_CULTIST;" in quest

rlyeh = quest[quest.index("step10b_rlyeh_emit("):
              quest.index("/* special levels can include", quest.index("step10b_rlyeh_create("))]
assert rlyeh.index("int chance = d(1, 100)") < rlyeh.index("if (rn2(20))")
for exact in ("d(2, 3)", "d(2, 4)", "rnd(4)", "rn1(2, 1)",
              "rnd(3)", "rn1(2, 2)", "rnd(6)", "rn1(4, 3)"):
    assert exact in rlyeh
assert rlyeh.count("num >= 0") == 8
assert "G_GENOD" in rlyeh and "G_EXTINCT" not in rlyeh
assert "step10b_rlyeh_create(" not in (repo / "src/makemon.c").read_text(encoding="utf8")
assert "step10b_rlyeh_create(" not in (repo / "src/mklev.c").read_text(encoding="utf8")

mhitu = (repo / "src/mhitu.c").read_text(encoding="utf8")
mhitm = (repo / "src/mhitm.c").read_text(encoding="utf8")
uhitm = (repo / "src/uhitm.c").read_text(encoding="utf8")
mcastu = (repo / "src/mcastu.c").read_text(encoding="utf8")
makemon = (repo / "src/makemon.c").read_text(encoding="utf8")
assert "mattk->aatyp == AT_REACH2" in mhitu and "AT_REACH2 ? 2" in mhitm
assert "case AD_SHRD:" in uhitm and "erode_armor(mdef, ERODE_CORRODE)" in uhitm
assert "case AD_ILUR:" in mhitu and "step10b_forget_memories" in mhitu
assert "case AD_PSON:" in mcastu and "PM_STAR_SPAWN" in mcastu
assert "mndx == PM_HUNTING_HORROR" in makemon and "? 2 : rn2(5)" in makemon
assert "mattk->adtyp == AD_BLAS" in mhitu and "mattk->adtyp == AD_MIST" in mhitu
passive = mhitm[mhitm.index("passivemm("):]
assert "mdead | mhit" in passive and "!mdead" in passive
combined = "\n".join((mhitu, mhitm, uhitm, mcastu, makemon))
for forbidden in ("u.usanity", "u.uinsight", "u.umadness"):
    assert forbidden not in combined

labels = re.findall(r"^# tile (\d+) \((.*?),(male|female)\)$", tiles, re.M)
tail = [row for row in labels if 952 <= int(row[0]) <= 1015]
assert len(tail) == 64 and [int(row[0]) for row in tail] == list(range(952, 1016))
assert "# tile 1028 (invisible monster, nogender)" in tiles
assert "Step 10B2-3" in (repo / "doc/step10.md").read_text(encoding="utf8")
for protected in ("dat/dungeon.lua", "README.md"):
    assert "step10b_rlyeh_create" not in (repo / protected).read_text(encoding="utf8")
print("PASS Step 10B2-3 source/selectors/tiles")
