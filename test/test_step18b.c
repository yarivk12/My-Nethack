/* Native Step 18B transactions, using the existing Forge fixture helpers. */
static const struct forge_recipe step18b_expected[] = {
    { HELM_OF_BRILLIANCE, { { HELMET, 1 }, { POT_GAIN_ABILITY, 1 } } },
    { HELM_OF_CAUTION, { { HELMET, 1 }, { RIN_WARNING, 1 } } },
    { HELM_OF_TELEPATHY, { { HELMET, 1 }, { AMULET_OF_ESP, 1 } } },
    { CLOAK_OF_PROTECTION, { { LEATHER_CLOAK, 1 }, { AMULET_OF_GUARDING, 1 } } },
    { CLOAK_OF_INVISIBILITY, { { LEATHER_CLOAK, 1 }, { RIN_INVISIBILITY, 1 } } },
    { CLOAK_OF_MAGIC_RESISTANCE, { { LEATHER_CLOAK, 1 }, { AMULET_OF_UNCHANGING, 1 } } },
    { CLOAK_OF_DISPLACEMENT, { { LEATHER_CLOAK, 1 }, { RIN_TELEPORTATION, 1 } } },
    { ALCHEMY_SMOCK, { { ROBE, 1 }, { AMULET_VERSUS_POISON, 1 } } },
    { GAUNTLETS_OF_POWER, { { LEATHER_GLOVES, 1 }, { RIN_GAIN_STRENGTH, 1 } } },
    { GAUNTLETS_OF_DEXTERITY, { { LEATHER_GLOVES, 1 }, { RIN_INCREASE_ACCURACY, 1 } } },
    { GAUNTLETS_OF_FUMBLING, { { LEATHER_GLOVES, 1 }, { POT_CONFUSION, 1 } } },
    { ELVEN_BOOTS, { { LOW_BOOTS, 1 }, { RIN_STEALTH, 1 } } },
    { SPEED_BOOTS, { { ELVEN_BOOTS, 1 }, { SPE_HASTE_SELF, 1 } } },
    { WATER_WALKING_BOOTS, { { HIGH_BOOTS, 1 }, { AMULET_OF_MAGICAL_BREATHING, 1 } } },
    { JUMPING_BOOTS, { { HIGH_BOOTS, 1 }, { SPE_JUMPING, 1 } } },
    { KICKING_BOOTS, { { IRON_SHOES, 1 }, { RIN_INCREASE_DAMAGE, 1 } } },
    { FUMBLE_BOOTS, { { HIGH_BOOTS, 1 }, { POT_CONFUSION, 1 } } },
    { LEVITATION_BOOTS, { { LOW_BOOTS, 1 }, { RIN_LEVITATION, 1 } } },
    { SHIELD_OF_DRAIN_RESISTANCE, { { LARGE_SHIELD, 1 }, { SPE_DRAIN_LIFE, 1 } } },
    { SHIELD_OF_SHOCK_RESISTANCE, { { LARGE_SHIELD, 1 }, { RIN_SHOCK_RESISTANCE, 1 } } },
    { BAG_OF_HOLDING, { { SACK, 1 }, { RIN_LEVITATION, 1 } } },
    { MAGIC_WHISTLE, { { TIN_WHISTLE, 1 }, { SCR_TAMING, 1 } } },
    { MAGIC_FLUTE, { { WOODEN_FLUTE, 1 }, { SCR_TAMING, 1 } } },
    { MAGIC_HARP, { { WOODEN_HARP, 1 }, { SCR_TAMING, 1 } } }
};

static void
step18b_transactions(void)
{
    int i, j, reverse;
    struct obj *base, *catalyst, *out;
    struct obj saved;
    unsigned base_id, catalyst_id;

    assert(SIZE(step18b_expected) == 24);
    assert(SIZE(forge_recipes) == SIZE(step18a_expected) + 24);
    assert(forge_catalog_valid(forge_recipes, SIZE(forge_recipes)));
    for (i = 0; i < SIZE(step18b_expected); ++i) {
        const struct forge_recipe *r = step18a_find_recipe(&step18b_expected[i]);
        assert(r);
        for (j = 0; j < i; ++j)
            assert(r->output != step18b_expected[j].output);
        for (reverse = 0; reverse < 2; ++reverse) {
            forge_test_clear();
            base = forge_test_item(r->need[0].otyp, 1);
            catalyst = forge_test_item(r->need[1].otyp,
                objects[r->need[1].otyp].oc_merge ? 4 : 1);
            base_id = base->o_id; catalyst_id = catalyst->o_id;
            base->spe = -3;
            catalyst->spe = 97; /* Unsupported fields must not leak. */
            if (base->oclass == ARMOR_CLASS) {
                base->o_socket_capacity = 1;
                base->o_sockets[0].property = EP_DR_I;
            }
            if (catalyst->oclass == RING_CLASS || catalyst->oclass == AMULET_CLASS) {
                catalyst->owornmask = catalyst->oclass == RING_CLASS ? W_RINGL : W_AMUL;
                assert(!forge_available(r, NULL));
                assert(forge_reason(catalyst, NULL) == FORGE_EQUIPPED);
                catalyst->owornmask = 0;
            }
            saved = *catalyst;
            out = step18a_craft(r, reverse);
            assert(!forge_find(base_id));
            assert(!socket_count(out) && !out->o_sockets[0].property
                   && !out->o_sockets[1].property);
            assert(!out->recharged && !out->cobj);
            if (out->oclass == ARMOR_CLASS)
                assert(out->spe == (saved.oclass == RING_CLASS
                    && objects[saved.otyp].oc_charged ? 97 : -3));
            if (out->otyp == MAGIC_FLUTE || out->otyp == MAGIC_HARP)
                assert(out->spe >= 4 && out->spe <= 8);
            if (out->otyp == MAGIC_WHISTLE) assert(out->spe == 0);
            if (saved.quan > 1) {
                catalyst = forge_find(catalyst_id);
                assert(catalyst);
                saved.quan--;
                saved.owt = weight(&saved);
                /* Consuming the base and adding output changes list links. */
                saved.nobj = catalyst->nobj;
                forge_test_same_object(&saved, catalyst);
            } else assert(!forge_find(catalyst_id));
        }
    }
    forge_test_clear();
    puts("PASS Step 18B exact 24 distinct outputs, both orders, stacks, worn jewelry and sockets");
}

static void
step18b_containers_and_chain(void)
{
    const struct forge_recipe *bag = step18a_find_recipe(&step18b_expected[20]);
    struct obj *sack, *contents, *out, saved, saved_contents;
    int i;
    forge_test_clear();
    sack = forge_test_item(SACK, 1);
    contents = forge_output(DAGGER);
    add_to_container(sack, contents);
    forge_test_item(RIN_LEVITATION, 1);
    saved = *sack; saved_contents = *contents;
    assert(!forge_available(bag, NULL));
    assert(forge_reason(sack, NULL) == FORGE_CONTENTS);
    forge_test_script();
    forge_test_choose("Use the forge", 1, -1);
    forge_test_choose("Choose a category", 3, -1);
    forge_test_choose("Tools", 0, -1);
    assert(forge_menu(NULL) == ECMD_OK);
    for (i = 0; i < forge_last_recipe_n; ++i)
        assert(forge_last_recipe_ids[i] != (int) (bag - forge_recipes) + 1);
    forge_test_same_object(&saved, sack);
    forge_test_same_object(&saved_contents, contents);
    forge_test_clear();
    step18a_seed_recipe(bag);
    out = step18a_craft(bag, FALSE);
    assert(!out->cobj && !out->olocked && !out->otrapped);
    forge_test_clear();
    step18a_seed_recipe(&step18b_expected[11]);
    out = step18a_craft(step18a_find_recipe(&step18b_expected[11]), FALSE);
    makeknown(out->otyp);
    forge_test_item(SPE_HASTE_SELF, 1);
    assert(step18a_craft(step18a_find_recipe(&step18b_expected[12]), TRUE)->otyp == SPEED_BOOTS);
    forge_test_clear();
    puts("PASS Step 18B empty/nonempty containers and Low Boots -> Elven Boots -> Speed Boots");
}

static void
step18b_spe_channels(void)
{
    static const int donors[] = { WAN_FIRE, MAGIC_FLUTE, MAGIC_MARKER,
        LONG_SWORD, HELMET, PICK_AXE, RIN_GAIN_STRENGTH,
        RIN_INCREASE_ACCURACY, RIN_INCREASE_DAMAGE };
    static const int targets[] = { LONG_SWORD, HELMET, PICK_AXE,
        RIN_GAIN_STRENGTH, MAGIC_FLUTE, MAGIC_HARP, WAN_FIRE, MAGIC_WHISTLE };
    int i, j, before;
    struct obj *source, *target;
    for (i = 0; i < SIZE(donors); ++i) {
        struct forge_state state = { 0 };
        source = forge_output(donors[i]);
        source->spe = 97;
        source->recharged = 6;
        forge_gather(&state, source);
        assert(state.spe_present == (i >= 3));
        for (j = 0; j < SIZE(targets); ++j) {
            target = forge_output(targets[j]);
            before = target->spe;
            forge_inherit(target, &state);
            assert(target->spe == (i >= 3 && j < 4 ? 97 : before));
            assert(!target->recharged);
            obfree(target, NULL);
        }
        obfree(source, NULL);
    }
    puts("PASS Step 18B enchantment donors and high-charge exclusion across target semantics");
}

static void
step18b_fresh_charges(void)
{
    int i, seed, expected, next, seen = 0;
    struct obj *out, *fresh;
    struct obj probe = cg.zeroobj;
    /* Exercise the shared native seam directly, independent of Forge. */
    for (seed = 1; seed <= 32; ++seed) {
        probe.otyp = MAGIC_HARP; probe.oclass = TOOL_CLASS; probe.spe = 97;
        init_isaac64(seed, rn2);
        expected = rn1(5, 4); next = rn2(1000000);
        init_isaac64(seed, rn2);
        assert(init_obj_charges(&probe) && probe.spe == expected);
        assert(rn2(1000000) == next);
    }
    probe.otyp = RIN_GAIN_STRENGTH; probe.oclass = RING_CLASS; probe.spe = -3;
    init_isaac64(18, rn2); next = rn2(1000000);
    init_isaac64(18, rn2);
    assert(!init_obj_charges(&probe) && probe.spe == -3);
    assert(rn2(1000000) == next);
    for (i = 22; i < 24; ++i) {
        const struct forge_recipe *r = step18a_find_recipe(&step18b_expected[i]);
        for (seed = 1; seed <= 32; ++seed) {
            forge_test_clear();
            step18a_seed_recipe(r);
            makeknown(r->output);
            /* Account for the canonical constructor's own random draws. */
            init_isaac64(seed, rn2);
            fresh = mksobj(r->output, FALSE, FALSE);
            expected = rn1(5, 4);
            next = rn2(1000000);
            obfree(fresh, NULL);
            init_isaac64(seed, rn2);
            out = step18a_craft(r, FALSE);
            assert(out->spe == expected && out->spe >= 4 && out->spe <= 8);
            assert(rn2(1000000) == next);
            seen |= 1 << (out->spe - 4);
        }
    }
    assert(seen == 31);
    forge_test_clear();
    puts("PASS Step 18B seeded native 4..8 charges and exact RNG advancement");
}

static void
step18b_test_main(void)
{
    step18b_transactions();
    step18b_containers_and_chain();
    step18b_spe_channels();
    step18b_fresh_charges();
}
