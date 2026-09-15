#include "hack.h"
#include <assert.h>
#include <stdio.h>

#define MCASTU_ENUM
enum step10b2_4_spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM

struct instance_globals_saved_l svl;

int
mith_roll_dr(struct monst *mon)
{
    nhUse(mon);
    return 0;
}

int
main(void)
{
    struct monst mon = { 0 };
    struct obj weapon = { 0 };

    monst_globals_init();
    objects_globals_init();

    assert(PM_BLASPHEMOUS_LURKER == 496);
    assert(PM_ALHOON == 497 && PM_CENTER_OF_ALL == 498);
    assert(PM_FATHER_DAGON == 499 && PM_MOTHER_HYDRA == 500);
    assert(PM_GREAT_CTHULHU == 501 && PM_WITCH_S_FAMILIAR == 502);
    assert(NUMMONS == 503);
    assert(NUM_OBJECTS == 547 && AFTER_LAST_ARTIFACT == 47);
    assert(MAX_GLYPH == 12410);

    assert(mons[PM_WITCH_S_FAMILIAR].mlet == S_RODENT);
    assert(mons[PM_WITCH_S_FAMILIAR].mlevel == 5);
    assert(mons[PM_WITCH_S_FAMILIAR].mmove == 6);
    assert(mons[PM_WITCH_S_FAMILIAR].ac == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].mr == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].maligntyp == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].geno == G_NOGEN);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[0].aatyp == AT_BITE);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[0].adtyp == AD_VAMP);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[0].damn == 1);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[0].damd == 3);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[1].aatyp == AT_MAGC);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[1].adtyp == AD_SPEL);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[1].damn == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].mattk[1].damd == 6);
    assert(mons[PM_WITCH_S_FAMILIAR].cwt == 20);
    assert(mons[PM_WITCH_S_FAMILIAR].cnutrit == 50);
    assert(mons[PM_WITCH_S_FAMILIAR].msound == MS_SQEEK);
    assert(mons[PM_WITCH_S_FAMILIAR].msize == MZ_TINY);
    assert(mons[PM_WITCH_S_FAMILIAR].mresists == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].mconveys == 0);
    assert(mons[PM_WITCH_S_FAMILIAR].mflags1
           == (M1_ANIMAL | M1_OMNIVORE));
    assert(mons[PM_WITCH_S_FAMILIAR].mflags2
           == (M2_FEMALE | M2_NOPOLY));
    assert(mons[PM_WITCH_S_FAMILIAR].mflags3 == M3_INFRAVISIBLE);
    assert(step10b_spell_cooldown(&mons[PM_WITCH_S_FAMILIAR], 7) == 0);
    assert(step10b_species_spell(&mons[PM_WITCH_S_FAMILIAR], 0, 0)
           == MCAST_OPEN_WOUNDS);
    assert(step10b_is_witch(&mons[PM_APPRENTICE_WITCH]));
    assert(step10b_is_witch(&mons[PM_WITCH]));
    assert(step10b_is_witch(&mons[PM_COVEN_LEADER]));
    assert(!step10b_is_witch(&mons[PM_WITCH_S_FAMILIAR]));
    assert(!step10b_is_witch(&mons[PM_GIANT_RAT]));

    {
        struct monst witch = { 0 }, familiar = { 0 }, other = { 0 };

        witch.data = &mons[PM_WITCH];
        witch.m_id = 1234U;
        witch.m_lev = 10;
        witch.mhp = 31;
        witch.mhpmax = 47;
        witch.mpeaceful = 1;
        familiar.data = &mons[PM_WITCH_S_FAMILIAR];
        step10b_link_witch_familiar(&witch, &familiar);
        assert(familiar.m_lev == 10 && familiar.mhp == 31);
        assert(familiar.mhpmax == 47 && familiar.mpeaceful == 1);
        assert((unsigned long) familiar.mspare1 == 1234UL);
        svl.level.monlist = &familiar;
        assert(!step10b_witch_needs_familiar(&witch));
        familiar.mspare1 = 999L;
        assert(step10b_witch_needs_familiar(&witch));
        other.data = &mons[PM_HUMAN];
        assert(!step10b_witch_needs_familiar(&other));
    }

    assert(mons[PM_ALHOON].mattk[1].adtyp == AD_DRIN);
    assert(mons[PM_ALHOON].mattk[4].adtyp == AD_SPEL);
    assert(step10b_natural_dr(&mons[PM_ALHOON]) == 8);
    assert(step10b_spell_cooldown(&mons[PM_ALHOON], 7) == 0);
    assert(step10b_eldritch_presence_kind(&mons[PM_ALHOON]) == 2);

    mon.data = &mons[PM_ALHOON];
    weapon.otyp = SPEAR;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 5);
    weapon.otyp = MACE;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 20);

    assert(step10b_alhoon_key_choice(FALSE, FALSE) == STEP10B_KEY_SECOND);
    assert(step10b_alhoon_key_choice(TRUE, FALSE) == STEP10B_KEY_THIRD);
    assert(step10b_alhoon_key_choice(FALSE, TRUE) == STEP10B_KEY_SECOND);
    assert(step10b_alhoon_key_choice(TRUE, TRUE) == STEP10B_KEY_ORDINARY);

    assert(step10b_center_candidate(STEP10B_CENTER_NEUTRAL, 0, 0)
           == PM_CENTER_OF_ALL);
    assert(step10b_center_candidate(STEP10B_CENTER_LOST_CITIES, 0, 0)
           == PM_CENTER_OF_ALL);
    assert(step10b_center_candidate(STEP10B_CENTER_NEUTRAL, 1, 0) == NON_PM);
    assert(step10b_center_candidate(0, 0, 0) == NON_PM);
    assert(step10b_center_candidate(STEP10B_CENTER_NEUTRAL, 0, G_GONE)
           == NON_PM);
    assert(step10b_center_peaceful(FALSE));
    assert(!step10b_center_peaceful(TRUE));
    assert(step10b_innate_reflection(&mons[PM_CENTER_OF_ALL]));
    assert(step10b_innate_magic(&mons[PM_CENTER_OF_ALL]));
    assert(step10b_species_spell(&mons[PM_CENTER_OF_ALL], 1, 0)
           == MCAST_RILMANI_GOLDEN_WAVE);
    assert(step10b_species_spell(&mons[PM_CENTER_OF_ALL], 0, 6)
           == MCAST_RILMANI_PRISMATIC_SPRAY);

    assert(mons[PM_FATHER_DAGON].geno == (G_NOGEN | G_NOHELL));
    assert(mons[PM_MOTHER_HYDRA].geno == (G_NOGEN | G_NOHELL));
    assert(mith_offhand_attack(&mons[PM_FATHER_DAGON], 1));
    assert(mith_offhand_attack(&mons[PM_MOTHER_HYDRA], 5));
    assert(!mith_offhand_attack(&mons[PM_MOTHER_HYDRA], 1));
    assert(step10b_eldritch_presence_kind(&mons[PM_FATHER_DAGON]) == 2);
    assert(step10b_eldritch_presence_kind(&mons[PM_MOTHER_HYDRA]) == 2);

    assert(mons[PM_GREAT_CTHULHU].mattk[1].aatyp == AT_WDGZ);
    assert(mons[PM_GREAT_CTHULHU].mattk[1].adtyp == AD_WISD);
    assert(mons[PM_GREAT_CTHULHU].mattk[2].adtyp == AD_POSN);
    assert(step10b_natural_dr(&mons[PM_GREAT_CTHULHU]) == 21);
    assert(step10b_cthulhu_psychic_damage(75, FALSE, FALSE) == 75);
    assert(step10b_cthulhu_psychic_damage(75, TRUE, FALSE) == 38);
    assert(step10b_cthulhu_psychic_damage(75, TRUE, TRUE) == 19);
    assert(step10b_wisdom_drain_amount(18, 10, 3) == 10);
    assert(step10b_wisdom_drain_amount(7, 10, 3) == 4);
    assert(step10b_wisdom_drain_amount(3, 10, 3) == 0);

    puts("PASS Step 10B2-4 declarations and bounded helpers");
    return 0;
}
