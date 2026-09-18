/* Included only by mkroom.c in the existing STEP11_TEST diagnostic build.
 * Scripts affect Library decisions only; native object/monster RNG is real. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
static int library_test_roll = -1, library_test_theme = -1;
static int library_test_calls, library_test_bound;
static int library_test_pair[2], library_test_pair_left;
static int library_test_loose_roll = -1, library_test_loose_type;
static int library_test_loose_calls;
static boolean library_test_loose_pending;

staticfn int
step12_library_rn2(int bound)
{
    ++library_test_calls;
    library_test_bound = bound;
    if (library_test_loose_roll >= 0 && bound == 20) {
        ++library_test_loose_calls;
        library_test_loose_pending = (library_test_loose_roll == 0);
        return library_test_loose_roll;
    }
    if (library_test_loose_pending) {
        assert(bound == 2);
        library_test_loose_pending = FALSE;
        return library_test_loose_type;
    }
    if (library_test_pair_left) {
        assert(bound == 100);
        return library_test_pair[2 - library_test_pair_left--];
    }
    if (library_test_theme >= 0) {
        int roll = library_test_theme;
        library_test_theme = -1;
        assert(bound == 100);
        return roll;
    }
    return library_test_roll < 0 ? rn2(bound) : library_test_roll % bound;
}

void
step12_library_tables(void)
{
    const int depths[] = {5,29,30,59,60,99,100,149,150,199};
    const int expected[5][4] = {{50,50,0,0},{30,45,25,0},{10,30,45,15},
                              {0,15,55,30},{0,5,50,45}};
    const int pools[4][2] = {{PM_KOBOLD_SHAMAN,PM_ORC_SHAMAN},
        {PM_GNOMISH_WIZARD,PM_GNOMISH_WIZARD},{PM_LICH,PM_DEMILICH},
        {PM_MIND_FLAYER,PM_MASTER_MIND_FLAYER}};
    int i, j, t, a, b, choice, counts[4], categories[5] = {0};
    for (i = 0; i < SIZE(depths); ++i) {
        int hits = 0, bound = depths[i] < 60 ? 4 : depths[i] < 100 ? 3 : 5;
        memset(counts, 0, sizeof counts);
        for (j = 0; j < 100; ++j) ++counts[library_theme(depths[i], j)];
        for (t = 0; t < 4; ++t) assert(counts[t] == expected[i/2][t]);
        for (j = 0; j < bound; ++j) {
            library_test_roll = j;
            hits += library_density(depths[i]);
            assert(library_test_bound == bound);
        }
        assert(hits == (depths[i] < 100 ? 1 : 2));
    }
    library_test_roll = -1;
    for (i = 0; i < SIZE(depths); ++i) {
        struct obj *chest = mksobj(CHEST, FALSE, FALSE);
        int lock = depths[i] < 60 ? 20 : depths[i] < 100 ? 35 : 50;
        int trap = depths[i] < 60 ? 5 : depths[i] < 100 ? 10 : 15;
        int states[4] = {0};
        assert(!chest->cobj && !chest->olocked && !chest->otrapped && !chest->tknown);
        for (a = 0; a < 100; ++a)
            for (b = 0; b < 100; ++b) {
                library_test_pair[0] = a; library_test_pair[1] = b;
                library_test_pair_left = 2;
                library_chest_state(chest, depths[i]);
                assert(!library_test_pair_left);
                assert(chest->olocked == (a < lock) && chest->otrapped == (b < trap));
                ++states[2 * chest->olocked + chest->otrapped];
            }
        assert(states[3] == lock * trap && states[2] == lock * (100-trap));
        assert(states[1] == (100-lock) * trap && states[0] == (100-lock) * (100-trap));
        obfree(chest, (struct obj *) 0);
    }
    for (t = 0; t < 4; ++t) {
        unsigned char f0 = svm.mvitals[pools[t][0]].mvflags;
        unsigned char f1 = svm.mvitals[pools[t][1]].mvflags;
        for (a = 0; a < 4; ++a)
            for (choice = 0; choice < 2; ++choice) {
                svm.mvitals[pools[t][0]].mvflags = a & 1 ? G_GENOD : 0;
                svm.mvitals[pools[t][1]].mvflags = a & 2 ? G_EXTINCT : 0;
                j = library_species(t, choice);
                if (svm.mvitals[pools[t][0]].mvflags & G_GONE
                    && svm.mvitals[pools[t][1]].mvflags & G_GONE)
                    assert(j == NON_PM);
                else {
                    assert(j == pools[t][0] || j == pools[t][1]);
                    assert(!(svm.mvitals[j].mvflags & G_GONE));
                    if (!a) assert(j == pools[t][choice]);
                }
            }
        svm.mvitals[pools[t][0]].mvflags = f0;
        svm.mvitals[pools[t][1]].mvflags = f1;
    }
    /* Exhaust the actual item factory's complete 100 x 100 decision space. */
    for (a = 0; a < 100; ++a) {
        int misc[3] = {0}, utility[2] = {0};
        for (b = 0; b < 100; ++b) {
            struct obj *obj = library_item(a, b);
            if (a < 25) assert(obj->oclass == SCROLL_CLASS);
            else if (a < 65) assert(obj->oclass == SPBOOK_CLASS);
            else if (a < 85) {
                assert(obj->oclass == (b < 33 ? WAND_CLASS : b < 66 ? RING_CLASS : AMULET_CLASS));
                ++misc[b < 33 ? 0 : b < 66 ? 1 : 2];
            } else {
                assert(obj->otyp == (a < 95 ? (b < 50 ? MAGIC_MARKER : MAGIC_WHISTLE)
                                           : (b < 50 ? MAGIC_LAMP : BAG_OF_HOLDING)));
                ++utility[b < 50 ? 0 : 1];
            }
            obfree(obj, (struct obj *) 0);
        }
        ++categories[a < 25 ? 0 : a < 65 ? 1 : a < 85 ? 2 : a < 95 ? 3 : 4];
        if (a >= 65 && a < 85) assert(misc[0] == 33 && misc[1] == 33 && misc[2] == 34);
        if (a >= 85) assert(utility[0] == 50 && utility[1] == 50);
    }
    assert(categories[0] == 25 && categories[1] == 40 && categories[2] == 20
           && categories[3] == 10 && categories[4] == 5);
    puts("PASS Library exact theme, density, species/fallback, loot tables and independent locks/traps");
}

void
step12_library_validate(struct mkroom *room)
{
    int x, y, chests = 0, monsters = 0, theme = -1, dl = u.uz.dlevel;
    assert(library_squares(room) >= 12 && room->rlit);
    assert(room->rtype == THEMEROOM || room->rtype == OROOM);
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y) {
            struct obj *obj;
            struct monst *mon = m_at(x, y);
            assert(levl[x][y].lit);
            for (obj = svl.level.objects[x][y]; obj; obj = obj->nexthere) {
                assert(library_floor(room, x, y));
                if (obj->otyp == CHEST) {
                    struct obj *item;
                    int count = 0;
                    ++chests;
                    assert(!mon && obj->owt == weight(obj));
                    for (item = obj->cobj; item; item = item->nobj) {
                        count += (int) item->quan;
                        assert(item->oclass == SCROLL_CLASS || item->oclass == SPBOOK_CLASS
                               || item->oclass == WAND_CLASS || item->oclass == RING_CLASS
                               || item->oclass == AMULET_CLASS || item->otyp == MAGIC_MARKER
                               || item->otyp == MAGIC_WHISTLE || item->otyp == MAGIC_LAMP
                               || item->otyp == BAG_OF_HOLDING);
                    }
                    assert(count >= (dl < 60 ? 1 : 2));
                    assert(count <= (dl < 60 ? 3 : dl < 100 ? 4 : 5));
                } else {
                    assert(!sobj_at(CHEST, x, y));
                    assert(obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS);
                }
            }
            if (mon) {
                int t = mon->mnum == PM_KOBOLD_SHAMAN || mon->mnum == PM_ORC_SHAMAN ? 0
                    : mon->mnum == PM_GNOMISH_WIZARD ? 1
                    : mon->mnum == PM_LICH || mon->mnum == PM_DEMILICH ? 2
                    : mon->mnum == PM_MIND_FLAYER || mon->mnum == PM_MASTER_MIND_FLAYER ? 3 : -1;
                assert(t >= 0 && (theme < 0 || theme == t));
                theme = t;
                assert(mon->msleeping && library_floor(room, x, y));
                assert(!sobj_at(CHEST, x, y));
                ++monsters;
            }
        }
    assert(chests >= (dl < 60 ? 1 : 2) && chests <= (dl < 100 ? 4 : 5));
    printf("LIBRARY|dl=%d|chests=%d|monsters=%d|theme=%d\n", dl, chests, monsters, theme);
}

/* Called with a fresh level, then freed by the existing Step 11 harness. */
void
step12_library_fixture(int dl, int roll, int theme_roll, boolean exhausted)
{
    struct mkroom *room;
    struct obj *obj;
    int x, y, total = 0, theme, p0, p1, lastx = 0, lasty = 0;
    unsigned char f0, f1;
    clear_level_structures();
    gi.in_mklev = TRUE;
    u.uz.dnum = medusa_level.dnum; u.uz.dlevel = dl;
    add_room(10, 5, 15, 10, FALSE, OROOM, FALSE);
    room = &svr.rooms[0];
    room->needjoining = TRUE;
    levl[9][7].typ = DOOR;
    add_door(9, 7, room);
    levl[11][5].typ = STAIRS;
    levl[12][5].typ = STONE;
    assert(!library_floor(room, 10, 7) && !library_floor(room, 11, 5)
           && !library_floor(room, 12, 5));
    /* Prove the exact host minimum using the same production predicate. */
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            if (library_floor(room,x,y)) {
                if (++total > 12) levl[x][y].typ = STONE;
                else { lastx = x; lasty = y; }
            }
    assert(library_squares(room) == 12);
    levl[lastx][lasty].typ = STONE;
    assert(library_squares(room) == 11 && !library_room());
    levl[lastx][lasty].typ = ROOM;
    obj = mksobj_at(CHEST, lastx, lasty, FALSE, FALSE);
    assert(!library_room());
    obj_extract_self(obj);
    obfree(obj, (struct obj *) 0);
    /* Populate every eligible square through the real native constructor. */
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            if (library_floor(room, x, y))
                assert(makemon(&mons[PM_KOBOLD], x, y, MM_NOGRP));
    assert(!library_room());
    while (fmon) {
        mongone(fmon);
        dmonsfree();
    }
    /* The fake stair is terrain-only; room-level stair checks use stairways. */
    assert(library_room() == room);
    theme = library_theme(dl, theme_roll);
    p0 = library_species(theme, 0); p1 = library_species(theme, 1);
    assert(p0 != NON_PM && p1 != NON_PM);
    f0 = svm.mvitals[p0].mvflags; f1 = svm.mvitals[p1].mvflags;
    if (exhausted) svm.mvitals[p0].mvflags = svm.mvitals[p1].mvflags = G_GENOD;
    library_test_roll = roll;
    library_test_theme = theme_roll;
    library_test_calls = 0;
    fill_special_room(room);
    assert(library_test_theme == -1 && room->needfill == FILL_NONE);
    total = library_test_calls;
    fill_special_room(room);
    assert(library_test_calls == total); /* never choose/refill a theme twice */
    library_test_roll = -1;
    step12_library_validate(room);
    total = 0;
    for (obj = fobj; obj; obj = obj->nobj)
        if (obj->otyp == CHEST) {
            struct obj *item;
            int count = 0;
            ++total;
            for (item = obj->cobj; item; item = item->nobj) count += (int) item->quan;
            assert(count == (!roll ? (dl < 60 ? 1 : 2) : (dl < 60 ? 3 : dl < 100 ? 4 : 5)));
        }
    assert(total == (!roll ? (dl < 100 ? 4 : 5) : (dl < 60 ? 1 : 2)));
    if (library_test_loose_roll >= 0) {
        int loose = 0;
        assert(library_test_loose_calls == 12 - total);
        for (obj = fobj; obj; obj = obj->nobj)
            if (obj->otyp != CHEST) {
                ++loose;
                assert(obj->oclass == (library_test_loose_type ? SCROLL_CLASS : SPBOOK_CLASS));
            }
        assert(loose == (!library_test_loose_roll ? 12 - total : 0));
    }
    if (exhausted) assert(!fmon);
    else assert(fmon);
    if (!roll && !exhausted) {
        int coexist = 0;
        for (obj = fobj; obj; obj = obj->nobj)
            if (obj->otyp != CHEST && MON_AT(obj->ox,obj->oy)) ++coexist;
        assert(coexist > 0); /* monster occupancy must not block loose loot */
    }
    assert(levl[11][5].typ == STAIRS && levl[12][5].typ == STONE);
    svm.mvitals[p0].mvflags = f0; svm.mvitals[p1].mvflags = f1;
    if (fmon) {
        struct monst *before = fmon;
        int mx = before->mx, my = before->my;
        assert(library_floor(room, mx, my));
        library_monster(theme, mx, my);
        assert(fmon == before && m_at(mx, my) == before);
    }
    puts("PASS Library 12-square host, reliable chests, prohibited floor, one theme, exhaustion, loose coexistence, deferred fill");
}

void
step12_library_loose_case(int roll, int type)
{
    library_test_loose_roll = roll;
    library_test_loose_type = type;
    library_test_loose_calls = 0;
    step12_library_fixture(5, 299, 99, TRUE);
    library_test_loose_roll = -1;
}
