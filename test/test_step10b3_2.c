#include "hack.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
{
    objects_globals_init();

    assert(NUMMONS == 503);
    assert(NUM_OBJECTS == 547);
    assert(AFTER_LAST_ARTIFACT == 47);
    assert(NROFARTIFACTS == 46);
    assert(MAX_GLYPH == 12410);
    assert(FIRST_STEP10B_OBJECT == 526 && LAST_STEP10B_OBJECT == 546);

    assert(ART_FIRST_KEY_OF_NEUTRALITY == 37);
    assert(ART_SECOND_KEY_OF_NEUTRALITY == 38);
    assert(ART_THIRD_KEY_OF_NEUTRALITY == 39);
    assert(ART_INFINITY_S_MIRRORED_ARC == 40);
    assert(ART_STAFF_OF_TWELVE_MIRRORS == 41);
    assert(ART_SANSARA_MIRROR == 42);
    assert(ART_MIRROR_BRAND == 43);
    assert(ART_SOULMIRROR == 44);
    assert(NROFARTIFACTS <= 127);

    assert(step10b_alhoon_key_choice(FALSE, FALSE) == STEP10B_KEY_SECOND);
    assert(step10b_alhoon_key_choice(TRUE, FALSE) == STEP10B_KEY_THIRD);
    assert(step10b_alhoon_key_choice(FALSE, TRUE) == STEP10B_KEY_SECOND);
    assert(step10b_alhoon_key_choice(TRUE, TRUE) == STEP10B_KEY_ORDINARY);

    assert(objects[UNIVERSAL_KEY].oc_class == TOOL_CLASS);
    assert(objects[DOUBLE_LIGHTSABER].oc_bimanual);
    assert(objects[KHAKKHARA].oc_class == WEAPON_CLASS);
    assert(objects[MIRRORBLADE].oc_class == WEAPON_CLASS);

    puts("PASS Step 10B3-2 artifact IDs, capacity, and key matrix");
    return 0;
}
