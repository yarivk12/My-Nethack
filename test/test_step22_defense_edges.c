/* Native D1 hand/interruption, D2 active-pool, and D4 beam/Mask contracts. */
extern int step21_ray_checks, step21_ray_redirects, step21_ray_contacts;
static int step22_edge_positions;

static int
step22_edge_poskey(coordxy *x, coordxy *y, int *mod)
{
    /* getpos clears the entire canned queue when a queued key cancels.
     * Exercise real input instead: cancel source one, click the hero for two. */
    assert(step22_edge_positions < 2);
    *x = u.ux; *y = u.uy; *mod = 0;
    return step22_edge_positions++ == 0 ? '\033' : 0;
}

static int
step22_edge_select(winid win, int how, MENU_ITEM_P **picked)
{
    /* Native inventory/status refresh during form changes can display an
     * informational menu. It requests no item selection. */
    if (how == PICK_NONE) { *picked = NULL; return 0; }
    return inspect_select(win, how, picked);
}

static void
step22_edge_reset(struct monst *m)
{
    m->mhp = m->mhpmax = 100000;
    m->mpeaceful = m->mtame = m->msleeping = m->mcan = 0;
    m->mcanmove = m->mcansee = 1;
    m->mfrozen = m->mconf = m->mstun = m->mflee = 0;
    m->seen_resistance = 0; m->mspec_used = 0;
    m->mux = u.ux; m->muy = u.uy;
}

static void
step22_defense_edge_tests(void)
{
    struct you saved = u, replay;
    struct window_procs windows = windowprocs;
    struct obj *main, *off, *mask, *amulet;
    struct monst *m;
    struct ex_melee_context event;
    struct attack beam = { AT_MAGC, AD_COLD, 2, 6 };
    int seed, expected, tail, first, second, both = 0, damage, stopped = 0;
    int oldmulti = gm.multi, oldwizard = wizard, oldstatus = iflags.status_updates;
    boolean oldnoteleport = svl.level.flags.noteleport;
    long oldstasis = svl.level.flags.stasis_until;
    assert(!gi.invent && !Upolyd);
    inspect_window_setup(); iflags.status_updates = FALSE;
    windowprocs.win_status_init = step21_status_init;
    windowprocs.win_status_enablefield = step21_status_field;
    windowprocs.win_status_update = step21_status_update;
    windowprocs.win_mark_synch = step21_no_animation_delay;
    windowprocs.win_wait_synch = step21_no_animation_delay;
    windowprocs.win_curs = step21_cursor;
    windowprocs.win_clear_nhwindow = step21_clear;
    windowprocs.win_print_glyph = step21_glyph;
    windowprocs.win_select_menu = step22_edge_select;
    memset(u.uprops, 0, sizeof u.uprops);
    u.uhp = u.uhpmax = 10000; u.uhitinc = 100; u.ulevel = 30;
    main = inspect_item(LONG_SWORD); main->oartifact = ART_STING;
    off = addinv(mksobj(DAGGER, FALSE, FALSE)); off->oartifact = ART_ORCRIST;
    main->spe = 9; off->spe = 1;
    setuwep(main); setuswapwep(off); u.twoweap = TRUE;
    ex_test_grant(main, EX_COUNTER, EX_WIELDED, EX_HERO, W_WEP);
    ex_test_grant(off, EX_COUNTER, EX_WIELDED, EX_HERO, W_SWAPWEP);
    m = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK | NO_MINVENT);
    assert(m && m_next2u(m));
    for (seed = 1; seed <= 200; ++seed) {
        step22_edge_reset(m); gm.multi = 0; replay = u;
        init_isaac64(235000UL + seed, rn2);
        first = ex_roll(EX_COUNTER);
        if (first) { ex_secondary_begin(); (void) ex_counterattack(m, main); ex_secondary_end(); }
        second = ex_roll(EX_COUNTER);
        if (second) { ex_secondary_begin(); (void) ex_counterattack(m, off); ex_secondary_end(); }
        damage = 100000 - m->mhp; tail = rn2(1000000);
        u = replay; step22_edge_reset(m);
        init_isaac64(235000UL + seed, rn2);
        ex_melee_begin(&event, m); ex_melee_end(&event);
        assert(100000 - m->mhp == damage && rn2(1000000) == tail);
        if (first && second) ++both;
    }
    assert(both > 8);
    mongone(m);
    puts("PASS Step22 D1 200 paired native dual-grant events match one main/offhand riposte per successful independent roll, including multiple successes and exact RNG (235001..235200)");

    /* A first contact's native paralysis prevents the second source's roll.
     * Compare against exactly one native riposte, including its passive. */
    m = makemon(&mons[PM_GELATINOUS_CUBE], u.ux + 1, u.uy, MM_ADJACENTOK | NO_MINVENT);
    assert(m && m_next2u(m));
    for (seed = 1; seed <= 1000 && !stopped; ++seed) {
        step22_edge_reset(m); gm.multi = 0; replay = u;
        init_isaac64(236000UL + seed, rn2);
        first = ex_roll(EX_COUNTER);
        if (first) { ex_secondary_begin(); (void) ex_counterattack(m, main); ex_secondary_end(); }
        if (!first || gm.multi >= 0) { u = replay; gm.multi = 0; continue; }
        damage = 100000 - m->mhp; tail = rn2(1000000);
        u = replay; gm.multi = 0; step22_edge_reset(m);
        init_isaac64(236000UL + seed, rn2);
        ex_melee_begin(&event, m); ex_melee_end(&event);
        assert(gm.multi < 0 && 100000 - m->mhp == damage && rn2(1000000) == tail);
        stopped = 1;
    }
    assert(stopped); gm.multi = 0; mongone(m);
    puts("PASS Step22 D1 native gelatinous passive paralysis stops later source before its roll and preserves one-contact RNG (236001..237000)");

    m = makemon(&mons[PM_ACID_BLOB], u.ux + 1, u.uy, MM_ADJACENTOK | NO_MINVENT);
    assert(m && m_next2u(m)); stopped = 0; wizard = FALSE;
    for (seed = 1; seed <= 500 && !stopped; ++seed) {
        long mortality, hits;
        step22_edge_reset(m); u.uhp = 1; u.uhpmax = 100; gm.multi = 0;
        amulet = addinv(mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE));
        setworn(amulet, W_AMUL); assert(Lifesaved);
        mortality = u.umortality; hits = u.uconduct.weaphit;
        init_isaac64(237000UL + seed, rn2);
        ex_melee_begin(&event, m); ex_melee_end(&event);
        if (u.umortality > mortality) {
            assert(u.umortality == mortality + 1 && u.uhp > 0);
            /* Only count a discriminating replay: the suppressed next source
             * would have passed its roll at the current RNG position. */
            if (rn2(100) < 33) {
                assert(u.uconduct.weaphit == hits + 1); stopped = 1;
            }
        }
        if (uamul) { setworn(NULL, W_AMUL); freeinv(amulet); obfree(amulet, NULL); }
    }
    assert(stopped); mongone(m); u.uhp = u.uhpmax = 10000;
    puts("PASS Step22 D1 native acid-passive lifesaving stops remaining ripostes even when the next source would succeed (237001..237500)");
    ex_test_clear(main); ex_test_clear(off);
    setuwep(NULL); setuswapwep(NULL); u.twoweap = FALSE;

    /* Actual form transition and damage use the active monster HP pool;
     * human HP stays healthy. A canceled first source leaves the next ready. */
    ex_test_grant(main, EX_TELEPORT, EX_DIRECT, EX_HERO, 0);
    ex_test_grant(off, EX_TELEPORT, EX_DIRECT, EX_HERO, 0);
    m = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK | NO_MINVENT);
    assert(m && m_next2u(m)); step22_edge_reset(m);
    u.uhp = u.uhpmax = 100; wizard = FALSE;
    svl.level.flags.noteleport = FALSE; svl.level.flags.stasis_until = 0;
    u.uhave.amulet = FALSE; HTeleport_control = INTRINSIC;
    assert(polymon(PM_TROLL)); u.mhmax = 100; u.mh = 26;
    u.ex_teleport_disarmed = FALSE;
    cmdq_clear(CQ_CANNED);
    step22_edge_positions = 0;
    windowprocs.win_nh_poskey = step22_edge_poskey;
    ex_melee_begin(&event, m); mdamageu_damage(m, 1, 1); ex_melee_end(&event);
    assert(step22_edge_positions == 2 && Upolyd && u.mh == 25
           && u.uhp == 100 && u.ex_teleport_disarmed);
    windowprocs.win_nh_poskey = windows.win_nh_poskey;
    rehumanize(); assert(!Upolyd && u.uhp == 100 && !u.ex_teleport_disarmed);
    cmdq_clear(CQ_CANNED); mongone(m);
    puts("PASS Step22 D2 actual polymorph HP qualifies independently of healthy human HP; first-source control cancellation permits second-source completion and rehumanization rearms");

    ex_test_clear(off); ex_test_clear(main);
    ex_test_grant(main, EX_ABSORB, EX_DIRECT, EX_HERO, 0);
    mask = addinv(mksobj(MASK, FALSE, FALSE)); mask->oartifact = ART_MIRRORED_MASK;
    setworn(mask, W_TOOL); assert(ublindf == mask);
    HCold_resistance = INTRINSIC;
    u.uenmax = 10000;
    m = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK | NO_MINVENT);
    assert(m && m_next2u(m)); both = stopped = 0;
    for (seed = 1; seed <= 300; ++seed) {
        step22_edge_reset(m); m->m_lev = 50; u.uen = 0;
        init_isaac64(238000UL + seed, rn2);
        first = rn2(3) != 0;
        expected = first && rn2(100) < 20;
        damage = expected ? d(6, 6) : 0;
        tail = rn2(1000000);
        step21_ray_checks = step21_ray_redirects = step21_ray_contacts = -1;
        init_isaac64(238000UL + seed, rn2);
        (void) buzzmu(m, &beam);
        assert(u.uen == damage);
        if (expected) {
            assert(m->mhp == 100000 && step21_ray_checks == -1
                   && step21_ray_redirects == -1 && step21_ray_contacts == -1);
            assert(rn2(1000000) == tail); ++both;
        } else if (first) {
            assert(step21_ray_checks >= 0); ++stopped;
        }
    }
    assert(both > 15 && stopped > 80);
    puts("PASS Step22 D4 300 actual buzzmu spells with worn Mirrored Mask: absorption restores exact 6d6 once before ray/Mask RNG, failed absorption reaches native ray defenses (238001..238300)");
    mongone(m); setworn(NULL, W_TOOL); freeinv(mask); obfree(mask, NULL);
    ex_test_clear(main); freeinv(main); obfree(main, NULL);
    freeinv(off); obfree(off, NULL);
    cmdq_clear(CQ_CANNED); u = saved; set_uasmon();
    windowprocs = windows; iflags.status_updates = oldstatus;
    svl.level.flags.noteleport = oldnoteleport; svl.level.flags.stasis_until = oldstasis;
    gm.multi = oldmulti; wizard = oldwizard;
}
