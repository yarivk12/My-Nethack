#include "hack.h"
#include <assert.h>
#include <stdio.h>
#define MCASTU_ENUM
enum step10b_mcast_spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM
#include "step10b2_1_selectors.h"

int
main(void)
{
    static const int expected[] = {
        PM_PLUMACH_RILMANI, PM_FERRUMACH_RILMANI,
        PM_CUPRILACH_RILMANI, PM_ARGENACH_RILMANI,
        PM_AURUMACH_RILMANI, PM_AMM_KAMEREL, PM_HUDOR_KAMEREL,
        PM_SHARAB_KAMEREL, PM_ARA_KAMEREL, PM_ARGENTUM_GOLEM
    };
    struct permonst *p;
    int i;

    monst_globals_init();
    objects_globals_init();
    assert(PM_OGRE_MAGE == 430 && NUMMONS >= 441);
    for (i = 0; i < SIZE(expected); ++i)
        assert(expected[i] == 431 + i && mons[expected[i]].pmidx == expected[i]);

    p = &mons[PM_PLUMACH_RILMANI];
    assert(p->mlevel == 4 && p->mmove == 8 && p->ac == 4 && p->mr == 0);
    assert(p->mresists == (MR_ACID | MR_POISON | MR_ELEC));
    assert((p->mflags1 & (M1_HUMANOID | M1_THICK_HIDE | M1_POIS))
           == (M1_HUMANOID | M1_THICK_HIDE | M1_POIS));
    assert((p->mflags2 & (M2_STRONG | M2_MINION | M2_NOPOLY))
           == (M2_STRONG | M2_MINION | M2_NOPOLY));
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[0].damn == 1 && p->mattk[0].damd == 4);
    assert(p->mattk[1].aatyp == AT_MAGC && p->mattk[1].adtyp == AD_SPEL && p->mattk[1].damd == 4);
    p = &mons[PM_FERRUMACH_RILMANI];
    assert(p->mlevel == 6 && p->mmove == 10 && p->ac == 3);
    assert(p->mattk[0].damn == 1 && p->mattk[0].damd == 8 && p->mattk[1].damd == 4);
    p = &mons[PM_CUPRILACH_RILMANI];
    assert(p->mlevel == 8 && p->mmove == 18 && p->ac == 0 && (p->mflags2 & M2_STALK));
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[1].aatyp == AT_WEAP
           && p->mattk[2].aatyp == AT_MAGC && p->mattk[2].damd == 4);
    p = &mons[PM_ARGENACH_RILMANI];
    assert(p->mlevel == 9 && p->mmove == 15 && p->ac == 5 && (p->mflags1 & M1_SEE_INVIS));
    assert(p->mattk[0].damd == 10 && p->mattk[1].damd == 6);
    p = &mons[PM_AURUMACH_RILMANI];
    assert(p->mlevel == 12 && p->mmove == 15 && p->ac == 5);
    assert(p->mresists == (MR_COLD | MR_FIRE | MR_ACID | MR_POISON | MR_ELEC));
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[1].aatyp == AT_WEAP
           && p->mattk[2].aatyp == AT_WEAP && p->mattk[3].adtyp == AD_SPEL);

    p = &mons[PM_AMM_KAMEREL];
    assert(p->mlevel == 3 && p->mmove == 12 && p->ac == 10);
    assert(p->mresists == (MR_STONE | MR_ELEC));
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[1].adtyp == AD_CLRC);
    p = &mons[PM_HUDOR_KAMEREL];
    assert(p->mlevel == 6 && p->mmove == 12 && p->ac == 0);
    assert(p->mattk[0].aatyp == AT_TUCH && p->mattk[0].adtyp == AD_WET
           && p->mattk[0].damn == 2 && p->mattk[0].damd == 6);
    assert((p->mflags1 & (M1_SWIM | M1_AMPHIBIOUS)) == (M1_SWIM | M1_AMPHIBIOUS));
    assert(p->mflags3 & M3_STATIONARY);
    p = &mons[PM_SHARAB_KAMEREL];
    assert(p->mlevel == 10 && (p->mflags1 & M1_FLY));
    assert((p->mflags3 & (M3_WAITFORU | M3_CLOSE))
           == (M3_WAITFORU | M3_CLOSE));
    assert(p->mattk[0].aatyp == AT_CLAW && p->mattk[1].aatyp == AT_CLAW
           && p->mattk[2].adtyp == AD_CLRC);
    p = &mons[PM_ARA_KAMEREL];
    assert(p->mlevel == 15 && p->mmove == 9 && p->ac == 4);
    assert(p->mresists == (MR_SLEEP | MR_POISON | MR_ACID | MR_STONE | MR_ELEC));
    assert(p->mflags1 & M1_BREATHLESS);
    assert(p->mattk[1].adtyp == AD_SAMU && (p->mflags3 & M3_WANTSAMUL));
    p = &mons[PM_ARGENTUM_GOLEM];
    assert(p->mlevel == 18 && p->mmove == 9 && p->ac == 6 && p->mr == 100);
    assert(p->mresists == (MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON));
    assert((p->mflags1 & (M1_BREATHLESS | M1_MINDLESS | M1_THICK_HIDE))
           == (M1_BREATHLESS | M1_MINDLESS | M1_THICK_HIDE));
    assert((p->mflags2 & (M2_HOSTILE | M2_STRONG | M2_NOPOLY))
           == (M2_HOSTILE | M2_STRONG | M2_NOPOLY));
    assert(p->mflags3 & M3_WAITFORU);
    assert(p->mattk[0].aatyp == AT_WEAP && p->mattk[0].damn == 4 && p->mattk[0].damd == 8);
    assert(p->mattk[1].aatyp == AT_ARRW && p->mattk[1].adtyp == AD_SLVR);

    for (i = 0; i < SIZE(expected); ++i)
        assert(mons[expected[i]].geno == (G_NOGEN | G_NOCORPSE));
    assert(step10b_neutral_montype(0, 0, 99, 0) == STEP10B_NEUTRAL_QUADRUPED);
    assert(step10b_neutral_montype(0, 0, 99, 1) == PM_HORSE);
    assert(step10b_neutral_montype(1, 9, 99, 0) == PM_ARGENACH_RILMANI);
    assert(step10b_neutral_montype(1, 29, 99, 0) == PM_CUPRILACH_RILMANI);
    assert(step10b_neutral_montype(1, 59, 99, 0) == PM_CUPRILACH_RILMANI);
    assert(step10b_neutral_montype(1, 60, 99, 0) == PM_PLUMACH_RILMANI);
    assert(step10b_neutral_montype(2, 4, 99, 0) == PM_ARA_KAMEREL);
    assert(step10b_neutral_montype(2, 14, 99, 0) == PM_SHARAB_KAMEREL);
    assert(step10b_neutral_montype(2, 15, 99, 0) == PM_AMM_KAMEREL);
    assert(step10b_neutral_montype(3, 0, 99, 2) == STEP10B_NEUTRAL_QUADRUPED);
    assert(step10b_neutral_montype(3, 0, 99, 3)
           == PM_SHATTERED_ZIGGURAT_CULTIST);
    assert(step10b_neutral_montype(4, 0, 99, 0) == PM_PLAINS_CENTAUR);
    assert(step10b_sum_montype(4, 99) == PM_AURUMACH_RILMANI);
    assert(step10b_sum_montype(14, 99) == PM_ARGENACH_RILMANI);
    assert(step10b_sum_montype(34, 99) == PM_CUPRILACH_RILMANI);
    assert(step10b_sum_montype(64, 99) == PM_CUPRILACH_RILMANI);
    assert(step10b_sum_montype(65, 99) == PM_PLUMACH_RILMANI);
    assert(step10b_neutral_squad(0, 1, 0) == PM_FERRUMACH_RILMANI);
    assert(step10b_neutral_squad(0, 79, 0) == PM_FERRUMACH_RILMANI);
    assert(step10b_neutral_squad(0, 80, 0) == PM_IRON_GOLEM);
    assert(step10b_neutral_squad(20, 94, 0) == PM_IRON_GOLEM);
    assert(step10b_neutral_squad(20, 98, 0) == PM_ARGENTUM_GOLEM);
    assert(step10b_neutral_squad(20, 99, 0) == PM_CUPRILACH_RILMANI);
    for (i = 0; i < 4; ++i)
        assert(step10b_neutral_squad(20, 100, i)
               == (i == 0 ? PM_FERRUMACH_RILMANI : i == 1 ? PM_IRON_GOLEM
                   : i == 2 ? PM_ARGENTUM_GOLEM : PM_CUPRILACH_RILMANI));
    assert(step10b_natural_dr(&mons[PM_PLUMACH_RILMANI]) == 4);
    assert(step10b_natural_dr(&mons[PM_FERRUMACH_RILMANI]) == 5);
    assert(step10b_natural_dr(&mons[PM_CUPRILACH_RILMANI]) == 0);
    assert(step10b_natural_dr(&mons[PM_ARGENACH_RILMANI]) == 4);
    assert(step10b_natural_dr(&mons[PM_AURUMACH_RILMANI]) == 4);
    assert(step10b_natural_dr(&mons[PM_ARA_KAMEREL]) == 9);
    assert(step10b_natural_dr(&mons[PM_ARGENTUM_GOLEM]) == 9);
    for (i = PM_AMM_KAMEREL; i <= PM_ARA_KAMEREL; ++i)
        assert(step10b_innate_reflection(&mons[i]));
    assert(step10b_innate_reflection(&mons[PM_ARGENTUM_GOLEM]));
    assert(!step10b_innate_reflection(&mons[PM_IRON_GOLEM]));
    assert(step10b_innate_magic(&mons[PM_AURUMACH_RILMANI]));
    assert(step10b_innate_magic(&mons[PM_ARA_KAMEREL]));
    assert(!step10b_innate_magic(&mons[PM_ARGENACH_RILMANI]));
    assert(step10b_spell_cooldown(&mons[PM_AURUMACH_RILMANI], 7) == 0);
    assert(step10b_spell_cooldown(&mons[PM_ARGENACH_RILMANI], 7) == 7);
    assert(step10b_species_spell(&mons[PM_PLUMACH_RILMANI], 0, 0)
           == MCAST_RILMANI_SOLID_FOG);
    assert(step10b_species_spell(&mons[PM_FERRUMACH_RILMANI], 0, 0)
           == MCAST_RILMANI_SOLID_FOG);
    assert(step10b_species_spell(&mons[PM_FERRUMACH_RILMANI], 1, 0)
           == MCAST_RILMANI_HAIL_FLURY);
    for (i = 0; i < 6; ++i)
        assert(step10b_species_spell(&mons[PM_CUPRILACH_RILMANI], 0, i)
               == (i == 0 ? MCAST_RILMANI_DRAIN_LIFE
                   : i == 1 ? MCAST_RILMANI_ACID_BLAST
                   : i == 2 ? MCAST_RILMANI_SOLID_FOG
                   : i == 3 ? MCAST_DISAPPEAR
                   : i == 4 ? MCAST_RILMANI_POISON_GAS
                            : MCAST_RILMANI_MAKE_VISIBLE));
    assert(step10b_species_spell(&mons[PM_ARGENACH_RILMANI], 1, 0)
           == MCAST_RILMANI_SILVER_RAYS);
    assert(step10b_species_spell(&mons[PM_ARGENACH_RILMANI], 0, 3)
           == MCAST_RILMANI_MAKE_VISIBLE);
    assert(step10b_species_spell(&mons[PM_AURUMACH_RILMANI], 1, 0)
           == MCAST_RILMANI_GOLDEN_WAVE);
    assert(step10b_species_spell(&mons[PM_AURUMACH_RILMANI], 0, 6)
           == MCAST_RILMANI_PRISMATIC_SPRAY);
    assert(step10b_species_spell(&mons[PM_AMM_KAMEREL], 0, 0)
           == MCAST_OPEN_WOUNDS);
    assert(step10b_species_spell(&mons[PM_HUDOR_KAMEREL], 0, 0)
           == MCAST_OPEN_WOUNDS);
    assert(step10b_species_spell(&mons[PM_SHARAB_KAMEREL], 0, 0)
           == MCAST_PSI_BOLT);
    assert(step10b_species_spell(&mons[PM_ARA_KAMEREL], 0, 0)
           == MCAST_OPEN_WOUNDS);
    assert(step10b_species_spell(&mons[PM_OGRE_MAGE], 0, 0) == -1);
    assert(step10b_backstab_die(&mons[PM_CUPRILACH_RILMANI], 8, TRUE) == 12);
    assert(!step10b_backstab_die(&mons[PM_CUPRILACH_RILMANI], 8, FALSE));
    assert(!step10b_backstab_die(&mons[PM_ARGENACH_RILMANI], 9, TRUE));
    assert(mith_offhand_attack(&mons[PM_CUPRILACH_RILMANI], 1));
    assert(!mith_offhand_attack(&mons[PM_CUPRILACH_RILMANI], 0));
    puts("PASS Step 10B2-1 append-only declarations");
    return 0;
}
