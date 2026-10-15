extern void step22_test_mon_distress(struct monst *);
static void
step22_set_innate_mute(struct monst *mon, int ability)
{
    mon->ex_mute_species = monsndx(mon->data);
    mon->ex_mute_until = svm.moves + 6; mon->ex_mute_ability = ability;
}

static void
step22_native_innate_dispatch_tests(void)
{
    struct monst *actor, *other, *m, *next, *oldstuck = u.ustuck;
    struct trap *trap;
    coord oldhero;
    int x, y, dx, dy, clear, seed, moved = 0, produced = 0, webbed = 0, nextdraw;
    int oldnoteleport = svl.level.flags.noteleport;
    unsigned short abilities[EX_MAX_ABILITIES];
    oldhero.x = u.ux; oldhero.y = u.uy;
    for (x = 9; x < COLNO - 10; ++x) {
        for (y = 3; y < ROWNO - 3; ++y) {
            if (levl[x - 1][y].typ != ROOM || levl[x][y].typ != ROOM
                || levl[x + 1][y].typ != ROOM || levl[x + 2][y].typ != ROOM
                || levl[x + 3][y].typ != ROOM || t_at(x, y) || t_at(x + 2, y)) continue;
            clear = 1;
            for (dx = -6; dx <= 6; ++dx) for (dy = -6; dy <= 6; ++dy)
                if (isok(x + dx, y + dy) && m_at(x + dx, y + dy)) clear = 0;
            if (clear) break;
        }
        if (y < ROWNO - 3) break;
    }
    assert(x < COLNO - 10);
    u.ux = x - 1; u.uy = y; svl.level.flags.noteleport = 0;
    actor = makemon(&mons[PM_MASTER_LICH], x + 3, y, NO_MM_FLAGS); assert(actor);
    while (actor->minvent) m_useupall(actor, actor->minvent);
    actor->mhp = actor->mhpmax = 1000; actor->mflee = 0;
    step22_set_innate_mute(actor, EX_INNATE_TELEPORT);
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233000UL + seed, rn2); (void) tactics(actor);
        assert(actor->mx == x + 3 && actor->my == y);
    }
    actor->ex_mute_until = 0;
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233000UL + seed, rn2); (void) tactics(actor);
        if (actor->mx != x + 3 || actor->my != y) { moved = 1; break; }
    }
    assert(moved); mongone(actor);

    actor = makemon(&mons[PM_WITCH], x, y, NO_MM_FLAGS); assert(actor);
    for (m = fmon; m; m = next) {
        next = m->nmon;
        if (m->data == &mons[PM_WITCH_S_FAMILIAR]
            && (unsigned long) m->mspare1 == actor->m_id) mongone(m);
    }
    dmonsfree(); /* native next-turn removal of dismissed familiar links */
    assert(step10b_witch_needs_familiar(actor));
    actor->mcanmove = 0; actor->msleeping = 0; actor->mspec_used = 0;
    step22_set_innate_mute(actor, EX_INNATE_SUMMON);
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233200UL + seed, rn2); (void) dochug(actor);
        assert(step10b_witch_needs_familiar(actor));
    }
    actor->ex_mute_until = 0;
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233200UL + seed, rn2); (void) dochug(actor);
        if (!step10b_witch_needs_familiar(actor)) { produced = 1; break; }
    }
    assert(produced);
    for (m = fmon; m; m = next) {
        next = m->nmon;
        if (m->data == &mons[PM_WITCH_S_FAMILIAR]
            && (unsigned long) m->mspare1 == actor->m_id) mongone(m);
    }
    mongone(actor);

    actor = makemon(&mons[PM_CHILLBUG], x, y, NO_MM_FLAGS); assert(actor);
    other = makemon(&mons[PM_CHILLBUG], x + 1, y, NO_MM_FLAGS); assert(other);
    actor->mpeaceful = other->mpeaceful = 1;
    actor->mcanmove = 1; actor->msleeping = 0; actor->mspec_used = 100;
    actor->mhp = 10; actor->mhpmax = 100; other->mhp = other->mhpmax = 100;
    other->mflee = 0;
    step22_set_innate_mute(actor, EX_INNATE_GROUP);
    init_isaac64(233401UL, rn2); (void) dochug(actor);
    assert(!other->mflee && other->mhp == 100);
    actor->ex_mute_until = 0; actor->mhp = 10; actor->mspec_used = 100;
    init_isaac64(233401UL, rn2); (void) dochug(actor);
    assert(other->mflee);
    mongone(actor); mongone(other);

    actor = makemon(&mons[PM_DRIDER], x, y, NO_MM_FLAGS); assert(actor);
    other = makemon(&mons[PM_GOBLIN], x + 2, y, NO_MM_FLAGS); assert(other);
    actor->mpeaceful = 0; actor->mtame = 0; actor->mcan = 0;
    actor->mspec_used = 10000; /* isolate the independent native web channel */
    while (actor->minvent) m_useupall(actor, actor->minvent);
    other->mpeaceful = 1; other->mhp = other->mhpmax = 10000;
    step22_set_innate_mute(actor, EX_INNATE_WEB);
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233500UL + seed, rn2); (void) mattackm(actor, other);
        assert(!t_at(other->mx, other->my) && !other->mtrapped && !DEADMONSTER(other));
    }
    actor->ex_mute_until = 0;
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233500UL + seed, rn2); (void) mattackm(actor, other);
        trap = t_at(other->mx, other->my);
        if (trap && trap->ttyp == WEB) { webbed = 1; deltrap(trap); break; }
    }
    assert(webbed); mongone(actor); mongone(other);

    actor = makemon(&mons[PM_SHADOW], x, y, NO_MM_FLAGS); assert(actor);
    actor->mcan = 0; levl[x][y].lit = 1;
    step22_set_innate_mute(actor, EX_INNATE_DARKEN);
    m_postmove_effect(actor); assert(levl[x][y].lit);
    actor->ex_mute_until = 0; m_postmove_effect(actor); assert(!levl[x][y].lit);
    levl[x][y].lit = 1; mongone(actor);

    actor = makemon(&mons[PM_FIRST_WRAITHWORM], x, y, NO_MM_FLAGS); assert(actor);
    actor->mhp = actor->mhpmax; actor->cham = NON_PM;
    /* Native anchoring prevents the positive-control wind from moving the
     * hero outside this isolated arena; the real native handler still runs. */
    u.ustuck = actor;
    step22_set_innate_mute(actor, EX_INNATE_WIND);
    init_isaac64(233701UL, rn2); nextdraw = rn2(100000);
    init_isaac64(233701UL, rn2); step22_test_mon_distress(actor);
    assert(rn2(100000) == nextdraw && u.ux == x - 1 && u.uy == y);
    actor->ex_mute_until = 0;
    init_isaac64(233701UL, rn2); step22_test_mon_distress(actor);
    assert(rn2(100000) != nextdraw && u.ux == x - 1 && u.uy == y);
    u.ustuck = oldstuck; mongone(actor);

    actor = makemon(&mons[PM_VAMPIRE_LEADER], x, y, NO_MM_FLAGS); assert(actor);
    if (actor->data != &mons[PM_WOLF])
        assert(newcham(actor, &mons[PM_WOLF], NO_NC_FLAGS));
    assert(is_vampshifter(actor) && !is_shapeshifter(actor->data));
    assert(ex_ability_count(actor, abilities, SIZE(abilities)) == 1
           && abilities[0] == EX_INNATE_TRANSFORM);
    actor->mhpmax = 1000; actor->mhp = 1;
    ex_apply_mute(actor);
    assert(ex_innate_muted(actor, EX_INNATE_TRANSFORM) && !can_fog(actor));
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64(233800UL + seed, rn2); nextdraw = rn2(100000);
        init_isaac64(233800UL + seed, rn2); decide_to_shapeshift(actor);
        assert(actor->data == &mons[PM_WOLF] && rn2(100000) == nextdraw);
    }
    actor->ex_mute_until = svm.moves; actor->ex_regen_until = svm.moves + 5;
    for (seed = 1; seed <= 30 && actor->data == &mons[PM_WOLF]; ++seed) {
        init_isaac64(233900UL + seed, rn2); decide_to_shapeshift(actor);
    }
    assert(actor->data == &mons[PM_VAMPIRE_LEADER] && !actor->ex_mute_until
           && !actor->ex_mute_ability
           && actor->ex_regen_until == svm.moves + 5);
    mongone(actor);
    svl.level.flags.noteleport = oldnoteleport;
    u.ux = oldhero.x; u.uy = oldhero.y;
    vision_reset(); vision_recalc(0);
    puts("PASS Step22 U7 native tactics teleport, frozen-witch dochug summon, chillbug dochug group, drider ranged mattackm web, shadow movement darkening and per-monster distress wind: selected mute suppresses real effects and unmuted controls execute (seeds233001..233701)");
    puts("PASS Step22 U7 native retained-cham vampire leader in ordinary wolf form exposes its actual transform capability; selected mute blocks automatic shift/fog, positive native shift clears old-form mute and preserves O3 (seeds233801..233930)");
}
