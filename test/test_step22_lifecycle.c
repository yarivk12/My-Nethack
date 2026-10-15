/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Native D3/T4 lifecycle and persistent source ownership fixtures. */
static struct ex_guardian_window *
step22_window(unsigned source, long until)
{
    unsigned i;
    for (i = 0; i < EX_MAX_SOURCES; ++i)
        if (u.ex_guardians[i].source == source
            && u.ex_guardians[i].until == until) return &u.ex_guardians[i];
    return NULL;
}

static struct monst *
step22_guardian(unsigned source)
{
    struct monst *m;
    for (m = fmon; m; m = m->nmon)
        if (!DEADMONSTER(m) && m->ex_guardian_source == source) return m;
    return NULL;
}

static void
step22_emergency(struct monst *enemy)
{
    struct ex_melee_context e;
    u.uhp = 26;
    ex_melee_begin(&e, enemy);
    u.uhp = 25;
    ex_melee_end(&e);
}

static void
step22_lifecycle_tests(void)
{
    struct obj *a, *b, *o, *nexto, *amulet;
    struct monst *enemy, *m, *nextm, *clone, *survivor;
    struct ex_guardian_window *w;
    struct ex_guardian_window oldwindows[EX_MAX_SOURCES];
    struct prop oldprops[LAST_PROP + 1];
    unsigned char vitals[sizeof svm.mvitals], terrain[COLNO][ROWNO];
    unsigned aid, bid, mid, sid, eid, lootid, count, beforecount;
    int x, y, i, seed, oldhp = u.uhp, oldmax = u.uhpmax;
    int oldlevel = u.ulevel, olddisarmed = u.ex_teleport_disarmed;
    long now = svm.moves, deadline, xp;
    d_level olduz = u.uz;
    NHFILE *file;
    coord dest;
    struct you decoded;

    assert(!gi.invent);
    inspect_window_setup();
    memcpy(oldprops, u.uprops, sizeof oldprops);
    memcpy(oldwindows, u.ex_guardians, sizeof oldwindows);
    memcpy(vitals, svm.mvitals, sizeof vitals);
    memset(u.uprops, 0, sizeof oldprops);
    memset(u.ex_guardians, 0, sizeof oldwindows);
    u.uz.dnum = medusa_level.dnum; u.uz.dlevel = 1;
    u.ulevel = 1; u.uhpmax = 100;
    for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y) {
        terrain[x][y] = levl[x][y].typ;
        levl[x][y].typ = ROOM;
    }
    a = inspect_item(DAGGER); a->oartifact = ART_STING; aid = a->o_id;
    b = addinv(mksobj(BROADSWORD, FALSE, FALSE));
    b->oartifact = ART_ORCRIST; bid = b->o_id;
    ex_test_grant(a, EX_GUARDIANS, EX_DIRECT, EX_HERO, 0);
    ex_test_grant(b, EX_GUARDIANS, EX_INVOKED, EX_HERO, 0);
    enemy = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, NO_MINVENT);
    assert(enemy); eid = enemy->m_id; enemy->mpeaceful = enemy->mtame = 0;

    /* An empty native generation pool produces no window and no retry loop. */
    for (i = LOW_PM; i < NUMMONS; ++i) svm.mvitals[i].mvflags |= G_GENOD;
    step22_emergency(enemy);
    assert(!a->exotic.guardian_until && !step22_guardian(aid));

    /* A rolled group of three with one legal square is one completed window,
     * and opening more squares never queues a delayed top-up. */
    svm.mvitals[PM_GOBLIN].mvflags &= ~G_GONE;
    /* Earlier combat fixtures legitimately reached native species birth
     * limits. This controlled generation pool starts with fresh vitality. */
    svm.mvitals[PM_GOBLIN].born = 0;
    for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
        levl[x][y].typ = STONE;
    levl[u.ux][u.uy].typ = levl[enemy->mx][enemy->my].typ = ROOM;
    step22_emergency(enemy);
    assert(!a->exotic.guardian_until && !step22_guardian(aid));
    levl[u.ux - 1][u.uy].typ = ROOM;
    for (seed = 1; ; ++seed) {
        init_isaac64(224000UL + seed, rn2);
        if (rnd(3) == 3) break;
    }
    init_isaac64(224000UL + seed, rn2);
    step22_emergency(enemy);
    deadline = svm.moves + 30;
    w = step22_window(aid, deadline);
    assert(w && w->count == 1 && a->exotic.guardian_until == deadline);
    m = step22_guardian(aid); assert(m && !m->minvent && m->mtame);
    for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
        levl[x][y].typ = ROOM;
    ++svm.moves; step22_emergency(enemy);
    assert(w->count == 1 && a->exotic.guardian_until == deadline);
    mondied(m);
    assert(!step22_guardian(aid) && w->count == 0);
    step22_emergency(enemy);
    assert(w->count == 0 && a->exotic.guardian_until == deadline);
    puts("PASS Step22 T4 native zero/partial placement, no queued top-up and early-death window retention");

    /* A second activation mode has its own deadline and source membership. */
    b->exotic.invoked = 1;
    step22_emergency(enemy);
    assert(b->exotic.guardian_until == svm.moves + 30);
    assert(b->exotic.guardian_until != deadline && step22_guardian(bid));
    svm.moves = deadline; step22_emergency(enemy);
    assert(a->exotic.guardian_until == deadline + 30);
    assert(b->exotic.guardian_until == deadline + 1);
    m = step22_guardian(aid); assert(m); m->mhp = m->mhpmax = 40;
    w = step22_window(aid, m->ex_guardian_until); assert(w);
    beforecount = w->count;
    clone = clone_mon(m, 0, 0); assert(clone);
    assert(clone->ex_guardian_source == aid
           && clone->ex_guardian_until == m->ex_guardian_until
           && clone->m_id != m->m_id && w->count == beforecount + 1);
    mondied(clone);
    assert(w->count == beforecount);

    /* Native acquired lifesaving is consumed once by expiry; the surviving
     * ordinary pet remains detached across later turns and reload. */
    survivor = m; sid = m->m_id;
    amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
    (void) mpickobj(m, amulet); amulet->owornmask = W_AMUL;
    m->misc_worn_check |= W_AMUL;
    svm.moves = m->ex_guardian_until;
    xp = u.uexp;
    assert(!ex_guardian_expire(m));
    assert(!DEADMONSTER(m) && m->mtame && !which_armor(m, W_AMUL)
           && !m->ex_guardian_source && !m->ex_guardian_until && u.uexp == xp);
    svm.moves += 90; ex_world_turn();
    assert(!DEADMONSTER(survivor) && survivor->mtame);
    puts("PASS Step22 T4 independent activation windows, native clone membership and permanent expiry-lifesaving detachment");

    /* Persist real objects and monster links via native level codecs. The
     * hero codec used by savegamestate persists the episode and group table. */
    a->exotic.guardian_until = svm.moves + 30;
    m = makemon(&mons[PM_GOBLIN], u.ux - 2, u.uy, MM_EDOG | NO_MINVENT);
    assert(m); initedog(m, FALSE); mid = m->m_id;
    m->ex_guardian_source = aid; m->ex_guardian_until = a->exotic.guardian_until;
    svm.mvitals[PM_MIND_FLAYER].mvflags &= ~G_GONE;
    assert(newcham(m, &mons[PM_MIND_FLAYER], NO_NC_FLAGS));
    assert(m->ex_guardian_source == aid && m->ex_guardian_until == a->exotic.guardian_until);
    /* Reuse an expired registry entry without depending on engine internals. */
    u.ex_guardians[0].source = aid;
    u.ex_guardians[0].until = m->ex_guardian_until;
    u.ex_guardians[0].count = 1;
    o = mksobj(DIAMOND, FALSE, FALSE); lootid = o->o_id; (void) mpickobj(m, o);
    m->ex_regen_until = svm.moves + 5;
    ex_apply_mute(m);
    m->ex_mute_until = svm.moves + 6;
    ex_test_grant(a, EX_DIG, EX_MANUAL, EX_HERO, 0);
    ex_test_grant(a, EX_TRANSMUTE, EX_MANUAL, EX_HERO, 0);
    assert(ex_spend(a, 0, 2) && ex_spend(a, 1, 3));
    a->exotic.charge_turn[0] -= 55;
    a->exotic.charge_turn[1] -= 5;
    freeinv(b); place_object(b, u.ux, u.uy);
    freeinv(a); place_object(a, u.ux, u.uy);
    u.ex_teleport_disarmed = TRUE;
    file = get_freeing_nhfile();
    file_mode(file, WRITING | FREEING, open("step22-lifecycle.tmp",
              O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    savelev(file, ledger_no(&u.uz)); close_nhfile(file);
    file = get_freeing_nhfile();
    file_mode(file, READING, open("step22-lifecycle.tmp", O_RDONLY | O_BINARY));
    getlev(file, 0, ledger_no(&u.uz)); close_nhfile(file);
    a = b = NULL; count = 0;
    for (o = fobj; o; o = o->nobj) {
        if (o->o_id == aid) a = o;
        if (o->o_id == bid) b = o;
    }
    assert(a && b && a->exotic.guardian_until == svm.moves + 30
           && a->exotic.activation[EX_GUARDIANS] == EX_DIRECT
           && b->exotic.activation[EX_GUARDIANS] == EX_INVOKED);
    assert(a->exotic.charges[0] == 3 && a->exotic.charges[1] == 7
           && a->exotic.charge_turn[0] == svm.moves - 55
           && a->exotic.charge_turn[1] == svm.moves - 5);
    for (m = fmon; m; m = m->nmon) {
        if (m->m_id == sid) {
            ++count; assert(!DEADMONSTER(m) && m->mtame
                && !m->ex_guardian_until && !m->ex_guardian_source);
        }
    }
    assert(count == 1);
    m = step22_guardian(aid);
    assert(m && m->m_id == mid && m->ex_guardian_until == svm.moves + 30
           && m->minvent && m->minvent->o_id == lootid);
    assert(m->ex_regen_until == svm.moves + 5
           && m->ex_mute_until == svm.moves + 6
           && m->ex_mute_species == PM_MIND_FLAYER
           && (ex_muted(m, AT_TENT, AD_DRIN)
               || ex_innate_muted(m, EX_INNATE_PSYCHIC)));
    file = get_freeing_nhfile();
    file_mode(file, WRITING, open("step22-hero.tmp",
              O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    Sfo_you(file, &u, "gamestate-you"); close_nhfile(file);
    memset(&decoded, 0, sizeof decoded);
    file = get_freeing_nhfile();
    file_mode(file, READING, open("step22-hero.tmp", O_RDONLY | O_BINARY));
    Sfi_you(file, &decoded, "gamestate-you"); close_nhfile(file);
    assert(decoded.ex_teleport_disarmed
           && !memcmp(decoded.ex_guardians, u.ex_guardians, sizeof u.ex_guardians));
    assert(u.ex_teleport_disarmed);
    puts("PASS Step22 native level and hero codecs retain artifact grants/windows, guardian IDs/loot, detached pets, D2 episode and D3 membership");

    /* A native migrating guardian is not killed at overloaded migration
     * coordinates. Legal arrival resolves expiry and native loot before AI. */
    dest.x = u.ux - 2; dest.y = u.uy;
    migrate_to_level(m, ledger_no(&u.uz), MIGR_EXACT_XY, &dest);
    assert(m->mstate & MON_MIGRATING);
    svm.moves += 31;
    ex_world_turn();
    assert(m->ex_guardian_until && !DEADMONSTER(m));
    for (o = fobj; o; o = o->nobj) assert(o->o_id != lootid);
    losedogs();
    assert(DEADMONSTER(m) && !m->ex_guardian_until && u.uexp == xp);
    count = 0;
    for (o = fobj; o; o = o->nobj) if (o->o_id == lootid) {
        ++count; assert(isok(o->ox, o->oy) && o->where == OBJ_FLOOR);
    }
    assert(count == 1);
    puts("PASS Step22 T4 overdue native migration expires after legal placement, drops acquired loot exactly once and grants no hero XP");

    for (m = fmon; m; m = nextm) {
        nextm = m->nmon;
        if (!DEADMONSTER(m) && (m->m_id == eid || m->m_id == mid
            || m->m_id == sid || m->ex_guardian_source == aid
            || m->ex_guardian_source == bid)) mongone(m);
    }
    for (o = fobj; o; o = nexto) {
        nexto = o->nobj;
        if (o->o_id == aid || o->o_id == bid || o->o_id == lootid) {
            obj_extract_self(o); obfree(o, NULL);
        }
    }
    for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
        levl[x][y].typ = terrain[x][y];
    memcpy(svm.mvitals, vitals, sizeof vitals);
    memcpy(u.uprops, oldprops, sizeof oldprops);
    memcpy(u.ex_guardians, oldwindows, sizeof oldwindows);
    svm.moves = now; u.uz = olduz; u.ulevel = oldlevel;
    u.uhp = oldhp; u.uhpmax = oldmax;
    u.ex_teleport_disarmed = olddisarmed;
    step15b_window_setup();
}
