/* Included by the native runtime harness. No production artifact bindings. */
static void
step22_utility_raw(const char *text)
{
    fprintf(stderr, "Step22 utility native diagnostic: %s\n", text);
}

#include "test_step22_dig.c"

static void
step22_untrap_probability_tests(struct obj *first)
{
    int depths[] = { 1, 5, 6, 10, 11, 76, 80, 81, 200 };
    int chances[] = { 100, 100, 95, 95, 90, 25, 25, 20, 20 };
    struct obj *second = addinv(mksobj(DAGGER, FALSE, FALSE));
    struct obj *third = addinv(mksobj(SHORT_SWORD, FALSE, FALSE));
    int level, seed, n, i, expected, next;
    second->oartifact = ART_STING;
    third->oartifact = ART_ORCRIST;
    for (n = 1; n <= 3; ++n) {
        ex_test_grant(second, EX_UNTRAP, n >= 2 ? EX_DIRECT : EX_INACTIVE,
                      EX_HERO, 0L);
        ex_test_grant(third, EX_UNTRAP, n >= 3 ? EX_DIRECT : EX_INACTIVE,
                      EX_HERO, 0L);
        for (level = 0; level < SIZE(depths); ++level) {
            u.uz.dlevel = depths[level];
            assert(depth(&u.uz) == depths[level]);
            for (seed = 1; seed <= 200; ++seed) {
                init_isaac64((unsigned long) seed, rn2);
                expected = 0;
                for (i = 0; i < n; ++i)
                    if (rn2(100) < chances[level]) {
                        expected = 1;
                        break;
                    }
                next = rn2(100000);
                init_isaac64((unsigned long) seed, rn2);
                assert(ex_untrap_attempt() == expected);
                assert(rn2(100000) == next);
            }
        }
    }
    freeinv(second);
    obfree(second, (struct obj *) 0);
    freeinv(third);
    obfree(third, (struct obj *) 0);
    assert(ex_has(EX_UNTRAP) && first->where == OBJ_INVENT);
    u.uz.dlevel = 1;
    puts("PASS Step 22 exact U1 depth boundaries, 5400 deterministic 1/2/3-source independent sequential rolls and stop-on-success RNG accounting");
}

static void
step22_native_discovery_tests(void)
{
    struct obj *box = mksobj(CHEST, FALSE, FALSE);
    int seed, level = u.ulevel, hp = u.uhp, wisdom_exercise = AEXE(A_WIS);
    AEXE(A_WIS) = 50; /* avoid unrelated native exercise RNG in this fixture */
    box->otrapped = 1;
    box->tknown = 0;
    box->dknown = 1;
    place_object(box, u.ux, u.uy);
    u.ulevel = 1;
    for (seed = 1;; ++seed) {
        init_isaac64((unsigned long) seed, rn2);
        if (rn2(MAXULEV) >= 10)
            break;
    }
    init_isaac64((unsigned long) seed, rn2);
    assert(untrap(FALSE, 0, 0, box) == 1);
    assert(box->otrapped && !box->tknown && u.uhp == hp);
    /* Native discovery is retained; the artifact does not reveal a box
     * just because its disarming property is active. */
    for (seed = 1;; ++seed) {
        init_isaac64((unsigned long) seed, rn2);
        if (rn2(MAXULEV) < 10)
            break;
    }
    cmdq_add_key(CQ_CANNED, 'y');
    init_isaac64((unsigned long) seed, rn2);
    assert(untrap(FALSE, 0, 0, box) == 1);
    assert(!box->otrapped && !box->tknown && u.uhp == hp);

    u.uz.dlevel = 81;
    box->otrapped = box->tknown = 1;
    for (seed = 1;; ++seed) {
        init_isaac64((unsigned long) seed, rn2);
        (void) rn2(MAXULEV);
        if (rn2(100) >= 20)
            break;
    }
    cmdq_add_key(CQ_CANNED, 'y');
    init_isaac64((unsigned long) seed, rn2);
    assert(untrap(FALSE, 0, 0, box) == 1);
    assert(box->otrapped && box->tknown && u.uhp == hp);
    obj_extract_self(box);
    obfree(box, (struct obj *) 0);
    levl[41][10].typ = DOOR;
    levl[41][10].doormask = D_CLOSED | D_TRAPPED;
    /* A door is deliberately inspected again on every native attempt. */
    u.ulevel = MAXULEV;
    for (seed = 1;; ++seed) {
        init_isaac64((unsigned long) seed, rn2);
        if (rn2(11) < 10 && rn2(100) >= 20)
            break;
    }
    cmdq_add_key(CQ_CANNED, 'y');
    init_isaac64((unsigned long) seed, rn2);
    assert(untrap(FALSE, 41, 10, (struct obj *) 0) == 1);
    assert(levl[41][10].doormask == (D_CLOSED | D_TRAPPED) && u.uhp == hp);
    u.uz.dlevel = 1;
    cmdq_add_key(CQ_CANNED, 'y');
    init_isaac64((unsigned long) seed, rn2);
    assert(untrap(FALSE, 41, 10, (struct obj *) 0) == 1);
    assert(levl[41][10].doormask == D_CLOSED);
    levl[41][10].typ = ROOM;
    levl[41][10].flags = 0;
    u.ulevel = level;
    AEXE(A_WIS) = wisdom_exercise;
    puts("PASS Step 22 native box/door inspection, no forced discovery, known trap state, safe failures and successful retry");
}

static void
step22_utility_tests(void)
{
    struct obj *source = inspect_item(CRYSTAL_BALL), *gold;
    struct trap *trap;
    struct monst thief;
    d_level saved_level = u.uz;
    coordxy oldx = u.ux, oldy = u.uy;
    long oldmoves = svm.moves;
    int x, y, typ, water[] = { POOL, MOAT, BOG, PUDDLE };
    boolean shopdamage = FALSE;
    void (*old_raw)(const char *) = windowprocs.win_raw_print;
    void (*old_raw_bold)(const char *) = windowprocs.win_raw_print_bold;

    windowprocs.win_raw_print = windowprocs.win_raw_print_bold = step22_utility_raw;
    source->oartifact = ART_ORB_OF_FATE;
    u.ux = 40;
    u.uy = 10;
    u.uz.dnum = 0;
    u.uz.dlevel = 1;
    for (x = 34; x <= 46; ++x)
        for (y = 4; y <= 16; ++y) {
            levl[x][y].typ = ROOM;
            levl[x][y].flags = 0;
            levl[x][y].lit = 1;
            unblock_point(x, y);
        }
    vision_reset();
    vision_recalc(0);
    ex_test_grant(source, EX_UNTRAP, EX_DIRECT, EX_HERO, 0L);
    step22_untrap_probability_tests(source);
    step22_native_discovery_tests();
    fprintf(stderr, "Step22 utility diagnostic: floor trap suite begins\n");
    trap = maketrap(40, 10, BEAR_TRAP);
    assert(trap);
    trap->tseen = 1;
    set_utrap(10, TT_BEARTRAP);
    assert(ex_untrap_floor(trap) == 1);
    assert(!u.utrap && !t_at(40, 10));

    trap = maketrap(41, 10, POLY_TRAP);
    assert(trap);
    assert(ex_untrap_floor(trap) == 0 && t_at(41, 10) == trap);
    trap->tseen = 1;
    assert(ex_untrap_floor(trap) == 1 && !t_at(41, 10));
    for (typ = 1; typ < TRAPNUM; ++typ) {
        if (typ == MAGIC_PORTAL || typ == VIBRATING_SQUARE
            || typ == TRAPPED_DOOR || typ == TRAPPED_CHEST)
            continue;
        levl[41][10].typ = ROOM;
        fprintf(stderr, "Step22 utility diagnostic: floor trap type %d\n", typ);
        /* Launch geometry/statue generation belongs to native level
         * generation. This fixture isolates each existing trap code's
         * disarm path without creating unrelated monsters or boulders. */
        trap = maketrap(41, 10, ARROW_TRAP);
        assert(trap);
        trap->ttyp = typ;
        trap->tseen = 1;
        assert(ex_untrap_floor(trap) == 1 && !t_at(41, 10));
    }
    for (typ = 0; typ < 3; ++typ) {
        int trapped_type[] = { TT_WEB, TT_PIT, TT_PIT };
        int floor_type[] = { WEB, PIT, SPIKED_PIT };
        trap = maketrap(40, 10, floor_type[typ]);
        assert(trap);
        trap->tseen = 1;
        set_utrap(10, trapped_type[typ]);
        assert(ex_untrap_floor(trap) == 1);
        assert(!u.utrap && !t_at(40, 10));
    }
    trap = maketrap(40, 10, WEB);
    assert(trap);
    trap->tseen = 1;
    set_utrap(10, TT_INFLOOR);
    assert(!ex_untrap_floor(trap) && u.utrap && t_at(40, 10) == trap);
    reset_utrap(FALSE);
    deltrap(trap);
    {
        int seed, hp = u.uhp;
        u.uz.dlevel = 81;
        for (seed = 1;; ++seed) {
            init_isaac64((unsigned long) seed, rn2);
            if (rn2(100) >= 20)
                break;
        }
        trap = maketrap(40, 10, BEAR_TRAP);
        trap->tseen = 1;
        set_utrap(10, TT_BEARTRAP);
        init_isaac64((unsigned long) seed, rn2);
        assert(ex_untrap_floor(trap) == 1);
        assert(t_at(40, 10) == trap && u.utrap == 10 && u.uhp == hp);
        reset_utrap(FALSE);
        deltrap(trap);
        u.uz.dlevel = 1;
    }
    trap = maketrap(41, 10, MAGIC_PORTAL);
    assert(trap);
    trap->tseen = 1;
    assert(ex_untrap_floor(trap) == 0 && t_at(41, 10) == trap);
    deltrap(trap);
    trap = maketrap(41, 10, TRAPDOOR);
    assert(trap);
    trap->tseen = 1;
    {
        d_level here = u.uz;
        u.uz = stronghold_level;
        assert(!ex_untrap_floor(trap) && t_at(41, 10) == trap);
        u.uz = here;
        trap->dst.dnum = u.uz.dnum + 1;
        assert(!ex_untrap_floor(trap) && t_at(41, 10) == trap);
    }
    deltrap(trap);
    for (x = 40; x <= 41; ++x) {
        struct obj *salvage;
        while ((salvage = svl.level.objects[x][10]) != 0) {
            obj_extract_self(salvage);
            obfree(salvage, (struct obj *) 0);
        }
    }

    ex_test_grant(source, EX_WARD, EX_DIRECT, EX_HERO, 0L);
    memset(&thief, 0, sizeof thief);
    thief.data = &mons[PM_WOOD_NYMPH];
    thief.mx = 41;
    thief.my = 10;
    assert(steal(&thief, (char *) 0) == 0 && gi.invent == source);
    gold = addinv(mksobj(GOLD_PIECE, FALSE, FALSE));
    gold->quan = 100;
    stealgold(&thief);
    assert(gold->where == OBJ_INVENT && gold->quan == 100);
    assert(!thief.minvent);
    freeinv(gold);
    obfree(gold, (struct obj *) 0);

    ex_test_grant(source, EX_TRANSMUTE, EX_MANUAL, EX_HERO, 0L);
    for (typ = 0; typ < SIZE(water); ++typ) {
        levl[41][10].typ = water[typ];
        levl[41][10].flags = 0;
        assert(ex_transmute(source, 41, 10) == 1);
        assert(levl[41][10].typ == ICE);
        assert(!spot_time_left(41, 10, MELT_ICE_AWAY));
        /* Native external fire uses the same exact saved provenance. */
        (void) zap_over_floor(41, 10, AD_FIRE - 1,
                             &shopdamage, TRUE, 0);
        assert(levl[41][10].typ == water[typ]);
    }
    levl[41][10].typ = LAVAPOOL;
    assert(ex_transmute(source, 41, 10) == 1);
    assert(levl[41][10].typ == ROOM);
    assert(source->exotic.charges[1] == 4);
    assert(!ex_transmute(source, 46, 10));
    assert(!ex_transmute(source, 41, 10));
    levl[41][10].typ = WATER;
    assert(!ex_transmute(source, 41, 10));
    assert(source->exotic.charges[1] == 4);
    levl[41][10].typ = ROOM;
    cmdq_clear(CQ_CANNED);
    reset_commands(FALSE);
    fprintf(stderr, "Step22 utility diagnostic: digging charge grant\n");
    ex_test_grant(source, EX_DIG, EX_MANUAL, EX_HERO, 0L);
    ex_resources(source);
    assert(source->exotic.charges[0] == 5);
    cmdq_add_key(CQ_CANNED, '\033');
    fprintf(stderr, "Step22 utility diagnostic: digging direction cancel\n");
    assert(!ex_utility_invoke(source, EX_DIG));
    assert(source->exotic.charges[0] == 5);
    /* Confirmed ineffective native digging still spends its own charge. */
    cmdq_add_dir(CQ_CANNED, 1, 0, 0);
    fprintf(stderr, "Step22 utility diagnostic: native digging beam\n");
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    fprintf(stderr, "Step22 utility diagnostic: native digging complete\n");
    assert(source->exotic.charges[0] == 4
           && source->exotic.charges[1] == 4);
    /* Recharge uses world turns, preserves partial intervals when spending
     * again, and banks no credit while full. */
    svm.moves += 9;
    ex_resources(source);
    assert(source->exotic.charges[1] == 4);
    assert(ex_spend(source, 1, 1));
    ++svm.moves;
    ex_resources(source);
    assert(source->exotic.charges[1] == 4);
    svm.moves += 89;
    ex_resources(source);
    assert(source->exotic.charges[0] == 4);
    ++svm.moves;
    ex_resources(source);
    assert(source->exotic.charges[0] == 5);
    assert(source->exotic.charges[1] == 10);
    svm.moves += 1000;
    ex_resources(source);
    assert(ex_spend(source, 0, 1) && ex_spend(source, 1, 1));
    ex_resources(source);
    assert(source->exotic.charges[0] == 4
           && source->exotic.charges[1] == 9);
    levl[45][15].typ = POOL;
    assert(ex_transmute(source, 45, 15) == 1);
    assert(levl[45][15].typ == ICE && source->exotic.charges[1] == 8);
    assert(ex_transmute(source, 45, 15) == 1);
    assert(levl[45][15].typ == POOL && source->exotic.charges[1] == 7);
    levl[42][10].typ = STONE;
    block_point(42, 10);
    levl[43][10].typ = POOL;
    assert(!ex_transmute(source, 43, 10));
    assert(source->exotic.charges[1] == 7 && levl[43][10].typ == POOL);
    levl[42][10].typ = ROOM;
    unblock_point(42, 10);
    levl[43][10].typ = ROOM;
    levl[45][15].typ = ROOM;
    step22_native_dig_tests(source);
    ex_test_clear(source);
    inspect_discard(source);
    svm.moves = oldmoves;
    u.uz = saved_level;
    u.ux = oldx;
    u.uy = oldy;
    windowprocs.win_raw_print = old_raw;
    windowprocs.win_raw_print_bold = old_raw_bold;
    puts("PASS Step 22 native safe untrapping/self-release, ordinary theft ward, four water subtypes, external thaw, permanent ice and lava transition");
}
