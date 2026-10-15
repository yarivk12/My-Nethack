/* Included by the native runtime harness. Exercises native commands and
 * native storage/timers, not a second implementation of the property rules. */
extern int step22_test_icebox_insert(struct obj *, struct obj *);
extern int step22_test_edibility(struct obj *);
static char step22_food_warning[BUFSZ];
static struct obj *step22_preserve_target;
static boolean step22_preserve_listed;
static void
step22_preserve_menu(winid win, const glyph_info *glyph, const ANY_P *id,
                     char letter, char group, int attr, int color,
                     const char *s, unsigned int flags)
{
    if (id->a_obj == step22_preserve_target)
        step22_preserve_listed = TRUE;
    inspect_add(win, glyph, id, letter, group, attr, color, s, flags);
}
static int
step22_preserve_select(winid win, int how, MENU_ITEM_P **pick)
{
    (void) win;
    assert(how == PICK_ONE && step22_preserve_listed);
    *pick = (menu_item *) alloc(sizeof **pick);
    (*pick)->item.a_obj = step22_preserve_target;
    (*pick)->count = 1;
    return 1;
}
static char
step22_food_decline(const char *prompt, const char *choices, char def)
{
    (void) choices; (void) def;
    Strcpy(step22_food_warning, prompt);
    return 'n';
}

static struct obj *
step22_item(int type)
{
    struct obj *obj = mksobj(type, FALSE, FALSE);
    assert(obj);
    obj->nomerge = TRUE;
    obj->blessed = obj->cursed = FALSE;
    obj->dknown = obj->known = TRUE;
    return addinv(obj);
}

static void
step22_discard(struct obj *obj)
{
    obj_extract_self(obj);
    obfree(obj, (struct obj *) 0);
}

static struct obj *
step22_corpse(int monster, long age)
{
    struct obj *body = step22_item(CORPSE);
    obj_stop_timers(body);
    body->age = svm.moves - age;
    /* The initially generated species is random. Use the native species
     * transition so its corpse weight and timer match the chosen fixture. */
    set_corpsenm(body, monster);
    return body;
}

static void
step22_preservation_tests(void)
{
    struct obj *source, *body, *part, *box;
    long oldmoves = svm.moves, deadline, before;
    int buc, age, i;
    const int excluded[] = { PM_LIZARD, PM_LICHEN, PM_DEATH };
    assert(!gi.invent);
    svm.moves = max(svm.moves, 1000L);
    source = step22_item(FLINT);
    artifact_exists(source, "The Lodestone of Gravity", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_PRESERVE, EX_MANUAL, EX_HERO, 0L);
    for (buc = -1; buc <= 1; ++buc)
        for (i = 0; i < 2; ++i) {
            age = 40 + buc * 20 - 1 + i;
            body = step22_corpse(PM_NEWT, age);
            body->cursed = buc < 0; body->blessed = buc > 0;
            before = source->exotic.preservation_count;
            assert(ex_preserve(source, body) == !i);
            assert(source->exotic.preservation_count == (unsigned) before + !i);
            assert(body->exotic.preserved == !i);
            step22_discard(body);
        }
    for (i = 0; i < SIZE(excluded); ++i) {
        body = step22_corpse(excluded[i], 0);
        assert(!ex_preserve(source, body));
        step22_discard(body);
    }
    body = step22_corpse(PM_NEWT, 0);
    body->orotten = TRUE;
    assert(!ex_preserve(source, body));
    step22_discard(body);
    for (i = 0; i < 4; ++i) {
        const unsigned counts[] = {19,20,59,60};
        const unsigned multipliers[] = {10,20,20,30};
        source->exotic.preservation_count = counts[i];
        body = step22_corpse(PM_NEWT, 35);
        deadline = peek_timer(ROT_CORPSE, obj_to_any(body)) - svm.moves;
        assert(ex_preserve(source, body));
        assert(body->exotic.multiplier == multipliers[i]);
        assert(source->exotic.preservation_count == counts[i] + 1);
        assert(peek_timer(ROT_CORPSE, obj_to_any(body)) - svm.moves
               == deadline * (long) multipliers[i]);
        assert(!ex_preserve(source, body));
        svm.moves += multipliers[i] * 5;
        assert(ex_corpse_age(body) == 40);
        step22_discard(body);
    }
    source->exotic.preservation_count = 0;
    body = step22_corpse(PM_NEWT, 0);
    body->quan = 3; body->owt = weight(body);
    assert(ex_preserve(source, body));
    part = body->nobj;
    assert(body->quan == 2 && !body->exotic.preserved
           && part && part->quan == 1 && part->exotic.preserved);
    assert(source->exotic.preservation_count == 1);
    step22_discard(part); step22_discard(body);
    source->exotic.preservation_count = 0;
    /* Run the actual #apply picker and read the real #inspect supplement. */
    body = step22_corpse(PM_NEWT, 0);
    inspect_window_setup();
    step22_preserve_target = body; step22_preserve_listed = FALSE;
    windowprocs.win_add_menu = step22_preserve_menu;
    windowprocs.win_select_menu = step22_preserve_select;
    cmdq_add_key(CQ_CANNED, source->invlet);
    assert(near_capacity() < EXT_ENCUMBER);
    assert(doapply() == ECMD_TIME && body->exotic.preserved);
    inspect_screen[0] = '\0';
    ex_preserve_inspect(body, 1);
    assert(strstr(inspect_screen, "Preserved: 10x slower aging"));
    discover_artifact(source->oartifact);
    inspect_screen[0] = '\0';
    ex_preserve_inspect(source, 1);
    assert(strstr(inspect_screen, "rank I") && strstr(inspect_screen, "1 successes"));
    step22_discard(body);
    step15b_window_setup();
    body = step22_corpse(PM_NEWT, 35);
    assert(ex_preserve(source, body));
    ++svm.moves;
    assert(ex_corpse_age(body) == 35 && body->exotic.age_units == 4212);
    body->quan = 3; body->owt = weight(body);
    part = splitobj(body, 1);
    assert(part->exotic.age_units == body->exotic.age_units
           && part->exotic.multiplier == 10 && part->exotic.preserved);
    assert(body->quan + part->quan == 3 && !mergable(body, part));
    step22_discard(part);
    box = step22_item(ICE_BOX);
    assert(step22_test_icebox_insert(body, box) == 1);
    assert(body->where == OBJ_CONTAINED && (body->exotic.frozen & 2));
    before = body->exotic.age_units;
    svm.moves += 1000;
    assert(ex_corpse_age(body) == 35 && body->exotic.age_units == before);
    obj_extract_self(body);
    removed_from_icebox(body);
    body = addinv(body);
    svm.moves += 9;
    assert(ex_corpse_age(body) == 36 && body->exotic.age_units == 4320);
    assert(strstr(doname(body), "preserved"));
    step22_discard(body);
    step22_discard(box);
    /* Floor ice uses the same native movement hook as pickup and melting. */
    body = step22_corpse(PM_NEWT, 0);
    assert(ex_preserve(source, body));
    freeinv(body);
    { int typ = levl[u.ux][u.uy].typ;
      levl[u.ux][u.uy].typ = ICE;
      place_object(body, u.ux, u.uy);
      assert(body->exotic.frozen & 1);
      svm.moves += 19;
      assert(ex_corpse_age(body) == 0 && body->exotic.age_units == 114);
      levl[u.ux][u.uy].typ = ROOM;
      obj_ice_effects(u.ux, u.uy, FALSE);
      assert(!(body->exotic.frozen & 1));
      svm.moves += 1;
      assert(ex_corpse_age(body) == 1 && body->exotic.age_units == 126);
      deadline = peek_timer(ROT_CORPSE, obj_to_any(body));
      before = body->exotic.decay_units;
      for (i = 0; i < 100; ++i) {
          levl[u.ux][u.uy].typ = ICE;
          obj_ice_effects(u.ux, u.uy, FALSE);
          levl[u.ux][u.uy].typ = ROOM;
          obj_ice_effects(u.ux, u.uy, FALSE);
          assert(body->exotic.decay_units == before
                 && peek_timer(ROT_CORPSE, obj_to_any(body)) == deadline);
      }
      step22_discard(body);
      levl[u.ux][u.uy].typ = typ;
    }
    body = step22_corpse(PM_NEWT, 39);
    freeinv(body);
    { int typ = levl[u.ux][u.uy].typ;
      levl[u.ux][u.uy].typ = ICE;
      place_object(body, u.ux, u.uy);
      ++svm.moves; /*39.5 effective turns, still fresh, retain the half turn*/
      assert(ex_preserve(source, body));
      assert(body->exotic.age_units == 4740L);
      step22_discard(body); levl[u.ux][u.uy].typ = typ;
    }
    /* Native timer classes stay separate for reviving/zombifying bodies. */
    body = step22_corpse(PM_TROLL, 0);
    obj_stop_timers(body);
    start_timer(7L, TIMER_OBJECT, REVIVE_MON, obj_to_any(body));
    deadline = peek_timer(REVIVE_MON, obj_to_any(body));
    assert(ex_preserve(source, body));
    assert(peek_timer(REVIVE_MON, obj_to_any(body)) == deadline);
    step22_discard(body);
    /* Exercise the actual native food hazard/freshness checks. */
    { long oldvegan = u.uconduct.unvegan, oldveg = u.uconduct.unvegetarian;
      u.uconduct.unvegan = u.uconduct.unvegetarian = 1;
      windowprocs.win_yn_function = step22_food_decline;
      for (buc = -1; buc <= 1; ++buc) {
          body = step22_corpse(PM_NEWT, 0);
          body->cursed = buc < 0; body->blessed = buc > 0;
          assert(ex_preserve(source, body));
          svm.moves += (40L + buc * 20L) * 10L - 1L;
          assert(!step22_test_edibility(body));
          ++svm.moves;
          step22_food_warning[0] = '\0';
          assert(step22_test_edibility(body) == 1
                 && strstr(step22_food_warning, "rotten"));
          step22_discard(body);
      }
      body = step22_corpse(PM_COCKATRICE, 0);
      assert(ex_preserve(source, body));
      { long stoning = HStone_resistance, stoning_e = EStone_resistance;
        HStone_resistance = EStone_resistance = 0;
        assert(step22_test_edibility(body) == 1
               && strstr(step22_food_warning, "dangerous"));
        HStone_resistance = stoning; EStone_resistance = stoning_e;
      }
      step22_discard(body);
      u.uconduct.unvegan = oldvegan; u.uconduct.unvegetarian = oldveg;
      step15b_window_setup();
    }
    step22_discard(source);
    svm.moves = oldmoves;
    puts("PASS Step22 U3 BUC freshness boundaries, exclusions, rot, rank20/60, native decay timers, once-only lock, split quantities, fractional ice/icebox aging and revival separation");
}

static void
step22_consumption_tests(void)
{
    struct obj *source, *source2, *obj, *wand;
    struct monst shk = {0}, *oldfmon;
    struct eshk *eshk;
    unsigned seed, id;
    int expected, draw, next, quantity, effect;
    long debt;
    const int types[] = { POT_HEALING, POT_GAIN_ENERGY, SCR_CHARGING };
    int t, i;
    int healing_descr = objects[POT_HEALING].oc_descr_idx;
    int oldhp = u.uhp, oldmaxhp = u.uhpmax, oldpeak = u.uhppeak;
    assert(!gi.invent);
    source = step22_item(FLINT);
    artifact_exists(source, "The Lodestone of Gravity", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_CONSERVE, EX_DIRECT, EX_HERO, 0L);
    source2 = step22_item(LUCKSTONE);
    artifact_exists(source2, "The Heart of Ahriman", TRUE, ONAME_WISH);
    ex_test_grant(source2, EX_CONSERVE, EX_WIELDED, EX_HERO, 0L);
    /* Raw commitment boundary verifies exact per-source RNG and quantity
       identity without preceding effect RNG obscuring which draws are used. */
    setuwep(source2);
    for (t = 0; t < SIZE(types); ++t)
        for (seed = 1; seed <= 100; ++seed) {
            int cost = objects[types[t]].oc_cost;
            int chance = cost <= 100 ? 30 : cost <= 200 ? 20 : 10;
            obj = step22_item(types[t]); id = obj->o_id;
            obj->blessed = seed % 3 == 0;
            obj->cursed = seed % 3 == 1;
            obj->known = seed % 2;
            obj->quan = quantity = seed % 2 ? 1 : 3;
            init_isaac64(seed, rn2);
            expected = rn2(100) < chance;
            if (!expected) expected = rn2(100) < chance;
            next = rn2(100000);
            init_isaac64(seed, rn2);
            ex_consumption_begin(obj);
            useup(obj);
            ex_consumption_end();
            assert(rn2(100000) == next);
            obj = gi.invent;
            while (obj && obj->o_id != id) obj = obj->nobj;
            if (expected || quantity > 1) {
                assert(obj && obj->quan == quantity - !expected && !obj->in_use);
                step22_discard(obj);
            } else assert(!obj);
        }
    setuwep((struct obj *) 0);
    obj = step22_item(POT_HEALING); obj->quan = 3;
    init_isaac64(222UL, rn2); next = rn2(100000);
    init_isaac64(222UL, rn2);
    ex_consumption_begin(obj);
    freeinv(source); /* captured but no longer active at commit */
    setuwep(source2); /* newly active must wait for the next event */
    useup(obj); ex_consumption_end();
    assert(obj->quan == 2 && rn2(100000) == next);
    source = addinv(source); setuwep((struct obj *) 0);
    step22_discard(obj);
    /* q executes healing exactly once, retains exact original id/BUC,
       while initial selection cancellation never consumes or charges. */
    obj = step22_item(POT_HEALING); obj->quan = 100; obj->owt = weight(obj);
    objects[POT_HEALING].oc_descr_idx = objects[POT_WATER].oc_descr_idx;
    id = obj->o_id; makeknown(POT_HEALING);
    for (seed = 1, effect = 0; seed < 200 && !effect; ++seed) {
        long before;
        int native_hp;
        /* Compare to the actual native one-effect path with the same RNG,
           rather than accepting a broad range that could hide repetition. */
        u.uhpmax = 1000; u.uhp = 100;
        init_isaac64(seed, rn2);
        assert(dopotion(obj) == ECMD_TIME);
        native_hp = u.uhp;
        before = obj->quan;
        u.uhpmax = 1000; u.uhp = 100;
        init_isaac64(seed, rn2);
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(dodrink() == ECMD_TIME);
        assert(u.uhp > 100 && u.uhp == native_hp);
        assert(obj->o_id == id && !obj->blessed && !obj->cursed && !obj->in_use);
        effect = obj->quan == before;
    }
    assert(effect);
    quantity = obj->quan;
    cmdq_add_key(CQ_CANNED, '\033');
    assert(dodrink() == ECMD_CANCEL && obj->quan == quantity);
    step22_discard(obj);
    /* r commits charging before its later picker cancellation. */
    wand = step22_item(WAN_LIGHT);
    obj = step22_item(SCR_CHARGING); obj->quan = 100; obj->owt = weight(obj);
    makeknown(SCR_CHARGING); id = obj->o_id;
    for (seed = 1, effect = 0; seed < 200 && !effect; ++seed) {
        quantity = obj->quan;
        init_isaac64(seed, rn2);
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_key(CQ_CANNED, '\033');
        assert(doread() == ECMD_TIME);
        assert(obj->o_id == id && !obj->in_use);
        effect = obj->quan == quantity;
    }
    assert(effect);
    step22_discard(obj);
    step22_discard(wand);
    obj = step22_item(SCR_BLANK_PAPER); makeknown(SCR_BLANK_PAPER);
    init_isaac64(222UL, rn2); next = rn2(100000);
    init_isaac64(222UL, rn2);
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doread() == ECMD_TIME && obj->quan == 1);
    assert(rn2(100000) == next); /* native no-consumption means no rolls */
    step22_discard(obj);
    /* Legal original bill identity stays unpaid; each retained use adds
       exactly one price, independent of source count and future return. */
    neweshk(&shk); eshk = ESHK(&shk); eshk->bill_p = eshk->bill;
    shk.isshk = TRUE; shk.mhp = 10; shk.mpeaceful = TRUE;
    oldfmon = fmon; shk.nmon = oldfmon; fmon = &shk;
    obj = step22_item(POT_HEALING); obj->unpaid = TRUE;
    eshk->billct = 1; eshk->bill[0].bo_id = obj->o_id;
    eshk->bill[0].bquan = 1; eshk->bill[0].price = 137;
    for (seed = 1; ; ++seed) {
        init_isaac64(seed, rn2); draw = rn2(100);
        if (draw < 30) break;
    }
    for (i = 0; i < 3; ++i) {
        debt = eshk->debit;
        init_isaac64(seed, rn2);
        ex_consumption_begin(obj); useup(obj); ex_consumption_end();
        assert(obj->unpaid && obj->quan == 1
               && eshk->bill[0].bo_id == obj->o_id && eshk->billct == 1);
        assert(eshk->debit == debt + 137);
    }
    subfrombill(obj, &shk);
    assert(!obj->unpaid && !eshk->billct && eshk->debit == 411);
    step22_discard(obj);
    fmon = oldfmon; free_eshk(&shk); free((genericptr_t) shk.mextra);
    step22_discard(source2); step22_discard(source);
    objects[POT_HEALING].oc_descr_idx = healing_descr;
    u.uhp = oldhp; u.uhpmax = oldmaxhp; u.uhppeak = oldpeak;
    puts("PASS Step22 U4 independent cost-tier RNG, original one/stack identities, native q healing once, native r charging late cancel, initial cancel and repeated unpaid fees surviving return");
}

static void
step22_items_persistence_tests(void)
{
    struct obj *source, *body, *frozen, *box, *potion, *obj;
    struct ex_object_state source_state, corpse_state, frozen_state;
    unsigned source_id, corpse_id, box_id, frozen_id, potion_id, seed;
    long moves = svm.moves, deadline;
    NHFILE *file;
    assert(!gi.invent);
    source = step22_item(FLINT);
    artifact_exists(source, "The Lodestone of Gravity", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_PRESERVE, EX_MANUAL, EX_HERO, 0L);
    ex_test_grant(source, EX_CONSERVE, EX_DIRECT, EX_HERO, 0L);
    source->exotic.preservation_count = 20;
    body = step22_corpse(PM_NEWT, 35);
    assert(ex_preserve(source, body));
    ++svm.moves;
    assert(ex_corpse_age(body) == 35 && body->exotic.age_units == 4206);
    frozen = step22_corpse(PM_NEWT, 10);
    assert(ex_preserve(source, frozen));
    box = step22_item(ICE_BOX);
    assert(step22_test_icebox_insert(frozen, box) == 1);
    potion = step22_item(POT_HEALING); potion_id = potion->o_id;
    for (seed = 1; ; ++seed) {
        init_isaac64(seed, rn2);
        if (rn2(100) < 30) break;
    }
    init_isaac64(seed, rn2);
    ex_consumption_begin(potion); useup(potion); ex_consumption_end();
    assert(potion->o_id == potion_id && potion->quan == 1 && !potion->in_use);
    source_id = source->o_id; corpse_id = body->o_id;
    frozen_id = frozen->o_id; box_id = box->o_id;
    source_state = source->exotic; corpse_state = body->exotic;
    frozen_state = frozen->exotic;
    deadline = peek_timer(ROT_CORPSE, obj_to_any(body));
    freeinv(source); place_object(source, u.ux, u.uy);
    freeinv(body); place_object(body, u.ux, u.uy);
    freeinv(box); place_object(box, u.ux, u.uy);
    freeinv(potion); place_object(potion, u.ux, u.uy);
    /* Placement can reconcile the corpse clock, so capture the saved state. */
    corpse_state = body->exotic;
    file = get_freeing_nhfile();
    file_mode(file, WRITING | FREEING,
              open("step22-items-level.tmp", O_CREAT | O_TRUNC | O_WRONLY
                   | O_BINARY, _S_IREAD | _S_IWRITE));
    savelev(file, ledger_no(&u.uz)); close_nhfile(file);
    file = get_freeing_nhfile();
    file_mode(file, READING, open("step22-items-level.tmp", O_RDONLY | O_BINARY));
    getlev(file, 0, ledger_no(&u.uz)); close_nhfile(file);
    source = body = box = potion = 0;
    for (obj = fobj; obj; obj = obj->nobj) {
        if (obj->o_id == source_id) source = obj;
        if (obj->o_id == corpse_id) body = obj;
        if (obj->o_id == box_id) box = obj;
        if (obj->o_id == potion_id) potion = obj;
    }
    assert(source && body && box && potion);
    frozen = box->cobj;
    assert(frozen && frozen->o_id == frozen_id && frozen->where == OBJ_CONTAINED);
    assert(!memcmp(&source->exotic, &source_state, sizeof source_state));
    assert(!memcmp(&body->exotic, &corpse_state, sizeof corpse_state));
    assert(!memcmp(&frozen->exotic, &frozen_state, sizeof frozen_state));
    assert(peek_timer(ROT_CORPSE, obj_to_any(body)) == deadline);
    assert(potion->quan == 1 && !potion->in_use);
    assert(!ex_active(source, EX_PRESERVE, &gy.youmonst));
    obj_extract_self(source); source = addinv(source);
    assert(ex_active(source, EX_PRESERVE, &gy.youmonst)
           && ex_active(source, EX_CONSERVE, &gy.youmonst));
    svm.moves += 19;
    assert(ex_corpse_age(body) == 36 && body->exotic.age_units == 4320);
    assert(ex_corpse_age(frozen) == 10 && frozen->exotic.age_units == 1200);
    step22_discard(body); step22_discard(frozen); step22_discard(box);
    step22_discard(potion); step22_discard(source);
    svm.moves = moves;
    puts("PASS Step22 U3/U4 native savelev/getlev: grant modes, source proficiency, corpse fractional age/decay deadline, frozen custody and conserved original identity survive unload/reload and resume");
}

static void
step22_items_tests(void)
{
    step22_preservation_tests();
    step22_consumption_tests();
    step22_items_persistence_tests();
}
