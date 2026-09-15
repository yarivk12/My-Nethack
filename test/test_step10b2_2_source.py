"""Static integration gate for the bounded Step 10B2-2 implementation."""
from pathlib import Path
import re
import subprocess

BASE = "48fe150af4a087fd2f4ff576b43c1c96cc08c6fe"
NEW = [
    "living doll", "living lectern", "parasitized doll",
    "bestial dervish", "ethereal dervish", "flashing lake",
    "frosted lake", "smoldering lake", "sparkling lake",
    "blood shower", "many-taloned thing", "deep blue cube",
    "pitch black cube", "prayerful thing", "hemorrhagic thing",
    "many-eyed seeker", "voice in the dark", "tiny being of light",
    "man-faced millipede", "mirrored moonflower", "crimson writher",
    "radiant pyramid", "Kuker", "lurking one",
]


def read(repo, name):
    return (repo / name).read_text(encoding="utf8")


if __name__ == "__main__":
    repo = Path(__file__).resolve().parents[1]
    monsters = read(repo, "include/monsters.h")
    start = monsters.index('MON(NAM("living doll")')
    end = monsters.index("    /*\n     * mons_init()", start)
    actual = re.findall(r'MON\(NAM\("([^"]+)"\)', monsters[start:end])
    assert actual[:len(NEW)] == NEW, actual

    monattk = read(repo, "include/monattk.h")
    assert "#define AT_DEVA 19" in monattk
    assert "#define AT_REND 20" in monattk
    assert "#define AD_EELC 55" in monattk

    uhitm = read(repo, "src/uhitm.c")
    mhitm = read(repo, "src/mhitm.c")
    for source in (uhitm, mhitm):
        assert "step10b_elemental_passive_ready" in source
        assert "step10b_passive_dice" in source
        for adtyp in ("AD_COLD", "AD_FIRE", "AD_ELEC", "AD_MAGM"):
            assert "case " + adtyp in source
    assert "resists_cold(magr)" in mhitm
    assert "resists_fire(magr)" in mhitm
    assert "resists_elec(magr)" in mhitm
    assert "resists_magm(magr)" in mhitm
    assert "Cold_resistance" in uhitm and "Fire_resistance" in uhitm
    assert "Shock_resistance" in uhitm and "Antimagic" in uhitm
    # Magic retaliation is deliberately before the alive/cancelled/chance gate.
    assert mhitm.index("case AD_MAGM:", mhitm.index("passivemm(")) < mhitm.index(
        "step10b_elemental_passive_ready", mhitm.index("passivemm("))
    passive_start = uhitm.index("\npassive(\n")
    assert uhitm.index("case AD_MAGM:", passive_start) < uhitm.index(
        "step10b_elemental_passive_ready", passive_start)

    weapon = read(repo, "src/weapon.c")
    for required in ("mith_multiweapon_slot", "mith_select_multiweapon",
                     "for (otmp = mtmp->minvent", "wanted-- == 0"):
        assert required in weapon
    assert "static struct obj *" not in weapon[weapon.index(
        "mith_select_multiweapon"):weapon.index("possibly_unwield", weapon.index(
            "mith_select_multiweapon"))]
    for path in ("src/mhitu.c", "src/mhitm.c"):
        combat = read(repo, path)
        assert "PM_LURKING_ONE" in combat
        assert "mith_select_multiweapon" in combat
        assert "case AT_DEVA:" in combat and "case AT_REND:" in combat
        assert "deva_penalty += 4" in combat

    monmove = read(repo, "src/monmove.c")
    mondata = read(repo, "src/mondata.c")
    mondata_h = read(repo, "include/mondata.h")
    assert "#define MITH_ELDRITCH_SEEN 0x01L" in mondata_h
    assert "step10b_mark_eldritch_seen(mtmp)" in monmove
    assert "dist2(mtmp->mx, mtmp->my, u.ux, u.uy) > 64" in monmove
    assert "canseemon(mtmp)" in monmove
    encounter_call = monmove.index("step10b_eldritch_encounter(mtmp);")
    assert encounter_call < monmove.index("if (!mtmp->mcanmove", encounter_call)
    assert "mon->mspare1 |= MITH_ELDRITCH_SEEN" in mondata
    assert "PM_TINY_BEING_OF_LIGHT" in monmove and "flag |= NOTONL" in monmove
    assert "PM_VOICE_IN_THE_DARK" in uhitm
    hates_silver = mondata[mondata.index("hates_silver(struct permonst *ptr)"):]
    assert "ptr == &mons[PM_VOICE_IN_THE_DARK]" in hates_silver

    mcastu = read(repo, "src/mcastu.c")
    for spell in ("MCAST_CONFUSE_YOU", "MCAST_RILMANI_MAKE_VISIBLE",
                  "MCAST_KUKER_EVIL_EYE", "MCAST_CURSE_ITEMS",
                  "MCAST_KUKER_PROTECTION", "MCAST_PUNISHMENT"):
        assert spell in mcastu
    cooldown = mcastu[mcastu.index("step10b_spell_cooldown"):
                      mcastu.index("step10b_species_spell")]
    assert "PM_AURUMACH_RILMANI" in cooldown and "PM_KUKER" in cooldown
    assert "? 0 : normal" in cooldown
    assert "PM_FLASHING_LAKE" in mcastu and "PM_SPARKLING_LAKE" in mcastu
    assert "monkilled(target, \"\", (int) attack->adtyp)" in mcastu

    for unchanged in ("README.md",):
        old = subprocess.check_output(
            ["git", "show", BASE + ":" + unchanged], cwd=repo)
        assert (repo / unchanged).read_bytes() == old.replace(b"\n", b"\r\n") \
            or (repo / unchanged).read_bytes().replace(b"\r\n", b"\n") == old

    diff = subprocess.check_output(
        ["git", "diff", "--unified=0", BASE], cwd=repo, text=True)
    additions = "\n".join(line[1:] for line in diff.splitlines()
                            if line.startswith("+") and not line.startswith("+++"))
    for forbidden in ("u.usanity", "u.uinsight", "u.umadness", "sanity.c"):
        assert forbidden not in additions
    print("PASS Step 10B2-2 exact append order and bounded production integration")
    print("PASS passive hero/monster call sites, resistance/death routing, mental pacing and multiweapon rescans")
    print("PASS README unchanged; later-phase topology is independently gated")
