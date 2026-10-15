/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Activation is per property, per object and per actor, never category-wide. */
static void
step22_source_tests(void)
{
    struct obj *a, *b, *box;
    struct monst *mon;
    struct ex_sources captured;
    struct ex_object_state before;
    int p, oldtwo = u.twoweap;
    long now = svm.moves;
    assert(!gi.invent);
    inspect_window_setup();
    a = inspect_item(DAGGER);
    a->oartifact = ART_STING;
    b = addinv(mksobj(BROADSWORD, FALSE, FALSE));
    b->oartifact = ART_ORCRIST;
    box = addinv(mksobj(SACK, FALSE, FALSE));
    for (p = 0; p < EX_COUNT; ++p) {
        ex_test_grant(a, p, EX_DIRECT, EX_HERO, 0);
        assert(ex_active(a, p, &gy.youmonst));
    }
    freeinv(a); add_to_container(box, a);
    for (p = 0; p < EX_COUNT; ++p) {
        assert(!ex_active(a, p, &gy.youmonst));
        ex_test_grant(a, p, EX_CARRIED, EX_HERO, 0);
        assert(ex_active(a, p, &gy.youmonst));
    }
    ex_snapshot(&captured, &gy.youmonst);
    assert(captured.count == 1 && captured.ids[0] == a->o_id);
    freeinv(box); place_object(box, u.ux, u.uy);
    assert(!ex_source(&captured, 0, EX_WARD, &gy.youmonst));
    obj_extract_self(box); box = addinv(box);
    obj_extract_self(a); a = addinv(a);
    ex_test_clear(a);
    ex_test_grant(a, EX_WARD, EX_WORN, EX_HERO, W_TOOL);
    a->owornmask = W_TOOL;
    assert(ex_active(a, EX_WARD, &gy.youmonst));
    a->owornmask = W_ARM;
    assert(!ex_active(a, EX_WARD, &gy.youmonst));
    a->owornmask = 0;
    ex_test_grant(a, EX_WARD, EX_INVOKED, EX_HERO, 0);
    assert(!ex_active(a, EX_WARD, &gy.youmonst));
    ex_snapshot(&captured, &gy.youmonst);
    a->exotic.invoked = 1;
    assert(ex_active(a, EX_WARD, &gy.youmonst) && !captured.count);
    ex_test_grant(a, EX_WARD, EX_WIELDED, EX_HERO, 0);
    setuwep(a);
    assert(ex_active(a, EX_WARD, &gy.youmonst));
    setuwep(NULL);
    assert(!ex_active(a, EX_WARD, &gy.youmonst));
    ex_test_clear(a);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, NO_MINVENT);
    assert(mon);
    freeinv(a); (void) mpickobj(mon, a);
    ex_test_grant(a, EX_WARD, EX_DIRECT, EX_MONSTER, 0);
    mon->mtame = 0;
    assert(ex_active(a, EX_WARD, mon) && !ex_active(a, EX_WARD, &gy.youmonst));
    mon->mtame = 5;
    assert(!ex_active(a, EX_WARD, mon));
    ex_test_grant(a, EX_WARD, EX_DIRECT, EX_PET, 0);
    ex_snapshot(&captured, mon);
    assert(captured.count == 1 && ex_source(&captured, 0, EX_WARD, mon) == a);
    obj_extract_self(a); a = addinv(a);
    assert(!ex_source(&captured, 0, EX_WARD, mon));
    mongone(mon);

    /* Inspection computes recharge on a copy; saving, transferring and
     * re-equipping cannot bank time while full or reset partial intervals. */
    ex_test_clear(a); ex_test_clear(b);
    ex_test_grant(a, EX_DIG, EX_MANUAL, EX_HERO, 0);
    ex_test_grant(b, EX_DIG, EX_MANUAL, EX_HERO, 0);
    assert(ex_spend(a, 0, 2));
    svm.moves += 55;
    before = a->exotic;
    a->known = a->dknown = TRUE; discover_artifact(a->oartifact);
    ex_inspect(a, WIN_MESSAGE);
    assert(!memcmp(&before, &a->exotic, sizeof before));
    assert(a->exotic.charges[0] == 3 && b->exotic.charges[0] == 5);
    freeinv(a); place_object(a, u.ux, u.uy);
    svm.moves += 45; ex_resources(a);
    assert(a->exotic.charges[0] == 4);
    obj_extract_self(a); a = addinv(a);
    assert(ex_spend(a, 0, 1));
    svm.moves += 100; ex_resources(a);
    assert(a->exotic.charges[0] == 4 && b->exotic.charges[0] == 5);
    setnotworn(a); freeinv(a); obfree(a, NULL);
    freeinv(b); obfree(b, NULL);
    freeinv(box); obfree(box, NULL);
    svm.moves = now; u.twoweap = oldtwo;
    step15b_window_setup();
    puts("PASS Step22 all18 direct/nested grants, slot/invoked/wielded activation, snapshot removal/new activation, explicit monster/pet ownership, independent recharge and read-only inspection");
}
