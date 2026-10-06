/* Native glove lifecycle and enchant-scroll outcomes for both identities. */
static void
step19_hand_tests(void)
{
    struct obj *hand, *scroll, before;
    int i, failed = 0, succeeded = 0, strength;

    forge_test_clear();
    hand = forge_test_item(MUMMIFIED_HAND, 1L);
    hand->spe = 0; hand->cursed = hand->blessed = 0;
    strength = ACURR(A_STR);
    assert(!enhancement_eligible(hand) && !socket_capacity(hand));
    assert(!obj_resists(hand, 0, 0));
    hand->oerodeproof = hand->greased = 0;
    hand->oeroded = hand->oeroded2 = 0;
    assert(erode_obj(hand, "hand", ERODE_ROT, EF_DESTROY) == ER_DAMAGED);
    assert(hand->oeroded2 == 1);
    setworn(hand, W_ARMG); set_wear(hand);
    assert(ACURR(A_STR) == strength);
    assert(!(ERegeneration & W_ARMG));
    assert(!(EHalf_physical_damage & W_ARMG));
    assert(!(ESick_resistance & W_ARMG));
    scroll = forge_test_item(SCR_ENCHANT_ARMOR, 1L);
    scroll->blessed = scroll->cursed = 0;
    assert(seffects(scroll) == 0); /* caller must consume the scroll */
    useup(scroll);
    assert(hand->spe > 0);
    (void) Gloves_off();
    assert(!uarmg && !(hand->owornmask & W_ARMG));
    setworn(hand, W_ARMG); set_wear(hand);
    remove_worn_item(hand, FALSE);
    assert(!uarmg && !(hand->owornmask & W_ARMG));
    forge_test_clear();

    hand = forge_test_item(MUMMIFIED_HAND, 1L);
    hand->oartifact = ART_HAND_OF_VECNA;
    hand->spe = 0; hand->cursed = hand->blessed = 0;
    setworn(hand, W_ARMG); set_wear(hand);
    assert(ACURR(A_STR) == STR19(25));
    assert((ERegeneration & W_ARMG) && (EHalf_physical_damage & W_ARMG));
    assert(ESick_resistance & W_ARMG);
    assert(!enhancement_eligible(hand) && !socket_capacity(hand));
    (void) Gloves_off();
    assert(uarmg == hand && (hand->owornmask & W_ARMG));
    remove_worn_item(hand, TRUE);
    assert(uarmg == hand && (hand->owornmask & W_ARMG));
    assert(obj_resists(hand, 0, 0));
    before = *hand;
    assert(erode_obj(hand, "hand", ERODE_BURN, EF_DESTROY) == ER_NOTHING);
    assert(erode_obj(hand, "hand", ERODE_ROT, EF_DESTROY) == ER_NOTHING);
    assert(hand->oeroded == before.oeroded && hand->oeroded2 == before.oeroded2);
    assert(!disintegrate_arm(hand));
    assert(uarmg == hand);
    hand->oeroded = hand->oeroded2 = 2;
    before = *hand;
    forge_test_script();
    cast_repair_armor(P_BASIC);
    assert(!forge_choice_next);
    forge_test_same_object(&before, hand);

    /* Restore's property rebuild must restore every worn passive. */
    set_artifact_intrinsic(hand, FALSE, W_ARMG);
    assert(!(ERegeneration & W_ARMG) && !(EHalf_physical_damage & W_ARMG));
    assert(!(ESick_resistance & W_ARMG));
    set_artifact_intrinsic(hand, TRUE, W_ARMG);
    assert((ERegeneration & W_ARMG) && (EHalf_physical_damage & W_ARMG));
    assert(ESick_resistance & W_ARMG);

    init_isaac64(190019UL, rn2);
    for (i = 0; i < 100 && (!failed || !succeeded); ++i) {
        hand->spe = 6;
        scroll = forge_test_item(SCR_ENCHANT_ARMOR, 1L);
        scroll->blessed = 1; scroll->cursed = 0;
        /* Both destructive and successful outcomes retain normal caller
         * consumption. Destruction resistance must not fall through. */
        assert(seffects(scroll) == 0);
        useup(scroll);
        assert(uarmg == hand && hand->spe >= 6);
        if (hand->spe == 6) ++failed;
        else ++succeeded;
    }
    assert(failed && succeeded);
    hand->spe = 0;
    scroll = forge_test_item(SCR_ENCHANT_ARMOR, 1L);
    scroll->cursed = 1; scroll->blessed = 0;
    assert(seffects(scroll) == 0);
    useup(scroll);
    assert(hand->spe < 0 && uarmg == hand);
    /* Fixture teardown deliberately bypasses the player removal guard. */
    setworn((struct obj *) 0, W_ARMG);
    hand->oartifact = 0;
    forge_test_clear();
    puts("PASS Step 19 Hand binding, passives, destruction, repair exclusion and enchantment outcomes");
}
