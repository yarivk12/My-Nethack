#include "hack.h"
#include <assert.h>
#include <stdio.h>

#define MCASTU_ENUM
enum step10b2_3_spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM

int
main(void)
{
    static const int ids[] = {
        PM_SMALL_GOAT_SPAWN, PM_GOAT_SPAWN, PM_GIANT_GOAT_SPAWN,
        PM_BLESSED, PM_MOUTH_OF_THE_GOAT, PM_APPRENTICE_WITCH, PM_WITCH,
        PM_COVEN_LEADER, PM_THE_GOOD_NEIGHBOR, PM_HMNYW_PHARAOH,
        PM_MIGO_WORKER, PM_MIGO_SOLDIER, PM_MIGO_PHILOSOPHER, PM_MIGO_QUEEN,
        PM_BYAKHEE, PM_DARK_YOUNG, PM_DEEP_DWELLER, PM_DEMINYMPH,
        PM_GNOLL_GHOUL, PM_GUG, PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES,
        PM_NIGHTGAUNT, PM_OREAD, PM_MINOTAUR_PRIESTESS,
        PM_PRIEST_OF_AN_UNKNOWN_GOD, PM_SHOGGOTH, PM_STAR_SPAWN,
        PM_SHATTERED_ZIGGURAT_CULTIST, PM_SHATTERED_ZIGGURAT_KNIGHT,
        PM_SHATTERED_ZIGGURAT_WIZARD, PM_HUNTING_HORROR,
        PM_BLASPHEMOUS_LURKER
    };
    int i;

    monst_globals_init();
    objects_globals_init();

    assert(PM_LURKING_ONE == 464 && NUMMONS == 503);
    for (i = 0; i < SIZE(ids); ++i)
        assert(ids[i] == 465 + i);
    assert(NUM_OBJECTS == 547 && AFTER_LAST_ARTIFACT == 47);
    assert(MAX_GLYPH == 12410);

    assert(mons[PM_SMALL_GOAT_SPAWN].mattk[2].aatyp == AT_BUTT);
    assert(mons[PM_BLESSED].mattk[0].aatyp == AT_WDGZ);
    assert(mons[PM_MOUTH_OF_THE_GOAT].mattk[2].adtyp == AD_DGST);
    assert(mons[PM_THE_GOOD_NEIGHBOR].mattk[0].aatyp == AT_REACH2);
    assert(mons[PM_THE_GOOD_NEIGHBOR].mattk[0].adtyp == AD_SHRD);
    assert(mons[PM_MIGO_SOLDIER].mattk[0].adtyp == AD_MIST);
    assert(mons[PM_MIGO_QUEEN].mattk[1].adtyp == AD_PLYS);
    assert(mons[PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES].mattk[0].adtyp
           == AD_ILUR);
    assert(mons[PM_PRIEST_OF_AN_UNKNOWN_GOD].mattk[0].aatyp == AT_NONE);
    assert(mons[PM_PRIEST_OF_AN_UNKNOWN_GOD].mattk[0].adtyp == AD_UNKN);
    assert(mons[PM_SHOGGOTH].mattk[3].aatyp == AT_NONE);
    assert(mons[PM_STAR_SPAWN].mattk[3].adtyp == AD_PSON);
    assert(mons[PM_HUNTING_HORROR].geno == (G_GENO | G_NOHELL
                                             | G_LGROUP | 1));
    assert(mons[PM_BLASPHEMOUS_LURKER].mattk[4].adtyp == AD_BLAS);

    assert(step10b_natural_dr(&mons[PM_BLESSED]) == 5);
    assert(step10b_natural_dr(&mons[PM_THE_GOOD_NEIGHBOR]) == 10);
    assert(step10b_natural_dr(&mons[PM_STAR_SPAWN]) == 10);
    assert(step10b_innate_magic(&mons[PM_BLESSED]));
    assert(step10b_innate_magic(&mons[PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES]));
    assert(step10b_spell_cooldown(&mons[PM_WITCH], 7) == 0);
    assert(step10b_species_spell(&mons[PM_STAR_SPAWN], 0, 0)
           == MCAST_PSI_BOLT);
    assert(step10b_eldritch_presence_kind(&mons[PM_SMALL_GOAT_SPAWN]) == 2);
    assert(step10b_eldritch_presence_kind(&mons[PM_SHOGGOTH]) == 2);
    assert(step10b_illurien_forget_percent(0, 100) == 0);
    assert(step10b_illurien_forget_percent(1, 100) == 1);
    assert(step10b_illurien_forget_percent(50, 100) == 10);
    assert(step10b_neutral_montype(3, 0, 99, 3)
           == PM_SHATTERED_ZIGGURAT_CULTIST);
    assert(step10b_neutral_montype(1, 59, 99, 0)
           == PM_CUPRILACH_RILMANI);
    assert(step10b_sum_montype(64, 99) == PM_CUPRILACH_RILMANI);

    puts("PASS Step 10B2-3 declarations and bounded helpers");
    return 0;
}
