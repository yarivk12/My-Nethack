/* NetHack 5.0	monattk.h	$NHDT-Date: 1781973083 2026/06/20 16:31:23 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.23 $ */
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1988, M. Stephenson */

#ifndef MONATTK_H
#define MONATTK_H

/*      Add new attack types below - ordering affects experience (exper.c).
 *      Attacks > AT_BUTT are worth extra experience.
 */
#define AT_ANY (-1) /* fake attack; dmgtype_fromattack wildcard */
#define AT_NONE 0   /* passive monster (ex. acid blob) */
#define AT_CLAW 1   /* claw (punch, hit, etc.) */
#define AT_BITE 2   /* bite */
#define AT_KICK 3   /* kick */
#define AT_BUTT 4   /* head butt (ex. a unicorn) */
#define AT_TUCH 5   /* touches */
#define AT_STNG 6   /* sting */
#define AT_HUGS 7   /* crushing bearhug */
#define AT_SPIT 10  /* spits substance - ranged */
#define AT_ENGL 11  /* engulf (swallow or by a cloud) */
#define AT_BREA 12  /* breath - ranged */
#define AT_EXPL 13  /* explodes - proximity */
#define AT_BOOM 14  /* explodes when killed */
#define AT_GAZE 15  /* gaze - ranged */
#define AT_TENT 16  /* tentacles */
#define AT_REACH5 17 /* Mithardir: First Wraithworm's five-square bite */
#define AT_ARRW 18 /* Step 10B: internal silver-projectile launcher */
#define AT_DEVA 19 /* Step 10B2-2: many-taloned repeated-arm strike */
#define AT_REND 20 /* Step 10B2-2: follows two successful attacks */
#define AT_REACH2 21 /* Step 10B2-3: two-square lurch/reach */
#define AT_WDGZ 22 /* Step 10B2-3: wide gaze */
#define AT_BKGT 23 /* Step 10B2-3: blessed random primary attack */
#define AT_BKG2 24 /* Step 10B2-3: blessed random secondary attack */

#define AT_WEAP 254 /* uses weapon */
#define AT_MAGC 255 /* uses magic spell(s) */

#define DISTANCE_ATTK_TYPE(atyp) ((atyp) == AT_SPIT \
                                  || (atyp) == AT_BREA \
                                  || (atyp) == AT_ARRW \
                                  || (atyp) == AT_REACH2 \
                                  || (atyp) == AT_MAGC \
                                  || (atyp) == AT_GAZE)

/*      Add new damage types below.
 *
 *      Note that 1-10 correspond to the types of attack used in buzz().
 *      Please don't disturb the order unless you rewrite the buzz() code.
 */
#define AD_ANY (-1) /* fake damage; attacktype_fordmg wildcard */
#define AD_PHYS 0   /* ordinary physical */
#define AD_MAGM 1   /* magic missiles */
#define AD_FIRE 2   /* fire damage */
#define AD_COLD 3   /* frost damage */
#define AD_SLEE 4   /* sleep ray */
#define AD_DISN 5   /* disintegration (death ray) */
#define AD_ELEC 6   /* shock damage */
#define AD_DRST 7   /* drains str (poison) */
#define AD_ACID 8   /* acid damage */
#define AD_SPC1 9   /* for extension of buzz() */
#define AD_LAVA AD_SPC1 /* Step9B: lava jet uses the reserved ninth ray slot */
#define AD_SPC2 10  /* for extension of buzz() */
#define AD_BLND 11  /* blinds (yellow light) */
#define AD_STUN 12  /* stuns */
#define AD_SLOW 13  /* slows */
#define AD_PLYS 14  /* paralyzes */
#define AD_DRLI 15  /* drains life levels (Vampire) */
#define AD_DREN 16  /* drains magic energy */
#define AD_LEGS 17  /* damages legs (xan) */
#define AD_STON 18  /* petrifies (Medusa, cockatrice) */
#define AD_STCK 19  /* sticks to you (mimic) */
#define AD_SGLD 20  /* steals gold (leppie) */
#define AD_SITM 21  /* steals item (nymphs) */
#define AD_SEDU 22  /* seduces & steals multiple items */
#define AD_TLPT 23  /* teleports you (Quantum Mech.) */
#define AD_RUST 24  /* rusts armour (Rust Monster)*/
#define AD_CONF 25  /* confuses (Umber Hulk) */
#define AD_DGST 26  /* digests opponent (trapper, etc.) */
#define AD_HEAL 27  /* heals opponent's wounds (nurse) */
#define AD_WRAP 28  /* special "stick" for eels */
#define AD_WERE 29  /* confers lycanthropy */
#define AD_DRDX 30  /* drains dexterity (quasit) */
#define AD_DRCO 31  /* drains constitution */
#define AD_DRIN 32  /* drains intelligence (mind flayer) */
#define AD_DISE 33  /* confers diseases */
#define AD_DCAY 34  /* decays organics (brown Pudding) */
#define AD_SSEX 35  /* Succubus seduction (extended) */
#define AD_HALU 36  /* causes hallucination */
#define AD_DETH 37  /* for Death only */
#define AD_PEST 38  /* for Pestilence only */
#define AD_FAMN 39  /* for Famine only */
#define AD_SLIM 40  /* turns you into green slime */
#define AD_ENCH 41  /* remove enchantment (disenchanter) */
#define AD_CORR 42  /* corrode armor (black pudding) */
#define AD_POLY 43  /* polymorph the target (genetic engineer) */
#define AD_SPOR 44  /* release a swamp fern spore */

#define AD_FREZ 45  /* movement-only freezing (Sheol) */
#define AD_PUNI 46  /* Punisher spell selection */
#define AD_LUCK 47  /* evil-eye luck drain */
#define AD_BLNK 48  /* weeping-angel mental invasion */
#define AD_LVLT 49  /* weeping-angel level teleport */
#define AD_DESC 50  /* Mithardir: desiccation, healing the attacker */
#define AD_VAMP 51  /* Mithardir: blood and life-force drain */
#define AD_SOUL 52  /* deep-one death strengthens its surviving kin */
#define AD_WET 53   /* Step 10B: Hudor soaks carried equipment */
#define AD_SLVR 54  /* Step 10B: silver projectile marker */
#define AD_EELC 55  /* Step 10B2-2: elemental shock; resistance halves */
#define AD_EACD 56  /* Step 10B2-3: enhanced acid; resistance halves */
#define AD_MIST 57  /* Step 10B2-3: Mi-go mist gaze */
#define AD_SHRD 58  /* Step 10B2-3: shred worn armor */
#define AD_TCKL 59  /* Step 10B2-3: nightgaunt tickle */
#define AD_PSON 60  /* Step 10B2-3: psionic spell selection */
#define AD_ILUR 61  /* Step 10B2-3: Illurien memory engulf */
#define AD_UNKN 62  /* Step 10B2-3: unknown-god passive marker */
#define AD_CNFT 63  /* Step 10B2-3: conflict attack */
#define AD_BLAS 64  /* Step 10B2-3: blasphemous blasting gaze */
#define AD_WISD 65  /* Step 10B2-4: Great Cthulhu's wisdom-draining gaze */
#define AD_LOAD 66  /* Step 10B2-4: internal cursed loadstone launcher */
#define AD_POSN 67  /* Step 10B2-4: noxious death marker */
#define AD_CNCL 68  /* Step 19: Beholder cancellation gaze */

#define AD_CLRC 240 /* random clerical spell */
#define AD_SPEL 241 /* random magic spell */
#define AD_RBRE 242 /* random breath weapon */

#define AD_SAMU 252 /* hits, may steal Amulet (Wizard) */
#define AD_CURS 253 /* random curse (ex. gremlin) */

struct mhitm_data {
    boolean fatal; /* native instant death encoded as numerical damage */
    int artifact_nonphysical; /* native elemental artifact component */
    int direct_physical; /* damage inflicted early by native pudding splitting */
    boolean alignment_added; /* physical handler already included weapon dice */
    int direct_damage; /* immediate damage already inflicted (weapon poison) */
    int nonphysical_damage; /* pending poison, excluded from physical mitigation */
    struct obj *weapon; /* current attack's weapon; transient, never saved */
    int damage;
    int hitflags; /* M_ATTK_DEF_DIED | M_ATTK_AGR_DIED | ... */
    boolean done;
    boolean permdmg;
    int specialdmg;
    int dieroll;
};

/*
 *  Monster-to-monster attacks.  When a monster attacks another (mattackm),
 *  any or all of the following can be returned.  See mattackm() for more
 *  details.
 */
#define M_ATTK_MISS 0x0     /* aggressor missed */
#define M_ATTK_HIT 0x1      /* aggressor hit defender */
#define M_ATTK_DEF_DIED 0x2 /* defender died */
#define M_ATTK_AGR_DIED 0x4 /* aggressor died */
#define M_ATTK_AGR_DONE 0x8 /* aggressor is done with their turn */

#endif /* MONATTK_H */
