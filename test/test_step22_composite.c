/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Paired actual hmon strikes make the combined ordering observable through
 * native HP thresholds, fatal death, and independently committed hop damage. */
static void
step22_composite_strike_tests(void)
{
    struct obj *source, *paper;
    struct monst *primary = NULL, *secondary;
    coord oldhero;
    int x, y, dx, dy, clear, seed, remaining, hopdamage;
    int executions = 0, chains = 0, joint = 0;
    oldhero.x = u.ux; oldhero.y = u.uy;
    for (x = 4; x < COLNO - 5; ++x) {
        for (y = 3; y < ROWNO - 3; ++y) {
            if (levl[x - 1][y].typ != ROOM || levl[x][y].typ != ROOM
                || levl[x + 1][y].typ != ROOM || t_at(x, y) || t_at(x + 1, y))
                continue;
            clear = 1;
            for (dx = -2; dx <= 3; ++dx) for (dy = -2; dy <= 2; ++dy)
                if (m_at(x + dx, y + dy)) clear = 0;
            if (clear) break;
        }
        if (y < ROWNO - 3) break;
    }
    assert(x < COLNO - 5);
    u.ux = x - 1; u.uy = y;
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Fire Brand", TRUE, ONAME_WISH); setuwep(source);
    secondary = makemon(&mons[PM_GOBLIN], x + 1, y, NO_MM_FLAGS); assert(secondary);
    secondary->mpeaceful = secondary->mtame = 0;
    paper = mksobj(SCR_IDENTIFY, FALSE, FALSE); (void) mpickobj(secondary, paper);
    for (seed = 1; seed <= 300; ++seed) {
        if (!primary) {
            primary = makemon(&mons[PM_GOBLIN], x, y, NO_MM_FLAGS); assert(primary);
            primary->mpeaceful = primary->mtame = 0;
            while (primary->minvent) m_useup(primary, primary->minvent);
        }
        if (primary->mx != x || primary->my != y) {
            remove_monster(primary->mx, primary->my);
            place_monster(primary, x, y);
        }
        ex_test_clear(source);
        ex_test_grant(source, EX_FRENZY, EX_WIELDED, EX_HERO, W_WEP);
        ex_test_grant(source, EX_CHAIN, EX_WIELDED, EX_HERO, W_WEP);
        primary->mhp = 1000; primary->mhpmax = 4000;
        primary->mflee = 0;
        secondary->mhp = secondary->mhpmax = 10000;
        gb.bhitpos.x = x; gb.bhitpos.y = y;
        init_isaac64((unsigned long) (229000 + seed), rn2);
        assert(hmon(primary, source, HMON_MELEE, 10));
        remaining = primary->mhp;
        hopdamage = 10000 - secondary->mhp;
        assert(remaining > 250 && remaining < 1000);
        assert(paper->where == OBJ_MINVENT && paper->ocarry == secondary);
        /* Same strike and RNG, now with Execution enabled. The target starts
         * ABOVE its execution threshold. Its final threshold equals the
         * observed post-physical/Frenzy/intrinsic-fire HP exactly. Therefore
         * an execution can occur only after the native elemental component.
         * Adding Execution performs no RNG before that final commitment. */
        ex_test_grant(source, EX_EXECUTION, EX_WIELDED, EX_HERO, W_WEP);
        if (primary->mx != x || primary->my != y) {
            remove_monster(primary->mx, primary->my);
            place_monster(primary, x, y);
        }
        primary->mhp = 1000; primary->mhpmax = 4 * remaining;
        primary->mflee = 0;
        secondary->mhp = 10000;
        init_isaac64((unsigned long) (229000 + seed), rn2);
        (void) hmon(primary, source, HMON_MELEE, 10);
        assert(10000 - secondary->mhp == hopdamage);
        assert(paper->where == OBJ_MINVENT && paper->ocarry == secondary);
        if (hopdamage) ++chains;
        if (DEADMONSTER(primary)) {
            ++executions; if (hopdamage) ++joint;
            primary = NULL;
        } else assert(primary->mhp == remaining);
    }
    assert(executions > 15 && executions < 110 && chains > 30 && joint > 3);
    if (primary) mongone(primary);
    mongone(secondary);
    setuwep(NULL); step22_combat_discard(source);
    u.ux = oldhero.x; u.uy = oldhero.y;
    puts("PASS Step22 O4+O5+O6 combined real hmon Fire Brand: exact paired post-Frenzy/intrinsic-element execution threshold and precommitted identical HP-only hops even after native execution death (300 paired seeds229001..229300)");
}
