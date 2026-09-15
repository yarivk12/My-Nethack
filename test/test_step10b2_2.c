#include "hack.h"
#include <assert.h>
#include <stdio.h>

static boolean touch_safe = TRUE;
boolean
can_touch_safely(struct monst *mon, struct obj *obj)
{
    (void) mon;
    (void) obj;
    return touch_safe;
}

#define MCASTU_ENUM
enum step10b2_2_spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM

static void
check_dolls(void)
{
    struct permonst *p;

    assert(PM_OGRE_MAGE == 430);
    assert(PM_ARGENTUM_GOLEM == 440);
    assert(PM_LIVING_DOLL == 441);
    assert(PM_LIVING_LECTERN == 442);
    assert(PM_PARASITIZED_DOLL == 443);
    assert(NUMMONS >= 444);

    p = &mons[PM_LIVING_DOLL];
    assert(p->mlevel == 15 && p->mmove == 10 && p->ac == 10 && p->mr == 50);
    assert(p->geno == G_NOGEN);
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[0].adtyp == AD_PHYS
           && p->mattk[0].damn == 4 && p->mattk[0].damd == 4);
    assert(p->mresists == (MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP
                           | MR_POISON | MR_STONE));
    assert((p->mflags1 & (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID
                          | M1_THICK_HIDE))
           == (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE));
    assert((p->mflags2 & (M2_PEACEFUL | M2_STRONG | M2_NOPOLY))
           == (M2_PEACEFUL | M2_STRONG | M2_NOPOLY));
    assert(step10b_natural_dr(p) == 4);

    p = &mons[PM_LIVING_LECTERN];
    assert(p->mlevel == 7 && p->mmove == 6 && p->ac == 2 && p->mr == 50);
    assert(p->geno == (G_NOGEN | G_NOCORPSE));
    assert(p->mattk[0].aatyp == AT_CLAW && p->mattk[0].damn == 3
           && p->mattk[0].damd == 4);
    assert(p->mattk[1].aatyp == AT_CLAW && p->mattk[1].damn == 3
           && p->mattk[1].damd == 4);
    assert(p->mattk[2].aatyp == AT_MAGC && p->mattk[2].adtyp == AD_SPEL
           && p->mattk[2].damn == 0 && p->mattk[2].damd == 4);
    assert(p->mresists == (MR_SLEEP | MR_POISON));
    assert((p->mflags1 & (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID
                          | M1_THICK_HIDE))
           == (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE));
    assert(p->mflags2 & M2_HOSTILE);
    assert(step10b_natural_dr(p) == 4);
    assert(step10b_innate_magic(p));

    p = &mons[PM_PARASITIZED_DOLL];
    assert(p->mlevel == 30 && p->mmove == 10 && p->ac == 0 && p->mr == 75);
    assert(p->geno == G_NOGEN);
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[0].adtyp == AD_PHYS
           && p->mattk[0].damn == 4 && p->mattk[0].damd == 4);
    assert(p->mresists == (MR_SLEEP | MR_POISON | MR_STONE));
    assert((p->mflags1 & (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID
                          | M1_THICK_HIDE))
           == (M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE));
    assert((p->mflags2 & (M2_HOSTILE | M2_STRONG | M2_NOPOLY))
           == (M2_HOSTILE | M2_STRONG | M2_NOPOLY));
    assert(p->mflags3 & M3_WAITFORU);
    assert(step10b_natural_dr(p) == 2);
}

static void
check_dervishes_and_lakes(void)
{
    static const int expected[] = {
        PM_BESTIAL_DERVISH, PM_ETHEREAL_DERVISH, PM_FLASHING_LAKE,
        PM_FROSTED_LAKE, PM_SMOLDERING_LAKE, PM_SPARKLING_LAKE
    };
    struct permonst *p;
    int i, j;

    for (i = 0; i < SIZE(expected); ++i)
        assert(expected[i] == 444 + i);
    assert(NUMMONS >= 450);
    for (i = PM_BESTIAL_DERVISH; i <= PM_ETHEREAL_DERVISH; ++i) {
        p = &mons[i];
        assert(p->mlevel == 28 && p->mmove == 24 && p->ac == 0);
        assert(p->geno == (G_NOGEN | G_NOCORPSE));
        for (j = 0; j < 4; ++j)
            assert(p->mattk[j].aatyp == AT_CLAW && p->mattk[j].adtyp == AD_PHYS
                   && p->mattk[j].damn == 4 && p->mattk[j].damd == 4);
        assert(p->mresists == (MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON));
        assert((p->mflags2 & (M2_PEACEFUL | M2_STRONG | M2_NOPOLY))
               == (M2_PEACEFUL | M2_STRONG | M2_NOPOLY));
        assert(p->mflags3 & M3_STATIONARY);
    }
    for (i = PM_FLASHING_LAKE; i <= PM_SPARKLING_LAKE; ++i) {
        int ad = i == PM_FLASHING_LAKE ? AD_ELEC
                 : i == PM_FROSTED_LAKE ? AD_COLD
                 : i == PM_SMOLDERING_LAKE ? AD_FIRE : AD_MAGM;
        p = &mons[i];
        assert(p->mlevel == 24 && p->mmove == 9 && p->ac == 6);
        assert(p->geno == (G_NOGEN | G_NOCORPSE));
        assert(p->mattk[0].aatyp == AT_TUCH && p->mattk[0].adtyp == AD_WET
               && p->mattk[0].damn == 8 && p->mattk[0].damd == 8);
        assert(p->mattk[1].aatyp == AT_MAGC && p->mattk[1].adtyp == ad
               && p->mattk[1].damn == 0 && p->mattk[1].damd == 6);
        assert(p->mattk[2].aatyp == AT_NONE && p->mattk[2].adtyp == ad
               && p->mattk[2].damn == 0 && p->mattk[2].damd == 6);
        assert((p->mflags1 & (M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES
                              | M1_NOLIMBS | M1_NOHEAD))
               == (M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS
                   | M1_NOHEAD));
        assert((p->mflags2 & (M2_PEACEFUL | M2_NEUTER | M2_NOPOLY))
               == (M2_PEACEFUL | M2_NEUTER | M2_NOPOLY));
        assert(p->mflags3 & M3_STATIONARY);
        assert(step10b_innate_magic(p));
        assert(step10b_passive_dice(p, 24) == 9);
    }
    assert(step10b_passive_dice(&mons[PM_BLUE_JELLY], 24) == 25);
}

static void
check_physical_morphs(void)
{
    struct permonst *p;
    int i;

    assert(PM_BLOOD_SHOWER == 450 && PM_MANY_TALONED_THING == 451);
    assert(PM_DEEP_BLUE_CUBE == 452 && PM_PITCH_BLACK_CUBE == 453);
    assert(PM_PRAYERFUL_THING == 454 && PM_HEMORRHAGIC_THING == 455);
    assert(NUMMONS >= 456);
    p = &mons[PM_BLOOD_SHOWER];
    assert(p->mlevel == 18 && p->mmove == 12 && p->ac == 0);
    assert(p->mattk[0].aatyp == AT_ENGL && p->mattk[0].damn == 6
           && p->mattk[0].damd == 6);
    assert((p->mflags1 & (M1_AMORPHOUS | M1_FLY | M1_BREATHLESS
                          | M1_MINDLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
                          | M1_UNSOLID))
           == (M1_AMORPHOUS | M1_FLY | M1_BREATHLESS | M1_MINDLESS
               | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_UNSOLID));
    assert(p->mflags3 & M3_STATIONARY);
    p = &mons[PM_MANY_TALONED_THING];
    assert(p->mlevel == 18 && p->mmove == 6 && p->ac == -6);
    assert(p->mattk[0].aatyp == AT_CLAW && p->mattk[1].aatyp == AT_CLAW
           && p->mattk[2].aatyp == AT_HUGS && p->mattk[3].aatyp == AT_DEVA);
    assert(step10b_natural_dr(p) == 6 && (p->mflags1 & M1_TUNNEL));
    for (i = PM_DEEP_BLUE_CUBE; i <= PM_PITCH_BLACK_CUBE; ++i) {
        p = &mons[i];
        assert(p->mlevel == 36 && p->mmove == 6 && p->ac == 8);
        assert(p->mattk[0].aatyp == AT_TUCH && p->mattk[0].adtyp == AD_WET);
        assert(p->mattk[1].aatyp == AT_NONE && p->mattk[1].adtyp == AD_PLYS
               && p->mattk[1].damn == 2 && p->mattk[1].damd == 4);
        assert((p->mflags1 & (M1_AMORPHOUS | M1_AMPHIBIOUS | M1_BREATHLESS
                              | M1_MINDLESS | M1_NOEYES | M1_NOLIMBS
                              | M1_NOHEAD))
               == (M1_AMORPHOUS | M1_AMPHIBIOUS | M1_BREATHLESS | M1_MINDLESS
                   | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD));
        assert(p->mflags3 & M3_STATIONARY);
    }
    assert(mons[PM_DEEP_BLUE_CUBE].mattk[0].damn == 4);
    assert(mons[PM_PITCH_BLACK_CUBE].mattk[0].damn == 8);
    p = &mons[PM_PRAYERFUL_THING];
    assert(p->mlevel == 36 && p->mmove == 6 && p->ac == -6 && p->mr == 25);
    assert(p->mattk[0].adtyp == AD_EELC && p->mattk[1].adtyp == AD_EELC);
    assert(p->mattk[2].aatyp == AT_NONE && p->mattk[2].adtyp == AD_MAGM);
    assert(p->mflags1 & M1_FLY);
    p = &mons[PM_HEMORRHAGIC_THING];
    assert(p->mlevel == 18 && p->mmove == 10 && p->ac == 4);
    assert(p->mattk[0].adtyp == AD_VAMP && p->mattk[1].adtyp == AD_WET
           && p->mattk[2].aatyp == AT_REND && p->mattk[2].adtyp == AD_DISE);
}

static void
check_remaining_morphs(void)
{
    struct permonst *p;

    assert(PM_MANY_EYED_SEEKER == 456 && PM_VOICE_IN_THE_DARK == 457);
    assert(PM_TINY_BEING_OF_LIGHT == 458 && PM_MAN_FACED_MILLIPEDE == 459);
    assert(PM_MIRRORED_MOONFLOWER == 460 && PM_CRIMSON_WRITHER == 461);
    assert(PM_RADIANT_PYRAMID == 462 && NUMMONS >= 463);
    p = &mons[PM_MANY_EYED_SEEKER];
    assert(p->mlevel == 22 && p->mmove == 1 && p->ac == 9 && p->mr == 10);
    assert(p->mattk[0].aatyp == AT_NONE && p->mattk[0].adtyp == AD_PLYS
           && p->mattk[0].damn == 4 && p->mattk[0].damd == 4);
    assert((p->mflags1 & (M1_FLY | M1_AMPHIBIOUS | M1_NOTAKE | M1_NOLIMBS))
           == (M1_FLY | M1_AMPHIBIOUS | M1_NOTAKE | M1_NOLIMBS));
    p = &mons[PM_VOICE_IN_THE_DARK];
    assert(p->mlevel == 24 && p->mmove == 12 && p->ac == 10 && p->mr == 100);
    assert(p->mattk[0].aatyp == AT_GAZE && p->mattk[0].adtyp == AD_DRLI);
    assert((p->mflags1 & (M1_FLY | M1_BREATHLESS | M1_WALLWALK
                          | M1_NOLIMBS | M1_NOHEAD | M1_UNSOLID))
           == (M1_FLY | M1_BREATHLESS | M1_WALLWALK | M1_NOLIMBS
               | M1_NOHEAD | M1_UNSOLID));
    assert(p->mflags2 & M2_STALK && p->mflags3 & M3_STATIONARY);
    p = &mons[PM_TINY_BEING_OF_LIGHT];
    assert(p->mlevel == 33 && p->mmove == 15 && p->ac == -5 && p->mr == 100);
    assert(p->mattk[0].aatyp == AT_TUCH && p->mattk[1].aatyp == AT_TUCH
           && p->mattk[0].damn == 8 && p->mattk[0].damd == 8);
    assert((p->mflags1 & (M1_FLY | M1_BREATHLESS | M1_UNSOLID | M1_HUMANOID
                          | M1_REGEN))
           == (M1_FLY | M1_BREATHLESS | M1_UNSOLID | M1_HUMANOID | M1_REGEN));
    p = &mons[PM_MAN_FACED_MILLIPEDE];
    assert(p->mlevel == 22 && p->mmove == 4 && p->ac == -3);
    assert(p->mattk[0].aatyp == AT_BITE && p->mattk[0].adtyp == AD_DRST);
    assert(p->mattk[1].damn == 16 && p->mattk[2].damn == 16);
    assert((p->mflags1 & (M1_CONCEAL | M1_OMNIVORE | M1_SLITHY | M1_NOHANDS
                          | M1_OVIPAROUS))
           == (M1_CONCEAL | M1_OMNIVORE | M1_SLITHY | M1_NOHANDS
               | M1_OVIPAROUS));
    p = &mons[PM_MIRRORED_MOONFLOWER];
    assert(p->mlevel == 14 && p->mmove == 6 && p->ac == 0);
    assert(p->mattk[0].aatyp == AT_NONE && p->mattk[0].adtyp == AD_MAGM);
    assert(step10b_innate_reflection(p));
    assert(p->mflags3 & M3_STATIONARY);
    p = &mons[PM_CRIMSON_WRITHER];
    assert(p->mlevel == 11 && p->mmove == 9 && p->ac == 5);
    assert(p->mattk[0].adtyp == AD_VAMP && p->mattk[1].adtyp == AD_DISE);
    assert((p->mflags1 & (M1_CARNIVORE | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS
                          | M1_NOTAKE))
           == (M1_CARNIVORE | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS | M1_NOTAKE));
    assert(p->mflags3 & M3_STATIONARY);
    p = &mons[PM_RADIANT_PYRAMID];
    assert(p->mlevel == 18 && p->mmove == 8 && p->ac == 0);
    assert(p->mattk[0].aatyp == AT_BITE && p->mattk[0].adtyp == AD_STUN
           && p->mattk[0].damn == 4 && p->mattk[0].damd == 6);
    assert(p->mresists == (MR_ACID | MR_COLD | MR_FIRE));
    assert(step10b_natural_dr(p) == 8 && (p->mflags3 & M3_STATIONARY));
}

static void
check_complex_support(void)
{
    struct monst mon = { 0 };
    struct obj mainhand = { 0 }, first = { 0 }, second = { 0 }, third = { 0 };
    struct obj *chosen[4];
    struct permonst *p;
    int i, j;

    assert(PM_KUKER == 463 && PM_LURKING_ONE == 464 && NUMMONS >= 465);
    p = &mons[PM_KUKER];
    assert(p->mlevel == 18 && p->mmove == 7 && p->ac == 6 && p->mr == 30);
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[1].aatyp == AT_WEAP);
    assert(p->mattk[0].damn == 4 && p->mattk[0].damd == 8
           && p->mattk[1].damn == 4 && p->mattk[1].damd == 8);
    assert(p->mattk[2].aatyp == AT_MAGC && p->mattk[2].adtyp == AD_CLRC
           && p->mattk[2].damn == 0 && p->mattk[2].damd == 6);
    assert(p->mresists == (MR_FIRE | MR_ELEC | MR_COLD | MR_SLEEP | MR_POISON));
    assert((p->mflags2 & (M2_PEACEFUL | M2_COLLECT | M2_STALK | M2_STRONG
                          | M2_MINION | M2_NOPOLY))
           == (M2_PEACEFUL | M2_COLLECT | M2_STALK | M2_STRONG | M2_MINION
               | M2_NOPOLY));
    assert(p->mflags1 & M1_OMNIVORE);
    assert(step10b_innate_magic(p));
    assert(step10b_spell_cooldown(p, 7) == 0);
    assert(step10b_species_spell(p, 0, 0) == MCAST_CONFUSE_YOU);
    assert(step10b_species_spell(p, 0, 1) == MCAST_RILMANI_MAKE_VISIBLE);
    assert(step10b_species_spell(p, 0, 2) == MCAST_KUKER_EVIL_EYE);
    assert(step10b_species_spell(p, 0, 3) == MCAST_CURSE_ITEMS);
    assert(step10b_species_spell(p, 0, 4) == MCAST_KUKER_PROTECTION);
    assert(step10b_species_spell(p, 0, 5) == MCAST_PUNISHMENT);

    p = &mons[PM_LURKING_ONE];
    assert(p->mlevel == 45 && p->mmove == 10 && p->ac == -8 && p->mr == 90);
    for (i = 0; i < 4; ++i) {
        assert(p->mattk[i].aatyp == AT_WEAP && p->mattk[i].adtyp == AD_PHYS
               && p->mattk[i].damn == 1 && p->mattk[i].damd == 8);
        assert(mith_multiweapon_slot(p, i) == i);
    }
    assert(p->mattk[4].aatyp == AT_TENT && p->mattk[4].damn == 4
           && p->mattk[4].damd == 8);
    assert(p->mattk[5].aatyp == AT_GAZE && p->mattk[5].adtyp == AD_ELEC
           && p->mattk[5].damn == 4 && p->mattk[5].damd == 8);
    assert((p->mflags1 & (M1_FLY | M1_HUMANOID)) == (M1_FLY | M1_HUMANOID));
    assert((p->mflags2 & (M2_ROCKTHROW | M2_STRONG | M2_NOPOLY))
           == (M2_ROCKTHROW | M2_STRONG | M2_NOPOLY));
    assert(p->mflags1 & M1_OMNIVORE);
    assert(p->mflags3 & M3_WAITFORU);
    assert(step10b_innate_magic(p));

    mon.data = p;
    mon.m_lev = 45;
    mon.minvent = &mainhand;
    mon.mw = &mainhand;
    mainhand.otyp = LONG_SWORD;
    mainhand.oclass = WEAPON_CLASS;
    mainhand.nobj = &first;
    first.otyp = SCIMITAR;
    first.oclass = WEAPON_CLASS;
    first.nobj = &second;
    second.otyp = MACE;
    second.oclass = WEAPON_CLASS;
    second.nobj = &third;
    third.otyp = DAGGER;
    third.oclass = WEAPON_CLASS;
    for (i = 0; i < 4; ++i) {
        chosen[i] = mith_select_multiweapon(&mon, i);
        assert(chosen[i]);
        for (j = 0; j < i; ++j)
            assert(chosen[i] != chosen[j]);
    }
    assert(chosen[0] == &mainhand);
    /* The current inventory is rescanned on every arm: removing a selected
       object cannot leave a stale saved pointer for the next attack. */
    second.nobj = 0;
    assert(mith_select_multiweapon(&mon, 1));
    assert(mith_select_multiweapon(&mon, 2));
    assert(!mith_select_multiweapon(&mon, 3));
    mainhand.nobj = 0;
    assert(!mith_select_multiweapon(&mon, 1));
    mon.minvent = 0;
    assert(mith_select_multiweapon(&mon, 0) == &mainhand);
    assert(!mith_select_multiweapon(&mon, 1));
    mon.minvent = &mainhand;
    mainhand.nobj = &first;
    first.nobj = &second;
    second.nobj = &third;
    mon.misc_worn_check = W_ARMS;
    assert(!mith_select_multiweapon(&mon, 1));
    mon.misc_worn_check = 0;
    first.cursed = 1;
    second.oartifact = 1;
    third.otyp = TWO_HANDED_SWORD;
    assert(!mith_select_multiweapon(&mon, 1));
    third.otyp = DAGGER;
    touch_safe = FALSE;
    assert(!mith_select_multiweapon(&mon, 1));
    touch_safe = TRUE;
    assert(mith_multiweapon_slot(&mons[PM_HUMAN], 0) == -1);

    assert(step10b_eldritch_presence_kind(&mons[PM_LIVING_DOLL]) == 0);
    assert(step10b_eldritch_presence_kind(&mons[PM_BESTIAL_DERVISH]) == 2);
    assert(step10b_eldritch_presence_kind(&mons[PM_FLASHING_LAKE]) == 1);
    assert(step10b_eldritch_presence_kind(&mons[PM_TINY_BEING_OF_LIGHT]) == 0);
    assert(step10b_eldritch_presence_kind(&mons[PM_LURKING_ONE]) == 2);
    mon.data = &mons[PM_LURKING_ONE];
    assert(step10b_mark_eldritch_seen(&mon));
    assert(!step10b_mark_eldritch_seen(&mon));
    assert(mon.mspare1 & MITH_ELDRITCH_SEEN);

    for (i = PM_FLASHING_LAKE; i <= PM_SPARKLING_LAKE; ++i) {
        p = &mons[i];
        assert(!step10b_elemental_passive_ready(p, FALSE, TRUE, FALSE));
        assert(step10b_elemental_passive_ready(p, TRUE, TRUE, FALSE));
        assert(!step10b_elemental_passive_ready(p, TRUE, FALSE, FALSE));
        assert(!step10b_elemental_passive_ready(p, TRUE, TRUE, TRUE));
    }
    /* Native passives keep their original hit-or-miss trigger contract. */
    assert(step10b_elemental_passive_ready(&mons[PM_BLUE_JELLY], FALSE,
                                           TRUE, FALSE));
}

int
main(void)
{
    monst_globals_init();
    objects_globals_init();
    check_dolls();
    check_dervishes_and_lakes();
    check_physical_morphs();
    check_remaining_morphs();
    check_complex_support();
    puts("PASS Step 10B2-2 declarations, dolls, morphs, Kuker, lurking-one inventory rescans and passive trigger contracts");
    return 0;
}
