/* Runs in the real engine, after the existing Step 19 regression fixtures. */
extern int step20_claw_damage(struct monst *, struct monst *);
static const struct {
    int pm, depth, level, speed, ac, mr, geno, poly;
} step20_roster[] = {
    { PM_VAMPIRE_MAGE, 50, 20, 14, -4, 50, 1, 1 },
    { PM_DEEPEST_ONE, 50, 30, 15, -5, 70, 1, 1 },
    { PM_DRIDER, 55, 14, 15, 2, 15, 1, 1 },
    { PM_ASTRAL_DEVA, 60, 18, 18, -6, 90, 0, 0 },
    { PM_SHOGGOTH, 60, 18, 15, -5, 25, 1, 1 },
    { PM_DEATH_KNIGHT, 65, 17, 9, -4, 45, 1, 1 },
    { PM_HOUND_OF_TINDALOS, 70, 14, 12, 2, 0, 0, 1 },
    { PM_PLANETAR, 70, 29, 16, -10, 80, 0, 0 },
    { PM_VORPAL_JABBERWOCK, 80, 20, 12, -2, 50, 1, 1 },
    { PM_NEOTHELID, 80, 32, 12, 2, 60, 0, 0 },
    { PM_GUG, 85, 15, 18, 5, 15, 1, 1 },
    { PM_GIANT_SHOGGOTH, 85, 36, 20, -10, 50, 1, 1 },
    { PM_VOID_DRAGON, 90, 25, 9, -10, 20, 0, 0 },
    { PM_PRIESTESS_OF_GHAUNADAUR, 95, 18, 15, 2, 10, 0, 0 },
    { PM_ALHOON, 95, 26, 9, -6, 90, 1, 0 },
    { PM_SOLAR, 95, 39, 16, -10, 80, 0, 0 },
    { PM_JUGGERNAUT, 100, 30, 9, 7, 0, 1, 1 },
    { PM_ELDER_BRAIN, 130, 30, 12, 0, 60, 0, 0 }
};

static int
step20_index(int pm)
{
    int i;
    for (i = 0; i < SIZE(step20_roster); ++i)
        if (step20_roster[i].pm == pm) return i;
    return -1;
}

static void
step20_generation_tests(void)
{
    static const int depths[] = { 60, 80, 100, 130, 150, 199 };
    struct you saved = u;
    struct permonst *ptr;
    int i, j, k, samples[SIZE(step20_roster)], other, none;
    u.uz.dnum = medusa_level.dnum;
    for (i = 0; i < SIZE(step20_roster); ++i) {
        ptr = &mons[step20_roster[i].pm];
        assert(ptr->mlevel == step20_roster[i].level);
        assert(ptr->mmove == step20_roster[i].speed);
        assert(ptr->ac == step20_roster[i].ac && ptr->mr == step20_roster[i].mr);
        assert(!!(ptr->geno & G_GENO) == step20_roster[i].geno);
        assert(!!polyok(ptr) == step20_roster[i].poly);
        assert((ptr->geno & G_FREQ) == 1);
        assert(!(ptr->geno & (G_NOGEN | G_UNIQ | G_HELL | G_NOHELL | G_SGROUP | G_LGROUP)));
        assert(ptr->mconveys == 0 && !corpse_intrinsic(ptr));
        u.uz.dlevel = step20_roster[i].depth - 1;
        assert(!step19_generation_ok(step20_roster[i].pm));
        ++u.uz.dlevel;
        assert(step19_generation_ok(step20_roster[i].pm));
        u.uz.dlevel = 199;
        assert(step19_generation_ok(step20_roster[i].pm));
        u.uz.dnum = mines_dnum;
        assert(!step19_generation_ok(step20_roster[i].pm));
        u.uz.dnum = medusa_level.dnum;
    }
    assert(dragon_armor_type(PM_VOID_DRAGON, FALSE) == STRANGE_OBJECT);
    assert(little_to_big(PM_VOID_DRAGON) == PM_VOID_DRAGON);
    assert(big_to_little(PM_VOID_DRAGON) == PM_VOID_DRAGON);
    assert(!is_giant(&mons[PM_GUG]));
    assert(pm_to_cham(PM_VAMPIRE_MAGE) == PM_VAMPIRE_MAGE);
    /* Stored donor difficulty, not the independent mstrength() heuristic. */
    assert(mons[PM_DRIDER].difficulty == 16);
    assert(mstrength(&mons[PM_DRIDER]) == 18);
    u.ulevel = 30;
    for (k = 0; k < SIZE(depths); ++k) {
        u.uz.dlevel = depths[k];
        init_isaac64(202000UL + depths[k], rn2);
        memset(samples, 0, sizeof samples); other = none = 0;
        for (j = 0; j < 10000; ++j) {
            ptr = rndmonst();
            if (!ptr) { ++none; continue; }
            i = step20_index(monsndx(ptr));
            if (i < 0) ++other;
            else {
                assert(depths[k] >= step20_roster[i].depth);
                assert((ptr->geno & G_FREQ) == 1);
                ++samples[i];
            }
        }
        assert(other > 0);
        printf("SAMPLE Step20 DL%d n=10000 native=%d none=%d", depths[k], other, none);
        for (i = 0; i < SIZE(step20_roster); ++i)
            if (samples[i]) printf(" | %s=%d", pmname(&mons[step20_roster[i].pm], NEUTRAL), samples[i]);
        puts("");
    }
    u = saved;
    puts("PASS Step 20 definitions, eligibility, depth boundaries and seeded native generation");
}

static struct monst *
step20_test_mon(int pm, coordxy x, coordxy y)
{
    struct monst *mon = makemon(&mons[pm], x, y,
                               NO_MINVENT | MM_NOGRP | MM_NOCOUNTBIRTH);
    assert(mon);
    mon->mcanmove = mon->mcansee = 1;
    mon->msleeping = mon->mfrozen = mon->mflee = mon->mpeaceful = 0;
    mon->mhp = mon->mhpmax = 10000;
    return mon;
}

static void
step20_combat_tests(void)
{
    static const int offsets[][2] = { {2,0}, {0,2}, {2,1}, {1,2}, {2,2} };
    struct monst *a, *b;
    struct trap *trap;
    struct mhitm_data result;
    coordxy x = 4, y = 3, xx, yy;
    struct rm saved[7][7];
    int i, trial, hp, hits, misses;
    forge_test_clear();
    for (xx = 0; xx < 7; ++xx)
        for (yy = 0; yy < 7; ++yy) {
            assert(!MON_AT(x + xx, y + yy) && !u_at(x + xx, y + yy));
            saved[xx][yy] = levl[x + xx][y + yy];
            if (t_at(x + xx, y + yy)) deltrap(t_at(x + xx, y + yy));
            levl[x + xx][y + yy].typ = ROOM;
            unblock_point(x + xx, y + yy);
        }
    a = step20_test_mon(PM_HOUND_OF_TINDALOS, x, y);
    a->m_lev = 100; a->mcan = 1;
    for (i = 0; i < SIZE(offsets); ++i) {
        b = step20_test_mon(PM_COCKATRICE, x + offsets[i][0], y + offsets[i][1]);
        b->m_lev = 100;
        gb.bhitpos.x = b->mx; gb.bhitpos.y = b->my; gn.notonhead = FALSE;
        hp = b->mhp;
        (void) mattackm(a, b);
        assert(!DEADMONSTER(a) && a->mhp == 10000);
        assert(hp - b->mhp >= 2 && hp - b->mhp <= 12);
        mongone(b);
    }
    b = step20_test_mon(PM_HUMAN, x + 2, y);
    gb.bhitpos.x = b->mx; gb.bhitpos.y = b->my;
    levl[x + 1][y].typ = STONE; block_point(x + 1, y);
    hp = b->mhp; (void) mattackm(a, b); assert(b->mhp == hp);
    levl[x + 1][y].typ = DOOR; levl[x + 1][y].doormask = D_CLOSED;
    (void) mattackm(a, b); assert(b->mhp == hp);
    levl[x + 1][y].typ = ROOM; unblock_point(x + 1, y);
    a->m_lev = 1; a->mcan = 1;
    b->data = &mons[PM_BLACK_DRAGON];
    hits = misses = 0;
    for (trial = 0; trial < 200; ++trial) {
        hp = b->mhp; (void) mattackm(a, b);
        if (b->mhp == hp) ++misses; else ++hits;
    }
    assert(misses > 0); /* no automatic ranged hit */
    b->data = &mons[PM_HUMAN];
    a->data = &mons[PM_ELDER_BRAIN]; a->mcan = 0; a->m_lev = 100;
    b->data = &mons[PM_MEDUSA];
    hp = b->mhp;
    (void) mattackm(a, b);
    assert(!DEADMONSTER(a) && a->mhp == 10000 && b->mhp < hp);
    assert(a->mspec_used == 0); /* neither spell slot ran */
    assert(m_move(a, 0) == MMOVE_NOTHING && a->mx == x && a->my == y);
    b->data = &mons[PM_HUMAN];
    assert(step20_web(b));
    trap = t_at(b->mx, b->my); assert(trap && trap->ttyp == WEB);
    assert(!step20_web(b) && t_at(b->mx, b->my) == trap);
    deltrap(trap); b->mtrapped = 0;
    trap = maketrap(b->mx, b->my, PIT);
    assert(!step20_web(b) && t_at(b->mx, b->my) == trap && trap->ttyp == PIT);
    deltrap(trap);
    levl[b->mx][b->my].typ = STAIRS;
    assert(!step20_web(b) && !t_at(b->mx, b->my));
    levl[b->mx][b->my].typ = ROOM;
    a->data = &mons[PM_DRIDER]; a->m_lev = 100;
    hp = b->mhp;
    (void) mattackm(a, b);
    assert(b->mhp == hp && t_at(b->mx, b->my));
    deltrap(t_at(b->mx, b->my)); b->mtrapped = 0;
    a->data = &mons[PM_VORPAL_JABBERWOCK];
    hits = 0;
    for (trial = 0; trial < 4000; ++trial) {
        memset(&result, 0, sizeof result); result.damage = 15;
        mhitm_adtyping(a, &a->data->mattk[2], b, &result);
        hits += result.fatal != 0;
    }
    assert(hits > 50 && hits < 150);
    a->mcan = 1;
    for (trial = 0; trial < 100; ++trial) {
        memset(&result, 0, sizeof result); result.damage = 15;
        mhitm_adtyping(a, &a->data->mattk[2], b, &result);
        assert(!result.fatal);
    }
    assert(!vorpal_target(&mons[PM_SHOGGOTH]));
    assert(vorpal_target(&mons[PM_HUMAN]));
    mongone(b);
    a->mcan = 0;
    init_isaac64(202040UL, rn2);
    hits = misses = 0;
    for (trial = 0; trial < 400; ++trial) {
        struct obj *amulet;
        boolean lifesaving = trial >= 200;
        b = step20_test_mon(PM_WATER_DOLPHIN, x + 1, y);
        b->mhp = b->mhpmax = 200;
        a->m_lev = 100;
        if (lifesaving) {
            amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
            amulet->owornmask = W_AMUL;
            b->misc_worn_check |= W_AMUL;
            (void) mpickobj(b, amulet);
            assert(mlifesaver(b));
        }
        gb.bhitpos.x = b->mx; gb.bhitpos.y = b->my;
        gn.notonhead = FALSE;
        (void) mattackm(a, b);
        if (DEADMONSTER(b)) {
            ++hits;
        } else {
            if (lifesaving && !mlifesaver(b)) ++misses;
            /* Two full 3d10 bites plus two quartered claws do at most 74.
             * A surviving, partially mitigated beheading must not pass. */
            assert(b->mhp >= 126);
            mongone(b);
        }
    }
    assert(hits > 0 && misses > 0);
    /* Select a seed whose actual damage dice are followed by the 1/40
     * trigger. Replay it through the complete native damage/death path. */
    for (i = 1; i < 10000; ++i) {
        init_isaac64((unsigned long) i, rn2);
        (void) d(3, 10);
        if (!rn2(40)) break;
    }
    assert(i < 10000);
    for (trial = 0; trial < 3; ++trial) {
        struct obj *amulet;
        b = step20_test_mon(PM_WATER_DOLPHIN, x + 1, y);
        b->mhp = b->mhpmax = 200;
        if (trial == 1) {
            amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
            amulet->owornmask = W_AMUL;
            b->misc_worn_check |= W_AMUL;
            (void) mpickobj(b, amulet);
        }
        a->mcan = trial == 2;
        gn.notonhead = FALSE;
        init_isaac64((unsigned long) i, rn2);
        (void) step20_claw_damage(a, b);
        if (trial == 0) assert(DEADMONSTER(b));
        else {
            assert(!DEADMONSTER(b));
            if (trial == 1) assert(!mlifesaver(b) && b->mhp == 200);
            else assert(b->mhp >= 193 && b->mhp < 200);
            mongone(b);
        }
    }
    puts("PASS Step 20 complete vorpal damage path: resistance, lifesaving and cancelled ordinary claws");
    mongone(a);
    for (xx = 0; xx < 7; ++xx)
        for (yy = 0; yy < 7; ++yy) {
            levl[x + xx][y + yy] = saved[xx][yy];
            if (IS_STWALL(saved[xx][yy].typ)) block_point(x + xx, y + yy);
        }
    puts("PASS Step 20 native range-2 obstruction/non-contact, Web placement/action, stationarity and vorpal dispatch");
}

static void
step20_equipment_tests(void)
{
    int i, branches[3] = { 0, 0, 0 };
    struct monst *mon;
    struct obj *obj;
    init_isaac64(202095UL, rn2);
    for (i = 0; i < 150; ++i) {
        int swords = 0, branch = 0;
        mon = makemon(&mons[PM_PRIESTESS_OF_GHAUNADAUR], 0, 0,
                      MM_NOGRP | MM_NOCOUNTBIRTH | MM_NATURAL);
        assert(mon);
        for (obj = mon->minvent; obj; obj = obj->nobj) {
            if (obj->otyp == CRYSTAL_SWORD) {
                ++swords;
                assert(!obj->oartifact);
            }
            if (obj->otyp == CRYSTAL_PLATE_MAIL) branch = 1;
            if (obj->otyp == CLOAK_OF_PROTECTION) branch = 2;
        }
        assert(swords == 1);
        ++branches[branch];
        mongone(mon);
    }
    assert(branches[0] && branches[1] && branches[2]);
    puts("PASS Step 20 Priestess crystal sword in all three armor branches");
}

static void
step20_corpse_tests(void)
{
    struct monst *mon;
    struct obj *obj;
    coordxy x, y;
    schar terrain;
    int i, trial, corpses, pm, corpse_pm;
    unsigned saved_mvflags;
    boolean no_corpse;

    /* Exercise mondied -> corpse_chance -> make_corpse, including the
     * explicit species switch used by the project's post-release build. */
    for (x = 2; x < COLNO - 1; ++x) {
        for (y = 1; y < ROWNO - 1; ++y)
            if (!MON_AT(x, y) && !svl.level.objects[x][y]
                && !u_at(x, y) && !t_at(x, y))
                break;
        if (y < ROWNO - 1) break;
    }
    assert(x < COLNO - 1);
    terrain = levl[x][y].typ;
    levl[x][y].typ = ROOM;
    for (i = 0; i < SIZE(step20_roster); ++i) {
        pm = step20_roster[i].pm;
        corpse_pm = pm == PM_VAMPIRE_MAGE ? PM_HUMAN : pm;
        /* The diagnostic entry skips allmain's initialization of native
         * no-corpse policy in mvitals; reproduce that initialization. */
        saved_mvflags = svm.mvitals[pm].mvflags;
        svm.mvitals[pm].mvflags |= mons[pm].geno & G_NOCORPSE;
        no_corpse = pm != PM_VAMPIRE_MAGE
                    && ((mons[pm].geno & G_NOCORPSE) != 0
                        || mons[pm].mlet == S_LICH);
        corpses = 0;
        init_isaac64(202060UL + i, rn2);
        for (trial = 0; trial < 32; ++trial) {
            mon = makemon(&mons[pm], x, y,
                          NO_MINVENT | MM_NOGRP | MM_NOCOUNTBIRTH);
            assert(mon);
            /* Native vampire creation can select a shifted form. Retain
             * its underlying species and return it before the death test. */
            if (mon->data != &mons[pm])
                (void) newcham(mon, &mons[pm], NO_NC_FLAGS);
            assert(mon->data == &mons[pm]);
            mondied(mon);
            obj = sobj_at(CORPSE, x, y);
            if (no_corpse) {
                assert(!obj);
            } else if (obj) {
                assert(obj->corpsenm == corpse_pm);
                if (pm == PM_VAMPIRE_MAGE)
                    assert(obj->age == max(svm.moves, 1L)
                                       - (TAINT_AGE + 1));
                ++corpses;
            }
            while (svl.level.objects[x][y])
                delobj_core(svl.level.objects[x][y], TRUE);
        }
        assert(no_corpse || corpses > 0);
        svm.mvitals[pm].mvflags = saved_mvflags;
    }
    levl[x][y].typ = terrain;
    puts("PASS Step 20 full native death/corpse dispatch and no-corpse policies");
}

static void
step20_test_main(void)
{
    step20_generation_tests();
    step20_combat_tests();
    step20_equipment_tests();
    step20_corpse_tests();
}
