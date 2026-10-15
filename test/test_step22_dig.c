/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* U2 runs the native wand geometry, vertical risks, progression protection
 * and shop accounting through the actual selected-source invocation. */
static void
step22_native_dig_tests(struct obj *source)
{
    struct you saved_hero = u;
    struct ex_object_state saved_source = source->exotic;
    struct levelflags saved_flags = svl.level.flags;
    struct rm saved_line[7], saved_keeper_tile;
    struct mkroom saved_room = svr.rooms[0];
    struct monst keeper = { 0 }, *oldmonsters = fmon;
    struct damage *old_damage = svl.level.damagelist, *damage, *nextdamage;
    struct trap *trap;
    struct obj *rock;
    long now = svm.moves;
    int x, hp, energy, oldrooms = svn.nroom;
    coordxy ox = u.ux, oy = u.uy;
    assert(ox + 6 < COLNO && oy > 0 && !u.utrap && !t_at(ox, oy));
    for (x = 0; x < 7; ++x) {
        assert(!m_at(ox + x, oy) && !t_at(ox + x, oy)
               && !svl.level.objects[ox + x][oy]);
        saved_line[x] = levl[ox + x][oy];
        levl[ox + x][oy].typ = ROOM;
        levl[ox + x][oy].flags = 0;
        levl[ox + x][oy].roomno = NO_ROOM;
    }
    saved_keeper_tile = levl[ox][oy - 1];
    assert(!m_at(ox, oy - 1));
    memset(u.uprops, 0, sizeof u.uprops);
    u.uhp = u.uhpmax = 1000;
    u.uen = energy = 123;
    u.uz.dnum = medusa_level.dnum; u.uz.dlevel = 40;
    u.ushops[0] = u.ushops0[0] = '\0';
    svl.level.flags.is_maze_lev = FALSE;
    svl.level.flags.is_cavernous_lev = FALSE;
    svl.level.flags.hardfloor = FALSE;
    ex_test_clear(source);
    ex_test_grant(source, EX_DIG, EX_MANUAL, EX_HERO, 0);
    assert(source->exotic.charges[0] == 5);
    cmdq_clear(CQ_CANNED);
    /* Three consecutive native rock tiles must all be excavated by one
     * charge; the later protected tile must stop the same beam. */
    for (x = 1; x <= 3; ++x) levl[ox + x][oy].typ = STONE;
    levl[ox + 5][oy].typ = STONE;
    levl[ox + 5][oy].wall_info = W_NONDIGGABLE;
    cmdq_add_dir(CQ_CANNED, 1, 0, 0);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    for (x = 1; x <= 3; ++x) assert(levl[ox + x][oy].typ == CORR);
    assert(levl[ox + 5][oy].typ == STONE
           && (levl[ox + 5][oy].wall_info & W_NONDIGGABLE));
    assert(source->exotic.charges[0] == 4 && u.uen == energy);

    levl[ox + 1][oy].typ = HWALL;
    levl[ox + 1][oy].wall_info = W_NONDIGGABLE;
    levl[ox + 2][oy].typ = STONE;
    cmdq_add_dir(CQ_CANNED, 1, 0, 0);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    assert(levl[ox + 1][oy].typ == HWALL && levl[ox + 2][oy].typ == STONE);
    assert(source->exotic.charges[0] == 3 && u.uen == energy);

    svl.level.flags.hardfloor = TRUE;
    cmdq_add_dir(CQ_CANNED, 0, 0, 1);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    trap = t_at(ox, oy);
    assert(trap && trap->ttyp == PIT && u.utrap && u.utraptype == TT_PIT);
    assert(source->exotic.charges[0] == 2 && u.uen == energy);
    reset_utrap(FALSE); deltrap(trap);
    svl.level.flags.hardfloor = FALSE;
    HLevitation = INTRINSIC; /* observe the native hole without changing level */
    assert(Can_dig_down(&u.uz));
    cmdq_add_dir(CQ_CANNED, 0, 0, 1);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    trap = t_at(ox, oy);
    assert(trap && trap->ttyp == HOLE && u.uz.dlevel == 40);
    assert(source->exotic.charges[0] == 1 && u.uen == energy);
    deltrap(trap); HLevitation = 0;

    hp = u.uhp;
    assert(!sobj_at(ROCK, ox, oy));
    cmdq_add_dir(CQ_CANNED, 0, 0, -1);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    rock = sobj_at(ROCK, ox, oy);
    assert(rock && rock->quan == 1 && hp - u.uhp >= 1 && hp - u.uhp <= 6);
    assert(!source->exotic.charges[0] && u.uen == energy);
    delobj(rock);
    /* Empty charge pools are free; confirmed native protection still costs
     * one charge after one real recharge interval. */
    assert(!ex_utility_invoke(source, EX_DIG));
    svm.moves += 100; ex_resources(source);
    assert(source->exotic.charges[0] == 1);
    trap = maketrap(ox, oy, MAGIC_PORTAL); assert(trap);
    cmdq_add_dir(CQ_CANNED, 0, 0, 1);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    assert(t_at(ox, oy) == trap && trap->ttyp == MAGIC_PORTAL);
    assert(!source->exotic.charges[0] && u.uen == energy);
    deltrap(trap);
    puts("PASS Step22 U2 native multi-tile geometry, protected-wall ineffective charge, hardfloor pit/self-restraint, legal downward hole, upward rock/HP risk, portal protection, zero-charge and exact Pw-free costs");

    /* A native shop scene with no money forces the actual damage-liability
     * path rather than replacing payment or pursuit with a test stub. */
    memset(&svr.rooms[0], 0, sizeof svr.rooms[0]);
    svr.rooms[0].lx = ox - 1; svr.rooms[0].hx = ox + 4;
    svr.rooms[0].ly = oy - 2; svr.rooms[0].hy = oy + 2;
    svr.rooms[0].rtype = svr.rooms[0].orig_rtype = SHOPBASE;
    svn.nroom = max(1, svn.nroom);
    keeper.data = &mons[PM_SHOPKEEPER]; keeper.mnum = PM_SHOPKEEPER;
    keeper.m_id = 239000; keeper.mhp = keeper.mhpmax = 100;
    keeper.isshk = keeper.mpeaceful = keeper.mcanmove = keeper.mcansee = TRUE;
    keeper.mx = ox; keeper.my = oy - 1;
    neweshk(&keeper);
    ESHK(&keeper)->bill_p = ESHK(&keeper)->bill;
    ESHK(&keeper)->shoplevel = u.uz;
    ESHK(&keeper)->shoproom = ROOMOFFSET;
    ESHK(&keeper)->shk.x = ox; ESHK(&keeper)->shk.y = oy - 1;
    svr.rooms[0].resident = &keeper;
    keeper.nmon = oldmonsters; fmon = &keeper;
    for (x = 0; x <= 4; ++x) {
        levl[ox + x][oy].typ = ROOM;
        levl[ox + x][oy].flags = 0;
        levl[ox + x][oy].roomno = ROOMOFFSET;
    }
    levl[ox][oy - 1].roomno = ROOMOFFSET;
    levl[ox + 1][oy].typ = HWALL;
    svl.level.flags.has_shop = TRUE;
    svl.level.damagelist = NULL;
    u.ushops[0] = ROOMOFFSET; u.ushops[1] = '\0';
    assert(inhishop(&keeper) && !money_cnt(gi.invent));
    svm.moves += 100; ex_resources(source);
    cmdq_add_dir(CQ_CANNED, 1, 0, 0);
    assert(ex_utility_invoke(source, EX_DIG) == 1);
    damage = svl.level.damagelist;
    assert(damage && damage->place.x == ox + 1 && damage->place.y == oy
           && damage->cost == SHOP_WALL_COST && damage->when == svm.moves);
    assert(IS_DOOR(levl[ox + 1][oy].typ) && !keeper.mpeaceful
           && ESHK(&keeper)->following);
    assert(!source->exotic.charges[0] && u.uen == energy);
    puts("PASS Step22 U2 native shop wall destruction records exact liability and triggers unpaid shopkeeper pursuit through pay_for_damage");
    fmon = oldmonsters;
    free_eshk(&keeper); free((genericptr_t) keeper.mextra);
    for (damage = svl.level.damagelist; damage; damage = nextdamage) {
        nextdamage = damage->next; free((genericptr_t) damage);
    }
    svl.level.damagelist = old_damage;
    svr.rooms[0] = saved_room; svn.nroom = oldrooms;
    for (x = 0; x < 7; ++x) {
        levl[ox + x][oy] = saved_line[x];
        recalc_block_point(ox + x, oy);
    }
    levl[ox][oy - 1] = saved_keeper_tile;
    recalc_block_point(ox, oy - 1);
    source->exotic = saved_source;
    u = saved_hero; svm.moves = now; svl.level.flags = saved_flags;
    cmdq_clear(CQ_CANNED);
    gv.vision_full_recalc = 1;
}
