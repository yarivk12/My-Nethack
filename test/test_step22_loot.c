static void
step22_return_stolen(struct monst *mon, struct obj *obj)
{
    assert(obj->where == OBJ_INVENT || obj->where == OBJ_FLOOR);
    if (obj->where == OBJ_INVENT) { setnotworn(obj); freeinv(obj); }
    else obj_extract_self(obj);
    (void) mpickobj(mon, obj);
    assert(obj->where == OBJ_MINVENT && obj->ocarry == mon && !obj->owornmask);
}

static int
step22_top_owners(struct monst *mon, unsigned id)
{
    struct obj *o;
    int count = 0;
    for (o = mon->minvent; o; o = o->nobj) if (o->o_id == id) ++count;
    for (o = gi.invent; o; o = o->nobj) if (o->o_id == id) ++count;
    for (o = svl.level.objlist; o; o = o->nobj) if (o->o_id == id) ++count;
    return count;
}

static void
step22_native_loot_contract_tests(void)
{
    struct monst *mon;
    struct obj *entries[3], *cursed, *ordinary, *bag, *bell, *source, *welded;
    struct ex_hit_context ctx;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 2 };
    int counts[3] = { 0, 0, 0 }, kinds[2] = { 0, 0 }, seed, i, chosen;
    unsigned ids[3];
    long quantities[3];
    assert(!gi.invent);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK); assert(mon);
    while (mon->minvent) m_useupall(mon, mon->minvent);
    entries[0] = mksobj(DAGGER, FALSE, FALSE);
    entries[1] = mksobj(ARROW, FALSE, FALSE);
    entries[1]->quan = 10000L; entries[1]->owt = weight(entries[1]);
    entries[2] = mksobj(SCR_IDENTIFY, FALSE, FALSE);
    for (i = 0; i < 3; ++i) {
        entries[i]->cursed = 0;
        ids[i] = entries[i]->o_id; quantities[i] = entries[i]->quan;
        (void) mpickobj(mon, entries[i]);
    }
    assert(calc_capacity((int) entries[1]->owt) > near_capacity());
    for (seed = 1; seed <= 1000; ++seed) {
        init_isaac64((unsigned long) (230000 + seed), rn2);
        ex_steal(mon);
        chosen = -1;
        for (i = 0; i < 3; ++i) {
            assert(step22_top_owners(mon, ids[i]) == 1);
            assert(entries[i]->o_id == ids[i] && entries[i]->quan == quantities[i]);
            if (entries[i]->where != OBJ_MINVENT) { assert(chosen < 0); chosen = i; }
        }
        assert(chosen >= 0); ++counts[chosen];
        if (chosen == 1) assert(entries[1]->where == OBJ_FLOOR
                               && entries[1]->ox == u.ux && entries[1]->oy == u.uy);
        else assert(entries[chosen]->where == OBJ_INVENT);
        step22_return_stolen(mon, entries[chosen]);
    }
    for (i = 0; i < 3; ++i) assert(counts[i] > 250 && counts[i] < 420);
    while (mon->minvent) m_useupall(mon, mon->minvent);
    welded = mksobj(DAGGER, FALSE, FALSE); welded->cursed = 1;
    (void) mpickobj(mon, welded);
    mon->weapon_check = NEED_HTH_WEAPON; (void) mon_wield_item(mon);
    assert(MON_WEP(mon) == welded && (welded->owornmask & W_WEP));
    cursed = mksobj(SCR_IDENTIFY, FALSE, FALSE); cursed->cursed = 1;
    (void) mpickobj(mon, cursed);
    ordinary = mksobj(WAR_HAMMER, FALSE, FALSE);
    ordinary->cursed = 0;
    artifact_exists(ordinary, "Ogresmasher", TRUE, ONAME_WISH);
    (void) mpickobj(mon, ordinary);
    bag = mksobj(SACK, FALSE, FALSE); bell = mksobj(BELL_OF_OPENING, FALSE, FALSE);
    add_to_container(bag, bell); bag->owt = weight(bag); (void) mpickobj(mon, bag);
    for (seed = 1; seed <= 200; ++seed) {
        init_isaac64((unsigned long) (231000 + seed), rn2); ex_steal(mon);
        assert(welded->where == OBJ_MINVENT && MON_WEP(mon) == welded);
        assert(bag->where == OBJ_MINVENT && bell->where == OBJ_CONTAINED && bell->ocontainer == bag);
        assert((cursed->where == OBJ_MINVENT) != (ordinary->where == OBJ_MINVENT));
        chosen = cursed->where == OBJ_MINVENT;
        ++kinds[chosen];
        assert(step22_top_owners(mon, cursed->o_id) == 1
               && step22_top_owners(mon, ordinary->o_id) == 1);
        step22_return_stolen(mon, chosen ? ordinary : cursed);
    }
    assert(kinds[0] > 50 && kinds[1] > 50);
    /* Native hit event: a cursed held weapon is never disarmed. Removing
     * the curse leaves an ordinary artifact eligible for native unwield/drop. */
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Fire Brand", TRUE, ONAME_WISH); setuwep(source);
    ex_test_grant(source, EX_DISARM, EX_WIELDED, EX_HERO, W_WEP);
    for (seed = 1; seed <= 100; ++seed) {
        ex_hit_begin(&ctx, mon, source, &attack);
        init_isaac64((unsigned long) (231300 + seed), rn2); ex_hit_finish(&ctx, mon, 1);
        assert(MON_WEP(mon) == welded && welded->where == OBJ_MINVENT);
    }
    m_useupall(mon, welded);
    mon->weapon_check = NEED_HTH_WEAPON; (void) mon_wield_item(mon);
    assert(MON_WEP(mon) == ordinary);
    for (seed = 1; seed <= 100 && MON_WEP(mon); ++seed) {
        ex_hit_begin(&ctx, mon, source, &attack);
        init_isaac64((unsigned long) (231500 + seed), rn2); ex_hit_finish(&ctx, mon, 1);
    }
    assert(!MON_WEP(mon) && ordinary->where == OBJ_FLOOR && !ordinary->owornmask
           && step22_top_owners(mon, ordinary->o_id) == 1);
    obj_extract_self(ordinary); obfree(ordinary, NULL);
    setuwep(NULL); step22_combat_discard(source); mongone(mon);
    puts("PASS Step22 O1B uniform top-level sampling of unit/10000-unit heterogeneous stacks, native capacity floor transfer, unique ownership, cursed worn/unworn eligibility, ordinary artifact and nested progression protection (1200 seeds230001..231200)");
    puts("PASS Step22 O1A native cursed-welded rejection and ordinary-artifact unwield/drop eligibility with unique object identity (seeds231301..231600)");
}
