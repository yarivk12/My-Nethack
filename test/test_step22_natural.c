/* Actual command and polymorphed-contact paths, with paired native controls. */
static void
step22_native_kick_natural_tests(void)
{
    struct you saved = u;
    struct monst *mon;
    struct obj *source, *boots;
    struct attack attack;
    int x, y, seed, baseline, kickextra = 0, naturalextra = 0, riderextra = 0;
    int frozen, sleeping, movable, nextdraw, slept = 0, teleported = 0, telextra = 0;
    int tx, ty, oldnoteleport = svl.level.flags.noteleport;
    assert(!gi.invent && !uwep && !Upolyd && !u.usteed);
    for (x = 3; x < COLNO - 3; ++x) {
        for (y = 2; y < ROWNO - 2; ++y)
            if (levl[x][y].typ == ROOM && levl[x + 1][y].typ == ROOM
                && !m_at(x, y) && !m_at(x + 1, y)
                && !t_at(x, y) && !t_at(x + 1, y)
                && !sobj_at(BOULDER, x, y)) break;
        if (y < ROWNO - 2) break;
    }
    assert(x < COLNO - 3);
    u.ux = x; u.uy = y; u.udaminc = 0;
    u.uinwater = u.uswallow = 0; u.utrap = 0; u.ustuck = NULL;
    memset(u.uprops, 0, sizeof u.uprops);
    u.mith_syllables[MITH_AESH] = u.mith_timers[MITH_AESH] = 0;
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Fire Brand", TRUE, ONAME_WISH);
    boots = addinv(mksobj(KICKING_BOOTS, FALSE, FALSE));
    boots->spe = 0; boots->blessed = boots->cursed = 0; setworn(boots, W_ARMF);
    mon = makemon(&mons[PM_GOBLIN], x + 1, y, NO_MM_FLAGS); assert(mon);
    while (mon->minvent) m_useupall(mon, mon->minvent);
    mon->mpeaceful = mon->mtame = 0;
    vision_reset(); vision_recalc(0); cmdq_clear(CQ_CANNED);
    for (seed = 1; seed <= 200; ++seed) {
        ex_test_clear(source);
        mon->mhp = mon->mhpmax = 10000; mon->mcanmove = 0;
        init_isaac64(234000UL + seed, rn2);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0); assert(dokick() == ECMD_TIME);
        baseline = 10000 - mon->mhp; assert(baseline > 0);
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L);
        mon->mhp = 10000; mon->mcanmove = 0;
        init_isaac64(234000UL + seed, rn2);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0); assert(dokick() == ECMD_TIME);
        assert(10000 - mon->mhp >= baseline);
        if (10000 - mon->mhp > baseline) ++kickextra;
    }
    assert(kickextra > 15 && kickextra < 85);
    assert(newcham(mon, &mons[PM_MUMAK], NO_NC_FLAGS));
    assert(thick_skinned(mon->data));
    for (seed = 1; seed <= 100; ++seed) {
        ex_test_clear(source); mon->mhp = mon->mhpmax = 10000; mon->mcanmove = 0;
        init_isaac64(234300UL + seed, rn2);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0); assert(dokick() == ECMD_TIME);
        nextdraw = rn2(100000); assert(mon->mhp == 10000);
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L);
        mon->mhp = 10000; mon->mcanmove = 0;
        init_isaac64(234300UL + seed, rn2);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0); assert(dokick() == ECMD_TIME);
        assert(mon->mhp == 10000 && rn2(100000) == nextdraw);
    }
    step22_combat_discard(boots);
    assert(newcham(mon, &mons[PM_GOBLIN], NO_NC_FLAGS));
    assert(polymon(PM_TIGER) && Upolyd);
    attack = gy.youmonst.data->mattk[0];
    assert(attack.aatyp == AT_CLAW && attack.adtyp == AD_PHYS);
    for (seed = 1; seed <= 200; ++seed) {
        ex_test_clear(source); mon->mhp = mon->mhpmax = 10000;
        mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(234500UL + seed, rn2); (void) damageum(mon, &attack, 0);
        baseline = 10000 - mon->mhp; assert(baseline > 0);
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L);
        mon->mhp = 10000; mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(234500UL + seed, rn2); (void) damageum(mon, &attack, 0);
        assert(10000 - mon->mhp >= baseline);
        if (10000 - mon->mhp > baseline) ++naturalextra;
    }
    assert(naturalextra > 15 && naturalextra < 85);
    assert(polymon(PM_HOMUNCULUS));
    attack = gy.youmonst.data->mattk[0];
    assert(attack.aatyp == AT_BITE && attack.adtyp == AD_SLEE);
    for (seed = 1; seed <= 200; ++seed) {
        ex_test_clear(source); mon->mhp = mon->mhpmax = 10000;
        mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(234800UL + seed, rn2); (void) damageum(mon, &attack, 0);
        baseline = 10000 - mon->mhp;
        frozen = mon->mfrozen; sleeping = mon->msleeping; movable = mon->mcanmove;
        if (frozen || sleeping || !movable) ++slept;
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L);
        mon->mhp = 10000; mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(234800UL + seed, rn2); (void) damageum(mon, &attack, 0);
        assert(10000 - mon->mhp >= baseline && mon->mfrozen == frozen
               && mon->msleeping == sleeping && mon->mcanmove == movable);
        if (10000 - mon->mhp > baseline) ++riderextra;
    }
    assert(slept > 0 && riderextra > 15 && riderextra < 85);
    /* Teleport is deliberately non-idempotent: replaying its native rider
     * during a Frenzy tick would relocate the target a second time. */
    assert(polymon(PM_QUANTUM_MECHANIC));
    attack = gy.youmonst.data->mattk[0]; assert(attack.adtyp == AD_TLPT);
    svl.level.flags.noteleport = 0;
    for (seed = 1; seed <= 200; ++seed) {
        remove_monster(mon->mx, mon->my); place_monster(mon, x + 1, y);
        ex_test_clear(source); mon->mhp = mon->mhpmax = 10000;
        mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(235300UL + seed, rn2); (void) damageum(mon, &attack, 0);
        baseline = 10000 - mon->mhp; tx = mon->mx; ty = mon->my;
        if (tx != x + 1 || ty != y) ++teleported;
        remove_monster(mon->mx, mon->my); place_monster(mon, x + 1, y);
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L);
        mon->mhp = 10000; mon->msleeping = 0; mon->mcanmove = 1; mon->mfrozen = 0;
        init_isaac64(235300UL + seed, rn2); (void) damageum(mon, &attack, 0);
        assert(mon->mx == tx && mon->my == ty && 10000 - mon->mhp >= baseline);
        if (10000 - mon->mhp > baseline) ++telextra;
    }
    assert(teleported > 100 && telextra > 15 && telextra < 85);
    remove_monster(mon->mx, mon->my); place_monster(mon, x + 1, y);
    svl.level.flags.noteleport = oldnoteleport;
    assert(polymon(PM_FIRE_ELEMENTAL));
    attack = gy.youmonst.data->mattk[0]; assert(attack.adtyp == AD_FIRE);
    for (seed = 1; seed <= 100; ++seed) {
        ex_test_clear(source); mon->mhp = mon->mhpmax = 10000;
        init_isaac64(235100UL + seed, rn2); (void) damageum(mon, &attack, 0);
        baseline = 10000 - mon->mhp; nextdraw = rn2(100000); assert(baseline > 0);
        ex_test_grant(source, EX_FRENZY, EX_DIRECT, EX_HERO, 0L); mon->mhp = 10000;
        init_isaac64(235100UL + seed, rn2); (void) damageum(mon, &attack, 0);
        assert(10000 - mon->mhp == baseline && rn2(100000) == nextdraw);
    }
    rehumanize(); assert(!Upolyd);
    step22_combat_discard(source); mongone(mon); cmdq_clear(CQ_CANNED);
    u = saved; set_uasmon(); vision_reset(); vision_recalc(0);
    puts("PASS Step22 O4 actual dokick command and polymon Tiger claw Frenzy increases physical HP damage; thick-skin zero-physical kicks and Fire Elemental pure-element contact consume no Frenzy roll; Homunculus sleep state and non-idempotent Quantum Mechanic teleport destination match once-only native controls (1000 paired seeds234001..235500)");
}
