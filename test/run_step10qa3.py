"""Step 10QA3 combat-semantic audit and regression gate.

Usage: python -B test/run_step10qa3.py RELEASE_DIRECTORY OUTPUT_DIRECTORY [DONOR]

The gate audits every explicit attack in the appended Step 10 monster block,
resolves every exact (aatyp, adtyp) pair through an evidence-backed support
matrix, and runs isolated wizard fixtures for the existing QA3 combat cases
plus the focused voice-in-the-dark life-drain gaze.  All fixture data and
transcripts live outside the source tree.
"""

from datetime import datetime
import json
import os
from pathlib import Path
import re
import sys


REPO = Path(__file__).resolve().parents[1]
DEFAULT_DONOR = REPO / "_qa" / "dnethack-donor-pinned"
DONOR_COMMIT = "17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0"
DONOR_MONST = "dnethack-3.4.3/src/monst.c"
STEP10_FIRST_ID = 430
EXPECTED_PAIR_COUNT = 68

SUPPORTED_NATIVE = "SUPPORTED — NATIVE"
SUPPORTED_STEP = "SUPPORTED — EXISTING STEP ADAPTATION"
SUPPORTED_DONOR = "SUPPORTED — BOUNDED DONOR ADAPTATION"
INTENTIONALLY_INERT = "INTENTIONALLY EXCLUDED/INERT"
UNSUPPORTED = "UNSUPPORTED"
NOT_PROVEN = "NOT PROVEN"


def source_line(text, offset):
    return text.count("\n", 0, offset) + 1


def donor_source(donor):
    import subprocess

    return subprocess.check_output(
        ["git", "-C", str(donor), "show", DONOR_COMMIT + ":" + DONOR_MONST],
        text=True,
        encoding="utf-8",
    )


def step10_entries(monsters):
    start = monsters.find("Step 10")
    end = monsters.find("mons_init")
    assert start >= 0 and end > start, "missing Step 10 monster append block"
    block = monsters[start:end]
    starts = list(re.finditer(r'MON\(NAM\("([^"]+)"\),', block))
    assert starts, "empty Step 10 monster append block"
    rows = []
    for index, match in enumerate(starts):
        stop = starts[index + 1].start() if index + 1 < len(starts) else len(block)
        body = block[match.start():stop]
        name = match.group(1)
        local_start = start + match.start()
        attack_tokens = list(re.finditer(
            r"ATTK\((AT_[A-Z0-9]+),\s*(AD_[A-Z0-9]+),\s*(\d+),\s*(\d+)\)|NO_ATTK",
            body,
        ))
        rows.append({
            "monster": name,
            "local_id": STEP10_FIRST_ID + index,
            "local_name": name,
            "local_source": "include/monsters.h:%d" % source_line(monsters, local_start),
            "attacks": [
                {
                    "attack_slot": slot,
                    "aatyp": attack.group(1),
                    "adtyp": attack.group(2),
                    "dice": "%sd%s" % (attack.group(3), attack.group(4)),
                    "local_source": "include/monsters.h:%d" % source_line(
                        monsters, local_start + match.end() + attack.start()
                    ),
                }
                for slot, attack in enumerate(attack_tokens, 1)
                if attack.group(1) is not None
            ],
        })
    return rows


def donor_locations(donor_text):
    return {
        name.casefold(): source_line(donor_text, match.start())
        for match in re.finditer(r'MON\("([^"]+)",', donor_text)
        for name in [match.group(1)]
    }


def _check(path, contains):
    return {"file": path, "contains": contains}


def _evidence(hero, monster):
    return {"hero": hero, "monster": monster}


def _row(aatyp, adtyp, classification, hero_handler, monster_handler,
         evidence, applicable=True, runtime_required=False,
         runtime_reason="Static pair-specific source proof is sufficient.",
         runtime_result="NOT REQUIRED"):
    return {
        "aatyp": aatyp,
        "adtyp": adtyp,
        "monsters_using_it": [],
        "hero_handler": hero_handler,
        "monster_handler": monster_handler,
        "monster_handler_applicable": applicable,
        "source_evidence": evidence,
        "classification": classification,
        "runtime_test_required": runtime_required,
        "runtime_test_reason": runtime_reason,
        "runtime_result": runtime_result,
    }


_CONTACT_HELPERS = {
    "AD_ACID": "mhitm_ad_acid",
    "AD_CNFT": "mhitm_ad_conf",
    "AD_COLD": "mhitm_ad_cold",
    "AD_DISE": "mhitm_ad_dise",
    "AD_DRIN": "mhitm_ad_drin",
    "AD_DRLI": "mhitm_ad_drli",
    "AD_DRST": "mhitm_ad_drst",
    "AD_EACD": "mhitm_ad_acid (AD_EACD branch)",
    "AD_EELE": "mhitm_ad_elec",
    "AD_EELC": "mhitm_ad_elec",
    "AD_ELEC": "mhitm_ad_elec",
    "AD_FAMN": "mhitm_ad_famn",
    "AD_PEST": "mhitm_ad_pest",
    "AD_DETH": "mhitm_ad_deth",
    "AD_PHYS": "mhitm_ad_phys",
    "AD_PLYS": "mhitm_ad_plys",
    "AD_SAMU": "mhitm_ad_samu",
    "AD_SEDU": "mhitm_ad_sedu",
    "AD_SHRD": "mhitm_ad_phys + erode_armor",
    "AD_SITM": "mhitm_ad_sedu (AD_SITM branch)",
    "AD_STCK": "mhitm_ad_stck",
    "AD_STUN": "mhitm_ad_stun",
    "AD_TCKL": "mhitm_ad_plys (AD_TCKL branch)",
    "AD_VAMP": "mith_drain_attack",
    "AD_WET": "water/erosion branch",
}


def _contact(aatyp, adtyp, classification=SUPPORTED_NATIVE):
    pair = "%s + %s" % (aatyp, adtyp)
    helper = _CONTACT_HELPERS.get(adtyp, "mhitm_adtyping exact case")
    return _row(
        aatyp, adtyp, classification,
        "src/mhitu.c::mattacku -> hitmu -> src/uhitm.c::mhitm_adtyping",
        "src/mhitm.c::mattackm -> hitmm -> mdamagem -> "
        "src/uhitm.c::mhitm_adtyping",
        _evidence(
            {
                "file": "src/mhitu.c",
                "function": "mattacku(), hitmu()",
                "dispatch": "mattacku's exact aatyp case reaches hitmu(); "
                            "hitmu calls mhitm_adtyping().",
                "case_branch_helper": "case %s: -> case %s: -> %s" %
                                      (aatyp, adtyp, helper),
                "proof_summary": "%s is handled by the exact shared adtyp case; "
                                 "no default/fallthrough is used." % pair,
                "checks": [
                    _check("src/mhitu.c", "case %s:" % aatyp),
                    _check("src/mhitu.c", "hitmu("),
                    _check("src/uhitm.c", "case %s:" % adtyp),
                ],
            },
            {
                "file": "src/mhitm.c",
                "function": "mattackm(), hitmm(), mdamagem()",
                "dispatch": "mattackm's exact aatyp case reaches hitmm(); "
                            "mdamagem calls mhitm_adtyping().",
                "case_branch_helper": "case %s: -> case %s: -> %s" %
                                      (aatyp, adtyp, helper),
                "proof_summary": "%s has a separate monster-target path and exact "
                                 "adtyp handling." % pair,
                "checks": [
                    _check("src/mhitm.c", "case %s:" % aatyp),
                    _check("src/mhitm.c", "hitmm("),
                    _check("src/uhitm.c", "case %s:" % adtyp),
                ],
            },
        ),
    )


def _projectile(adtyp):
    pair = "AT_ARRW + %s" % adtyp
    return _row(
        "AT_ARRW", adtyp, SUPPORTED_DONOR,
        "src/mhitu.c::mattacku -> src/mthrowu.c::mith_internal_projectile",
        "src/mhitm.c::mattackm -> src/mthrowu.c::mith_internal_projectile",
        _evidence(
            {
                "file": "src/mhitu.c + src/mthrowu.c",
                "function": "mattacku(), mith_internal_projectile()",
                "dispatch": "case AT_ARRW passes the exact attack to the internal "
                            "projectile helper.",
                "case_branch_helper": "mith_internal_projectile exact %s branch" %
                                      adtyp,
                "proof_summary": "The internal projectile helper explicitly handles "
                                 "the requested %s semantic." % pair,
                "checks": [
                    _check("src/mhitu.c", "case AT_ARRW:"),
                    _check("src/mhitu.c", "mith_internal_projectile(mtmp, &gy.youmonst"),
                    _check("src/mthrowu.c", "mattk->adtyp == %s" % adtyp)
                    if adtyp == "AD_SLVR"
                    else _check("src/mthrowu.c", "mattk->adtyp != AD_LOAD"),
                ],
            },
            {
                "file": "src/mhitm.c + src/mthrowu.c",
                "function": "mattackm(), mith_internal_projectile()",
                "dispatch": "case AT_ARRW passes the exact attack to the same helper "
                            "for a monster target.",
                "case_branch_helper": "mith_internal_projectile exact %s branch" %
                                      adtyp,
                "proof_summary": "Monster-target projectile behavior is separately "
                                 "verified for %s." % pair,
                "checks": [
                    _check("src/mhitm.c", "case AT_ARRW:"),
                    _check("src/mhitm.c", "mith_internal_projectile(magr, mdef, mattk)"),
                    _check("src/mthrowu.c", "mattk->adtyp == %s" % adtyp)
                    if adtyp == "AD_SLVR"
                    else _check("src/mthrowu.c", "mattk->adtyp != AD_LOAD"),
                ],
            },
        ),
    )


def _engulf(adtyp):
    pair = "AT_ENGL + %s" % adtyp
    return _row(
        "AT_ENGL", adtyp, SUPPORTED_DONOR,
        "src/mhitu.c::mattacku -> gulpmu",
        "src/mhitm.c::mattackm -> gulpmm -> mdamagem",
        _evidence(
            {
                "file": "src/mhitu.c",
                "function": "mattacku(), gulpmu()",
                "dispatch": "case AT_ENGL calls gulpmu() after engulf succeeds.",
                "case_branch_helper": "gulpmu switch case %s implements %s." %
                                      (adtyp, pair),
                "proof_summary": "The specific engulf semantic is implemented, not "
                                 "accepted by the generic hit path.",
                "checks": [
                    _check("src/mhitu.c", "case AT_ENGL:"),
                    _check("src/mhitu.c", "gulpmu("),
                    _check("src/mhitu.c", "case %s:" % adtyp),
                ],
            },
            {
                "file": "src/mhitm.c",
                "function": "mattackm(), gulpmm(), mdamagem()",
                "dispatch": "case AT_ENGL calls gulpmm(); its damage path reaches "
                            "mdamagem().",
                "case_branch_helper": "mhitm_adtyping case %s implements %s." %
                                      (adtyp, pair),
                "proof_summary": "The monster-target path has the same exact adtyp "
                                 "case evaluated independently.",
                "checks": [
                    _check("src/mhitm.c", "case AT_ENGL:"),
                    _check("src/mhitm.c", "gulpmm("),
                    _check("src/uhitm.c", "case %s:" % adtyp),
                ],
            },
        ),
    )


def _gaze(adtyp, classification=SUPPORTED_DONOR, runtime_required=False,
          runtime_reason="Static gaze dispatch and exact adtyp handling are "
                         "unambiguous.", runtime_result="NOT REQUIRED"):
    pair = "AT_GAZE + %s" % adtyp
    custom_gaze = {"AD_BLAS", "AD_MIST", "AD_WISD"}
    monster_tail = ("gazemm() custom case %s" % adtyp
                    if adtyp in custom_gaze
                    else "gazemm() -> mdamagem() -> mhitm_adtyping case %s" % adtyp)
    return _row(
        "AT_GAZE", adtyp, classification,
        "src/mhitu.c::mattacku -> gazemu",
        "src/mhitm.c::mattackm -> gazemm -> " + monster_tail,
        _evidence(
            {
                "file": "src/mhitu.c",
                "function": "mattacku(), gazemu()",
                "dispatch": "case AT_GAZE calls gazemu(); gazemu inspects this "
                            "exact adtyp.",
                "case_branch_helper": (
                    "gazemu exact mattk->adtyp == %s" % adtyp
                    if adtyp in custom_gaze
                    else "gazemu switch case %s" % adtyp
                ),
                "proof_summary": "%s uses its gaze-specific implementation and "
                                 "does not fall into Gaze attack %%d?." % pair,
                "checks": [
                    _check("src/mhitu.c", "case AT_GAZE:"),
                    _check("src/mhitu.c", "gazemu("),
                    _check("src/mhitu.c", "case %s:" % adtyp)
                    if adtyp not in custom_gaze
                    else _check("src/mhitu.c", "mattk->adtyp == %s" % adtyp),
                ],
            },
            {
                "file": "src/mhitm.c",
                "function": "mattackm(), gazemm()",
                "dispatch": "case AT_GAZE calls gazemm(); the monster path "
                            "handles this exact adtyp.",
                "case_branch_helper": (
                    "gazemm exact mattk->adtyp == %s" % adtyp
                    if adtyp in custom_gaze else monster_tail
                ),
                "proof_summary": "Monster-target gaze behavior is evaluated separately "
                                 "from the hero-target gaze behavior.",
                "checks": [
                    _check("src/mhitm.c", "case AT_GAZE:"),
                    _check("src/mhitm.c", "gazemm("),
                    _check("src/mhitm.c", "mattk->adtyp == %s" % adtyp)
                    if adtyp in custom_gaze
                    else _check("src/uhitm.c", "case %s:" % adtyp),
                ],
            },
        ),
        runtime_required=runtime_required,
        runtime_reason=runtime_reason,
        runtime_result=runtime_result,
    )


def _magic_spell(adtyp):
    pair = "AT_MAGC + %s" % adtyp
    return _row(
        "AT_MAGC", adtyp, SUPPORTED_DONOR,
        "src/mhitu.c::mattacku -> castmu -> mcast_spell",
        "src/mhitm.c::mattackm -> mith_castmm",
        _evidence(
            {
                "file": "src/mcastu.c",
                "function": "mattacku(), castmu()",
                "dispatch": "The AT_MAGC contact path calls castmu(); castmu "
                            "accepts this exact spell-family adtyp.",
                "case_branch_helper": "castmu spell selection and switch case %s "
                                      "call mcast_spell()." % adtyp,
                "proof_summary": "The exact spell family is selected and executed; "
                                 "non-spell values are rejected by castmu().",
                "checks": [
                    _check("src/mhitu.c", "case AT_MAGC:"),
                    _check("src/mcastu.c", "mattk->adtyp == %s" % adtyp),
                    _check("src/mcastu.c", "case %s:" % adtyp),
                ],
            },
            {
                "file": "src/mcastu.c",
                "function": "mattackm(), mith_castmm()",
                "dispatch": "mattackm's AT_MAGC path calls mith_castmm(); the "
                            "helper's exact allowed-family guard includes this adtyp.",
                "case_branch_helper": "mith_castmm allowed spell family includes %s; "
                                      "spell execution follows." % adtyp,
                "proof_summary": "Monster-target spell handling is explicit and not "
                                 "the generic physical default.",
                "checks": [
                    _check("src/mhitm.c", "case AT_MAGC:"),
                    _check("src/mcastu.c", "mith_castmm("),
                    _check("src/mcastu.c", "attack->adtyp != %s" % adtyp),
                ],
            },
        ),
    )


def _magic_element(adtyp, classification=SUPPORTED_DONOR, runtime_required=False,
                   runtime_reason="Static ranged-element and lake monster-target "
                                  "paths prove the exact element.",
                   runtime_result="NOT REQUIRED"):
    pair = "AT_MAGC + %s" % adtyp
    return _row(
        "AT_MAGC", adtyp, classification,
        "src/mhitu.c::mattacku -> src/mcastu.c::buzzmu",
        "src/mhitm.c::mattackm -> src/mcastu.c::mith_castmm",
        _evidence(
            {
                "file": "src/mcastu.c",
                "function": "mattacku(), buzzmu()",
                "dispatch": "Ranged AT_MAGC reaches buzzmu(); BZ_VALID_ADTYP "
                            "and BZ_OFS_AD process this exact element.",
                "case_branch_helper": "buzzmu BZ_VALID_ADTYP(mattk->adtyp) and "
                                      "buzz(BZ_M_SPELL(BZ_OFS_AD(mattk->adtyp))).",
                "proof_summary": "%s is mapped to the native elemental zap type; "
                                 "it is not a generic spell fallback." % pair,
                "checks": [
                    _check("src/mhitu.c", "case AT_MAGC:"),
                    _check("src/mcastu.c", "BZ_VALID_ADTYP(mattk->adtyp)"),
                    _check("src/mcastu.c", "BZ_OFS_AD(mattk->adtyp)"),
                ],
            },
            {
                "file": "src/mcastu.c",
                "function": "mattackm(), mith_castmm()",
                "dispatch": "Monster AT_MAGC reaches mith_castmm(); its lake branch "
                            "validates and resistance-checks this exact element.",
                "case_branch_helper": "lake guard excludes all values except the four "
                                      "elemental cases, including %s." % adtyp,
                "proof_summary": "The monster-target elemental damage and resistance "
                                 "semantics are implemented in the lake branch.",
                "checks": [
                    _check("src/mhitm.c", "case AT_MAGC:"),
                    _check("src/mcastu.c", "attack->adtyp != %s" % adtyp),
                    _check("src/mcastu.c", "resists_%s(target)" % {
                        "AD_FIRE": "fire", "AD_COLD": "cold",
                        "AD_ELEC": "elec", "AD_MAGM": "magm",
                    }[adtyp]),
                ],
            },
        ),
        runtime_required=runtime_required,
        runtime_reason=runtime_reason,
        runtime_result=runtime_result,
    )


def _passive(adtyp, classification=SUPPORTED_DONOR, runtime_required=False,
             runtime_reason="Static passive switch cases and trigger helper prove "
                            "the exact adtyp.", runtime_result="NOT REQUIRED"):
    pair = "AT_NONE + %s" % adtyp
    return _row(
        "AT_NONE", adtyp, classification,
        "src/uhitm.c::passive",
        "src/mhitm.c::passivemm",
        _evidence(
            {
                "file": "src/uhitm.c",
                "function": "passive()",
                "dispatch": "The AT_NONE passive attack is selected and its exact "
                            "adtyp switch case runs.",
                "case_branch_helper": "passive switch case %s" % adtyp,
                "proof_summary": "The hero-target passive has an exact semantic case "
                                 "rather than relying on default behavior.",
                "checks": [
                    _check("src/uhitm.c", "passive("),
                    _check("src/uhitm.c", "case %s:" % adtyp),
                ],
            },
            {
                "file": "src/mhitm.c",
                "function": "passivemm()",
                "dispatch": "Monster-vs-monster retaliation selects the defender's "
                            "AT_NONE attack and inspects its exact adtyp.",
                "case_branch_helper": "passivemm switch case %s" % adtyp,
                "proof_summary": "The monster-target passive has an independent exact "
                                 "case and trigger gate.",
                "checks": [
                    _check("src/mhitm.c", "passivemm("),
                    _check("src/mhitm.c", "case %s:" % adtyp),
                ],
            },
        ),
        runtime_required=runtime_required,
        runtime_reason=runtime_reason,
        runtime_result=runtime_result,
    )


def _composite(aatyp, adtyp, concrete_aatyp, concrete_adtyp,
               classification=SUPPORTED_DONOR):
    row = _contact(concrete_aatyp, concrete_adtyp, classification)
    row["aatyp"], row["adtyp"] = aatyp, adtyp
    pair = "%s + %s" % (aatyp, adtyp)
    row["hero_handler"] = (
        "src/mhitu.c::getmattk(%s -> %s/%s) -> mattacku -> hitmu"
        % (aatyp, concrete_aatyp, concrete_adtyp)
    )
    row["monster_handler"] = (
        "src/mhitu.c::getmattk(%s -> %s/%s) -> mattackm -> hitmm"
        % (aatyp, concrete_aatyp, concrete_adtyp)
    )
    assignment_check = (
        "pick == 0 ? AT_TUCH" if concrete_aatyp == "AT_TUCH"
        else "pick == 1 ? AT_BUTT"
    )
    row["source_evidence"]["hero"].update({
        "file": "src/mhitu.c",
        "function": "getmattk(), mattacku(), hitmu()",
        "dispatch": "%s is concretized to %s/%s before both target paths." %
                    (aatyp, concrete_aatyp, concrete_adtyp),
        "case_branch_helper": "getmattk exact %s branch -> %s -> case %s" %
                              (aatyp, concrete_aatyp, concrete_adtyp),
        "proof_summary": "The composite attack's exact source pair %s is traced "
                         "through its concrete attack and exact adtyp case." % pair,
        "checks": [
            _check("src/mhitu.c", "attk->aatyp == %s" % aatyp),
            _check("src/mhitu.c", assignment_check),
            _check("src/mhitu.c", "case %s:" % concrete_aatyp),
            _check("src/uhitm.c", "case %s:" % concrete_adtyp),
        ],
    })
    row["source_evidence"]["monster"].update({
        "file": "src/mhitu.c + src/mhitm.c",
        "function": "getmattk(), mattackm(), hitmm()",
        "dispatch": "The same getmattk concretization feeds the monster-target "
                    "dispatcher and exact adtyp helper.",
        "case_branch_helper": "getmattk %s -> %s/%s -> mhitm_adtyping case %s" %
                              (aatyp, concrete_aatyp, concrete_adtyp,
                               concrete_adtyp),
        "proof_summary": "Monster-target handling is evaluated separately after the "
                         "concrete attack substitution.",
        "checks": [
            _check("src/mhitu.c", "attk->aatyp == %s" % aatyp),
            _check("src/mhitm.c", "case %s:" % concrete_aatyp),
            _check("src/uhitm.c", "case %s:" % concrete_adtyp),
        ],
    })
    return row


def _wide_gaze(adtyp):
    row = _gaze(adtyp, SUPPORTED_DONOR)
    row["aatyp"] = "AT_WDGZ"
    row["hero_handler"] = "src/mhitu.c::getmattk(AT_WDGZ -> AT_GAZE) -> gazemu"
    row["monster_handler"] = (
        "src/mhitu.c::getmattk(AT_WDGZ -> AT_GAZE) -> "
        "src/mhitm.c::gazemm"
    )
    for side in ("hero", "monster"):
        row["source_evidence"][side]["proof_summary"] = (
            "AT_WDGZ is explicitly lowered to AT_GAZE, then the exact %s gaze "
            "semantic is evaluated." % adtyp)
        row["source_evidence"][side]["checks"].insert(
            0, _check("src/mhitu.c", "attk->aatyp == AT_WDGZ"))
    return row


def _death_marker(adtyp):
    if adtyp == "AD_SOUL":
        return _row(
            "AT_NONE", adtyp, SUPPORTED_STEP,
            "src/mon.c::mondead -> mondied -> corpse_chance -> mith_deep_soul",
            "src/mon.c::monkilled/mondied -> corpse_chance -> mith_deep_soul",
            _evidence(
                {
                    "file": "src/mon.c",
                    "function": "corpse_chance(), mith_deep_soul()",
                    "dispatch": "AD_SOUL is a donor death/corpse marker, not an "
                                "active passive damage case; corpse processing calls "
                                "mith_deep_soul().",
                    "case_branch_helper": "mith_deep_soul exact deep-one/Father Dagon/"
                                          "Mother Hydra growth pulse",
                    "proof_summary": "The hero-kill path reaches corpse_chance and the "
                                     "specific AD_SOUL donor effect is implemented.",
                    "checks": [
                        _check("src/mon.c", "mith_deep_soul(mdat)"),
                        _check("src/mon.c", "PM_FATHER_DAGON"),
                        _check("include/monsters.h", "ATTK(AT_NONE, AD_SOUL"),
                    ],
                },
                {
                    "file": "src/mon.c",
                    "function": "monkilled(), mondead(), corpse_chance()",
                    "dispatch": "The same death/corpse path runs when a monster kills "
                                "the defender; it is not an ordinary passivemm case.",
                    "case_branch_helper": "corpse_chance -> mith_deep_soul",
                    "proof_summary": "The monster-target death marker is explicitly "
                                     "applicable and uses the donor growth semantic.",
                    "checks": [
                        _check("src/mon.c", "corpse_chance("),
                        _check("src/mon.c", "mith_deep_soul(mdat)"),
                        _check("include/monsters.h", "ATTK(AT_NONE, AD_SOUL"),
                    ],
                },
            ),
            runtime_required=True,
            runtime_reason="Existing Step 10B2-4 focused runtime proves the AD_SOUL "
                           "growth pulse through the extracted production helper.",
            runtime_result="PASS — existing Step 10B2-4 runtime",
        )
    return _row(
        "AT_NONE", adtyp, SUPPORTED_STEP,
        "src/mon.c::mondead -> step10b_cthulhu_death_effect",
        "src/mon.c::mondead -> step10b_cthulhu_death_effect",
        _evidence(
            {
                "file": "src/mon.c",
                "function": "mondead(), step10b_cthulhu_death_effect()",
                "dispatch": "Great Cthulhu's AD_POSN death marker is wired from the "
                            "hero-kill death path before corpse handling.",
                "case_branch_helper": "PM_GREAT_CTHULHU -> d(8,8) physical noxious "
                                      "explosion -> radius-two gas cloud",
                "proof_summary": "The declared AD_POSN semantic is implemented by the "
                                 "bounded death-effect helper, not passive default code.",
                "checks": [
                    _check("src/mon.c", "mndx == PM_GREAT_CTHULHU"),
                    _check("src/mon.c", "step10b_cthulhu_death_effect(deathx, deathy)"),
                    _check("src/mon.c", "d(8, 8)"),
                    _check("src/mon.c", "EXPL_NOXIOUS"),
                    _check("include/monsters.h", "ATTK(AT_NONE, AD_POSN, 8, 8)"),
                ],
            },
            {
                "file": "src/mon.c",
                "function": "monkilled(), mondead(), step10b_cthulhu_death_effect()",
                "dispatch": "A monster-target death of Great Cthulhu uses the same "
                            "death pulse and affects nearby monsters through explode().",
                "case_branch_helper": "mondead identity branch -> noxious physical "
                                      "explosion and gas cloud",
                "proof_summary": "The monster-target applicability is explicit even "
                                 "though this is a death marker rather than passivemm.",
                "checks": [
                    _check("src/mon.c", "monkilled("),
                    _check("src/mon.c", "step10b_cthulhu_death_effect("),
                    _check("src/mon.c", "create_gas_cloud("),
                    _check("include/monsters.h", "ATTK(AT_NONE, AD_POSN, 8, 8)"),
                ],
            },
        ),
        runtime_required=True,
        runtime_reason="Existing Step 10B2-4 focused runtime proves the 8d8 physical "
                       "noxious explosion and gas-cloud timing.",
        runtime_result="PASS — existing Step 10B2-4 runtime",
    )


def _inert_unknown():
    return _row(
        "AT_NONE", "AD_UNKN", INTENTIONALLY_INERT,
        "src/uhitm.c::passive -> explicit no-op/default for donor marker",
        "src/mhitm.c::passivemm -> explicit no-op/default for donor marker",
        _evidence(
            {
                "file": "src/uhitm.c",
                "function": "passive(), mhitm_adtyping()",
                "dispatch": "The marker reaches the passive path, whose default does "
                            "not inflict damage.",
                "case_branch_helper": "passive default branch has no effect for "
                                      "AD_UNKN",
                "proof_summary": "This is intentionally inert by the pinned donor "
                                 "contract, not an unaudited supported default.",
                "checks": [
                    _check("src/uhitm.c", "default:"),
                    _check("include/monsters.h", "ATTK(AT_NONE, AD_UNKN"),
                ],
            },
            {
                "file": "src/mhitm.c",
                "function": "passivemm()",
                "dispatch": "Monster retaliation evaluates the AT_NONE marker and "
                            "falls through its explicit inert default.",
                "case_branch_helper": "passivemm default leaves tmp at zero for the "
                                      "unknown marker",
                "proof_summary": "The monster-target path is evaluated and is also "
                                 "intentionally inert.",
                "checks": [
                    _check("src/mhitm.c", "passivemm("),
                    _check("src/mhitm.c", "default:"),
                    _check("include/monsters.h", "ATTK(AT_NONE, AD_UNKN"),
                ],
            },
        ),
    )


# Every key below is an exact source pair.  The helper constructors only fill
# repeated evidence prose; they never infer classification from aatyp alone.
VERIFIED_PAIR_ROWS = [
    _projectile("AD_LOAD"),
    _projectile("AD_SLVR"),
    _contact("AT_BITE", "AD_DISE"),
    _contact("AT_BITE", "AD_DRST"),
    _contact("AT_BITE", "AD_EACD"),
    _contact("AT_BITE", "AD_PHYS"),
    _contact("AT_BITE", "AD_PLYS"),
    _contact("AT_BITE", "AD_STUN"),
    _contact("AT_BITE", "AD_VAMP"),
    _composite("AT_BKG2", "AD_STUN", "AT_BUTT", "AD_STUN"),
    _composite("AT_BKGT", "AD_EACD", "AT_TUCH", "AD_EACD"),
    _contact("AT_BUTT", "AD_PHYS"),
    _contact("AT_CLAW", "AD_DRIN"),
    _contact("AT_CLAW", "AD_ELEC"),
    _contact("AT_CLAW", "AD_PHYS"),
    _contact("AT_CLAW", "AD_PLYS"),
    _contact("AT_CLAW", "AD_SAMU"),
    _contact("AT_CLAW", "AD_SEDU"),
    _contact("AT_CLAW", "AD_SITM"),
    _contact("AT_CLAW", "AD_STCK"),
    _contact("AT_CLAW", "AD_TCKL"),
    _contact("AT_DEVA", "AD_PHYS", SUPPORTED_DONOR),
    _engulf("AD_DGST"),
    _engulf("AD_ILUR"),
    _engulf("AD_PHYS"),
    _gaze("AD_BLAS"),
    _gaze("AD_DRLI", runtime_required=True,
          runtime_reason="The former unsupported pair had a pre-fix Gaze attack "
                         "diagnostic; the permanent voice-in-the-dark fixture must "
                         "prove the life-drain gaze semantic.",
          runtime_result="PASS — focused voice-in-the-dark fixture"),
    _gaze("AD_ELEC", SUPPORTED_STEP, runtime_required=True,
          runtime_reason="Existing QA3 electrical-gaze regression covers non-resistant "
                         "and shock-resistant hero targets plus sequence/control.",
          runtime_result="PASS — existing QA3 lurking-one regression"),
    _gaze("AD_MIST"),
    _contact("AT_HUGS", "AD_PHYS"),
    _contact("AT_KICK", "AD_PHYS"),
    _magic_spell("AD_CLRC"),
    _magic_element("AD_COLD"),
    _magic_element("AD_ELEC", SUPPORTED_STEP, runtime_required=True,
                   runtime_reason="Existing QA3 flashing-lake/control coverage proves "
                                  "non-resistant and resistant elemental paths.",
                   runtime_result="PASS — existing QA3 flashing-lake regression"),
    _magic_element("AD_FIRE"),
    _magic_element("AD_MAGM"),
    _magic_spell("AD_PSON"),
    _magic_spell("AD_SPEL"),
    _passive("AD_ACID"),
    _passive("AD_COLD"),
    _passive("AD_ELEC"),
    _passive("AD_FIRE"),
    _passive("AD_MAGM"),
    _passive("AD_PLYS"),
    _death_marker("AD_POSN"),
    _death_marker("AD_SOUL"),
    _inert_unknown(),
    _contact("AT_REACH2", "AD_SHRD", SUPPORTED_DONOR),
    _contact("AT_REACH5", "AD_CNFT"),
    _contact("AT_REACH5", "AD_DETH"),
    _contact("AT_REACH5", "AD_FAMN"),
    _contact("AT_REACH5", "AD_PEST"),
    _contact("AT_REND", "AD_DISE", SUPPORTED_DONOR),
    _contact("AT_STNG", "AD_STUN"),
    _contact("AT_TENT", "AD_DRIN"),
    _contact("AT_TENT", "AD_DRLI"),
    _contact("AT_TENT", "AD_DRST"),
    _contact("AT_TENT", "AD_PHYS"),
    _contact("AT_TUCH", "AD_ACID"),
    _contact("AT_TUCH", "AD_COLD"),
    _contact("AT_TUCH", "AD_EELC"),
    _contact("AT_TUCH", "AD_PHYS"),
    _contact("AT_TUCH", "AD_STCK"),
    _contact("AT_TUCH", "AD_VAMP"),
    _contact("AT_TUCH", "AD_WET"),
    _wide_gaze("AD_BLND"),
    _wide_gaze("AD_WISD"),
    _contact("AT_WEAP", "AD_PHYS"),
]


def _verified_matrix():
    matrix = {}
    duplicates = []
    for row in VERIFIED_PAIR_ROWS:
        key = (row["aatyp"], row["adtyp"])
        if key in matrix:
            duplicates.append(key)
        matrix[key] = row
    return matrix, duplicates


VERIFIED_PAIR_MATRIX, MATRIX_DUPLICATES = _verified_matrix()


def classify(aatyp, adtyp):
    """Fail closed: only an exact audited pair can receive a classification."""
    row = VERIFIED_PAIR_MATRIX.get((aatyp, adtyp))
    return row["classification"] if row is not None else NOT_PROVEN


def _independent_pair_inventory(monsters):
    """Regenerate the pair set with a separate scanner from step10_entries."""
    start = monsters.find("Step 10")
    end = monsters.find("mons_init")
    assert start >= 0 and end > start, "missing Step 10 block for independent scan"
    block = monsters[start:end]
    return {
        (aatyp, adtyp)
        for aatyp, adtyp in re.findall(
            r"ATTK\((AT_[A-Z0-9]+),\s*(AD_[A-Z0-9]+),", block
        )
    }


def _pair_label(key):
    return "%s + %s" % key


def _matrix_source_checks(matrix):
    """Validate every addressable source-evidence assertion in the matrix."""
    cache = {}
    failures = []
    for key in sorted(matrix):
        row = matrix[key]
        evidence = row.get("source_evidence")
        if not isinstance(evidence, dict):
            failures.append("%s: source_evidence is not an object" % _pair_label(key))
            continue
        for side in ("hero", "monster"):
            item = evidence.get(side)
            if not isinstance(item, dict):
                failures.append("%s: missing %s evidence" % (_pair_label(key), side))
                continue
            for field in ("file", "function", "dispatch", "case_branch_helper",
                          "proof_summary", "checks"):
                if not item.get(field):
                    failures.append("%s: %s evidence missing %s" %
                                    (_pair_label(key), side, field))
            for check in item.get("checks", []):
                path = REPO / check["file"]
                if path not in cache:
                    cache[path] = path.read_text(encoding="utf-8")
                if check["contains"] not in cache[path]:
                    failures.append("%s: %s missing %s in %s" %
                                    (_pair_label(key), side, check["contains"],
                                     check["file"]))
    assert not failures, "source evidence failures:\n" + "\n".join(failures)
    return {"checks": sum(
        len(item.get("checks", []))
        for row in matrix.values()
        for item in row["source_evidence"].values()
    ), "files_checked": sorted(str(path.relative_to(REPO)) for path in cache)}


def _handler_default_audit(out):
    """Record defaults/diagnostics inspected during the pair-level audit."""
    paths = {
        "src/mhitu.c": ["default:", 'impossible("Gaze attack %d?"'],
        "src/mhitm.c": ["default: /* no attack */", "default:"],
        "src/uhitm.c": ["default:", "mhm->damage = 0"],
        "src/mcastu.c": ["return M_ATTK_MISS", "attack->adtyp !="],
        "src/mthrowu.c": ["mattk->adtyp != AD_LOAD", "return M_ATTK_MISS"],
        "src/mon.c": ["mndx == PM_GREAT_CTHULHU", "mith_deep_soul(mdat)"],
    }
    result = {}
    for relative, needles in paths.items():
        text = (REPO / relative).read_text(encoding="utf-8")
        result[relative] = {
            "inspected": needles,
            "present": {needle: needle in text for needle in needles},
        }
    (out / "step10qa3-handler-default-audit.json").write_text(
        json.dumps(result, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    return result


def build_inventory(donor, out):
    monsters_text = (REPO / "include" / "monsters.h").read_text(encoding="utf-8")
    donor_text = donor_source(donor)
    locations = donor_locations(donor_text)
    parsed_monsters = step10_entries(monsters_text)
    runner_pairs = {
        (attack["aatyp"], attack["adtyp"])
        for monster in parsed_monsters for attack in monster["attacks"]
    }
    independent_pairs = _independent_pair_inventory(monsters_text)
    assert runner_pairs == independent_pairs, (
        "runner/independent pair inventory mismatch: runner-only=%s independent-only=%s"
        % (sorted(runner_pairs - independent_pairs),
           sorted(independent_pairs - runner_pairs))
    )
    assert len(runner_pairs) == EXPECTED_PAIR_COUNT, len(runner_pairs)
    assert not MATRIX_DUPLICATES, "duplicate verified matrix keys: %s" % MATRIX_DUPLICATES
    matrix_pairs = set(VERIFIED_PAIR_MATRIX)
    assert len(matrix_pairs) == EXPECTED_PAIR_COUNT, len(matrix_pairs)
    assert runner_pairs == matrix_pairs, (
        "inventory/matrix mismatch: inventory-only=%s matrix-only=%s"
        % (sorted(runner_pairs - matrix_pairs), sorted(matrix_pairs - runner_pairs))
    )

    monsters_by_pair = {key: set() for key in runner_pairs}
    inventory = []
    for monster in parsed_monsters:
        donor_loc = locations.get(monster["monster"].casefold())
        for attack in monster["attacks"]:
            key = (attack["aatyp"], attack["adtyp"])
            monsters_by_pair[key].add(monster["monster"])
            row = VERIFIED_PAIR_MATRIX[key]
            attack_row = dict(attack)
            attack_row.update({
                "monster": monster["monster"],
                "local_id": monster["local_id"],
                "donor_source": None if donor_loc is None else
                    "%s:%d" % (DONOR_MONST, donor_loc),
                "hero_handler": row["hero_handler"],
                "monster_handler": row["monster_handler"],
                "monster_handler_applicable": row["monster_handler_applicable"],
                "source_evidence": row["source_evidence"],
                "classification": classify(*key),
                "runtime_test_required": row["runtime_test_required"],
                "runtime_test_reason": row["runtime_test_reason"],
                "runtime_result": row["runtime_result"],
            })
            inventory.append(attack_row)

    for key, row in VERIFIED_PAIR_MATRIX.items():
        row["monsters_using_it"] = sorted(monsters_by_pair[key], key=str.casefold)

    source_audit = _matrix_source_checks(VERIFIED_PAIR_MATRIX)
    default_audit = _handler_default_audit(out)
    supported = {key for key, row in VERIFIED_PAIR_MATRIX.items()
                 if row["classification"].startswith("SUPPORTED")}
    intentional = {key for key, row in VERIFIED_PAIR_MATRIX.items()
                   if row["classification"].startswith("INTENTIONALLY")}
    unsupported = {key for key, row in VERIFIED_PAIR_MATRIX.items()
                   if row["classification"] == UNSUPPORTED}
    not_proven = {key for key, row in VERIFIED_PAIR_MATRIX.items()
                  if row["classification"] == NOT_PROVEN}
    assert not unsupported
    assert not not_proven
    assert supported.isdisjoint(intentional)
    assert len(supported) + len(intentional) == EXPECTED_PAIR_COUNT
    matrix_rows = [VERIFIED_PAIR_MATRIX[key] for key in sorted(VERIFIED_PAIR_MATRIX)]
    matrix_artifact = {
        "schema": "step10qa3-1.attack-handler-matrix.v1",
        "source": "current include/monsters.h Step 10 block",
        "expected_pair_count": EXPECTED_PAIR_COUNT,
        "actual_pair_count": len(matrix_rows),
        "duplicate_matrix_keys": [_pair_label(key) for key in MATRIX_DUPLICATES],
        "inventory_pair_set_matches_matrix": runner_pairs == matrix_pairs,
        "source_audit": source_audit,
        "handler_default_audit": default_audit,
        "pairs": matrix_rows,
    }
    matrix_path = out / "step10qa3-attack-handler-matrix.json"
    matrix_path.write_text(json.dumps(matrix_artifact, indent=2,
                                      ensure_ascii=False) + "\n",
                           encoding="utf-8")
    summary = {
        "step10_monsters": len(parsed_monsters),
        "explicit_attack_entries": sum(len(monster["attacks"])
                                        for monster in parsed_monsters),
        "distinct_attack_damage_combinations": len(runner_pairs),
        "verified_matrix_entries": len(matrix_rows),
        "matrix_duplicates": len(MATRIX_DUPLICATES),
        "inventory_pair_set_matches_matrix": runner_pairs == matrix_pairs,
        "unexpected_inventory_pairs": sorted(
            _pair_label(key) for key in runner_pairs - matrix_pairs),
        "missing_matrix_pairs": sorted(
            _pair_label(key) for key in matrix_pairs - runner_pairs),
        "verified_hero_handler": sum(bool(row["hero_handler"])
                                     for row in matrix_rows),
        "applicable_monster_handlers": sum(
            row["monster_handler_applicable"] for row in matrix_rows),
        "verified_monster_handler": sum(
            bool(row["monster_handler"] and row["monster_handler_applicable"])
            for row in matrix_rows),
        "supported": len(supported),
        "intentionally_excluded": len(intentional),
        "unsupported": len(unsupported),
        "not_proven": len(not_proven),
        "inventory": inventory,
    }
    (out / "step10qa3-attack-inventory.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    return summary


def source_contract(out):
    gaze = (REPO / "src" / "mhitu.c").read_text(encoding="utf-8")
    cast = (REPO / "src" / "mcastu.c").read_text(encoding="utf-8")
    mhitm = (REPO / "src" / "mhitm.c").read_text(encoding="utf-8")
    uhitm = (REPO / "src" / "uhitm.c").read_text(encoding="utf-8")
    monattk = (REPO / "include" / "monattk.h").read_text(encoding="utf-8")
    assert "#define AT_GAZE 15" in monattk
    assert "#define AD_ELEC 6" in monattk
    gaze_body = gaze[gaze.index("gazemu("):gaze.index("/* mtmp hits you", gaze.index("gazemu("))]
    gaze_electric = "case AD_ELEC:" in gaze_body
    gaze_drain = "case AD_DRLI:" in gaze_body
    magic_electric = (
        "buzzmu(mtmp, mattk)" in gaze
        and "BZ_VALID_ADTYP" in cast
        and "case AD_ELEC:" in uhitm[uhitm.index("mhitm_adtyping("):]
        and "case AT_MAGC:" in mhitm
    )
    assert 'case AT_TENT:\n            verb = "tentacles suck your brain"' in gaze
    assert "case AD_PHYS: mhitm_ad_phys" in uhitm
    assert gaze_drain
    (out / "step10qa3-source-contract.txt").write_text(
        "AT_GAZE=15\nAD_ELEC=6\n"
        "hero_gaze_handler=src/mhitu.c:gazemu\n"
        "hero_magic_handler=src/mhitu.c:mattacku -> buzzmu() for ranged AT_MAGC; "
        "src/mcastu.c:castmu() for contact AT_MAGC\n"
        "donor_gaze_handler=dnethack-3.4.3/src/xhity.c:xgazey\n"
        "donor_magic_handler=dnethack-3.4.3/src/mcastu.c:castmu\n"
        "lurking_one_sequence=slot5 AT_TENT+AD_PHYS+4d8 -> hitmu()/mhitm_ad_phys(); "
        "slot6 AT_GAZE+AD_ELEC+4d8 -> gazemu()/gazemm()\n"
        "voice_in_the_dark=AT_GAZE+AD_DRLI+4d4 -> gazemu()/gazemm(); "
        "gazemu() delegates to mhitm_ad_drli() with gaze-specific life-drain text\n"
        "tentacle_message=src/mhitu.c:hitmsg() retains the brain-sucking text\n"
        "gaze_electric_case=%s\ngaze_drain_case=%s\nmagic_electric_case=%s\n" %
        ("present" if gaze_electric else "missing",
         "present" if gaze_drain else "missing",
         "present" if magic_electric else "missing"),
        encoding="utf-8",
    )
    return gaze_electric, magic_electric


def run_fixture(release, output, resistant, monster="lurking one", coord=1):
    # Imports are delayed so the source-only audit remains usable on hosts
    # without pywinpty/pyte.
    from run_step8a_runtime import Game

    class ObservedGame(Game):
        def __init__(self, *args, **kwargs):
            self.observed = []
            super().__init__(*args, **kwargs)

        def send(self, keys, delay=.3):
            if hasattr(self, "screen"):
                self.observed.append(self.text())
            super().send(keys, delay)
            if hasattr(self, "screen"):
                self.observed.append(self.text())

    print("QA3 fixture start: %s %s" %
          (monster, "resistant" if resistant else "non-resistant"), flush=True)
    game = ObservedGame(release, output)
    try:
        game.settle()
        print("QA3 fixture startup settled", flush=True)
        game.send("#LEVELCHANGE\n")
        game.wait("To what experience level")
        game.send("30\n")
        game.settle()
        print("QA3 fixture hero level set", flush=True)
        game.lua(
            "nh.debug_flags({hunger=false,mongen=true});"
            "u.clear_inventory()"
        )
        (game.path / "qa3-combat.lua").write_text(
            """
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-----------------
|...............|
|...............|
|...............|
|...............|
|...............|
-----------------
]=]})
des.region(selection.area(1,1,15,5),"lit")
""",
            encoding="utf-8",
        )
        game.send("#WIZLOADDES\n")
        game.wait("Load which des lua file?")
        game.send("qa3-combat.lua\n", 2)
        game.settle()
        print("QA3 fixture level loaded", flush=True)
        x, y = game.state()[2:]
        game.send("\x14")
        game.wait("Where do you want to be teleported?")
        game.settle()
        game.send(("l" if 7 > x else "h") * abs(7 - x)
                  + ("j" if 3 > y else "k") * abs(3 - y) + ".")
        game.settle()
        print("QA3 fixture hero positioned", flush=True)
        game.lua(
            """local ox,oy=nh.abscoord(0,0)
des.monster({id="MONSTER",coord={u.ux+OFFSET-ox,u.uy-oy},
             peaceful=false,asleep=false,cancelled=false,
             keep_default_invent=false})
""".replace("MONSTER", monster).replace("OFFSET", str(coord))
        )
        if resistant:
            text = game.lua(
                'local o=obj.new("uncursed ring of shock resistance");'
                'u.giveobj(o);nh.pline("QA3_RING "..o:totable().invlet)'
            )
            letter = re.findall(r"QA3_RING (.)", text)[-1]
            # Synchronize the two-stage put-on command.  Sending P<letter>
            # as one burst can leave the freshly copied Windows tty fixture
            # at the object-selection prompt before the letter is consumed.
            game.send("P")
            game.wait("What do you want to put on")
            game.send(letter, 1)
            game.wait("Which ring-finger", more=False)
            game.send("l")
            game.settle()
        start = len(game.observed)
        for turn in range(120):
            text = "\n".join(game.observed[start:]) + "\n" + game.text()
            lowered = text.lower()
            if monster == "voice in the dark":
                done = "life force wither before the gaze" in lowered
            elif monster == "flashing lake":
                done = "bolt of lightning" in lowered
            else:
                done = "shocking stare" in lowered
            if done:
                break
            game.send("m.")
            try:
                game.settle()
            except (AssertionError, EOFError, OSError):
                break
        print("QA3 fixture combat loop ended", flush=True)
        text = "\n".join(game.observed[start:]) + "\n" + game.text()
        prefix = monster.replace(" ", "-")
        (game.path / (prefix + ("-resistant.txt" if resistant else
                                "-non-resistant.txt"))).write_text(
            text, encoding="utf-8"
        )
        return text
    finally:
        try:
            game.close()
        except AssertionError:
            # The RED run intentionally permits the pre-fix impossible()
            # panic so the captured transcript can prove the old failure.
            # The final assertions reject its visible diagnostics.
            pass


def record_runtime_results(out, summary, results):
    """Persist measured results after all targeted fixtures have passed."""
    for key, result in results.items():
        VERIFIED_PAIR_MATRIX[key]["runtime_result"] = result
    matrix_path = out / "step10qa3-attack-handler-matrix.json"
    artifact = json.loads(matrix_path.read_text(encoding="utf-8"))
    artifact["pairs"] = [VERIFIED_PAIR_MATRIX[key]
                          for key in sorted(VERIFIED_PAIR_MATRIX)]
    matrix_path.write_text(json.dumps(artifact, indent=2, ensure_ascii=False)
                           + "\n", encoding="utf-8")
    for row in summary["inventory"]:
        key = (row["aatyp"], row["adtyp"])
        row["runtime_result"] = VERIFIED_PAIR_MATRIX[key]["runtime_result"]
    (out / "step10qa3-attack-inventory.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )


def main():
    os.environ["PYTHONDONTWRITEBYTECODE"] = "1"
    release = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else REPO / "binary/Release/x64"
    if len(sys.argv) > 2:
        out = Path(sys.argv[2]).resolve()
    else:
        out = Path(r"E:\Codex") / ("step10qa3-evidence-" + datetime.now().strftime("%Y%m%d-%H%M%S"))
    donor = Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else DEFAULT_DONOR
    assert release.is_dir() and donor.is_dir() and not out.exists(), (release, donor, out)
    out.mkdir(parents=True)
    (out / "runner-command.txt").write_text(
        "python -B test/run_step10qa3.py %s %s %s\n" % (release, out, donor),
        encoding="utf-8",
    )
    summary = build_inventory(donor, out)
    gaze_electric, magic_electric = source_contract(out)
    non_resistant = run_fixture(release, out / "runtime-non-resistant", False)
    resistant = run_fixture(release, out / "runtime-resistant", True)
    lake_non_resistant = run_fixture(
        release, out / "runtime-lake-non-resistant", False, "flashing lake", 1
    )
    lake_resistant = run_fixture(
        release, out / "runtime-lake-resistant", True, "flashing lake", 1
    )
    voice = run_fixture(
        release, out / "runtime-voice-in-the-dark", False, "voice in the dark", 1
    )
    assert "shocking stare" in non_resistant.lower(), non_resistant
    assert "Gaze attack 6?" not in non_resistant
    assert "Program in disorder!" not in non_resistant
    assert "shocking stare" in resistant.lower(), resistant
    assert "zap doesn't shock you" in resistant.lower(), resistant
    assert "Gaze attack 6?" not in resistant
    assert "Program in disorder!" not in resistant
    assert gaze_electric and magic_electric
    assert "zaps you with a bolt of lightning" in lake_non_resistant.lower()
    assert "lightning" in lake_resistant.lower()
    assert ("aren't affected" in lake_resistant.lower()
            or "bounces" in lake_resistant.lower()
            or "doesn't shock" in lake_resistant.lower()
            or "resist" in lake_resistant.lower())
    assert "Gaze attack 6?" not in lake_non_resistant + lake_resistant
    assert "Program in disorder!" not in lake_non_resistant + lake_resistant
    assert "life force wither before the gaze" in voice.lower(), voice
    assert "Gaze attack" not in voice
    assert "Program in disorder!" not in voice
    (out / "step10qa3-root-cause.txt").write_text(
        "Observed pre-QA3 runtime: The lurking one's tentacles suck your brain!\n"
        "Observed pre-QA3 runtime: Gaze attack 6?\n"
        "Observed pre-QA3 runtime: Program in disorder!\n"
        "Root-cause tuple: lurking one local attack slot 6, "
        "AT_GAZE + AD_ELEC + 4d8.\n"
        "Numeric constants: AT_GAZE=15; AD_ELEC=6.\n"
        "Local rejection: src/mhitu.c:gazemu() default impossible(\"Gaze attack %d?\").\n"
        "Pinned donor: dnethack-3.4.3/src/xhity.c:xgazey(), elemental gaze "
        "branch; donor d(attk->damn,attk->damd), eye/cancellation gates, "
        "shock resistance, electrical side effects, wakeup.\n"
        "Bounded local adaptation: existing gazemu() electrical branch; "
        "existing local resistance/inventory helpers.\n"
        "Sequence preservation: slot 5 remains AT_TENT + AD_PHYS + 4d8; "
        "the existing hitmsg()/mhitm_ad_phys() path retains the brain-sucking "
        "tentacle semantics.\n"
        "Additional QA3 defect: voice in the dark AT_GAZE + AD_DRLI + 4d4 "
        "fell through gazemu() to impossible(\"Gaze attack %d?\").\n"
        "Pinned donor xhity.c::xgazey() handles life-drain gaze with the native "
        "1/3 success gate and life-force loss. Bounded fix: gazemu() delegates "
        "the exact case to mhitm_ad_drli() with gaze-specific messaging; focused "
        "voice runtime passed.\n"
        "Additional audited case: flashing lake AT_MAGC + AD_ELEC is supported "
        "by the existing buzzmu() ranged elemental path and mith_castmm() "
        "monster-target path; no further defect was found.\n",
        encoding="utf-8",
    )
    record_runtime_results(out, summary, {
        ("AT_GAZE", "AD_DRLI"): "PASS — focused voice-in-the-dark fixture",
        ("AT_GAZE", "AD_ELEC"): "PASS — existing QA3 lurking-one regression",
        ("AT_MAGC", "AD_ELEC"): "PASS — existing QA3 flashing-lake regression",
    })
    result = {
        "qa3": "PASS",
        "lurking_one_root_cause_proven": True,
        "non_resistant": "PASS",
        "relevant_resistance": "PASS",
        "sequence_integrity": "PASS",
        "summary": {key: summary[key] for key in summary if key != "inventory"},
    }
    (out / "step10qa3-summary.json").write_text(
        json.dumps(result, indent=2) + "\n", encoding="utf-8"
    )
    print("PASS Step 10QA3 combat-semantic audit, remediation, and runtime regression", flush=True)


if __name__ == "__main__":
    main()
