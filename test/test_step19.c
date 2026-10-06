/* Runs inside the real STEP15_TEST engine, with deterministic menu/RNG input. */
/* Windows GUI-subsystem CRT assertions otherwise exit without stderr. */
#undef assert
#define assert(expr) do { if (!(expr)) { \
    printf("%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #expr); \
    fflush(stdout); exit(3); } } while (0)
#include "test_step19_monsters.c"
#include "test_step19_bosses.c"
#include "test_step19_conversion.c"
#include "test_step19_hand.c"
#include "test_step19_gaze.c"

static void
step19_equipment_tests(void)
{
    struct obj *a, *b;
    int i, native, old, sum;
    static const int rings[] = { RIN_GAIN_INTELLIGENCE, RIN_GAIN_WISDOM };
    static const int attrs[] = { A_INT, A_WIS };
    forge_test_clear();
    for (i = 0; i < 2; ++i) {
        int spe;
        for (spe = -3; spe <= 3; spe += 6) {
            a = forge_test_item(rings[i], 1L); a->spe = spe;
            old = ABON(attrs[i]);
            setworn(a, W_RINGL); Ring_on(a);
            assert(ABON(attrs[i]) == old + spe);
            Ring_off(a); assert(ABON(attrs[i]) == old);
            forge_test_clear();

            a = forge_test_item(rings[i], 1L); a->spe = spe;
            setworn(a, W_RINGL); Ring_on(a);
            cancel_item(a);
            assert(a->spe == 0 && ABON(attrs[i]) == old);
            Ring_off(a); assert(ABON(attrs[i]) == old);
            forge_test_clear();
        }
        a = forge_test_item(rings[i], 1L); a->spe = 3;
        old = ABON(attrs[i]);
        setworn(a, W_RINGL); Ring_on(a);
        init_isaac64(191901UL, rn2);
        /* Ordinary objects can resist draining; retry without modifying spe. */
        for (spe = 0; spe < 100 && !drain_item(a, FALSE); ++spe)
            ;
        assert(spe < 100 && a->spe == 2);
        assert(ABON(attrs[i]) == old + 2);
        Ring_off(a); assert(ABON(attrs[i]) == old);
        forge_test_clear();
    }
    native = weight_cap();
    a = forge_test_item(RIN_CARRYING, 1L);
    b = forge_test_item(RIN_CARRYING, 1L);
    a->spe = 3; b->spe = 2;
    setworn(a, W_RINGL); setworn(b, W_RINGR);
    assert(weight_cap() == native * 125L / 100L);
    b->spe = -1; assert(weight_cap() == native * 110L / 100L);
    a->spe = b->spe = -2; assert(weight_cap() == native * 80L / 100L);
    a->spe = b->spe = -127; assert(weight_cap() == 1);
    Ring_off(a); Ring_off(b); assert(weight_cap() == native);
    forge_test_clear();
    a = forge_test_item(AMULET_OF_POWER, 1L);
    setworn(a, W_AMUL); assert(EEnergy_regeneration & W_AMUL);
    setworn((struct obj *) 0, W_AMUL); assert(!(EEnergy_regeneration & W_AMUL));
    assert(socket_capacity(a) == 1 && !enhancement_eligible(a));
    sum = 0;
    for (i = FIRST_AMULET; i < FAKE_AMULET_OF_YENDOR; ++i)
        sum += objects[i].oc_prob;
    assert(sum == 1000);
    forge_test_clear();
    a = forge_test_item(SACRIFICIAL_KNIFE, 1L);
    objects[a->otyp].oc_name_known = 0; a->dknown = 1;
    assert(!strcmp(xname(a), "knife"));
    objects[a->otyp].oc_name_known = 1;
    assert(strstr(xname(a), "sacrificial knife"));
    b = forge_test_item(KNIFE, 1L);
    assert(!mergable(a, b));
    assert(enhancement_eligible(a) && socket_capacity(a) == 1);
    assert(objects[KNIFE].oc_prob + objects[SACRIFICIAL_KNIFE].oc_prob == 20);
    forge_test_clear();
    a = forge_test_item(MUMMIFIED_HAND, 1L);
    a->oartifact = ART_HAND_OF_VECNA;
    /* Restore rebuilds artifact intrinsics without the donning callback. */
    assert(!(ESick_resistance & W_ARMG));
    set_artifact_intrinsic(a, TRUE, W_ARMG);
    assert(ESick_resistance & W_ARMG);
    set_artifact_intrinsic(a, FALSE, W_ARMG);
    assert(!(ESick_resistance & W_ARMG));
    a->oartifact = 0;
    forge_test_clear();
    puts("PASS Step 19 stat ring wear/cancel/drain, carrying boundaries, power property and knife identity");
}

static void
step19_dragon_tests(void)
{
    static const int dragons[] = { PM_GRAY_DRAGON, PM_YELLOW_DRAGON,
        PM_GLOWING_DRAGON, PM_CAVE_CHROMATIC_DRAGON, PM_SHIMMERING_DRAGON,
        PM_DEEP_DRAGON, PM_RAZOR_DRAGON, PM_FILTH_DRAGON,
        PM_SHADOW_DRAGON, PM_CELESTIAL_DRAGON };
    struct obj *a, before;
    int i, id, candidates[EP_COUNT], count;
    assert(PM_YELLOW_DRAGON - PM_GRAY_DRAGON == 9);
    for (i = 0; i < SIZE(dragons); ++i) {
        int scales = dragon_armor_type(dragons[i], FALSE);
        int mail = dragon_armor_type(dragons[i], TRUE);
        assert(dragon_armor_monster(scales) == dragons[i]);
        assert(dragon_armor_monster(mail) == dragons[i]);
        a = mksobj(scales, FALSE, FALSE);
        a->spe = -3; a->cursed = 1; a->oeroded = 2; a->oerodeproof = 1;
        assert(enhancement_slot_set(a, 0, EP_STEALTH, TRUE, -1));
        a->o_enh_quality = OQ_EXCEPTIONAL;
        before = *a;
        enhancement_change_type(a, mail);
        assert(a->spe == before.spe && a->cursed && a->oeroded == 2 && a->oerodeproof);
        assert(a->o_enh_quality == before.o_enh_quality);
        assert(!memcmp(a->o_affixes, before.o_affixes, sizeof a->o_affixes));
        assert(!memcmp(a->o_sockets, before.o_sockets, sizeof a->o_sockets));
        assert(enhancement_slots_valid(a));
        obfree(a, (struct obj *) 0);
    }
    a = mksobj(SHADOW_DRAGON_SCALES, FALSE, FALSE);
    a->o_sockets[0].property = EP_SLEEP_RES;
    a->o_sockets[0].known = 1;
    before = *a;
    enhancement_change_type(a, SHADOW_DRAGON_SCALE_MAIL);
    socket_normalize(a);
    assert(!memcmp(a->o_sockets, before.o_sockets, sizeof a->o_sockets));
    count = socket_candidates(a, equipment_property(EP_SLEEP_RES)->tier, 0, candidates);
    for (id = 0; id < count; ++id) assert(candidates[id] != EP_SLEEP_RES);
    obfree(a, (struct obj *) 0);
    assert(little_to_big(PM_BABY_SHIMMERING_DRAGON) == PM_SHIMMERING_DRAGON);
    assert(!lays_eggs(&mons[PM_DEEP_DRAGON]));
    assert(!lays_eggs(&mons[PM_SHADOW_DRAGON]));
    assert(!lays_eggs(&mons[PM_CELESTIAL_DRAGON]));
    puts("PASS Step 19 canonical mappings, native offsets and transformation metadata preservation");
}

static void
step19_spell_tests(void)
{
    struct obj *a, *b;
    int i;
    forge_test_clear(); forge_test_script();
    cast_repair_armor(P_BASIC); assert(!forge_choice_next);
    a = forge_test_item(PLATE_MAIL, 1L); a->oeroded = 2; a->spe = 4;
    a->cursed = 1; setworn(a, W_ARM);
    cast_repair_armor(P_BASIC);
    assert(a->oeroded == 1 && a->spe == 4 && a->cursed);
    b = forge_test_item(IRON_SHOES, 1L); b->oeroded = 2;
    setworn(b, W_ARMF);
    forge_test_choose("Repair which armor?", 0, 0);
    cast_repair_armor(P_BASIC);
    assert(a->oeroded == 1 && b->oeroded == 2);
    cast_repair_armor(P_UNSKILLED);
    assert(a->oeroded + b->oeroded == 2);
    setworn((struct obj *) 0, W_ARM); setworn((struct obj *) 0, W_ARMF);
    forge_test_clear();
    a = forge_test_item(UNICORN_HORN, 1L);
    a = oname(a, artiname(ART_NIGHTHORN), ONAME_NO_FLAGS); a->blessed = 1;
    ABASE(A_INT) = 8; AMAX(A_INT) = 18;
    HHallucination = 100;
    for (i = 0; i < 200; ++i) use_unicorn_horn(&a);
    assert(ABASE(A_INT) == 18 && !HHallucination);
    ABASE(A_INT) = 8; Fixed_abil = W_RINGL;
    for (i = 0; i < 40; ++i) use_unicorn_horn(&a);
    assert(ABASE(A_INT) == 8);
    Fixed_abil = 0; ABASE(A_INT) = 18;
    forge_test_clear();
    puts("PASS Step 19 repair target/cancel semantics and exact lineage Nighthorn curing");
}

static void
step19_test_main(void)
{
    d_level saved = u.uz;
    boolean saved_bot_disabled = gb.bot_disabled;
    int m;
    gb.bot_disabled = TRUE; /* fixtures have no interactive status window */
    step19_equipment_tests();
    step19_dragon_tests();
    step19_conversion_tests();
    step19_hand_tests();
    step19_spell_tests();
    for (m = LOW_PM; m < NUMMONS; ++m) if (step19_min_depth(m)) {
        u.uz.dnum = medusa_level.dnum;
        u.uz.dlevel = step19_min_depth(m) - svd.dungeons[u.uz.dnum].depth_start;
        assert(!step19_generation_ok(m));
        ++u.uz.dlevel; assert(step19_generation_ok(m));
    }
    u.uz = saved;
    puts("PASS Step 19 generation minimum boundaries");
    step19_monster_tests();
    step19_boss_tests();
    step19_gaze_tests();
    gb.bot_disabled = saved_bot_disabled;
}
