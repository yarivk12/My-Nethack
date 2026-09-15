"""Production-path, scope, lifecycle, and tile gate for Step 10B2-4."""
from pathlib import Path
import re

repo = Path(__file__).resolve().parents[1]
read = lambda p: (repo / p).read_text(encoding="utf8")
monsters = read("include/monsters.h")
mondata = read("include/mondata.h")
quest = read("src/questpgr.c")
makemon = read("src/makemon.c")
mthrowu = read("src/mthrowu.c")
mhitu = read("src/mhitu.c")
mhitm = read("src/mhitm.c")
monmove = read("src/monmove.c")
mon = read("src/mon.c")
weapon = read("src/weapon.c")
muse = read("src/muse.c")
tiles = read("win/share/monsters.txt")
restore = read("src/restore.c")

tokens = ["ALHOON", "CENTER_OF_ALL", "FATHER_DAGON", "MOTHER_HYDRA",
          "GREAT_CTHULHU"]
positions = [re.search(r"\b" + token + r"\),", monsters).start()
             for token in tokens]
assert positions == sorted(positions) and len(set(positions)) == 5
familiar = re.search(r'MON\(NAM\("witch\'s familiar"\)[\s\S]*?WITCH_S_FAMILIAR\),',
                     monsters)[0]
assert positions[-1] < monsters.index(familiar)
for fragment in (
    "S_RODENT", "LVL(5, 6, 0, 0, 0)", "G_NOGEN",
    "ATTK(AT_BITE, AD_VAMP, 1, 3)", "ATTK(AT_MAGC, AD_SPEL, 0, 6)",
    "SIZ(20, 50, MS_SQEEK, MZ_TINY)", "M1_ANIMAL | M1_OMNIVORE",
    "M2_FEMALE | M2_NOPOLY", "M3_INFRAVISIBLE", "WITCH_S_FAMILIAR",
):
    assert fragment in familiar

for rank in ("PM_APPRENTICE_WITCH", "PM_WITCH", "PM_COVEN_LEADER"):
    assert rank in makemon
assert "step10b_create_witch_familiar(mtmp)" in makemon
needs = re.search(r"(?m)^boolean\nstep10b_witch_needs_familiar[\s\S]*?^\}",
                  makemon)[0]
assert "PM_WITCH_S_FAMILIAR" in needs and "mspare1" in needs
assert "step10b_witch_needs_familiar(mtmp)" in read("src/mcastu.c")
assert "PM_WITCH_S_FAMILIAR" in read("src/mcastu.c")
resummon = re.search(r"step10b_witch_needs_familiar\(mtmp\)[\s\S]*?\n    \}",
                     monmove)[0]
assert "PM_COVEN_LEADER" in resummon and "rn2(4)" in resummon
assert "PM_WITCH" in resummon and "rn2(20)" in resummon
assert "PM_APPRENTICE_WITCH" not in resummon
familiar_death = re.search(
    r"PM_WITCH_S_FAMILIAR[\s\S]*?step10b_familiar_died\([^;]+;",
    mon)[0]
assert "mspare1" in familiar_death
assert "PM_COVEN_LEADER" in mon and "PM_WITCH" in mon
assert "PM_WITCH_S_FAMILIAR" in restore
assert "lookup_id_mapping((unsigned) mtmp->mspare1" in restore

assert "PM_ALHOON" not in re.search(
    r"#define is_mind_flayer[\s\S]*?\n\n", mondata)[0]
assert "case AD_DRIN: mhitm_ad_drin" in read("src/uhitm.c")
assert "mith_internal_projectile(mtmp, &gy.youmonst" in mhitu
assert "mith_internal_projectile(magr, mdef, mattk)" in mhitm
load = mthrowu[mthrowu.index("mith_internal_projectile("):]
assert "mksobj(LOADSTONE" in load and "curse(stone)" in load
assert "m_throw(" in load and ", 8, stone)" in load

candidate = re.search(r"(?m)^int\nstep10b_center_candidate[\s\S]*?^\}", quest)[0]
assert "STEP10B_CENTER_NEUTRAL" in candidate
assert "STEP10B_CENTER_LOST_CITIES" in candidate
assert "roll == 0" in candidate and "G_GONE" in candidate
for path in ("src/makemon.c", "src/mklev.c", "src/monmove.c"):
    assert "step10b_center_candidate(" not in read(path)
assert makemon.index("ptr == &mons[PM_CENTER_OF_ALL]") < makemon.index(
    "always_hostile(ptr)")
assert "is_shadow(ptr) || mndx == PM_CENTER_OF_ALL" in makemon
assert "PM_CENTER_OF_ALL" in muse and "step10b_innate_reflection" in muse
assert "PM_CENTER_OF_ALL" in read("src/mondata.c")

assert "ptr == &mons[PM_FATHER_DAGON]" in weapon
assert "ptr == &mons[PM_MOTHER_HYDRA]" in weapon and "slot == 5" in weapon
soul = re.search(r"(?m)^staticfn void\nmith_deep_soul[\s\S]*?^\}", mon)[0]
assert "PM_FATHER_DAGON" in soul and "PM_MOTHER_HYDRA" in soul
assert soul.count("PM_DEEPEST_ONE") == 2

psychic = re.search(
    r"(?m)^staticfn void\nstep10b_cthulhu_psychic\([\s\S]*?^\}",
    monmove)[0]
assert "d(5, 15)" in psychic and "m2->mconf = 1" in psychic
assert "BOLT_LIM" not in psychic and "make_stunned" in psychic
assert "make_confused" in psychic and "monkilled(m2, \"\", AD_DRIN)" in psychic
dispatch = monmove[monmove.index("/* the watch will look"):]
assert dispatch.index("PM_GREAT_CTHULHU") < dispatch.index("is_mind_flayer")

hero_wisd = mhitu[mhitu.index("mattk->adtyp == AD_WISD"):]
assert hero_wisd.index("mtmp->mspec_used = 4") < hero_wisd.index(
    "if ((mattk->adtyp == AD_LUCK")
monster_wisd = mhitm[mhitm.index("mattk->adtyp == AD_WISD"):]
assert monster_wisd.index("mdef->mconf = 1") < monster_wisd.index("magr->mcan")
death = mon[mon.index("step10b_cthulhu_death_effect("):
            mon.index("/* The Alabaster death", mon.index(
                "step10b_cthulhu_death_effect("))]
assert "explode(" in death and "create_gas_cloud(" in death
assert "makemon(" not in death
death_call = mon[mon.index("mndx == PM_GREAT_CTHULHU"):]
assert death_call.index("MITH_CTHULHU_DEATH_FIRED") < death_call.index(
    "step10b_cthulhu_death_effect(deathx, deathy)")

labels = re.findall(r"^# tile (\d+) \((.*?),(male|female)\)$", tiles, re.M)
tail = labels[-12:]
assert [int(row[0]) for row in tail] == list(range(1016, 1028))
assert [row[1] for row in tail[::2]] == [
    "alhoon", "Center of All", "Father Dagon", "Mother Hydra",
    "Great Cthulhu", "witch's familiar"]
assert "# tile 1028 (invisible monster, nogender)" in tiles

for protected in ("dat/dungeon.lua", "README.md"):
    text = read(protected)
    assert "CENTER_OF_ALL" not in text and "GREAT_CTHULHU" not in text
print("PASS Step 10B2-4 production paths/scope/lifecycle/tiles")
