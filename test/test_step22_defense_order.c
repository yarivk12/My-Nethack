/* Native counterattack/passive/emergency ordering at the same event boundary
 * used by mhitu.c. Incoming HP loss uses mdamageu_damage, not a fabricated
 * post-hit HP assignment; all retaliation and emergency effects are native. */
extern boolean teleok(coordxy, coordxy, boolean);
static struct monst *
step22_order_monster(int species, coordxy x, coordxy y)
{
    struct monst *m = makemon(&mons[species], x, y, NO_MINVENT | MM_NOMSG);
    assert(m);
    m->mpeaceful = m->mtame = m->msleeping = 0;
    m->mcanmove = m->mcansee = 1;
    m->mux = u.ux; m->muy = u.uy;
    return m;
}

static void
step22_order_reset(struct monst *m, struct obj *weapon, int hp)
{
    m->mhp = m->mhpmax = hp;
    m->m_lev = 10;
    m->mcan = m->mstun = m->mconf = m->mflee = m->mfrozen = 0;
    m->mcanmove = 1;
    weapon->oeroded = weapon->oeroded2 = 0;
    u.uhpmax = 100;
    u.ex_teleport_disarmed = FALSE;
    gm.multi = 0;
}

static void
step22_order_grants(struct obj *weapon, int active)
{
    ex_test_clear(weapon);
    ex_test_grant(weapon, EX_COUNTER, active, EX_HERO, W_WEP);
    ex_test_grant(weapon, EX_GUARDIANS, active, EX_HERO, W_WEP);
    ex_test_grant(weapon, EX_TELEPORT, active, EX_HERO, W_WEP);
}

static int
step22_order_guardians(unsigned source, coord origin, int remove)
{
    struct monst *m, *next;
    int count = 0;
    for (m = fmon; m; m = next) {
        next = m->nmon;
        if (m->ex_guardian_source != source)
            continue;
        ++count;
        assert(m->mtame && distmin(m->mx, m->my, origin.x, origin.y) <= 1);
        if (remove)
            mongone(m);
    }
    return count;
}

static void
step22_order_clear_loot(coord origin)
{
    struct obj *o;
    int x, y;
    for (x = origin.x - 2; x <= origin.x + 2; ++x)
        for (y = origin.y - 2; y <= origin.y + 2; ++y)
            while ((o = svl.level.objects[x][y]) != 0) {
                obj_extract_self(o);
                obfree(o, NULL);
            }
}

static void
step22_defense_order_tests(void)
{
    struct you saved_hero = u;
    struct window_procs saved_windows = windowprocs;
    struct mvitals saved_vitals[NUMMONS];
    struct obj *weapon;
    struct monst *m;
    struct ex_melee_context event;
    coord origin, destination;
    int x, y, dx, dy, seed, found, count, before;
    int oldmulti = gm.multi, oldwizard = wizard;
    boolean oldstatus = iflags.status_updates;
    boolean oldnoteleport = svl.level.flags.noteleport;
    long oldstasis = svl.level.flags.stasis_until;

    assert(!gi.invent && !Upolyd);
    memcpy(saved_vitals, svm.mvitals, sizeof saved_vitals);
    inspect_window_setup();
    iflags.status_updates = FALSE;
    memset(u.uprops, 0, sizeof u.uprops);
    /* Saturate unrelated native attribute exercise so seed replay cannot
     * gain/lose an exercise RNG draw as repeated probe strikes accumulate. */
    for (x = 0; x < A_MAX; ++x) AEXE(x) = 50;
    u.ulevel = 30; u.uhitinc = 100;
    u.uz.dnum = medusa_level.dnum; u.uz.dlevel = 40;
    u.uhave.amulet = FALSE;
    HTeleport_control = INTRINSIC;
    wizard = FALSE;
    svl.level.flags.noteleport = FALSE;
    svl.level.flags.stasis_until = 0;
    cmdq_clear(CQ_CANNED);
    /* Reserve an existing empty floor scene, without changing terrain or
     * deleting any earlier fixture's objects/monsters. */
    found = 0;
    for (x = 4; x < COLNO - 8 && !found; ++x)
        for (y = 3; y < ROWNO - 3 && !found; ++y) {
            int clear = 1;
            for (dx = -2; dx <= 2; ++dx)
                for (dy = -2; dy <= 2; ++dy)
                    if (levl[x + dx][y + dy].typ != ROOM
                        || m_at(x + dx, y + dy)
                        || svl.level.objects[x + dx][y + dy]
                        || t_at(x + dx, y + dy))
                        clear = 0;
            if (clear && levl[x + 5][y].typ == ROOM
                && !m_at(x + 5, y) && !t_at(x + 5, y)
                && !svl.level.objects[x + 5][y]) {
                origin.x = x; origin.y = y;
                destination.x = x + 5; destination.y = y;
                found = 1;
            }
        }
    assert(found);
    u.ux = origin.x; u.uy = origin.y;
    assert(teleok(destination.x, destination.y, FALSE));

    weapon = inspect_item(QUARTERSTAFF);
    weapon->oartifact = ART_STAFF_OF_AESCULAPIUS;
    weapon->known = weapon->dknown = TRUE;
    setuwep(weapon);
    m = step22_order_monster(PM_GOBLIN, origin.x + 1, origin.y);
    ex_test_grant(weapon, EX_COUNTER, EX_WIELDED, EX_HERO, W_WEP);
    /* First identify a deterministic real life-draining riposte. Replay the
     * exact native strike with both emergency grants added to its snapshot. */
    found = 0;
    for (seed = 1; seed <= 1000; ++seed) {
        step22_order_reset(m, weapon, 10000); u.uhp = 26;
        init_isaac64(232000UL + seed, rn2);
        ex_melee_begin(&event, m);
        mdamageu_damage(m, 1, 1);
        assert(u.uhp == 25 && event.damage == 1);
        ex_melee_end(&event);
        if (m->m_lev == 9 && u.uhp > 25) { found = 1; break; }
    }
    assert(found);
    step22_order_reset(m, weapon, 10000); u.uhp = 26;
    step22_order_grants(weapon, EX_WIELDED);
    cmdq_add_dir(CQ_CANNED, 5, 0, 0);
    init_isaac64(232000UL + seed, rn2);
    ex_melee_begin(&event, m);
    mdamageu_damage(m, 1, 1);
    assert(u.uhp == 25);
    ex_melee_end(&event);
    assert(m->m_lev == 9 && u.uhp > 25);
    assert(!weapon->exotic.guardian_until && !u.ex_teleport_disarmed);
    assert(!step22_order_guardians(weapon->o_id, origin, FALSE));
    assert(u_at(origin.x, origin.y) && !ex_secondary());
    cmdq_clear(CQ_CANNED);
    mongone(m);
    setuwep(NULL); ex_test_clear(weapon); inspect_discard(weapon);
    puts("PASS Step22 D1 native Aesculapius healing precedes and cancels originally qualified D3/D2 emergencies above25 HP");

    weapon = inspect_item(DAGGER); weapon->oartifact = ART_STING;
    weapon->known = weapon->dknown = TRUE; setuwep(weapon);
    m = step22_order_monster(PM_ACID_BLOB, origin.x + 1, origin.y);
    ex_test_grant(weapon, EX_COUNTER, EX_WIELDED, EX_HERO, W_WEP);
    found = 0;
    for (seed = 1; seed <= 1000; ++seed) {
        step22_order_reset(m, weapon, 10000); u.uhp = 32;
        init_isaac64(233000UL + seed, rn2);
        ex_melee_begin(&event, m);
        mdamageu_damage(m, 1, 1);
        assert(u.uhp == 31 && event.damage == 1);
        ex_melee_end(&event);
        if (m->mhp < 10000 && u.uhp <= 25) { found = 1; break; }
    }
    assert(found);
    step22_order_reset(m, weapon, 10000); u.uhp = 32;
    step22_order_grants(weapon, EX_WIELDED);
    cmdq_add_dir(CQ_CANNED, 5, 0, 0);
    init_isaac64(233000UL + seed, rn2);
    ex_melee_begin(&event, m);
    mdamageu_damage(m, 1, 1);
    assert(u.uhp == 31);
    ex_melee_end(&event);
    assert(m->mhp < 10000 && u.uhp > 0 && u.uhp <= 25);
    assert(!weapon->exotic.guardian_until && !u.ex_teleport_disarmed);
    assert(!step22_order_guardians(weapon->o_id, origin, FALSE));
    assert(u_at(origin.x, origin.y) && !ex_secondary());
    cmdq_clear(CQ_CANNED);
    mongone(m);
    puts("PASS Step22 D1 native acid passive alone crosses25 but cannot create original incoming-hit D3/D2 qualification");

    /* A dead original attacker stops further ripostes, but does not cancel
     * already-qualified summoning and teleportation. Guardian positions
     * around the OLD hero square prove D3 executes before actual D2 travel. */
    weapon->spe = 30;
    found = 0;
    for (seed = 1; seed <= 1000; ++seed) {
        m = step22_order_monster(PM_GOBLIN, origin.x + 1, origin.y);
        step22_order_reset(m, weapon, 1); u.uhp = 26;
        step22_order_grants(weapon, EX_WIELDED);
        cmdq_add_dir(CQ_CANNED, 5, 0, 0);
        init_isaac64(234000UL + seed, rn2);
        ex_melee_begin(&event, m);
        mdamageu_damage(m, 1, 1);
        assert(u.uhp == 25);
        ex_melee_end(&event);
        count = step22_order_guardians(weapon->o_id, origin, FALSE);
        assert(u.ex_teleport_disarmed && u_at(destination.x, destination.y));
        if (DEADMONSTER(m) && count) {
            assert(weapon->exotic.guardian_until == svm.moves + 30);
            found = 1;
        }
        (void) step22_order_guardians(weapon->o_id, origin, TRUE);
        if (!DEADMONSTER(m)) mongone(m);
        step22_order_clear_loot(origin);
        cmdq_clear(CQ_CANNED);
        teleds(origin.x, origin.y, TELEDS_TELEPORT);
        if (found) break;
    }
    assert(found && !ex_secondary());
    puts("PASS Step22 native lethal D1 riposte still permits D3 placement around original hero square before native D2 relocation");

    /* Saved candidate identities must be revalidated after genuine inventory
     * movement, and new activations are excluded from the captured event. */
    m = step22_order_monster(PM_GOBLIN, origin.x + 1, origin.y);
    step22_order_reset(m, weapon, 10000); u.uhp = 26;
    step22_order_grants(weapon, EX_WIELDED);
    ex_melee_begin(&event, m);
    assert(event.sources.count == 1);
    mdamageu_damage(m, 1, 1);
    setuwep(NULL); freeinv(weapon); place_object(weapon, origin.x, origin.y);
    ex_melee_end(&event);
    assert(m->mhp == 10000 && u.uhp == 25);
    assert(!weapon->exotic.guardian_until && !u.ex_teleport_disarmed);
    obj_extract_self(weapon); weapon = addinv(weapon); setuwep(weapon);
    step22_order_grants(weapon, EX_INACTIVE);
    u.uhp = 26;
    ex_melee_begin(&event, m);
    assert(event.sources.count == 0);
    mdamageu_damage(m, 1, 1);
    step22_order_grants(weapon, EX_WIELDED);
    before = m->mhp;
    ex_melee_end(&event);
    assert(m->mhp == before && u.uhp == 25);
    assert(!weapon->exotic.guardian_until && !u.ex_teleport_disarmed);
    mongone(m);
    setuwep(NULL); ex_test_clear(weapon); inspect_discard(weapon);
    step22_order_clear_loot(origin);
    if (iflags.purge_monsters) dmonsfree();
    cmdq_clear(CQ_CANNED);
    memcpy(svm.mvitals, saved_vitals, sizeof saved_vitals);
    u = saved_hero; windowprocs = saved_windows;
    gm.multi = oldmulti; wizard = oldwizard;
    iflags.status_updates = oldstatus;
    svl.level.flags.noteleport = oldnoteleport;
    svl.level.flags.stasis_until = oldstasis;
    puts("PASS Step22 defensive event source snapshot invalidation after native drop and exclusion of newly activated grants");
}
