/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
static void
step22_corpse_stress_tests(void)
{
    struct obj *source, *body, *box;
    long now = svm.moves, expected = 0;
    int i, elapsed, mode = 0, nextmode, oldterrain = levl[u.ux][u.uy].typ;
    unsigned id;
    assert(!gi.invent);
    source = step22_item(FLINT);
    artifact_exists(source, "The Lodestone of Gravity", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_PRESERVE, EX_MANUAL, EX_HERO, 0);
    source->exotic.preservation_count = 60;
    body = step22_corpse(PM_NEWT, 0); id = body->o_id;
    box = step22_item(ICE_BOX);
    assert(ex_preserve(source, body) && body->exotic.multiplier == 30);
    levl[u.ux][u.uy].typ = ICE;
    init_isaac64(230001UL, rn2);
    for (i = 0; i < 1000; ++i) {
        elapsed = rnd(13); nextmode = rn2(3);
        svm.moves += elapsed;
        if (mode == 0) expected += elapsed * 4L;
        if (mode == 1) expected += elapsed * 2L;
        assert(ex_corpse_age(body) == expected / 120L
               && body->exotic.age_units == expected && body->o_id == id
               && body->quan == 1 && body->exotic.preserved);
        if (body->where == OBJ_CONTAINED) {
            obj_extract_self(body); removed_from_icebox(body); body = addinv(body);
        } else if (body->where == OBJ_FLOOR) {
            obj_extract_self(body); body = addinv(body);
        }
        if (nextmode == 1) {freeinv(body); place_object(body, u.ux, u.uy);}
        if (nextmode == 2) assert(step22_test_icebox_insert(body, box) == 1);
        mode = nextmode;
        assert(body->o_id == id && body->exotic.age_units == expected);
    }
    step22_discard(body); step22_discard(box); step22_discard(source);
    levl[u.ux][u.uy].typ = oldterrain; svm.moves = now;
    puts("PASS Step22 T5 1000 seeded native inventory/floor-ice/icebox custody transitions preserve exact fractional effective age, original identity and one-time preservation (seed230001)");
}
