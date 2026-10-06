/* Included by the real-engine Step 19 suite. */
static void
step19_clear_floor(coordxy x, coordxy y)
{
    while (svl.level.objects[x][y])
        delobj_core(svl.level.objects[x][y], TRUE);
}

static void
step19_monster_tests(void)
{
    static const int golems[] = { PM_RUBY_GOLEM, PM_DIAMOND_GOLEM,
        PM_SAPPHIRE_GOLEM, PM_CRYSTAL_GOLEM };
    static const int gems[] = { RUBY, DIAMOND, SAPPHIRE, STRANGE_OBJECT };
    static const int classes[] = { S_DRAGON, S_GOLEM, S_UNICORN, S_EYE, S_LICH };
    struct monst mon = { 0 }, *live;
    struct obj *obj;
    d_level saved = u.uz;
    coordxy x, y;
    int i, trial, count, high, prop, sleep = 0, poison = 0;
    boolean native_dragon = FALSE, custom_dragon = FALSE;
    boolean native_golem = FALSE, custom_golem = FALSE;
    schar terrain;

    /* Use an empty square and restore its terrain after the death tests. */
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
    mon.mx = x; mon.my = y;
    /* mkclass_poly walks the raw monster table, not the generation sort. */
    for (i = 0; i < SIZE(classes); ++i) {
        int state = 0, pm;
        for (pm = LOW_PM; pm < SPECIAL_PM; ++pm) {
            if (mons[pm].mlet == classes[i]) {
                assert(state != 2); /* a class must never resume after a gap */
                state = 1;
            } else if (state == 1) {
                state = 2;
            }
        }
        assert(state);
    }
    mon.data = &mons[PM_SHADOW_DRAGON];
    assert(resists_drli(&mon));
    mon.data = &mons[PM_DEEP_DRAGON];
    assert(resists_drli(&mon));
    mon.data = &mons[PM_SHIMMERING_DRAGON];
    assert(mith_displaced(&mon));
    mon.data = &mons[PM_FILTH_DRAGON];
    assert(defended(&mon, AD_DISE));
    assert(emits_light(&mons[PM_SHADOW_DRAGON]) == 2);

    /* Real intrinsic selection must yield exactly one allowed property. */
    for (trial = 0; trial < 256; ++trial) {
        prop = corpse_intrinsic(&mons[PM_SHADOW_DRAGON]);
        assert(prop == SLEEP_RES || prop == POISON_RES);
        sleep += prop == SLEEP_RES;
        poison += prop == POISON_RES;
        prop = corpse_intrinsic(&mons[PM_CELESTIAL_DRAGON]);
        assert(prop == SLEEP_RES || prop == SHOCK_RES);
        assert(!corpse_intrinsic(&mons[PM_BABY_SHIMMERING_DRAGON]));
    }
    assert(sleep && poison);

    /* The ordinary-generation gate must not prohibit explicit creation. */
    u.uz.dnum = medusa_level.dnum;
    u.uz.dlevel = 1;
    assert(!step19_generation_ok(PM_BABY_SHIMMERING_DRAGON));
    for (trial = 0; trial < 4096; ++trial) {
        prop = mkclass_poly(S_DRAGON);
        native_dragon |= prop == PM_GRAY_DRAGON;
        custom_dragon |= prop == PM_SHIMMERING_DRAGON;
        prop = mkclass_poly(S_GOLEM);
        native_golem |= prop == PM_FLESH_GOLEM;
        custom_golem |= prop == PM_RUBY_GOLEM;
    }
    assert(native_dragon && custom_dragon && native_golem && custom_golem);
    live = makemon(&mons[PM_BABY_SHIMMERING_DRAGON], x, y, NO_MINVENT);
    assert(live);
    mondied(live);
    obj = sobj_at(CORPSE, x, y);
    assert(obj && obj->corpsenm == PM_BABY_SHIMMERING_DRAGON);
    step19_clear_floor(x, y);
    u.uz = saved;

    /* Exercise full reward rolls, including Crystal's per-death T4 cap. */
    for (i = 0; i < SIZE(golems); ++i) {
        mon.data = &mons[golems[i]];
        assert(defended(&mon, AD_DISE));
        for (trial = 0; trial < 128; ++trial) {
            step19_death_reward(&mon);
            count = high = 0;
            for (obj = svl.level.objects[x][y]; obj; obj = obj->nexthere) {
                assert(obj->otyp >= DILITHIUM_CRYSTAL && obj->otyp <= JADE);
                assert(!gems[i] || obj->otyp == gems[i]);
                count += (int) obj->quan;
                if (socket_gem_tier(obj->otyp) == 4)
                    high += (int) obj->quan;
            }
            assert(count >= 1 && count <= (i == 1 ? 2 : 3));
            if (i == 3) assert(high <= 1);
            step19_clear_floor(x, y);
        }
    }
    levl[x][y].typ = terrain;
    puts("PASS Step 19 donor traits, corpse intrinsics, shallow explicit creation, baby corpse and gem reward bounds");
}
