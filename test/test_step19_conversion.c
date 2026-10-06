/* Exercise the real scroll path, not just the type-change helper. */
static void
step19_conversion_tests(void)
{
    static const int dragons[] = { PM_GRAY_DRAGON, PM_YELLOW_DRAGON,
        PM_GLOWING_DRAGON, PM_CAVE_CHROMATIC_DRAGON,
        PM_SHIMMERING_DRAGON, PM_DEEP_DRAGON, PM_RAZOR_DRAGON,
        PM_FILTH_DRAGON, PM_SHADOW_DRAGON, PM_CELESTIAL_DRAGON };
    struct obj *armor, *scroll, before;
    int i, blessed;
    for (i = 0; i < SIZE(dragons); ++i)
        for (blessed = 0; blessed < 2; ++blessed) {
            forge_test_clear();
            armor = forge_test_item(dragon_armor_type(dragons[i], FALSE), 1L);
            armor->spe = -2;
            armor->cursed = !blessed;
            armor->blessed = blessed;
            armor->oeroded = 2; armor->oeroded2 = 1;
            armor->oerodeproof = 1;
            assert(enhancement_slot_set(armor, 0, EP_STEALTH, TRUE, -1));
            armor->o_enh_quality = OQ_EXCEPTIONAL;
            if (dragons[i] == PM_SHADOW_DRAGON) {
                armor->o_sockets[0].property = EP_SLEEP_RES;
                armor->o_sockets[0].known = 1;
            }
            setworn(armor, W_ARM);
            set_wear(armor);
            before = *armor;
            scroll = forge_test_item(SCR_ENCHANT_ARMOR, 1L);
            scroll->blessed = blessed;
            scroll->cursed = 0;
            (void) seffects(scroll);
            assert(uarm == armor);
            assert(armor->otyp == dragon_armor_type(dragons[i], TRUE));
            assert(armor->spe == before.spe);
            assert(armor->blessed == before.blessed && armor->cursed == before.cursed);
            assert(armor->oeroded == before.oeroded && armor->oeroded2 == before.oeroded2);
            assert(armor->oerodeproof == before.oerodeproof);
            assert(armor->o_enh_quality == before.o_enh_quality);
            assert(armor->o_enh_props == before.o_enh_props);
            assert(!memcmp(armor->o_affixes, before.o_affixes, sizeof armor->o_affixes));
            assert(!memcmp(armor->o_sockets, before.o_sockets, sizeof armor->o_sockets));
            if (dragons[i] == PM_SHADOW_DRAGON)
                assert((ESleep_resistance & W_ARM) && (EDrain_resistance & W_ARM));
            if (dragons[i] == PM_CELESTIAL_DRAGON)
                assert((ESleep_resistance & W_ARM) && (EShock_resistance & W_ARM));
            (void) Armor_off();
            assert(!(ESleep_resistance & W_ARM));
            assert(!(EDrain_resistance & W_ARM));
            assert(!(EShock_resistance & W_ARM));
        }
    forge_test_clear();
    puts("PASS Step 19 actual scales conversion preserves state and activates DSM properties");
}
