/* Included by the native enhancement harness. */
static struct obj *step16c_loop_target;
static int step16c_loop_calls;

/* The headless harness has no initialized native status window. */
static void step16c_noop(void) { }
static void step16c_status_field(int n, const char *s, const char *f, boolean b)
{ (void) n; (void) s; (void) f; (void) b; }
static void step16c_status_update(int n, genericptr_t p, int c, int pc, int col,
                                 unsigned long *m)
{ (void) n; (void) p; (void) c; (void) pc; (void) col; (void) m; }
static void step16c_cursor(winid w, int x, int y)
{ (void) w; (void) x; (void) y; }
static void step16c_display(winid w, boolean b)
{ (void) w; (void) b; }

static int
step16c_test_occupation(void)
{
    ++step16c_loop_calls;
    curse(step16c_loop_target);
    return 1;
}

static void
step16c_actual_loop_test(void)
{
    struct obj *source = item(TIN_WHISTLE), *target = item(LONG_SWORD);
    struct obj *late = item(MAGIC_WHISTLE);
    struct window_procs saved_windows = windowprocs;
    long turn = svm.moves;
    int oldmovement = u.umovement, oldhunger = u.uhunger;
    int oldlycan = u.ulycn;
    windowprocs.win_status_init = step16c_noop;
    windowprocs.win_status_finish = step16c_noop;
    windowprocs.win_status_enablefield = step16c_status_field;
    windowprocs.win_status_update = step16c_status_update;
    windowprocs.win_get_nh_event = step16c_noop;
    windowprocs.win_curs = step16c_cursor;
    windowprocs.win_display_nhwindow = step16c_display;
    status_initialize(FALSE);
    addinv(source);
    addinv(target);
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_PURIFICATION_I), OQ_STANDARD, FALSE));
    source->o_purification_remaining = 1;
    utility_turn_cancel();
    step16c_loop_target = target;
    step16c_loop_calls = 0;
    u.ulycn = NON_PM;
    u.uhunger = 900;
    u.umovement = NORMAL_SPEED;
    gm.multi = 0;
    svc.context.move = 0;
    go.occupation = step16c_test_occupation;
    moveloop_core();
    assert(svm.moves == turn && source->o_purification_remaining == 1);
    assert(target->cursed && step16c_loop_calls == 1);
    /* A fast action does not advance the normal-turn clock. */
    u.umovement = NORMAL_SPEED * 2;
    svc.context.move = 1;
    moveloop_core();
    assert(svm.moves == turn && source->o_purification_remaining == 1);
    assert(enhancement_set_mask(late, enhancement_mask_property(EP_PURIFICATION_I), OQ_STANDARD, FALSE));
    addinv(late);
    /* A zero-time input cannot resolve a Ready source or replace the sample,
     * even after an earlier fast action and a new source entering inventory. */
    svc.context.move = 0;
    moveloop_core();
    assert(svm.moves == turn && source->o_purification_remaining == 1);
    /* This normal elapsed turn resolves before the next occupation action,
     * whose independent curse must remain until the following turn. */
    svc.context.move = 1;
    moveloop_core();
    assert(svm.moves == turn + 1);
    assert(source->o_purification_remaining == 400);
    assert(late->o_purification_remaining == 400);
    assert(target->cursed && step16c_loop_calls == 4);
    go.occupation = NULL;
    utility_turn_cancel();
    freeinv(source);
    freeinv(target);
    freeinv(late);
    obfree(source, NULL);
    obfree(target, NULL);
    obfree(late, NULL);
    u.umovement = oldmovement;
    u.uhunger = oldhunger;
    u.ulycn = oldlycan;
    svc.context.move = 0;
    status_finish();
    gb.blinit = FALSE;
    windowprocs = saved_windows;
    puts("PASS Step 16C actual moveloop zero-time commands and occupation turn ordering");
}

static void
step16c_native_dig_test(void)
{
    struct obj *pick = item(PICK_AXE);
    struct rm oldterrain = levl[u.ux + 1][u.uy];
    int boosted, native, oldrace = gu.urace.mnum;
    int olddx = u.dx, olddy = u.dy, olddz = u.dz;
    addinv(pick);
    setuwep(pick);
    gu.urace.mnum = PM_HUMAN;
    u.dx = 1; u.dy = u.dz = 0;
    levl[u.ux + 1][u.uy].typ = STONE;
    levl[u.ux + 1][u.uy].wall_info = 0;
    assert(enhancement_set_mask(pick, enhancement_mask_property(EP_EXCAVATING), OQ_STANDARD, FALSE));
    (void) use_pick_axe2(pick);
    assert(go.occupation);
    svc.context.digging.effort = 0;
    init_isaac64(16163UL, rn2);
    assert((*go.occupation)() == 1);
    boosted = svc.context.digging.effort;
    assert(!enhancement_mask_has(enhancement_known(pick), EP_EXCAVATING));
    enhancement_clear(pick);
    svc.context.digging.effort = 0;
    init_isaac64(16163UL, rn2);
    assert((*go.occupation)() == 1);
    native = svc.context.digging.effort;
    assert(native > 0 && boosted == native * 2);
    assert(enhancement_set_mask(pick, enhancement_mask_property(EP_EXCAVATING), OQ_STANDARD, FALSE));
    svc.context.digging.effort = 100 - native;
    init_isaac64(16163UL, rn2);
    assert((*go.occupation)() == 0);
    assert(enhancement_mask_has(enhancement_known(pick), EP_EXCAVATING));
    assert(levl[u.ux + 1][u.uy].typ == CORR);
    go.occupation = NULL;
    memset(&svc.context.digging, 0, sizeof svc.context.digging);
    levl[u.ux + 1][u.uy] = oldterrain;
    gu.urace.mnum = oldrace;
    u.dx = olddx; u.dy = olddy; u.dz = olddz;
    setuwep(NULL);
    freeinv(pick);
    obfree(pick, NULL);
    puts("PASS Step 16C native digging occupation contribution and decisive identification");
}

static void
step16c_runtime_tests(void)
{
    struct obj *source = item(PICK_AXE), *target = item(LONG_SWORD);
    struct obj *bag = item(SACK);
    struct obj *second, *nested;
    int i;

    addinv(source);
    addinv(target);
    for (i = 0; i < 4; ++i) {
        assert(enhancement_set_mask(source, enhancement_mask_property(EP_EROSION_I + i), OQ_STANDARD, FALSE));
        assert(utility_erosion_protected(target, ERODE_RUST));
        assert(utility_erosion_protected(target, ERODE_ROT) == (i >= 1));
        assert(utility_erosion_protected(target, ERODE_CORRODE) == (i >= 2));
        assert(utility_erosion_protected(target, ERODE_BURN) == (i >= 3));
        assert(!utility_erosion_protected(target, ERODE_CRACK));
        assert(erode_obj(target, NULL, ERODE_RUST, EF_DESTROY) == ER_NOTHING);
        assert(!target->oeroded);
    }
    {
        struct obj *paper = item(SCR_IDENTIFY);
        addinv(paper);
        assert(!fire_damage(paper, TRUE, u.ux, u.uy));
        acid_damage(paper);
        assert(paper->otyp == SCR_IDENTIFY);
        freeinv(paper);
        add_to_container(bag, paper);
        acid_damage(paper);
        assert(paper->otyp == SCR_BLANK_PAPER);
    }
    freeinv(target);
    add_to_container(bag, target);
    addinv(bag);
    assert(!utility_erosion_protected(target, ERODE_RUST));
    nested = item(SACK);
    add_to_container(bag, nested);
    obj_extract_self(target);
    add_to_container(nested, target);
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_CURSE_II), OQ_STANDARD, FALSE));
    curse(target);
    assert(target->cursed);
    uncurse(target);
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_CURSE_IV), OQ_STANDARD, FALSE));
    curse(target);
    assert(!target->cursed);
    obj_extract_self(target);
    addinv(target);
    target->owornmask = W_WEP;
    assert(!utility_curse_protected(target));
    target->owornmask = W_QUIVER;
    assert(utility_curse_protected(target));
    target->owornmask = W_SWAPWEP;
    assert(utility_curse_protected(target));
    u.twoweap = TRUE;
    assert(!utility_curse_protected(target));
    u.twoweap = FALSE;
    target->owornmask = 0;
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_DISCERNMENT), OQ_STANDARD, FALSE));
    target->bknown = 0;
    curse(target);
    assert(target->bknown && target->cursed);
    target->bknown = 0;
    freeinv(target);
    add_to_container(nested, target);
    utility_discernment_refresh();
    assert(!target->bknown);
    obj_extract_self(target);
    addinv(target);
    assert(target->bknown);
    for (i = 0; i < 4; ++i) {
        assert(enhancement_set_mask(source, enhancement_mask_property(EP_PURIFICATION_I + i), OQ_STANDARD, FALSE));
        assert(source->o_purification_remaining == 400 - 100 * i);
    }
    source->o_purification_remaining = 1;
    utility_turn_snapshot();
    freeinv(source);
    add_to_container(bag, source);
    ++svm.moves;
    utility_turn_tick();
    utility_turn_end();
    assert(!source->o_purification_remaining && target->cursed);
    obj_extract_self(source);
    addinv(source);
    utility_turn_snapshot();
    ++svm.moves;
    utility_turn_tick();
    utility_turn_end();
    assert(!target->cursed && source->o_purification_remaining == 100);
    utility_turn_snapshot();
    utility_turn_cancel();
    assert(source->o_purification_remaining == 100);
    /* An inactive source which arrives this turn does not tick. Ready ones
     * can nevertheless resolve after the complete native curse effect. */
    freeinv(source);
    utility_turn_snapshot();
    addinv(source);
    ++svm.moves;
    utility_turn_tick();
    utility_turn_end();
    assert(source->o_purification_remaining == 100);
    source->o_purification_remaining = 0;
    utility_turn_snapshot();
    ++svm.moves;
    utility_turn_tick();
    utility_turn_end();
    assert(!source->o_purification_remaining); /* no target, stay Ready */
    second = item(TIN_WHISTLE);
    assert(enhancement_set_mask(second, enhancement_mask_property(EP_PURIFICATION_I), OQ_STANDARD, FALSE));
    addinv(second);
    second->o_purification_remaining = 0;
    assert(!mergable(source, second));
    curse(target);
    utility_turn_snapshot();
    ++svm.moves;
    utility_turn_tick();
    utility_turn_end();
    assert(!target->cursed);
    assert(!!source->o_purification_remaining
           != !!second->o_purification_remaining);
    /* Sampling survives serialization and credits just the elapsed sampled
     * turn, even if restoration occurs many turns later. */
    second->o_purification_remaining = 123;
    second->o_purification_sampled = svm.moves + 1;
    utility_purification_restore(second);
    assert(second->o_purification_remaining == 123);
    svm.moves += 100;
    utility_purification_restore(second);
    assert(second->o_purification_remaining == 122);
    utility_purification_restore(second);
    assert(second->o_purification_remaining == 122);
    utility_turn_cancel();
    utility_turn_snapshot();
    second = poly_obj(second, TIN_WHISTLE);
    assert(second && second->o_purification_remaining == 122);
    ++svm.moves;
    utility_turn_tick();
    assert(second->o_purification_remaining == 121);
    freeinv(second);
    obfree(second, NULL);
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_EXCAVATING), OQ_STANDARD, FALSE));
    assert(utility_excavating_effort(source, 75, 15, 100) == 30);
    assert(enhancement_mask_has(enhancement_known(source), EP_EXCAVATING));
    assert(enhancement_set_mask(source, enhancement_mask_property(EP_EXCAVATING), OQ_STANDARD, FALSE));
    source->o_enh_known2 = 0;
    assert(utility_excavating_effort(source, 95, 10, 100) == 20);
    assert(!enhancement_mask_has(enhancement_known(source), EP_EXCAVATING));
    assert(utility_excavating_effort(source, 70, 10, 100) == 20);
    assert(!enhancement_mask_has(enhancement_known(source), EP_EXCAVATING));
    freeinv(source);
    freeinv(target);
    freeinv(bag);
    obfree(source, NULL);
    obfree(target, NULL);
    obfree(bag, NULL);
    puts("PASS Step 16C Utility runtime scope, native curse/BUC, timing and digging");
    step16c_actual_loop_test();
    step16c_native_dig_test();
}
