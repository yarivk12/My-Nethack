"""Narrow source contract for Step 10B2-1 mechanics and non-activation."""
from pathlib import Path

repo = Path(__file__).resolve().parents[1]
monsters = (repo / "include/monsters.h").read_text(encoding="utf8")
quest = (repo / "src/questpgr.c").read_text(encoding="utf8")
mcast = (repo / "src/mcastu.c").read_text(encoding="utf8")
makemon = (repo / "src/makemon.c").read_text(encoding="utf8")
mondata = (repo / "src/mondata.c").read_text(encoding="utf8")
muse = (repo / "src/muse.c").read_text(encoding="utf8")
worn = (repo / "src/worn.c").read_text(encoding="utf8")
weapon = (repo / "src/weapon.c").read_text(encoding="utf8")
uhitm = (repo / "src/uhitm.c").read_text(encoding="utf8")
mthrowu = (repo / "src/mthrowu.c").read_text(encoding="utf8")
mhitu = (repo / "src/mhitu.c").read_text(encoding="utf8")
mhitm = (repo / "src/mhitm.c").read_text(encoding="utf8")

for name in ("plumach rilmani", "ferrumach rilmani", "cuprilach rilmani",
             "argenach rilmani", "aurumach rilmani", "amm kamerel",
             "hudor kamerel", "sharab kamerel", "ara kamerel", "argentum golem"):
    assert f'NAM("{name}")' in monsters
for fn in ("step10b_neutral_montype", "step10b_sum_montype",
           "step10b_neutral_squad"):
    assert fn in quest
assert "PM_SHATTERED_ZIGGURAT_CULTIST" in quest
assert "PM_CUPRILACH_RILMANI" in weapon
assert "PM_AURUMACH_RILMANI" in mcast and "mspec_used = 0" in mcast
assert "PM_ARGENTUM_GOLEM" in makemon and "SILVER_ARROW" in makemon
assert "otmp->obranch_material = SILVER" in makemon
for exact in ("HALBERD : BATTLE_AXE", "SHORT_SWORD", "BUCKLER",
              "BROADSWORD", "SHIELD_OF_REFLECTION", "PLATE_MAIL",
              "19L + rnd(8)"):
    assert exact in makemon
for dependency in ("SICKLE", "SCYTHE", "MIRRORBLADE", "ROUNDSHIELD",
                   "KAMEREL_VAJRA"):
    assert dependency in makemon
for completed in ("PM_PLUMACH_RILMANI", "PM_AMM_KAMEREL",
                  "PM_ARA_KAMEREL"):
    assert f"if (mm == {completed})" in makemon
assert "PM_ARGENTUM_GOLEM" in worn and "PM_ARA_KAMEREL" in worn
assert "PM_AURUMACH_RILMANI" in mondata and "PM_ARA_KAMEREL" in mondata
for pm in ("PM_AMM_KAMEREL", "PM_HUDOR_KAMEREL", "PM_SHARAB_KAMEREL",
           "PM_ARA_KAMEREL", "PM_ARGENTUM_GOLEM"):
    assert pm in muse
assert "case AD_WET:" in uhitm
assert "water_damage(gi.invent" in uhitm and "water_damage(mdef->minvent" in uhitm
assert "mith_silver_arrow(struct monst *magr, struct monst *mdef)" in mthrowu
assert "m_carrying(magr, SILVER_ARROW)" in mthrowu and "monshoot(magr, arrow" in mthrowu
assert "case AT_ARRW:" in mhitu and "mith_internal_projectile(mtmp, &gy.youmonst" in mhitu
assert "case AT_ARRW:" in mhitm and "mith_internal_projectile(magr, mdef, mattk)" in mhitm
assert "mattk->adtyp == AD_SLVR" in mthrowu and "mith_silver_arrow(magr, mdef)" in mthrowu
assert "OBP_CONCORDANT" in weapon and "sgn(target->data->maligntyp)" in weapon
assert "step10b_neutral_montype(" not in (repo / "src/makemon.c").read_text(encoding="utf8")
assert "step10b_neutral_squad(" not in (repo / "src/mkroom.c").read_text(encoding="utf8")
print("PASS Step 10B2-1 scoped mechanics and selectors are present but not globally active")
