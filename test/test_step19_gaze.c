/* Exercise the actual monster gaze dispatcher and shared damage handlers. */
extern int step19_gazemm(struct monst *, struct monst *, struct attack *);

static void
step19_hero_gaze_tests(struct monst *a)
{
    struct you saved = u;
    struct permonst *oldform = gy.youmonst.data, *oldattacker = a->data;
    struct attack gaze = { AT_GAZE, AD_STON, 0, 0 };
    struct obj *eyes, *shield;
    int oldmulti = gm.multi, oldvision = gv.vision_full_recalc;
    int oldviz = gv.viz_array[a->my][a->mx], trial, hits = 0, misses = 0;
    int oldhp, lost;

    assert(!uarms && !ublindf && !gi.invent);
    a->data = &mons[PM_BEHOLDER];
    a->mcansee = 1; a->mcan = a->minvis = a->msleeping = 0;
    a->seen_resistance = 0;
    u.ux = a->mx + 1; u.uy = a->my;
    u.umonnum = u.umonster;
    gy.youmonst.data = &mons[PM_HUMAN];
    u.ulevel = 10; u.uhp = u.uhpmax = 10000;
    HBlinded = HHallucination = Stoned = HStone_resistance = 0;
    EStone_resistance = HDisint_resistance = EDisint_resistance = 0;
    HAntimagic = EAntimagic = HSleep_resistance = ESleep_resistance = 0;
    gm.multi = 0;
    gv.vision_full_recalc = 0;
    gv.viz_array[a->my][a->mx] = IN_SIGHT | COULD_SEE;
    assert(!Blind && canseemon(a) && couldsee(a->mx, a->my));
    init_isaac64(191903UL, rn2);
    for (trial = 0; trial < 100 && !Stoned; ++trial)
        (void) gazemu(a, &gaze);
    assert(trial < 100 && Stoned == 5);
    make_stoned(0L, (char *) 0, 0, (char *) 0);
    a->mcan = 1;
    for (trial = 0; trial < 30; ++trial) (void) gazemu(a, &gaze);
    assert(!Stoned);
    a->mcan = 0; HStone_resistance = FROMOUTSIDE;
    for (trial = 0; trial < 30; ++trial) (void) gazemu(a, &gaze);
    assert(!Stoned);
    HStone_resistance = 0;

    eyes = addinv(mksobj(LENSES, FALSE, FALSE));
    eyes->oartifact = ART_EYES_OF_THE_OVERWORLD;
    setworn(eyes, W_TOOL);
    gv.vision_full_recalc = 0;
    oldhp = u.uhp;
    for (trial = 0; trial < 100 && u.uhp == oldhp; ++trial)
        (void) gazemu(a, &gaze);
    assert(trial < 100 && !Stoned && oldhp - u.uhp >= 4
           && oldhp - u.uhp <= 24);
    gaze.adtyp = AD_SLEE;
    for (trial = 0; trial < 30; ++trial) (void) gazemu(a, &gaze);
    assert(gm.multi == 0);
    setworn((struct obj *) 0, W_TOOL);
    eyes->oartifact = 0; useupall(eyes);
    gv.vision_full_recalc = 0;
    HSleep_resistance = FROMOUTSIDE;
    for (trial = 0; trial < 30; ++trial) (void) gazemu(a, &gaze);
    assert(gm.multi == 0);
    HSleep_resistance = 0;
    for (trial = 0; trial < 100 && gm.multi == 0; ++trial)
        (void) gazemu(a, &gaze);
    assert(trial < 100 && gm.multi >= -10 && gm.multi < 0);
    unmul("");

    gaze.adtyp = AD_DISN;
    shield = addinv(mksobj(SMALL_SHIELD, FALSE, FALSE));
    setworn(shield, W_ARMS);
    HDisint_resistance = FROMOUTSIDE;
    for (trial = 0; trial < 30; ++trial) (void) gazemu(a, &gaze);
    assert(uarms == shield);
    HDisint_resistance = 0; a->seen_resistance = 0;
    for (trial = 0; trial < 100 && uarms; ++trial)
        (void) gazemu(a, &gaze);
    assert(trial < 100 && !uarms);
    /* No further destructive gaze can execute after the shield is consumed. */

    gaze.adtyp = AD_CNCL;
    HAntimagic = FROMOUTSIDE;
    oldhp = u.uhp;
    for (trial = 0; trial < 100 && u.uhp == oldhp; ++trial)
        (void) gazemu(a, &gaze);
    assert(trial < 100 && oldhp - u.uhp >= 4 && oldhp - u.uhp <= 16);

    a->data = &mons[PM_VECNA]; a->seen_resistance = 0;
    gaze.adtyp = AD_DETH; gaze.damn = 2; gaze.damd = 6;
    for (trial = 0; trial < 200; ++trial) {
        u.uhp = u.uhpmax = 10000;
        (void) gazemu(a, &gaze);
        lost = 10000 - u.uhpmax;
        if (lost) {
            ++hits;
            assert(lost >= 2 && lost <= 12);
            assert(u.uhp == u.uhpmax);
        } else {
            ++misses;
            assert(u.uhp == 10000);
        }
    }
    /* 3/4 gaze activation times 18/20 draining outcomes with Antimagic.
       Broad bounds reject unconditional/rare drains without exact RNG coupling. */
    assert(hits > 100 && hits < 170 && misses);
    assert(!gi.invent && !Stoned);
    u = saved; gy.youmonst.data = oldform; a->data = oldattacker;
    gm.multi = oldmulti; gv.vision_full_recalc = oldvision;
    gv.viz_array[a->my][a->mx] = oldviz;
    puts("PASS Step 19 hero gaze countdown, Eyes protection, sleep, shield interception and antimagic life drain");
}

static void
step19_gaze_tests(void)
{
    struct monst *a, *d;
    struct obj *shield, *suit;
    struct attack gaze = { AT_GAZE, AD_DISN, 0, 0 };
    struct mhitm_data result;
    coordxy x, y;
    schar terrain[2];
    unsigned seed;
    int trial, cancelled = 0, missed = 0;

    for (x = 2; x < COLNO - 2; ++x) {
        for (y = 1; y < ROWNO - 1; ++y)
            if (!MON_AT(x, y) && !MON_AT(x + 1, y)
                && !u_at(x, y) && !u_at(x + 1, y)
                && !svl.level.objects[x][y] && !svl.level.objects[x + 1][y]
                && !t_at(x, y) && !t_at(x + 1, y))
                break;
        if (y < ROWNO - 1) break;
    }
    assert(x < COLNO - 2);
    terrain[0] = levl[x][y].typ; terrain[1] = levl[x + 1][y].typ;
    levl[x][y].typ = levl[x + 1][y].typ = ROOM;
    a = makemon(&mons[PM_HUMAN], x, y, NO_MINVENT);
    d = makemon(&mons[PM_HUMAN], x + 1, y, NO_MINVENT);
    assert(a && d);
    a->mcansee = d->mcansee = 1;
    a->msleeping = d->msleeping = a->minvis = 0;
    a->mhp = a->mhpmax = d->mhp = d->mhpmax = 100;
    shield = mksobj(SMALL_SHIELD, FALSE, FALSE);
    suit = mksobj(PLATE_MAIL, FALSE, FALSE);
    add_to_minv(d, shield); add_to_minv(d, suit);
    shield->owornmask = W_ARMS; suit->owornmask = W_ARM;
    d->misc_worn_check = W_ARMS | W_ARM;
    a->mcan = 1;
    for (trial = 0; trial < 30; ++trial)
        (void) step19_gazemm(a, d, &gaze);
    assert(which_armor(d, W_ARMS) == shield && d->mhp == 100);
    a->mcan = 0;
    d->mintrinsics |= MR_DISINT;
    for (trial = 0; trial < 30; ++trial)
        (void) step19_gazemm(a, d, &gaze);
    assert(which_armor(d, W_ARMS) == shield && d->mhp == 100);
    d->mintrinsics &= ~MR_DISINT;
    init_isaac64(191902UL, rn2);
    for (trial = 0; trial < 100 && which_armor(d, W_ARMS); ++trial)
        (void) step19_gazemm(a, d, &gaze);
    assert(trial < 100 && which_armor(d, W_ARM) == suit && d->mhp == 100);
    for (trial = 0; trial < 100 && which_armor(d, W_ARM); ++trial)
        (void) step19_gazemm(a, d, &gaze);
    assert(trial < 100 && d->mhp == 100);

    gaze.adtyp = AD_CNCL;
    for (trial = 0; trial < 120; ++trial) {
        d->mcan = 0;
        (void) step19_gazemm(a, d, &gaze);
        if (d->mcan) ++cancelled;
        else ++missed;
    }
    assert(cancelled && missed); /* activation must not be unconditional */
    gaze.adtyp = AD_SLEE;
    d->mcan = 0; d->msleeping = 0; d->mcanmove = 1; d->mfrozen = 0;
    (void) step19_gazemm(a, d, &gaze);
    assert(!d->mcanmove && d->mfrozen);
    d->msleeping = 0; d->mcanmove = 1; d->mfrozen = 0;

    gaze.adtyp = AD_DETH; gaze.damn = 2; gaze.damd = 6;
    /* Direct shared-handler inputs isolate the three donor probability bands. */
    for (seed = 1; seed < 1000; ++seed) {
        init_isaac64(seed, rn2);
        if (rn2(20) >= 17) break;
    }
    assert(seed < 1000);
    d->data = &mons[PM_GRAY_DRAGON];
    memset(&result, 0, sizeof result); result.damage = 10;
    init_isaac64(seed, rn2);
    mhitm_ad_deth(a, &gaze, d, &result);
    assert(!DEADMONSTER(d) && !result.done && result.damage == 10);
    d->data = &mons[PM_LICH];
    memset(&result, 0, sizeof result); result.damage = 10;
    init_isaac64(seed, rn2);
    mhitm_ad_deth(a, &gaze, d, &result);
    assert(!DEADMONSTER(d) && !result.done && result.damage == 10
           && d->mhpmax == 100);
    d->data = &mons[PM_HUMAN];
    for (seed = 1; seed < 1000; ++seed) {
        int roll;
        init_isaac64(seed, rn2); roll = rn2(20);
        if (roll >= 5 && roll < 17 && rn2(6) > 0) break;
    }
    assert(seed < 1000);
    memset(&result, 0, sizeof result); result.damage = 10;
    init_isaac64(seed, rn2);
    mhitm_ad_deth(a, &gaze, d, &result);
    assert(d->mhpmax < 100 && result.damage == 10 && !result.done);
    for (seed = 1; seed < 1000; ++seed) {
        init_isaac64(seed, rn2);
        if (rn2(20) < 5) break;
    }
    assert(seed < 1000);
    memset(&result, 0, sizeof result); result.damage = 10;
    init_isaac64(seed, rn2);
    mhitm_ad_deth(a, &gaze, d, &result);
    assert(result.damage == 0 && !result.done);
    /* A normal living, unarmored target must actually die to disintegration. */
    gaze.adtyp = AD_DISN; gaze.damn = gaze.damd = 0;
    for (trial = 0; trial < 100 && !DEADMONSTER(d); ++trial)
        (void) step19_gazemm(a, d, &gaze);
    assert(DEADMONSTER(d));
    assert(!svl.level.objects[x + 1][y]); /* no corpse or equipment reward */
    d = makemon(&mons[PM_HUMAN], x + 1, y, NO_MINVENT);
    assert(d);
    d->mhp = d->mhpmax = 100;
    gaze.adtyp = AD_DETH; gaze.damn = 2; gaze.damd = 6;
    for (seed = 1; seed < 1000; ++seed) {
        init_isaac64(seed, rn2);
        if (rn2(20) >= 17) break;
    }
    assert(seed < 1000);
    memset(&result, 0, sizeof result); result.damage = 10;
    init_isaac64(seed, rn2);
    mhitm_ad_deth(a, &gaze, d, &result);
    assert(DEADMONSTER(d) && result.done && (result.hitflags & M_ATTK_DEF_DIED));
    while (svl.level.objects[x + 1][y])
        delobj(svl.level.objects[x + 1][y]);
    step19_hero_gaze_tests(a);
    mongone(a);
    levl[x][y].typ = terrain[0]; levl[x + 1][y].typ = terrain[1];
    puts("PASS Step 19 monster gaze activation, disintegration armor/resistance/death and death-magic bands");
}
