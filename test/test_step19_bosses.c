extern void step19_place_bosses(void);

static const int step19_boss_ids[] = { PM_NIGHTMARE, PM_BEHOLDER, PM_VECNA };

static void
step19_reset_birth(int target)
{
    int i;
    for (i = 0; i < SIZE(step19_boss_ids); ++i) {
        int pm = step19_boss_ids[i];
        svm.mvitals[pm].born = (pm == target) ? 0 : 1;
        svm.mvitals[pm].mvflags &= ~(G_EXTINCT | G_GENOD);
    }
}

static void
step19_set_artifact(int art, int typ, boolean exists)
{
    struct obj *obj = mksobj(typ, FALSE, FALSE);
    artifact_exists(obj, artiname(art), exists, ONAME_NO_FLAGS);
    obfree(obj, (struct obj *) 0);
}

static void
step19_boss_tests(void)
{
    static const int thresholds[] = { 40, 50, 80 };
    static const int arts[] = { ART_NIGHTHORN, ART_EYE_OF_VECNA,
                               ART_HAND_OF_VECNA };
    static const int bases[] = { UNICORN_HORN, EYEBALL, MUMMIFIED_HAND };
    struct rm saved_map[COLNO][ROWNO];
    struct mkroom saved_room = svr.rooms[0];
    struct mvitals saved_vitals[3];
    boolean existed[3];
    boolean saved_maze = svl.level.flags.is_maze_lev;
    boolean saved_disintegested = gd.disintegested;
    struct monst *mon;
    struct obj *obj;
    d_level saved_level = u.uz;
    s_level *saved_special = svs.sp_levchn, special = { 0 };
    branch *saved_branches = svb.branches, junction = { 0 };
    int saved_nroom = svn.nroom, i, pm, px, py, round, rewards;
    coordxy x = 0, y = 0;
    char proto, fill;

    memcpy(saved_map, levl, sizeof saved_map);
    for (i = 0; i < 3; ++i) {
        saved_vitals[i] = svm.mvitals[step19_boss_ids[i]];
        existed[i] = exist_artifact(bases[i], artiname(arts[i]));
        assert((mons[step19_boss_ids[i]].geno & (G_UNIQ | G_NOGEN | G_GENO))
               == (G_UNIQ | G_NOGEN));
        assert(mons[step19_boss_ids[i]].mflags2 & M2_NOPOLY);
    }
    u.uz.dnum = medusa_level.dnum;
    proto = svd.dungeons[u.uz.dnum].proto[0];
    fill = svd.dungeons[u.uz.dnum].fill_lvl[0];
    svd.dungeons[u.uz.dnum].proto[0] = 0;
    svd.dungeons[u.uz.dnum].fill_lvl[0] = 0;
    svs.sp_levchn = NULL; svb.branches = NULL;
    memset(&svr.rooms[0], 0, sizeof svr.rooms[0]);
    svn.nroom = 1;
    for (px = 1; px < COLNO; ++px)
        for (py = 0; py < ROWNO; ++py)
            levl[px][py].typ = STONE;
    /* Isolate one safe placement square, retaining all existing entities. */
    for (px = 2; px < COLNO - 1 && !x; ++px) {
        for (py = 1; py < ROWNO - 1; ++py) {
            if (MON_AT(px, py) || svl.level.objects[px][py]
                || t_at(px, py) || stairway_at(px, py) || u_at(px, py))
                continue;
            levl[px][py].typ = ROOM;
            levl[px][py].roomno = ROOMOFFSET;
            levl[px][py].edge = 0;
            svr.rooms[0].lx = svr.rooms[0].hx = px;
            svr.rooms[0].ly = svr.rooms[0].hy = py;
            if (forge_candidate(px, py)) { x = px; y = py; break; }
            levl[px][py].typ = STONE;
        }
    }
    assert(x);
    svl.level.flags.is_maze_lev = FALSE;
    for (i = 0; i < 3; ++i) {
        pm = step19_boss_ids[i];
        step19_reset_birth(pm);
        u.uz.dlevel = thresholds[i] - svd.dungeons[u.uz.dnum].depth_start;
        step19_place_bosses();
        assert(!MON_AT(x, y) && !svm.mvitals[pm].born);
        ++u.uz.dlevel;
        special.dlevel = u.uz; svs.sp_levchn = &special;
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        svs.sp_levchn = NULL;
        junction.end1 = u.uz; svb.branches = &junction;
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        svb.branches = NULL;
        svd.dungeons[u.uz.dnum].proto[0] = 'x';
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        svd.dungeons[u.uz.dnum].proto[0] = 0;
        svd.dungeons[u.uz.dnum].fill_lvl[0] = 'x';
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        svd.dungeons[u.uz.dnum].fill_lvl[0] = 0;
        u.uz.dnum = mines_dnum;
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        u.uz.dnum = medusa_level.dnum;
        levl[x][y].typ = STONE;
        step19_place_bosses(); assert(!svm.mvitals[pm].born);
        levl[x][y].typ = ROOM;
        ++u.uz.dlevel; /* first subsequent level where placement is possible */
        step19_place_bosses();
        mon = m_at(x, y);
        assert(mon && monsndx(mon->data) == pm && svm.mvitals[pm].born == 1);
        step19_place_bosses(); assert(m_at(x, y) == mon);
        assert(svm.mvitals[pm].born == 1);
        mongone(mon);
        step19_clear_floor(x, y);
        step19_place_bosses(); /* born state also blocks a later revisit */
        assert(!MON_AT(x, y) && svm.mvitals[pm].born == 1);
    }

    /* Unindexed maze ROOM floor is eligible; corridor/custom floor is not. */
    step19_reset_birth(PM_NIGHTMARE);
    levl[x][y].roomno = NO_ROOM;
    svl.level.flags.is_maze_lev = TRUE;
    levl[x][y].typ = CORR;
    step19_place_bosses(); assert(!svm.mvitals[PM_NIGHTMARE].born);
    levl[x][y].typ = ROOM;
    svr.rooms[0].custom_id = CUSTOM_LIBRARY;
    step19_place_bosses(); assert(!svm.mvitals[PM_NIGHTMARE].born);
    svr.rooms[0].custom_id = 0;
    step19_place_bosses();
    mon = m_at(x, y);
    assert(mon && mon->data == &mons[PM_NIGHTMARE]);
    mongone(mon); step19_clear_floor(x, y);
    levl[x][y].roomno = ROOMOFFSET;
    svl.level.flags.is_maze_lev = FALSE;

    /* Genuine monster death entry points, independent of corpse creation. */
    for (i = 0; i < 3; ++i) step19_set_artifact(arts[i], bases[i], FALSE);
    for (i = 0; i < 2; ++i) {
        pm = i ? PM_VECNA : PM_NIGHTMARE;
        step19_reset_birth(pm);
        mon = makemon(&mons[pm], x, y, NO_MINVENT);
        assert(mon && !mon->minvent);
        assert(mon->data->geno & G_NOCORPSE);
        if (i)
            monkilled(mon, "", AD_PHYS);
        else
            mondied(mon);
        rewards = 0;
        for (obj = svl.level.objects[x][y]; obj; obj = obj->nexthere) {
            assert(obj->otyp != CORPSE);
            if (obj->oartifact) {
                assert(i ? (obj->oartifact == ART_EYE_OF_VECNA
                            || obj->oartifact == ART_HAND_OF_VECNA)
                         : obj->oartifact == ART_NIGHTHORN);
                assert(obj->spe == 0 && obj->cursed);
                ++rewards;
            }
        }
        assert(rewards == 1);
        step19_clear_floor(x, y);
        for (round = 0; round < 3; ++round)
            step19_set_artifact(arts[round], bases[round], FALSE);
        step19_reset_birth(pm);
        mon = makemon(&mons[pm], x, y, NO_MINVENT);
        assert(mon);
        /* The native monster disintegration path bypasses corpse rewards. */
        monkilled(mon, "", -AD_RBRE);
        assert(!svl.level.objects[x][y]);
        assert(!exist_artifact(UNICORN_HORN, artiname(ART_NIGHTHORN)));
        assert(!exist_artifact(EYEBALL, artiname(ART_EYE_OF_VECNA)));
        assert(!exist_artifact(MUMMIFIED_HAND, artiname(ART_HAND_OF_VECNA)));
    }

    /* Once selected, an existing Vecna reward must not reroll the other. */
    step19_set_artifact(ART_EYE_OF_VECNA, EYEBALL, TRUE);
    rewards = 0;
    for (round = 0; round < 64; ++round) {
        struct monst vecna = { 0 };
        vecna.data = &mons[PM_VECNA]; vecna.mx = x; vecna.my = y;
        step19_set_artifact(ART_HAND_OF_VECNA, MUMMIFIED_HAND, FALSE);
        step19_death_reward(&vecna);
        obj = svl.level.objects[x][y];
        if (obj) {
            assert(obj->oartifact == ART_HAND_OF_VECNA && !obj->nexthere);
            ++rewards;
        }
        step19_clear_floor(x, y);
    }
    assert(rewards > 0 && rewards < 64);
    step19_set_artifact(ART_NIGHTHORN, UNICORN_HORN, TRUE);
    step19_set_artifact(ART_HAND_OF_VECNA, MUMMIFIED_HAND, TRUE);
    for (i = 0; i < 2; ++i) {
        struct monst boss = { 0 };
        boss.data = &mons[i ? PM_VECNA : PM_NIGHTMARE]; boss.mx = x; boss.my = y;
        step19_death_reward(&boss);
        assert(!svl.level.objects[x][y]);
    }

    for (i = 0; i < 3; ++i) {
        svm.mvitals[step19_boss_ids[i]] = saved_vitals[i];
        step19_set_artifact(arts[i], bases[i], existed[i]);
    }
    svd.dungeons[u.uz.dnum].proto[0] = proto;
    svd.dungeons[u.uz.dnum].fill_lvl[0] = fill;
    u.uz = saved_level; svs.sp_levchn = saved_special;
    svb.branches = saved_branches; svn.nroom = saved_nroom;
    svr.rooms[0] = saved_room;
    memcpy(levl, saved_map, sizeof saved_map);
    svl.level.flags.is_maze_lev = saved_maze;
    gd.disintegested = saved_disintegested;
    puts("PASS Step 19 boss thresholds, protected levels, deferred placement, birth state and corpse-less artifact death paths");
}
