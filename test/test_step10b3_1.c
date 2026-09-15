#include "hack.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
{
    int i, count;

    objects_globals_init();

    fprintf(stderr, "observed NUM_OBJECTS=%d FIRST=%d LAST=%d MAX_GLYPH=%d\n",
            NUM_OBJECTS, FIRST_STEP10B_OBJECT, LAST_STEP10B_OBJECT, MAX_GLYPH);

    assert(NUMMONS == 503);
    assert(NUM_OBJECTS == 547);
    assert(AFTER_LAST_ARTIFACT == 47);
    assert(MAX_GLYPH == 12410);
    assert(FIRST_STEP10B_OBJECT == 526 && LAST_STEP10B_OBJECT == 546);

    assert(SICKLE == 526 && SCYTHE == 527 && MIRRORBLADE == 528);
    assert(KAMEREL_VAJRA == 529 && VIPERWHIP == 530 && RAKUYO == 531);
    assert(KHAKKHARA == 532 && ROUNDSHIELD == 533 && WITCH_HAT == 534);
    assert(WHITE_FACELESS_ROBE == 535 && BLACK_FACELESS_ROBE == 536);
    assert(SMOKY_VIOLET_FACELESS_ROBE == 537 && UNIVERSAL_KEY == 538);
    assert(TORCH == 539 && SHADOWLANDER_S_TORCH == 540);
    assert(DOUBLE_LIGHTSABER == 541 && EYEBALL == 542);
    assert(POT_AMNESIA == 543 && POT_SPACE_MEAD == 544);
    assert(SPE_SECRETS == 545 && LIFELESS_DOLL == 546);

    for (i = FIRST_STEP10B_OBJECT; i <= LAST_STEP10B_OBJECT; ++i) {
        assert(step10b_extension_otyp(i));
        assert(objects[i].oc_prob == 0);
    }
    assert(!step10b_extension_otyp(FREEZING_ICE));

    count = 0;
    for (i = step10b_oclass_first(WEAPON_CLASS); i != STRANGE_OBJECT;
         i = step10b_oclass_next(i, WEAPON_CLASS))
        if (step10b_extension_otyp(i))
            ++count;
    assert(count == 7);
    count = 0;
    for (i = step10b_oclass_first(ARMOR_CLASS); i != STRANGE_OBJECT;
         i = step10b_oclass_next(i, ARMOR_CLASS))
        if (step10b_extension_otyp(i))
            ++count;
    assert(count == 5);

    assert(objects[SICKLE].oc_class == WEAPON_CLASS);
    assert(objects[SICKLE].oc_weight == 20 && objects[SICKLE].oc_cost == 4);
    assert(objects[SICKLE].oc_wsdam == 4 && objects[SICKLE].oc_wldam == 1);
    assert(objects[SICKLE].oc_hitbon == -2 && objects[SICKLE].oc_skill == P_AXE);
    assert(objects[SCYTHE].oc_weight == 75 && objects[SCYTHE].oc_bimanual);
    assert(objects[MIRRORBLADE].oc_material == SILVER);
    assert(objects[KAMEREL_VAJRA].oc_material == GOLD);
    assert(objects[VIPERWHIP].oc_skill == P_WHIP);
    assert(objects[RAKUYO].oc_skill == P_SABER);
    assert(objects[KHAKKHARA].oc_skill == P_QUARTERSTAFF);

    assert(objects[ROUNDSHIELD].oc_class == ARMOR_CLASS);
    assert(objects[ROUNDSHIELD].oc_armcat == ARM_SHIELD);
    assert(objects[ROUNDSHIELD].oc_weight == 120);
    assert(objects[WITCH_HAT].oc_armcat == ARM_HELM);
    assert(objects[BLACK_FACELESS_ROBE].oc_oprop == COLD_RES);
    assert(objects[SMOKY_VIOLET_FACELESS_ROBE].oc_oprop == COLD_RES);

    assert(objects[UNIVERSAL_KEY].oc_class == TOOL_CLASS);
    assert(objects[UNIVERSAL_KEY].oc_material == SILVER);
    assert(objects[TORCH].oc_wsdam == 6 && objects[TORCH].oc_wldam == 3);
    assert(objects[DOUBLE_LIGHTSABER].oc_skill == P_QUARTERSTAFF);
    assert(objects[EYEBALL].oc_class == FOOD_CLASS);
    assert(objects[POT_AMNESIA].oc_class == POTION_CLASS);
    assert(objects[POT_SPACE_MEAD].oc_class == POTION_CLASS);
    assert(objects[SPE_SECRETS].oc_class == SPBOOK_CLASS);
    assert(objects[SPE_SECRETS].oc_level == 7);
    assert(objects[SPE_SECRETS].oc_skill == P_CLERIC_SPELL);
    assert(objects[LIFELESS_DOLL].oc_class == CHAIN_CLASS);

    assert(step10b_amnesia_percent(TRUE, FALSE) == 0);
    assert(step10b_amnesia_percent(FALSE, FALSE) == 10);
    assert(step10b_amnesia_percent(FALSE, TRUE) == 25);

    puts("PASS Step 10B3-1 object declarations and bounded helpers");
    return 0;
}
