/* Included in apply.c's native harness after the Step 15B test helpers.
 * Expected answers are explicit fixtures or independent scalar reductions. */
static struct obj *
forge15c_result(int typ, struct obj *left, struct obj *right)
{
    struct forge_recipe r = { 0 };
    struct forge_allocation a[2] = { 0 };
    struct obj *obj;
    r.output = typ;
    r.need[0].otyp = left->otyp; r.need[0].quantity = 1;
    r.need[1].otyp = right->otyp; r.need[1].quantity = 1;
    a[0].oid = left->o_id; a[0].quantity[0] = 1;
    a[1].oid = right->o_id; a[1].quantity[1] = 1;
    assert(forge_catalog_valid(&r, 1));
    assert(forge_commit(&r, a, 2) == ECMD_TIME);
    for (obj = gi.invent; obj && obj->otyp != typ; obj = obj->nobj) ;
    assert(obj);
    return obj;
}

static void
forge15c_reductions(void)
{
    static const int spe[][3] = {
        { -5, -2, -2 }, { -2, -5, -2 }, { -4, 0, 0 },
        { 17, 7, 17 }, { 7, 17, 17 }, { -99, -99, -99 }, { 99, 98, 99 }
    };
    int q0, q1, b0, b1, s, e0, e1, e2, e3, p, cases = 0;
    struct obj *a, *b, *out;
    for (q0 = 0; q0 < 3; ++q0)
        for (q1 = 0; q1 < 3; ++q1)
            for (b0 = -1; b0 <= 1; ++b0)
                for (b1 = -1; b1 <= 1; ++b1)
                    for (s = 0; s < SIZE(spe); ++s) {
                        forge_test_clear();
                        a = forge_test_item(LONG_SWORD, 1);
                        b = forge_test_item(BROADSWORD, 1);
                        assert(enhancement_set(a, 0, q0, FALSE));
                        assert(enhancement_set(b, 0, q1, FALSE));
                        a->blessed = b0 == 1; a->cursed = b0 == -1;
                        b->blessed = b1 == 1; b->cursed = b1 == -1;
                        a->spe = spe[s][0]; b->spe = spe[s][1];
                        out = forge15c_result(KATANA, a, b);
                        assert(out->o_enh_quality == (q0 > q1 ? q0 : q1));
                        assert((int) out->blessed == (b0 == 1 || b1 == 1));
                        assert((int) out->cursed == (b0 == -1 && b1 == -1));
                        assert(out->spe == spe[s][2]);
                        assert(!out->o_enh_props && !out->o_enh_known
                               && !out->o_enh_flags && !out->known && !out->bknown);
                        ++cases;
                    }
    for (e0 = 0; e0 < 4; ++e0)
        for (e1 = 0; e1 < 4; ++e1)
            for (e2 = 0; e2 < 4; ++e2)
                for (e3 = 0; e3 < 4; ++e3)
                    for (p = 0; p < 4; ++p) {
                        forge_test_clear();
                        a = forge_test_item(LONG_SWORD, 1);
                        b = forge_test_item(BROADSWORD, 1);
                        a->oeroded = e0; b->oeroded = e1;
                        a->oeroded2 = e2; b->oeroded2 = e3;
                        a->oerodeproof = p & 1; b->oerodeproof = (p >> 1) & 1;
                        /* Iron rust severity becomes wooden burn severity. */
                        out = forge15c_result(ELVEN_SHIELD, a, b);
                        assert(obj_material(out) == WOOD);
                        assert((int) out->oeroded == (e0 < e1 ? e0 : e1));
                        assert((int) out->oeroded2 == (e2 < e3 ? e2 : e3));
                        assert((int) out->oerodeproof == (p != 0));
                        assert(!out->rknown);
                        ++cases;
                    }
    forge_test_clear();
    printf("PASS Step 15C reductions cases=%d (quality/BUC/signed spe, independent erosion and proof)\n", cases);
}

static void
forge15c_properties(void)
{
    static const struct { int target; uint32 a, b, expected; } cases[] = {
        { KATANA, 0, 0, 0 },
        { KATANA, OEP_FIRE, 0, OEP_FIRE },
        { KATANA, OEP_FIRE, OEP_FIRE, OEP_FIRE },
        { KATANA, OEP_FIRE | OEP_COLD, OEP_FIRE | OEP_SHOCK, OEP_FIRE | OEP_COLD },
        { KATANA, OEP_COLD_III | OEP_FIRE_III, OEP_SHOCK_III | OEP_PRIMORDIAL,
          OEP_PRIMORDIAL | OEP_FIRE_III },
        { KATANA, OEP_FIRE_II, OEP_FIRE_III, OEP_FIRE_II | OEP_FIRE_III },
        { KATANA, OEP_FIRE, OEP_PRIMORDIAL, OEP_FIRE | OEP_PRIMORDIAL },
        { KATANA, OEP_MAGIC_RES | OEP_REFLECTION, OEP_COLD | OEP_FIRE, OEP_FIRE | OEP_COLD },
        { KATANA, OEP_MAGIC_RES, OEP_TRUEFLIGHT, 0 },
        { SPEAR, OEP_TRUEFLIGHT, OEP_FIRE, OEP_TRUEFLIGHT | OEP_FIRE },
        { DAGGER, OEP_TRUEFLIGHT, OEP_FIRE, OEP_FIRE },
        { BOW, OEP_TRUEFLIGHT, OEP_FIRE, OEP_TRUEFLIGHT | OEP_FIRE },
        { ARROW, OEP_TRUEFLIGHT, OEP_COLD, OEP_TRUEFLIGHT | OEP_COLD },
        { PLATE_MAIL, OEP_MAGIC_RES, OEP_REFLECTION, OEP_MAGIC_RES | OEP_REFLECTION },
        { PLATE_MAIL, OEP_WARNING, OEP_SEARCHING | OEP_STEALTH, OEP_SEARCHING | OEP_WARNING },
        { BLUE_DRAGON_SCALE_MAIL, OEP_SPEED | OEP_SHOCK_RES,
          OEP_REGEN | OEP_DISPLACED, OEP_REGEN | OEP_DISPLACED },
        { WHITE_DRAGON_SCALE_MAIL, OEP_SLOW_DIGEST | OEP_COLD_RES, OEP_WARNING, OEP_WARNING },
        { ALCHEMY_SMOCK, OEP_POISON_RES, OEP_FIRE_RES, OEP_FIRE_RES },
        { CHROMATIC_DRAGON_SCALE_MAIL, OEP_MAGIC_RES | OEP_REFLECTION,
          OEP_SPEED | OEP_FIRE_RES, OEP_SPEED },
        { PICK_AXE, OEP_PRIMORDIAL, OEP_FIRE, 0 },
        { POT_HEALING, OEP_REFLECTION, OEP_FIRE, 0 }
    };
    int i, swap;
    struct obj *a, *b, *out;
    for (i = 0; i < SIZE(cases); ++i)
        for (swap = 0; swap < 2; ++swap) {
            forge_test_clear();
            /* Choose legal source classes independently of the recipient. */
            a = forge_test_item((cases[i].a & OEP_WORN) ? PLATE_MAIL : SPEAR, 1);
            b = forge_test_item((cases[i].b & OEP_WORN) ? RING_MAIL : JAVELIN, 1);
            assert(enhancement_set(a, cases[i].a, OQ_FINE, FALSE));
            assert(enhancement_set(b, cases[i].b, OQ_EXCEPTIONAL, FALSE));
            out = forge15c_result(cases[i].target, swap ? b : a, swap ? a : b);
            assert(out->o_enh_props == cases[i].expected);
            assert(out->o_enh_quality == (i >= 19 ? OQ_STANDARD : OQ_EXCEPTIONAL));
            assert(!out->o_enh_known && !out->o_enh_flags);
        }
    forge_test_clear();
    printf("PASS Step 15C property cases=%d (ranking, filtering, identities and native secondary restrictions)\n", 2 * SIZE(cases));
}

static void
forge15c_capabilities(void)
{
    /* spe: 1 = signed enchantment, 2 = charges >= -1 (obj.h).
     * e1/e2/proof are literal native default-material expectations. */
    static const struct { int typ, quality, spe, e1, e2, proof, buc; } cases[] = {
        { KATANA, 1, 1, 1, 1, 1, 1 },
        { PICK_AXE, 0, 1, 1, 1, 1, 1 },
        { ELVEN_SHIELD, 1, 1, 1, 1, 1, 1 },
        { SILVER_SABER, 1, 1, 0, 0, 0, 1 },
        { CRYSTAL_PLATE_MAIL, 1, 1, 1, 0, 1, 1 },
        { BRONZE_PLATE_MAIL, 1, 1, 0, 1, 1, 1 },
        { CRYSKNIFE, 1, 1, 0, 0, 1, 1 },
        { RIN_GAIN_STRENGTH, 0, 1, 0, 0, 0, 1 },
        { RIN_WARNING, 0, 0, 0, 0, 0, 1 },
        { WAN_FIRE, 0, 2, 0, 0, 0, 1 },
        { CRYSTAL_BALL, 0, 2, 0, 0, 0, 1 },
        { MAGIC_MARKER, 0, 2, 0, 0, 0, 1 },
        { MAGIC_FLUTE, 0, 2, 0, 0, 0, 1 },
        { MAGIC_LAMP, 0, 0, 0, 0, 0, 1 },
        { TIN_OPENER, 0, 0, 0, 0, 0, 1 },
        { HEAVY_IRON_BALL, 0, 0, 1, 1, 1, 1 },
        { IRON_CHAIN, 0, 0, 1, 1, 1, 1 },
        { POT_HEALING, 0, 0, 0, 0, 0, 1 },
        { FOOD_RATION, 0, 0, 0, 0, 0, 1 },
        { SLIME_MOLD, 0, 0, 0, 0, 0, 1 },
        { STATUE, 0, 0, 0, 0, 0, 1 },
        { LARGE_BOX, 0, 0, 0, 0, 0, 1 },
        { GOLD_PIECE, 0, 0, 0, 0, 0, 0 }
    };
    static const int values[] = { -127, -2, -1, 23, 127 };
    int i, s, expected, low, tail;
    struct obj *a, *b, *out, *fresh;
    enum enhancement_context previous;
    for (i = 0; i < SIZE(cases); ++i)
        for (s = 0; s < SIZE(values); ++s) {
            forge_test_clear();
            a = forge_test_item(LONG_SWORD, 1);
            b = forge_test_item(BROADSWORD, 1);
            assert(enhancement_set(a, OEP_FIRE, OQ_EXCEPTIONAL, FALSE));
            a->spe = b->spe = values[s];
            a->cursed = b->cursed = 1;
            a->oeroded = b->oeroded = 1;
            a->oeroded2 = b->oeroded2 = 2;
            a->oerodeproof = 1;
            /* Base-type discovery can exercise Wisdom (native RNG), which
             * is independent of constructor and inheritance randomness. */
            makeknown(cases[i].typ);
            init_isaac64(151503UL, rn2);
            previous = enhancement_context_set(ENH_CONTEXT_NONE);
            fresh = mksobj(cases[i].typ, FALSE, FALSE);
            (void) enhancement_context_set(previous);
            tail = rn2(1000000);
            init_isaac64(151503UL, rn2);
            out = forge15c_result(cases[i].typ, a, b);
            assert(rn2(1000000) == tail); /* only native constructor RNG */
            low = cases[i].spe == 2 ? -1 : -99;
            expected = values[s] < low ? low : values[s] > 99 ? 99 : values[s];
            assert(out->spe == (cases[i].spe ? expected : fresh->spe));
            assert(out->o_enh_quality == (cases[i].quality ? OQ_EXCEPTIONAL : 0));
            assert((int) out->oeroded == cases[i].e1);
            assert((int) out->oeroded2 == 2 * cases[i].e2);
            assert((int) out->oerodeproof == cases[i].proof);
            assert((int) out->cursed == cases[i].buc && !out->blessed);
            assert(out->obranch_material == fresh->obranch_material);
            assert(out->known == fresh->known && out->cknown == fresh->cknown
                   && out->lknown == fresh->lknown && out->tknown == fresh->tknown);
            assert(!out->bknown && !out->rknown && !out->o_enh_known);
            obfree(fresh, NULL);
        }
    forge_test_clear();
    printf("PASS Step 15C capability cases=%d (fresh defaults, native limits, generic classes, constructor RNG)\n", SIZE(cases) * SIZE(values));
}

static void
forge15c_arrangements(void)
{
    static const int permutations[6][3] = {
        { 0, 1, 2 }, { 0, 2, 1 }, { 1, 0, 2 },
        { 1, 2, 0 }, { 2, 0, 1 }, { 2, 1, 0 }
    };
    static const uint32 props[3] = {
        OEP_FIRE_III | OEP_COLD_III, OEP_PRIMORDIAL | OEP_FIRE,
        OEP_SHOCK_III | OEP_SHOCK_II
    };
    static const int spe[3] = { 17, -2, 7 }, e1[3] = { 2, 1, 3 }, e2[3] = { 1, 3, 0 };
    struct forge_recipe r = { KATANA, { { LONG_SWORD, 3 }, { LONG_SWORD, 3 } } };
    struct forge_allocation a[8], temp;
    struct obj *obj, *hammer, *out, snapshots[10];
    int perm, split, known, reverse, i, k, n, ns, left, take, cases = 0;
    for (perm = 0; perm < 6; ++perm)
        for (split = 0; split < 2; ++split)
            for (known = 0; known < 2; ++known)
                for (reverse = 0; reverse < 2; ++reverse) {
                    forge_test_clear(); forge_test_script();
                    memset(a, 0, sizeof a);
                    n = split ? 6 : 3; left = 3;
                    for (i = 0; i < n; ++i) {
                        k = permutations[perm][i / (split ? 2 : 1)];
                        obj = forge_test_item(LONG_SWORD, split ? 2 : 3);
                        assert(enhancement_set(obj, props[k], k, FALSE));
                        obj->spe = spe[k]; obj->blessed = k == 2; obj->cursed = k == 1;
                        obj->oeroded = e1[k]; obj->oeroded2 = e2[k]; obj->oerodeproof = k == 1;
                        obj->greased = obj->opoisoned = 1;
                        obj->obranch_material = GOLD; obj->obranch_size = MZ_LARGE + 1;
                        obj->obranch_props = 1; obj->age = 123; obj->recharged = 3;
                        obj->bypass = obj->no_charge = 1;
                        obj = oname(obj, "no inherited name", ONAME_NO_FLAGS);
                        if (known) fully_identify_obj(obj);
                        a[i].oid = obj->o_id;
                        take = split ? 1 : 2;
                        a[i].quantity[0] = left > take ? take : left;
                        a[i].quantity[1] = take - a[i].quantity[0];
                        left -= (int) a[i].quantity[0];
                    }
                    /* Unselected matching stack and activating hammer: both
                     * would incorrectly improve spe/minima if included. */
                    obj = forge_test_item(LONG_SWORD, 1); obj->spe = 90;
                    a[n].oid = obj->o_id;
                    hammer = forge_test_item(WAR_HAMMER, 1); hammer->spe = 99;
                    a[n + 1].oid = hammer->o_id;
                    obj = forge_test_item(SACK, 1);
                    out = forge_output(LONG_SWORD); out->spe = 99;
                    add_to_container(obj, out);
                    if (reverse)
                        for (i = 0; i < (n + 2) / 2; ++i) {
                            temp = a[i]; a[i] = a[n + 1 - i]; a[n + 1 - i] = temp;
                        }
                    for (ns = 0, obj = gi.invent; obj; obj = obj->nobj)
                        snapshots[ns++] = *obj;
                    forge_test_choose("Confirm crafting", 2, -1);
                    assert(!forge_confirm(&r, a, n + 2));
                    for (i = 0; i < ns; ++i)
                        forge_test_same_object(&snapshots[i], forge_find(snapshots[i].o_id));
                    assert(forge_commit(&r, a, n + 2) == ECMD_TIME);
                    for (out = gi.invent; out && out->otyp != KATANA; out = out->nobj) ;
                    assert(out && out->spe == 17 && out->blessed && !out->cursed);
                    assert(out->o_enh_quality == OQ_EXCEPTIONAL);
                    assert(out->o_enh_props == (OEP_PRIMORDIAL | OEP_FIRE_III));
                    assert(out->oeroded == 1 && !out->oeroded2 && out->oerodeproof);
                    assert(out->dknown && objects[KATANA].oc_name_known);
                    assert(!out->known && !out->bknown && !out->rknown
                           && !out->cknown && !out->lknown && !out->tknown
                           && !out->o_enh_known && !out->o_enh_flags);
                    assert(!out->oextra && !out->greased && !out->opoisoned
                           && !out->obranch_material && !out->obranch_size
                           && !out->obranch_props && !out->recharged && !out->bypass
                           && !out->no_charge && !out->unpaid && !out->in_use
                           && !out->owornmask && !out->timed && !out->lamplit);
                    assert(out->age == max(svm.moves, 1L));
                    for (i = 0; i < ns; ++i) {
                        obj = forge_find(snapshots[i].o_id);
                        assert(obj && out->o_id != obj->o_id);
                        for (k = 0; k < n + 2; ++k)
                            if (a[k].oid == obj->o_id
                                && (a[k].quantity[0] || a[k].quantity[1])) {
                                snapshots[i].quan -= a[k].quantity[0] + a[k].quantity[1];
                                snapshots[i].owt = weight(&snapshots[i]);
                            }
                        snapshots[i].nobj = obj->nobj; /* addinv can relink */
                        forge_test_same_object(&snapshots[i], obj);
                    }
                    ++cases;
                }
    forge_test_clear();
    printf("PASS Step 15C equivalent allocation/knowledge/allowlist cases=%d; cancellation snapshots include hammer and topology\n", cases);
}

static void
forge15c_capacity(void)
{
    struct forge_recipe r;
    struct forge_allocation a[2];
    struct obj *first, *second, *obj, snapshots[52];
    int shared, freed, mode, i, n, expected, count, cases = 0;
    unsigned target;
    for (shared = 0; shared < 2; ++shared)
        for (freed = 0; freed < 2; ++freed)
            for (mode = 0; mode < 4; ++mode) {
                forge_test_clear(); forge_test_script();
                memset(a, 0, sizeof a);
                r.output = DAGGER;
                r.need[0].otyp = DAGGER; r.need[0].quantity = 1;
                r.need[1].otyp = shared ? DAGGER : STILETTO; r.need[1].quantity = 1;
                first = forge_test_item(DAGGER, (shared ? 2 : 1) + !freed);
                /* mode 0: old plain output would merge with first; inherited
                 * output cannot. mode 1: inherited output can merge with the
                 * surviving first stack, but old plain output could not. */
                if (mode != 0 || shared)
                    assert(enhancement_set(first, OEP_FIRE, OQ_FINE, FALSE));
                if (mode != 0 || shared) first->spe = 17;
                a[0].oid = first->o_id; a[0].quantity[0] = 1;
                if (shared) {
                    a[0].quantity[1] = 1; n = 1;
                    if (mode == 0) first->nomerge = 1;
                } else {
                    second = forge_test_item(STILETTO, 2);
                    second->spe = 17;
                    assert(enhancement_set(second, OEP_FIRE, OQ_FINE, FALSE));
                    a[1].oid = second->o_id; a[1].quantity[1] = 1; n = 2;
                }
                if (mode == 2) first->o_enh_known = OEP_FIRE;
                target = first->o_id;
                if (mode == 3) {
                    first->nomerge = 1;
                    obj = forge_test_item(DAGGER, 1);
                    assert(enhancement_set(obj, OEP_FIRE, OQ_FINE, FALSE));
                    obj->spe = 17; target = obj->o_id;
                }
                forge_test_item(WAR_HAMMER, 1);
                while (inv_cnt(FALSE) < 52) forge_test_item(LONG_SWORD, 1);
                for (i = 0, obj = gi.invent; obj; obj = obj->nobj) snapshots[i++] = *obj;
                expected = freed || mode == 1 || mode == 3;
                assert(forge_commit(&r, a, n) == (expected ? ECMD_TIME : ECMD_OK));
                if (!expected) {
                    for (i = 0; i < 52; ++i)
                        forge_test_same_object(&snapshots[i], forge_find(snapshots[i].o_id));
                } else {
                    assert(inv_cnt(FALSE) <= 52);
                    if (freed) assert(!forge_find(a[0].oid));
                    if (mode == 3 || (mode == 1 && !freed)) {
                        obj = forge_find(target); assert(obj && obj->quan == 2);
                    }
                    count = 0;
                    for (obj = gi.invent; obj; obj = obj->nobj)
                        if (obj->otyp == DAGGER && obj->o_enh_known == 0
                            && obj->o_enh_props == OEP_FIRE && obj->spe == 17)
                            count += (int) obj->quan;
                    assert(count >= 1);
                }
                ++cases;
            }
    forge_test_clear();
    printf("PASS Step 15C finalized-output capacity cases=%d (actual/knowledge compatibility, surviving/consumed targets, shared/partial stacks)\n", cases);
}

static void
forge15c_rng(void)
{
    struct obj *obj, *donor;
    struct forge_state state;
    enum enhancement_context previous;
    int context, i, tail, count = 0, natural = 0;
    previous = enhancement_context_set(ENH_CONTEXT_NONE);
    donor = forge_output(LONG_SWORD);
    assert(enhancement_set(donor, OEP_PRIMORDIAL | OEP_FIRE_III, OQ_EXCEPTIONAL, TRUE));
    donor->spe = -2; donor->cursed = 1; donor->oeroded = 1;
    for (context = ENH_CONTEXT_NONE; context <= ENH_CONTEXT_SHOP; ++context) {
        (void) enhancement_context_set(context);
        forge_fail_construction = TRUE;
        assert(!forge_output(KATANA));
        forge_fail_construction = FALSE;
        assert(enhancement_context_set(context) == context);
        for (i = 0; i < 16; ++i) {
            obj = forge_output(i % 2 ? PLATE_MAIL : KATANA);
            assert(!obj->o_enh_props && !obj->o_enh_quality); /* constructor plain */
            assert(enhancement_context_set(context) == context);
            init_isaac64(151504UL + i, rn2); tail = rn2(1000000);
            init_isaac64(151504UL + i, rn2);
            memset(&state, 0, sizeof state);
            forge_gather(&state, donor); forge_inherit(obj, &state);
            assert(rn2(1000000) == tail);
            assert(obj->o_enh_props == (i % 2 ? 0 : OEP_PRIMORDIAL | OEP_FIRE_III));
            assert(obj->spe == -2 && obj->cursed && obj->oeroded == 1);
            assert(!obj->o_enh_known && !obj->o_enh_flags);
            obfree(obj, NULL); ++count;
        }
    }
    init_isaac64(151505UL, rn2);
    /* Same restored SHOP scope must still opt later native constructors in. */
    for (i = 0; i < 100; ++i) {
        obj = mksobj(KATANA, FALSE, FALSE);
        natural += obj->o_enh_quality != 0 || obj->o_enh_props != 0;
        obfree(obj, NULL);
    }
    assert(natural > 0);
    obfree(donor, NULL);
    (void) enhancement_context_set(previous);
    printf("PASS Step 15C zero inheritance RNG/context cases=%d, construction failures=4; later natural enhancements=%d/100\n", count, natural);
}

static void forge16a_properties(void)
{
    struct obj *a,*b,*out;
    struct forge_state state={0};
    int i;
    const uint64 left[]={OEP_VAMPIRIC_I,OEP_STONING_I,OEP_ANARCHIC_I,OEP_AXIOMATIC_II};
    const uint64 right[]={OEP_VAMPIRIC_IV,OEP_STONING_IV,OEP_AXIOMATIC_II,OEP_ANARCHIC_II};
    const uint64 expect[]={OEP_VAMPIRIC_IV,OEP_STONING_IV,OEP_AXIOMATIC_II,OEP_ANARCHIC_II};
    for(i=0;i<4;++i) {
        forge_test_clear();a=forge_test_item(LONG_SWORD,1);b=forge_test_item(BROADSWORD,1);
        assert(enhancement_set(a,left[i],0,FALSE));assert(enhancement_set(b,right[i],0,FALSE));
        if(i==1){a->o_stoning_remaining=63;b->o_stoning_remaining=9;}
        out=forge15c_result(KATANA,a,b);
        assert(out->o_enh_props==expect[i]&&!out->o_stoning_remaining&&!out->o_enh_known);
    }
    forge_test_clear();
    state.present=TRUE;state.props.word[0]=OEP_STONING_IV|OEP_ANARCHIC_II;
    out=forge_output(ARROW);forge_inherit(out,&state);assert(!out->o_enh_props);obfree(out,NULL);
    out=forge_output(DAGGER);out->quan=3;forge_inherit(out,&state);
    assert(out->o_enh_props==OEP_ANARCHIC_II&&!out->o_stoning_remaining);obfree(out,NULL);
    puts("PASS Step 16A forge strongest eligible family, cross-family filtering, ammo exclusion and new Stoning object Ready");
}

static void
forge16c_properties(void)
{
    struct forge_state state={0};
    struct obj *a=forge_output(MAGIC_LAMP), *b=forge_output(PICK_AXE), *out;
    int next;
    assert(enhancement_set_mask(a, enhancement_mask_union(
        enhancement_mask_property(EP_PURIFICATION_I),
        enhancement_mask_property(EP_COMMERCE_III)), 0, TRUE));
    assert(enhancement_set_mask(b, enhancement_mask_union(
        enhancement_mask_property(EP_PURIFICATION_IV),
        enhancement_mask_property(EP_EXCAVATING)), 0, TRUE));
    a->o_purification_remaining=0;b->o_purification_remaining=7;
    forge_gather(&state,a);forge_gather(&state,b);
    out=forge_output(OIL_LAMP);
    init_isaac64(1616001,rn2);next=rn2(1000000);init_isaac64(1616001,rn2);
    forge_inherit(out,&state);
    assert(rn2(1000000)==next);
    assert(enhancement_has(out,EP_PURIFICATION_IV)&&enhancement_has(out,EP_COMMERCE_III));
    assert(out->o_purification_remaining==100 && !out->o_enh_known2 && !out->o_enh_quality);
    assert(!enhancement_has(out,EP_EXCAVATING));obfree(out,NULL);
    out=forge_output(PICK_AXE);forge_inherit(out,&state);
    assert(enhancement_has(out,EP_PURIFICATION_IV)&&enhancement_has(out,EP_EXCAVATING));
    assert(!enhancement_has(out,EP_COMMERCE_III)); /* Catalogue-order T3 tie. */
    obfree(out,NULL);
    out=forge_output(DAGGER);forge_inherit(out,&state);
    assert(!enhancement_mask_count(enhancement_actual(out)) && !out->o_purification_remaining);
    obfree(out,NULL);obfree(a,NULL);obfree(b,NULL);
    /* Origin, rather than a per-tool Utility roster, controls inheritance. */
    out=forge_output(ARROW);
    objects[ARROW].oc_class=out->oclass=TOOL_CLASS;
    forge_inherit(out,&state);
    assert(enhancement_has(out,EP_PURIFICATION_IV));
    assert(enhancement_has(out,EP_COMMERCE_III));
    assert(!enhancement_has(out,EP_EXCAVATING));
    objects[ARROW].oc_class=out->oclass=WEAPON_CLASS;
    obfree(out,NULL);
    {
        const int denied[]={SADDLE,CHEST,LARGE_BOX,ICE_BOX,IRON_SAFE,
            CANDELABRUM_OF_INVOCATION,BELL_OF_OPENING,BEARTRAP,LAND_MINE,
            BAG_OF_TRICKS,CRYSTAL_BALL,MIRROR,TINNING_KIT,FIGURINE,
            TALLOW_CANDLE,WAX_CANDLE,MAGIC_CANDLE,CAN_OF_GREASE,CRYSTAL_PICK,
            LIVING_MASK,MASK,SYLLABLE_OF_STRENGTH__AESH,FIRST_WORD,
            UNIVERSAL_KEY,TORCH,SHADOWLANDER_S_TORCH,DOUBLE_LIGHTSABER,
            GENERIC_TOOL};
        int i;
        for(i=0;i<SIZE(denied);++i) {
            out=forge_output(denied[i]);forge_inherit(out,&state);
            assert(!enhancement_mask_count(enhancement_actual(out)));
            obfree(out,NULL);
        }
    }
    puts("PASS Step 16C forge Utility recipient filtering, strongest family, deterministic tie, fresh full timer and zero RNG");
}

static void
step15c_test_main(void)
{
    forge16c_properties();
    forge16a_properties();
    forge15c_reductions();
    forge15c_properties();
    forge15c_capabilities();
    forge15c_arrangements();
    forge15c_capacity();
    forge15c_rng();
}
