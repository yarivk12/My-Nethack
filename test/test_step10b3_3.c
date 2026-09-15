#include "hack.h"
#include "artifact.h"
#include <assert.h>
#include <stdio.h>

static const struct silver_key_domain legal_domain = {
    { 1, 111 }, 10, 11, { 10, 8 }
};

int
main(void)
{
    d_level target, chosen = { 99, 99 };
    d_level candidates[] = {
        { 4, 1 }, { 10, 7 }, { 11, 13 }, { 10, 8 }, { 1, 111 }
    };

    objects_globals_init();

    assert(NUMMONS == 503);
    assert(NUM_OBJECTS == 547);
    assert(AFTER_LAST_ARTIFACT == 47);
    assert(NROFARTIFACTS == 46);
    assert(MAX_GLYPH == 12410);
    assert(ART_NECRONOMICON == 45);
    assert(ART_SILVER_KEY == 46);
    assert(NROFARTIFACTS <= 127);

    assert(objects[SPE_SECRETS].oc_class == SPBOOK_CLASS);
    assert(objects[UNIVERSAL_KEY].oc_class == TOOL_CLASS);

    assert(necronomicon_operation_pw_cost(NECRONOMICON_SUMMON_BYAKHEE) == 20);
    assert(necronomicon_operation_pw_cost(NECRONOMICON_SUMMON_NIGHTGAUNT) == 10);
    assert(necronomicon_operation_pw_cost(NECRONOMICON_DETECT_MONSTERS)
           == SPELL_LEV_PW(1));
    assert(necronomicon_operation_pw_cost(NECRONOMICON_HEALTH_RECOVERY) == 0);
    assert(necronomicon_operation_pw_cost(0) == -1);
    assert(necronomicon_operation_pw_cost(99) == -1);
    assert(necronomicon_operation_monster(NECRONOMICON_SUMMON_BYAKHEE)
           == PM_BYAKHEE);
    assert(necronomicon_operation_monster(NECRONOMICON_SUMMON_NIGHTGAUNT)
           == PM_NIGHTGAUNT);
    assert(necronomicon_operation_monster(NECRONOMICON_DETECT_MONSTERS)
           == NON_PM);

    target.dnum = 10;
    target.dlevel = 1;
    assert(silver_key_destination_valid(&target, &legal_domain));
    target.dlevel = 7;
    assert(silver_key_destination_valid(&target, &legal_domain));
    target.dlevel = 8;
    assert(silver_key_destination_valid(&target, &legal_domain));

    target.dnum = 11;
    target.dlevel = 13;
    assert(silver_key_destination_valid(&target, &legal_domain));
    target.dlevel = 14;
    assert(!silver_key_destination_valid(&target, &legal_domain));

    target.dnum = 10;
    target.dlevel = 8;
    assert(silver_key_destination_valid(&target, &legal_domain));
    target.dlevel = 9;
    assert(!silver_key_destination_valid(&target, &legal_domain));

    target.dnum = 1;
    target.dlevel = 111;
    assert(silver_key_destination_valid(&target, &legal_domain));
    target.dlevel = 110;
    assert(!silver_key_destination_valid(&target, &legal_domain));

    target.dnum = 4; /* unrelated/endgame/progression fixture */
    target.dlevel = 1;
    assert(!silver_key_destination_valid(&target, &legal_domain));
    target.dnum = -1;
    assert(!silver_key_destination_valid(&target, &legal_domain));
    assert(!silver_key_destination_valid(NULL, &legal_domain));
    assert(!silver_key_destination_valid(&target, NULL));

    assert(silver_key_choose_destination(candidates, 5, 1,
                                         &legal_domain, &chosen));
    assert(chosen.dnum == 10 && chosen.dlevel == 7);
    assert(silver_key_choose_destination(candidates, 5, 4,
                                         &legal_domain, &chosen));
    assert(chosen.dnum == 1 && chosen.dlevel == 111);
    assert(!silver_key_choose_destination(candidates, 5, 0,
                                          &legal_domain, &chosen));
    assert(chosen.dnum == -1 && chosen.dlevel == -1);
    assert(!silver_key_choose_destination(candidates, 5, 99,
                                          &legal_domain, &chosen));
    assert(!silver_key_choose_destination(NULL, 0, 0,
                                          &legal_domain, &chosen));

    {
        struct silver_key_domain malformed = legal_domain;
        malformed.lost_cities_dnum = malformed.neutral_dnum;
        target.dnum = 10;
        target.dlevel = 1;
        assert(!silver_key_destination_valid(&target, &malformed));

        malformed = legal_domain;
        malformed.approach.dlevel = 0;
        assert(!silver_key_destination_valid(&target, &malformed));
        malformed.approach.dlevel = MAXLEVEL + 1;
        assert(!silver_key_destination_valid(&target, &malformed));
    }

    puts("PASS Step 10B3-3 artifact IDs, Necronomicon operations, and Silver Key portal core");
    return 0;
}
