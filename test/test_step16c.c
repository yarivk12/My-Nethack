/* Contract-derived Utility foundations, included by the native harness. */
static boolean
step16c_approved_tool(int typ)
{
    static const int approved[] = {
        SACK, OILSKIN_SACK, BAG_OF_HOLDING, SKELETON_KEY, LOCK_PICK,
        CREDIT_CARD, BRASS_LANTERN, OIL_LAMP, MAGIC_LAMP, EXPENSIVE_CAMERA,
        LENSES, BLINDFOLD, TOWEL, LEASH, STETHOSCOPE, TIN_OPENER, MAGIC_MARKER,
        TIN_WHISTLE, MAGIC_WHISTLE, WOODEN_FLUTE, MAGIC_FLUTE, TOOLED_HORN,
        FROST_HORN, FIRE_HORN, HORN_OF_PLENTY, WOODEN_HARP, MAGIC_HARP,
        BELL, BUGLE, LEATHER_DRUM, DRUM_OF_EARTHQUAKE, PICK_AXE,
        GRAPPLING_HOOK, UNICORN_HORN
    };
    int i;
    for (i = 0; i < SIZE(approved); ++i)
        if (typ == approved[i]) return TRUE;
    return FALSE;
}

static struct enhancement_mask
step16c_pair(int first, int second)
{
    return enhancement_mask_union(enhancement_mask_property(first),
                                  enhancement_mask_property(second));
}

static void
step16c_foundations(void)
{
    static const int tiers[23] = {
        1,2,3,4,2,4,3,1,2,3,4,3,1,3,1,2,2,3,2,3,2,3,1
    };
    struct obj *o, *sack = item(SACK);
    struct enhancement_mask mask;
    int i, j, typ, generated = 0;
    /* A vanilla type newly classified as a tool must not need an entry in
     * Utility's positive roster. Restore the native class after the fixture. */
    o = item(ARROW);
    objects[ARROW].oc_class = o->oclass = TOOL_CLASS;
    assert(enhancement_eligible(o));
    assert(enhancement_set_mask(o, enhancement_mask_property(EP_COMMERCE_I),
                                OQ_STANDARD, TRUE));
    enhancement_normalize(o);
    assert(enhancement_has(o, EP_COMMERCE_I));
    assert(!enhancement_mask_allowed(o, enhancement_mask_property(EP_EXCAVATING)));
    for (i = 1; i <= 500; ++i) {
        enhancement_clear(o);
        init_isaac64(i, rn2);
        enhancement_generate(o, 150);
        generated += !!o->o_enh_props2;
    }
    assert(generated > 0);
    o->oartifact = ART_EXCALIBUR;
    assert(!enhancement_eligible(o));
    o->oartifact = 0;
    objects[ARROW].oc_unique = 1;
    assert(!enhancement_eligible(o));
    objects[ARROW].oc_unique = 0;
    objects[ARROW].oc_class = o->oclass = WEAPON_CLASS;
    obfree(o, NULL);
    assert(object_origin(ARROW) == OBJ_ORIGIN_VANILLA);
    assert(object_origin(FIGURINE) == OBJ_ORIGIN_VANILLA);
    assert(object_origin(CANDELABRUM_OF_INVOCATION) == OBJ_ORIGIN_VANILLA);
    assert(object_origin(CRYSTAL_PICK) == OBJ_ORIGIN_CUSTOM);
    assert(object_origin(TORCH) == OBJ_ORIGIN_CUSTOM);
    assert(object_origin(GENERIC_TOOL) == OBJ_ORIGIN_UNKNOWN);
    assert(object_origin(-1) == OBJ_ORIGIN_UNKNOWN);
    assert(object_origin(NUM_OBJECTS) == OBJ_ORIGIN_UNKNOWN);
    assert(EP_DR_IV == 93 && EP_EROSION_I == 94 && EP_WATERTIGHT == 116);
    assert(EP_COUNT == 117 && SIZE(enhancement_catalog) == 82);
    for (i = 0; i < 23; ++i) {
        const struct enhancement_entry *e = equipment_property(94 + i);
        mask = enhancement_mask_property(94 + i);
        assert(e && e->tier == tiers[i]);
        assert(!mask.word[0] && mask.word[1] == (1ULL << i));
        assert(enhancement_mask_count(mask) == 1);
        assert(enhancement_mask_has(mask, 94 + i));
        enhancement_mask_remove(&mask, 94 + i);
        assert(!enhancement_mask_count(mask));
        enhancement_mask_add(&mask, 94 + i);
        assert(mask.word[1] == (1ULL << i));
    }
    mask = step16c_pair(EP_DR_IV, EP_WATERTIGHT);
    assert(mask.word[0] == (1ULL << 59) && mask.word[1] == (1ULL << 22));
    assert(enhancement_mask_count(mask) == 2);
    enhancement_mask_add(&mask, EP_COMMERCE_I);
    assert(enhancement_mask_count(mask) == 3);
    assert(!enhancement_mask_allowed(sack, mask));
    for (typ = 1; typ < NUM_OBJECTS; ++typ) {
        o = item(typ);
        if (o->oclass != TOOL_CLASS) { obfree(o, NULL); continue; }
        assert(enhancement_eligible(o) == step16c_approved_tool(typ));
        assert(!socket_capacity(o));
        for (i = EP_EROSION_I; i <= EP_WATERTIGHT; ++i) {
            boolean allowed = step16c_approved_tool(typ);
            if (i <= EP_EROSION_IV && Is_container(o)) allowed = FALSE;
            if (i == EP_EXCAVATING && typ != PICK_AXE) allowed = FALSE;
            if (i >= EP_TREASURE_I && !Is_container(o)) allowed = FALSE;
            if (i == EP_WATERTIGHT && typ == OILSKIN_SACK) allowed = FALSE;
            mask = enhancement_mask_property(i);
            assert(enhancement_mask_allowed(o, mask) == allowed);
            assert(enhancement_set_mask(o, mask, OQ_STANDARD, FALSE) == allowed);
            if (allowed) {
                assert(!o->o_enh_props && !o->o_enh_quality);
                assert(enhancement_has(o, i));
                assert(!enhancement_mask_count(enhancement_known(o)));
            }
        }
        assert(!enhancement_set(o, OEP_FIRE, OQ_STANDARD, FALSE));
        assert(!enhancement_set(o, OEP_WARDING, OQ_STANDARD, FALSE));
        assert(!enhancement_set(o, 0, OQ_FINE, FALSE));
        assert(!enhancement_set(o, 0, OQ_EXCEPTIONAL, FALSE));
        /* The same recipient rule applies to generation, malformed debug
         * assignments, and the normalizer used after same-type polymorph. */
        if (!step16c_approved_tool(typ)) {
            for (i = 1; i <= 50; ++i) {
                enhancement_clear(o);
                init_isaac64(i, rn2);
                enhancement_generate(o, 150);
                assert(!enhancement_mask_count(enhancement_actual(o)));
            }
        }
        enhancement_clear(o);
        assert(enhancement_set_mask(o,enhancement_mask_property(EP_COMMERCE_I),
                                   OQ_STANDARD,TRUE)==step16c_approved_tool(typ));
        assert(enhancement_has(o, EP_COMMERCE_I) == step16c_approved_tool(typ));
        assert(!!o->o_enh_known2 == step16c_approved_tool(typ));
        obfree(o, NULL);
    }
    o = item(PICK_AXE);
    for (i = EP_EROSION_I; i <= EP_EROSION_IV; ++i)
        for (j = i + 1; j <= EP_EROSION_IV; ++j)
            assert(!enhancement_mask_allowed(o, step16c_pair(i, j)));
    assert(!enhancement_mask_allowed(o, step16c_pair(EP_PURIFICATION_I, EP_PURIFICATION_IV)));
    assert(!enhancement_mask_allowed(o, step16c_pair(EP_CURSE_II, EP_CURSE_IV)));
    assert(!enhancement_mask_allowed(o, step16c_pair(EP_COMMERCE_I, EP_COMMERCE_III)));
    assert(enhancement_set_mask(o, step16c_pair(EP_EXCAVATING, EP_PURIFICATION_IV), 0, TRUE));
    assert(o->o_purification_remaining == 100);
    assert(enhancement_known(o).word[1] == enhancement_actual(o).word[1]);
    o->o_purification_remaining = 17;
    enhancement_normalize(o);
    assert(o->o_purification_remaining == 17);
    enhancement_change_type(o, GRAPPLING_HOOK);
    assert(!enhancement_has(o, EP_EXCAVATING));
    assert(!enhancement_mask_has(enhancement_known(o), EP_EXCAVATING));
    assert(!enhancement_has(o, EP_PURIFICATION_IV) && !o->o_purification_remaining);
    enhancement_change_type(o, CRYSTAL_PICK);
    assert(!enhancement_mask_count(enhancement_actual(o)));
    assert(!enhancement_mask_count(enhancement_known(o)) && !o->o_purification_remaining);
    obfree(o, NULL);
    o = item(MAGIC_LAMP);
    assert(enhancement_set_mask(o, step16c_pair(EP_COMMERCE_III, EP_PURIFICATION_II), 0, TRUE));
    o->o_purification_remaining = 23;
    enhancement_change_type(o, OIL_LAMP);
    assert(enhancement_has(o, EP_COMMERCE_III) && enhancement_has(o, EP_PURIFICATION_II));
    assert(o->o_purification_remaining == 23);
    assert(enhancement_known(o).word[1] == enhancement_actual(o).word[1]);
    o->oartifact = ART_EXCALIBUR;
    enhancement_strip_for_artifact(o);
    assert(!enhancement_mask_count(enhancement_actual(o)));
    assert(!enhancement_mask_count(enhancement_known(o)) && !o->o_purification_remaining);
    obfree(o, NULL); obfree(sack, NULL);
    puts("PASS Step 16C exact IDs/mask coordinates, all tool recipients, family limits, Quality, sockets and transformations");
}

static void
step16c_generation_names(void)
{
    const int types[] = {SACK, OILSKIN_SACK, BAG_OF_HOLDING, PICK_AXE, MAGIC_LAMP,
                         MAGIC_MARKER, EXPENSIVE_CAMERA, UNICORN_HORN};
    struct obj *o;
    char prefix[BUFSZ], suffix[BUFSZ];
    int kind, seed, enhanced = 0, pairs = 0;
    for (kind = 0; kind < SIZE(types); ++kind) {
        o = item(types[kind]);
        for (seed = 1; seed <= 2000; ++seed) {
            enhancement_clear(o);
            init_isaac64(seed, rn2);
            enhancement_generate(o, 150);
            assert(!o->o_enh_quality && !o->o_enh_props && !socket_capacity(o));
            assert(enhancement_mask_count(enhancement_actual(o)) <= 2);
            assert(enhancement_mask_allowed(o, enhancement_actual(o)));
            assert(!enhancement_mask_count(enhancement_known(o)));
            enhanced += !!o->o_enh_props2;
            pairs += enhancement_mask_count(enhancement_actual(o)) == 2;
        }
        obfree(o, NULL);
    }
    assert(enhanced > 1000 && pairs > 100);
    o = item(SACK);
    assert(enhancement_set_mask(o, step16c_pair(EP_COMMERCE_I, EP_PURIFICATION_IV), 0, FALSE));
    enhancement_prefix(o, FALSE, prefix, sizeof prefix);
    enhancement_suffix(o, FALSE, suffix, sizeof suffix);
    assert(!*prefix && !*suffix);
    enhancement_identify(o);
    enhancement_prefix(o, FALSE, prefix, sizeof prefix);
    enhancement_suffix(o, FALSE, suffix, sizeof suffix);
    assert(strstr(prefix, "Absolving") && strstr(suffix, "of Bargains"));
    assert(enhancement_known(o).word[1] == enhancement_actual(o).word[1]);
    assert(enhancement_price_adjustment(o) == 550); /* Base 100 + T1 50 + T4 400. */
    assert(enhancement_set_mask(o, enhancement_mask_property(EP_COMMERCE_I), 0, FALSE));
    assert(!enhancement_mask_has(enhancement_known(o), EP_PURIFICATION_IV));
    assert(!o->o_purification_remaining);
    obfree(o, NULL);
    puts("PASS Step 16C fixed-seed generation, hidden/identified names, pricing and property-removal knowledge");
}

static void step14_roundtrip_batch(struct obj **);
static void step16c_recipient_codec(void);
static void
step16c_persistence(void)
{
    struct obj *chain = NULL, *o;
    int i, known, remaining;
    for (i = 0; i < 23; ++i)
        for (known = 0; known < 2; ++known) {
            o = item(i < 12 ? PICK_AXE : SACK);
            assert(enhancement_set_mask(o, enhancement_mask_property(94 + i), 0, known));
            o->nobj = chain; chain = o;
        }
    for (i = 0; i < 4; ++i)
        for (remaining = 0; remaining <= 400 - 100 * i; ++remaining) {
            o = item(MAGIC_LAMP);
            assert(enhancement_set_mask(o, step16c_pair(EP_PURIFICATION_I + i, EP_COMMERCE_I), 0, TRUE));
            o->o_purification_remaining = remaining;
            o->spe = 1; o->age = 1234;
            o->nobj = chain; chain = o;
        }
    step14_roundtrip_batch(&chain);
    step16c_recipient_codec();
    puts("PASS Step 16C native object codec all 23 word-1 identities, known states and all Purification timer values");
}
