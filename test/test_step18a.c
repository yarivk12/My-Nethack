/* Step 18A integration tests share apply.c's private Forge implementation. */
static const struct forge_recipe step18a_expected[] = {
    { KATANA, { { LONG_SWORD, 1 }, { LONG_SWORD, 1 } } },
    { TWO_HANDED_SWORD, { { LONG_SWORD, 1 }, { BROADSWORD, 1 } } },
    { TSURUGI, { { KATANA, 1 }, { TWO_HANDED_SWORD, 1 } } },
    { BATTLE_AXE, { { AXE, 1 }, { AXE, 1 } } },
    { DWARVISH_MATTOCK, { { PICK_AXE, 1 }, { DWARVISH_SHORT_SWORD, 1 } } },
    { TRIDENT, { { SCIMITAR, 1 }, { SPEAR, 1 } } },
    { ATHAME, { { DAGGER, 1 }, { STILETTO, 1 } } },
    { RUNESWORD, { { BROADSWORD, 1 }, { DAGGER, 1 } } },
    { DAGGER, { { KNIFE, 1 }, { KNIFE, 1 } } },
    { SHORT_SWORD, { { DAGGER, 1 }, { DAGGER, 1 } } },
    { LONG_SWORD, { { SHORT_SWORD, 1 }, { SHORT_SWORD, 1 } } },
    { HALBERD, { { SPEAR, 1 }, { AXE, 1 } } },
    { MORNING_STAR, { { MACE, 1 }, { MACE, 1 } } },
    { CHAIN_MAIL, { { RING_MAIL, 1 }, { RING_MAIL, 1 } } },
    { STUDDED_LEATHER_ARMOR,
      { { LEATHER_ARMOR, 1 }, { LEATHER_ARMOR, 1 } } },
    { SCALE_MAIL, { { STUDDED_LEATHER_ARMOR, 1 }, { RING_MAIL, 1 } } },
    { SPLINT_MAIL, { { SCALE_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { BANDED_MAIL, { { RING_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { PLATE_MAIL, { { SPLINT_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { PLATE_MAIL, { { BANDED_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { LARGE_SHIELD, { { SMALL_SHIELD, 1 }, { SMALL_SHIELD, 1 } } },
    { SHIELD_OF_REFLECTION,
      { { LARGE_SHIELD, 1 }, { AMULET_OF_REFLECTION, 1 } } },
    { ELVEN_SHIELD, { { ELVEN_DAGGER, 1 }, { SMALL_SHIELD, 1 } } }
};

static long
step18a_required(const struct forge_recipe *recipe, int typ)
{
    return (recipe->need[0].otyp == typ ? recipe->need[0].quantity : 0)
        + (recipe->need[1].otyp == typ ? recipe->need[1].quantity : 0);
}

static boolean
step18a_same_formula(const struct forge_recipe *a, const struct forge_recipe *b)
{
    return a->output == b->output
        && step18a_required(a, b->need[0].otyp)
               == step18a_required(b, b->need[0].otyp)
        && step18a_required(a, b->need[1].otyp)
               == step18a_required(b, b->need[1].otyp)
        && step18a_required(a, a->need[0].otyp)
               == step18a_required(b, a->need[0].otyp)
        && step18a_required(a, a->need[1].otyp)
               == step18a_required(b, a->need[1].otyp);
}

static const struct forge_recipe *
step18a_find_recipe(const struct forge_recipe *wanted)
{
    int i;
    for (i = 0; i < SIZE(forge_recipes); ++i)
        if (step18a_same_formula(&forge_recipes[i], wanted))
            return &forge_recipes[i];
    return (const struct forge_recipe *) 0;
}

static void
step18a_catalogue(void)
{
    int i, j, found, weapons = 0, armor = 0;

    assert(forge_catalog_valid(forge_recipes, SIZE(forge_recipes)));
    for (i = 0; i < SIZE(step18a_expected); ++i) {
        assert(step18a_find_recipe(&step18a_expected[i]));
        if (forge_category(step18a_expected[i].output) == 0) ++weapons;
        else if (forge_category(step18a_expected[i].output) == 1) ++armor;
    }
    assert(weapons == 13 && armor == 10);
    for (i = 0; i < SIZE(step18a_expected); ++i) {
        found = 0;
        for (j = 0; j < SIZE(forge_recipes); ++j)
            found += step18a_same_formula(&step18a_expected[i], &forge_recipes[j]);
        assert(found == 1);
        for (j = 0; j < i; ++j)
            assert(!step18a_same_formula(&step18a_expected[i],
                                         &step18a_expected[j]));
    }
    {
        const struct forge_recipe old_battle_axe = {
            BATTLE_AXE, { { AXE, 1 }, { BROADSWORD, 1 } }
        };
        const struct forge_recipe new_battle_axe = {
            BATTLE_AXE, { { AXE, 1 }, { AXE, 1 } }
        };
        const struct forge_recipe splint_plate = {
            PLATE_MAIL, { { SPLINT_MAIL, 1 }, { CHAIN_MAIL, 1 } }
        };
        const struct forge_recipe banded_plate = {
            PLATE_MAIL, { { BANDED_MAIL, 1 }, { CHAIN_MAIL, 1 } }
        };
        assert(!step18a_find_recipe(&old_battle_axe));
        assert(step18a_find_recipe(&new_battle_axe));
        assert(step18a_find_recipe(&splint_plate));
        assert(step18a_find_recipe(&banded_plate));
    }
    puts("PASS Step 18A exact 23 weapon/armor formulas, unique unordered inputs and replacement");
}

static struct obj *
step18a_craft(const struct forge_recipe *recipe, boolean reverse)
{
    struct forge_allocation allocations[100] = { { 0 } };
    struct forge_allocation swap;
    struct obj *obj, *output;
    int i, n = 0, slot;

    for (obj = gi.invent; obj; obj = obj->nobj) {
        assert(n < SIZE(allocations));
        allocations[n++].oid = obj->o_id;
    }
    if (reverse)
        for (i = 0; i < n / 2; ++i) {
            swap = allocations[i]; allocations[i] = allocations[n - i - 1];
            allocations[n - i - 1] = swap;
        }
    assert(forge_available(recipe, (struct obj *) 0));
    for (slot = 0; slot < 2; ++slot) {
        long left = recipe->need[slot].quantity;
        for (i = 0; i < n && left; ++i) {
            long remaining, take;
            obj = forge_find(allocations[i].oid);
            if (obj->otyp != recipe->need[slot].otyp
                || forge_reason(obj, (struct obj *) 0) != FORGE_USABLE)
                continue;
            remaining = obj->quan - allocations[i].quantity[0]
                        - allocations[i].quantity[1];
            take = min(left, remaining);
            allocations[i].quantity[slot] += take;
            left -= take;
        }
        assert(left == 0);
    }
    assert(forge_commit(recipe, allocations, n) == ECMD_TIME);
    for (output = gi.invent; output && output->otyp != recipe->output;
         output = output->nobj) ;
    assert(output && output->quan == 1);
    return output;
}

static void
step18a_seed_recipe(const struct forge_recipe *recipe)
{
    int slot, unit;
    for (slot = 0; slot < 2; ++slot)
        for (unit = 0; unit < recipe->need[slot].quantity; ++unit)
            (void) forge_test_item(recipe->need[slot].otyp, 1);
}

static struct obj *
step18a_nonmerge_item(int typ)
{
    struct obj *obj = forge_output(typ);
    obj->nomerge = 1;
    makeknown(typ);
    return addinv(obj);
}

struct step18a_output_state {
    int spe, quality;
    boolean blessed, cursed;
    uint8 eroded, eroded2, proof;
    struct enhancement_mask properties;
};

static void
step18a_recipe23_result(boolean reverse, struct step18a_output_state *state)
{
    const struct forge_recipe wanted = {
        SHIELD_OF_REFLECTION, { { LARGE_SHIELD, 1 }, { AMULET_OF_REFLECTION, 1 } }
    };
    const struct forge_recipe *recipe = step18a_find_recipe(&wanted);
    struct obj *shield, *amulet, *output, *fresh;

    assert(recipe);
    forge_test_clear();
    shield = forge_test_item(LARGE_SHIELD, 1);
    amulet = forge_test_item(AMULET_OF_REFLECTION, 1);
    shield->spe = -2;
    shield->o_enh_quality = OQ_FINE;
    shield->o_affixes[0].property = EP_DR_I;
    amulet->spe = 8;
    amulet->o_enh_quality = OQ_EXCEPTIONAL;
    amulet->o_affixes[0].property = EP_THORNS_I;
    amulet->oeroded = amulet->oeroded2 = 3;
    amulet->oerodeproof = 1;
    shield->cursed = 1;
    amulet->blessed = 1;
    fresh = forge_output(SHIELD_OF_REFLECTION);
    output = step18a_craft(recipe, reverse);
    assert(output->spe == -2);
    assert(output->o_enh_quality == OQ_FINE);
    assert(enhancement_has(output, EP_DR_I));
    assert(!enhancement_has(output, EP_THORNS_I));
    assert(output->blessed && !output->cursed);
    assert(output->oeroded == fresh->oeroded
           && output->oeroded2 == fresh->oeroded2
           && output->oerodeproof == fresh->oerodeproof);
    state->spe = output->spe;
    state->quality = output->o_enh_quality;
    state->blessed = output->blessed;
    state->cursed = output->cursed;
    state->eroded = output->oeroded;
    state->eroded2 = output->oeroded2;
    state->proof = output->oerodeproof;
    state->properties = enhancement_actual(output);
    obfree(fresh, NULL);
    forge_test_clear();
}

static void
step18a_recipe23_and_equipment(void)
{
    const struct forge_recipe weapon_recipe = {
        SHORT_SWORD, { { DAGGER, 1 }, { DAGGER, 1 } }
    };
    const struct forge_recipe studded_recipe = {
        STUDDED_LEATHER_ARMOR, { { LEATHER_ARMOR, 1 }, { LEATHER_ARMOR, 1 } }
    };
    const struct forge_recipe worn_recipe = {
        SHIELD_OF_REFLECTION, { { LARGE_SHIELD, 1 }, { AMULET_OF_REFLECTION, 1 } }
    };
    const struct forge_recipe *recipe;
    struct step18a_output_state first, second;
    struct obj *knife, *other, *out, *amulet, *shield;

    step18a_recipe23_result(FALSE, &first);
    step18a_recipe23_result(TRUE, &second);
    assert(first.spe == second.spe && first.quality == second.quality);
    assert(first.blessed == second.blessed && first.cursed == second.cursed);
    assert(first.eroded == second.eroded && first.eroded2 == second.eroded2
           && first.proof == second.proof);
    assert(first.properties.word[0] == second.properties.word[0]
           && first.properties.word[1] == second.properties.word[1]);

    forge_test_clear();
    recipe = step18a_find_recipe(&weapon_recipe);
    knife = step18a_nonmerge_item(DAGGER);
    other = step18a_nonmerge_item(DAGGER);
    knife->spe = -3; other->spe = 2; knife->cursed = 1; other->blessed = 1;
    other->o_enh_quality = OQ_EXCEPTIONAL;
    out = step18a_craft(recipe, FALSE);
    assert(out->spe == 2 && out->blessed && !out->cursed);
    assert(out->o_enh_quality == OQ_EXCEPTIONAL);
    forge_test_clear();

    forge_test_clear();
    recipe = step18a_find_recipe(&studded_recipe);
    knife = forge_test_item(LEATHER_ARMOR, 1);
    other = forge_test_item(LEATHER_ARMOR, 1);
    knife->spe = -1; other->spe = 3; knife->cursed = 1; other->blessed = 1;
    other->o_enh_quality = OQ_EXCEPTIONAL;
    out = step18a_craft(recipe, FALSE);
    assert(out->spe == 3 && out->blessed && !out->cursed);
    assert(out->o_enh_quality == OQ_EXCEPTIONAL);
    forge_test_clear();

    shield = forge_test_item(LARGE_SHIELD, 1);
    amulet = forge_test_item(AMULET_OF_REFLECTION, 1);
    recipe = step18a_find_recipe(&worn_recipe);
    amulet->owornmask = W_AMUL;
    assert(forge_reason(amulet, (struct obj *) 0) == FORGE_EQUIPPED);
    assert(!forge_available(recipe, (struct obj *) 0));
    amulet->owornmask = 0;
    assert(forge_available(recipe, (struct obj *) 0));
    forge_test_clear();
    (void) shield;

    forge_test_clear();
    recipe = step18a_find_recipe(&weapon_recipe);
    knife = forge_test_item(DAGGER, 1);
    other = forge_test_item(DAGGER, 1);
    knife->o_socket_capacity = 1;
    knife->o_sockets[0].property = EP_ANARCHIC_I;
    {
        unsigned socketed_id = knife->o_id;
        out = step18a_craft(recipe, FALSE);
        assert(!forge_find(socketed_id));
        assert(!socket_count(out) && !out->o_sockets[0].property
               && !out->o_sockets[0].value && !out->o_sockets[0].known);
    }
    forge_test_clear();

    forge_test_clear();
    puts("PASS Step 18A weapon/armor inheritance, Recipe 23 generic eligibility/order, and socket destruction");
}

static void
step18a_progressions(void)
{
    const struct forge_recipe knifed = {
        DAGGER, { { KNIFE, 1 }, { KNIFE, 1 } }
    };
    const struct forge_recipe shorted = {
        SHORT_SWORD, { { DAGGER, 1 }, { DAGGER, 1 } }
    };
    const struct forge_recipe longed = {
        LONG_SWORD, { { SHORT_SWORD, 1 }, { SHORT_SWORD, 1 } }
    };
    const struct forge_recipe katana = {
        KATANA, { { LONG_SWORD, 1 }, { LONG_SWORD, 1 } }
    };
    const struct forge_recipe studded = {
        STUDDED_LEATHER_ARMOR,
        { { LEATHER_ARMOR, 1 }, { LEATHER_ARMOR, 1 } }
    };
    const struct forge_recipe scale = {
        SCALE_MAIL, { { STUDDED_LEATHER_ARMOR, 1 }, { RING_MAIL, 1 } }
    };
    const struct forge_recipe splint = {
        SPLINT_MAIL, { { SCALE_MAIL, 1 }, { CHAIN_MAIL, 1 } }
    };
    const struct forge_recipe plate = {
        PLATE_MAIL, { { SPLINT_MAIL, 1 }, { CHAIN_MAIL, 1 } }
    };
    struct obj *output;

    forge_test_clear();
    step18a_seed_recipe(step18a_find_recipe(&knifed));
    assert(step18a_craft(step18a_find_recipe(&knifed), FALSE)->otyp == DAGGER);
    (void) forge_test_item(DAGGER, 1);
    assert(step18a_craft(step18a_find_recipe(&shorted), FALSE)->otyp == SHORT_SWORD);
    (void) forge_test_item(SHORT_SWORD, 1);
    assert(step18a_craft(step18a_find_recipe(&longed), FALSE)->otyp == LONG_SWORD);
    (void) forge_test_item(LONG_SWORD, 1);
    assert(forge_available(step18a_find_recipe(&katana), (struct obj *) 0));
    output = step18a_craft(step18a_find_recipe(&katana), FALSE);
    assert(output->otyp == KATANA);
    forge_test_clear();

    step18a_seed_recipe(step18a_find_recipe(&studded));
    assert(step18a_craft(step18a_find_recipe(&studded), FALSE)->otyp
           == STUDDED_LEATHER_ARMOR);
    (void) forge_test_item(RING_MAIL, 1);
    assert(step18a_craft(step18a_find_recipe(&scale), FALSE)->otyp == SCALE_MAIL);
    (void) forge_test_item(CHAIN_MAIL, 1);
    assert(step18a_craft(step18a_find_recipe(&splint), FALSE)->otyp == SPLINT_MAIL);
    (void) forge_test_item(CHAIN_MAIL, 1);
    assert(step18a_craft(step18a_find_recipe(&plate), FALSE)->otyp == PLATE_MAIL);
    forge_test_clear();
    puts("PASS Step 18A forged-output weapon and armor progression chains");
}

static void
step18a_test_main(void)
{
    step18a_catalogue();
    step18a_recipe23_and_equipment();
    step18a_progressions();
}
