/* Included by test_step13_runtime.c after item() and combat-path declarations.
 * Expectations below are specification data, independent of the catalog. */
static const struct {
    uint32 bit;
    int prop, dice, sides;
} step14_elements[] = {
    { OEP_FIRE, FIRE_RES, 1, 4 }, { OEP_COLD, COLD_RES, 1, 4 },
    { OEP_SHOCK, SHOCK_RES, 1, 4 },
    { OEP_FIRE_II, FIRE_RES, 3, 4 }, { OEP_COLD_II, COLD_RES, 3, 4 },
    { OEP_SHOCK_II, SHOCK_RES, 3, 4 },
    { OEP_FIRE_III, FIRE_RES, 5, 6 }, { OEP_COLD_III, COLD_RES, 5, 6 },
    { OEP_SHOCK_III, SHOCK_RES, 5, 6 },
    { OEP_PRIMORDIAL, FIRE_RES, 5, 6 },
    { OEP_PRIMORDIAL, COLD_RES, 5, 6 },
    { OEP_PRIMORDIAL, SHOCK_RES, 5, 6 }
};

static int
step14_expected_elements(uint32 bits, int resisted)
{
    int i, value = 0;
    for (i = 0; i < SIZE(step14_elements); ++i)
        if ((bits & step14_elements[i].bit)
            && step14_elements[i].prop != resisted)
            value += d(step14_elements[i].dice, step14_elements[i].sides);
    return value;
}

static void
step14_element_tests(void)
{
    const uint32 pairs[][2] = {
        { OEP_FIRE, OEP_FIRE }, { OEP_COLD_II, OEP_COLD_II },
        { OEP_SHOCK_III, OEP_SHOCK_III },
        { OEP_FIRE, OEP_FIRE_II }, { OEP_COLD_II, OEP_FIRE_III },
        { OEP_FIRE_II, OEP_PRIMORDIAL },
        { OEP_FIRE | OEP_FIRE_II, OEP_COLD | OEP_COLD_III },
        { OEP_PRIMORDIAL | OEP_SHOCK_III, OEP_PRIMORDIAL | OEP_FIRE_II }
    };
    const int resistances[] = { 0, FIRE_RES, COLD_RES, SHOCK_RES };
    struct obj *o = item(ARROW), *bow = item(BOW), saved;
    struct monst m = { 0 };
    int i, j, seed, hero, known, use, want, next, value, base;
    m.data = &mons[PM_HUMAN]; m.mhp = m.mhpmax = 1000;
    HBlinded = 1;
    for (i = 0; i < SIZE(step14_elements); ++i) {
        uint32 bits = step14_elements[i].bit;
        assert(enhancement_set(o, bits, OQ_EXCEPTIONAL, FALSE));
        for (j = 0; j < SIZE(resistances); ++j)
            for (hero = 0; hero < 2; ++hero)
                for (known = 0; known < 2; ++known)
                    for (seed = 1; seed <= 16; ++seed) {
                        int prop = resistances[j];
                        o->o_enh_known = known ? OEP_ALL : 0;
                        saved = *o;
                        m.mintrinsics = prop ? res_to_mr(prop) : 0;
                        if (prop) u.uprops[prop].intrinsic = FROMOUTSIDE;
                        init_isaac64(seed, rn2);
                        want = step14_expected_elements(bits, prop);
                        next = rn2(100000);
                        init_isaac64(seed, rn2);
                        value = enhancement_weapon_effects(o, NULL,
                            hero ? &gy.youmonst : &m, 0, ENHANCE_THROWN);
                        assert(value == want && rn2(100000) == next);
                        SAME(o, &saved);
                        if (prop) u.uprops[prop].intrinsic = 0;
                    }
        /* Elemental dice must never enter physical dmgval or its RNG stream. */
        m.mintrinsics = 0;
        init_isaac64(101, rn2); base = dmgval(o, &m); next = rn2(100000);
        o->o_enh_props = 0;
        init_isaac64(101, rn2);
        assert(dmgval(o, &m) == base && rn2(100000) == next);
    }
    for (i = 0; i < SIZE(pairs); ++i)
        for (j = 0; j < SIZE(resistances); ++j)
            for (use = ENHANCE_MELEE; use <= ENHANCE_AMMO; ++use)
                for (seed = 1; seed <= 24; ++seed) {
                    int prop = resistances[j];
                    assert(enhancement_set(o, pairs[i][0], OQ_FINE, FALSE));
                    assert(enhancement_set(bow, pairs[i][1], OQ_EXCEPTIONAL, FALSE));
                    m.mintrinsics = prop ? res_to_mr(prop) : 0;
                    init_isaac64(seed, rn2);
                    want = step14_expected_elements(pairs[i][0], prop);
                    if (use == ENHANCE_AMMO)
                        want += step14_expected_elements(pairs[i][1], prop);
                    next = rn2(100000);
                    init_isaac64(seed, rn2);
                    assert(enhancement_weapon_effects(o, bow, &m, 1000, use) == want);
                    assert(rn2(100000) == next);
                }
    m.mintrinsics = 0;
    for (i = 0; i <= 2; ++i) for (j = 0; j <= 2; ++j) {
        assert(enhancement_set(o, OEP_TRUEFLIGHT, i, FALSE));
        assert(enhancement_set(bow, OEP_TRUEFLIGHT, j, FALSE));
        assert(enhancement_hit_bonus(o, bow, &m, ENHANCE_AMMO) == j + 2);
        assert(enhancement_damage_bonus(o, bow, &m, ENHANCE_AMMO) == i);
        assert(enhancement_hit_bonus(o, bow, &m, ENHANCE_THROWN) == i + 2);
        assert(enhancement_hit_bonus(o, bow, &m, ENHANCE_MELEE) == i);
        enhancement_observe_attack(o, bow, ENHANCE_AMMO);
        HBlinded = 0;
        enhancement_observe_hit(o, bow, &m, ENHANCE_AMMO);
        assert(!o->o_enh_known && !bow->o_enh_known);
        HBlinded = 1;
    }
    obfree(o, NULL); obfree(bow, NULL); HBlinded = 0;
    puts("PASS Step 14 exact native component dice/RNG, resistance, knowledge independence, launcher stacking and Trueflight cap");
}

static void
step14_armor_tests(void)
{
    const int types[] = { LEATHER_ARMOR, SMALL_SHIELD, HELMET, LEATHER_GLOVES,
                         LOW_BOOTS, CLOAK_OF_PROTECTION, HAWAIIAN_SHIRT };
    const long slots[] = { W_ARM, W_ARMS, W_ARMH, W_ARMG, W_ARMF, W_ARMC, W_ARMU };
    const uint32 bits[] = { OEP_SEARCHING, OEP_WARNING, OEP_STEALTH,
        OEP_FIRE_RES, OEP_COLD_RES, OEP_SHOCK_RES, OEP_POISON_RES,
        OEP_SPEED, OEP_REGEN, OEP_DISPLACED, OEP_SLOW_DIGEST,
        OEP_MAGIC_RES, OEP_REFLECTION };
    const int props[] = { SEARCHING, WARNING, STEALTH, FIRE_RES, COLD_RES,
        SHOCK_RES, POISON_RES, FAST, REGENERATION, DISPLACED, SLOW_DIGESTION,
        ANTIMAGIC, REFLECTING };
    int i, j;
    HBlinded = 1;
    for (i = 0; i < SIZE(types); ++i) for (j = 0; j < SIZE(bits); ++j) {
        struct obj *o = item(types[i]);
        long old = u.uprops[props[j]].extrinsic;
        assert(enhancement_set(o, bits[j], OQ_FINE, FALSE));
        o = addinv(o); setworn(o, slots[i]);
        assert(u.uprops[props[j]].extrinsic & slots[i]);
        setnotworn(o);
        assert(u.uprops[props[j]].extrinsic == old);
        freeinv(o); obfree(o, NULL);
    }
    for (j = 0; j < SIZE(bits); ++j) {
        struct obj *a = item(LOW_BOOTS), *b = item(LEATHER_GLOVES);
        long old = u.uprops[props[j]].extrinsic;
        long intrinsic = u.uprops[props[j]].intrinsic;
        assert(enhancement_set(a, bits[j], OQ_STANDARD, FALSE));
        assert(enhancement_set(b, bits[j], OQ_STANDARD, FALSE));
        u.uprops[props[j]].intrinsic = FROMOUTSIDE;
        a = addinv(a); b = addinv(b); setworn(a, W_ARMF); setworn(b, W_ARMG);
        setnotworn(a);
        assert(!(u.uprops[props[j]].extrinsic & W_ARMF));
        assert(u.uprops[props[j]].extrinsic & W_ARMG);
        setnotworn(b);
        assert(u.uprops[props[j]].extrinsic == old);
        assert(u.uprops[props[j]].intrinsic == FROMOUTSIDE);
        u.uprops[props[j]].intrinsic = intrinsic;
        freeinv(a); freeinv(b); obfree(a, NULL); obfree(b, NULL);
    }
    for (i = 0; i < SIZE(types); ++i) for (j = 0; j < SIZE(bits); ++j) {
        struct monst m = { 0 };
        struct obj *a = item(types[i]),
                   *b = item(i == 3 ? LOW_BOOTS : LEATHER_GLOVES);
        long moves = svm.moves;
        m.data = &mons[PM_HUMAN]; m.mhp = 50; m.mhpmax = 100;
        assert(enhancement_set(a, bits[j], OQ_STANDARD, FALSE));
        assert(enhancement_set(b, bits[j], OQ_STANDARD, FALSE));
        add_to_minv(&m, a); add_to_minv(&m, b);
        a->owornmask = slots[i]; b->owornmask = i == 3 ? W_ARMF : W_ARMG;
        m.misc_worn_check = a->owornmask | b->owornmask;
        update_mon_extrinsics(&m, a, TRUE, TRUE);
        update_mon_extrinsics(&m, b, TRUE, TRUE);
        if (j >= 3 && j <= 6) assert(m.mextrinsics & res_to_mr(props[j]));
        if (props[j] == FAST) assert(m.mspeed == MFAST);
        if (props[j] == ANTIMAGIC) assert(resists_magm(&m));
        if (props[j] == REFLECTING) assert(mon_reflects(&m, NULL));
        if (props[j] == DISPLACED) assert(mith_displaced(&m));
        svm.moves = 1; mon_regen(&m, FALSE);
        assert(m.mhp == (props[j] == REGENERATION ? 51 : 50));
        svm.moves = moves;
        if (j < 3 || props[j] == SLOW_DIGESTION)
            assert(!m.mextrinsics && !m.mspeed && !m.minvis);
        a->owornmask = 0;
        update_mon_extrinsics(&m, a, FALSE, TRUE);
        if (j >= 3 && j <= 6) assert(m.mextrinsics & res_to_mr(props[j]));
        if (props[j] == FAST) assert(m.mspeed == MFAST);
        b->owornmask = 0;
        update_mon_extrinsics(&m, b, FALSE, TRUE);
        assert(!m.mextrinsics && !m.mspeed);
        assert(!resists_magm(&m) && !mon_reflects(&m, NULL) && !mith_displaced(&m));
        assert(a->o_enh_props == bits[j] && b->o_enh_props == bits[j]);
        assert(!a->o_enh_known && !b->o_enh_known);
        obj_extract_self(a); obj_extract_self(b); obfree(a, NULL); obfree(b, NULL);
    }
    HBlinded = 0;
    puts("PASS Step 14 13 native armor properties in seven slots, redundant sources, intrinsic retention and monster native effects/exceptions");
}

static void
step14_combat_path_tests(void)
{
    const uint32 bits[] = { OEP_FIRE_II, OEP_COLD_III, OEP_PRIMORDIAL };
    const int minimum[] = { 3, 5, 15 }, maximum[] = { 12, 30, 90 };
    struct monst *a = makemon(&mons[PM_HUMAN], 31, 10, NO_MINVENT);
    struct monst *defender = makemon(&mons[PM_DWARF], 32, 10, NO_MINVENT);
    int path, kind, seed, pass, marker, loss[3], elemental_delta = 0;
    aligntyp old_alignment = u.ualign.type;
    assert(a && defender); a->m_lev = 100;
    HBlinded = 1; u.uconduct.weaphit = 10; u.ualign.type = A_LAWFUL;
    for (path = 0; path < 9; ++path) for (kind = 0; kind < SIZE(bits); ++kind)
        for (seed = 1; seed <= 12; ++seed) {
            boolean shot = path == 2 || path == 6 || path == 8;
          for (marker = 0; marker < 3; ++marker) {
            for (pass = 0; pass < 3; ++pass) {
                struct obj *o = item(shot ? ARROW : DAGGER), *bow = item(BOW);
                enum enhance_use use = shot ? ENHANCE_AMMO : ENHANCE_THROWN;
                o->obranch_props = marker == 1 ? OBP_ANARCHIC
                                     : marker == 2 ? OBP_CONCORDANT : 0;
                assert(enhancement_set(o, pass ? bits[kind] : 0, OQ_STANDARD, FALSE));
                assert(enhancement_set(bow, pass ? bits[kind] : 0, OQ_STANDARD, FALSE));
                defender->mhp = defender->mhpmax = a->mhp = a->mhpmax = 2000;
                u.uhp = u.uhpmax = 2000; u.uac = 10;
                defender->mintrinsics = pass == 2 ? MR_FIRE | MR_COLD | MR_ELEC : 0;
                HFire_resistance = HCold_resistance = HShock_resistance =
                    pass == 2 ? FROMOUTSIDE : 0;
                init_isaac64(seed, rn2);
                if (path < 3) {
                    if (shot) uwep = bow;
                    (void) hmon(defender, o, path == 0 ? HMON_MELEE : HMON_THROWN, 10);
                    uwep = NULL;
                } else if (path == 3) (void) step13_hitmu(a, o);
                else if (path == 4) (void) step13_mdamagem(a, defender, o);
                else if (path == 5 || path == 6) (void) step13_thitu(&o, bow, use);
                else {
                    gm.marcher = a; gm.mtarget = defender;
                    gb.bhitpos.x = defender->mx; gb.bhitpos.y = defender->my;
                    (void) step13_ohitmon(defender, o, bow, use);
                    gm.marcher = gm.mtarget = NULL; o = NULL;
                }
                loss[pass] = 2000 - ((path == 3 || path == 5 || path == 6)
                                     ? u.uhp : defender->mhp);
                if (o) obfree(o, NULL);
                obfree(bow, NULL);
                defender->mintrinsics = 0;
                HFire_resistance = HCold_resistance = HShock_resistance = 0;
            }
            assert(loss[0] > 0 && loss[2] == loss[0]);
            assert(loss[1] - loss[0] >= minimum[kind] * (shot ? 2 : 1));
            assert(loss[1] - loss[0] <= maximum[kind] * (shot ? 2 : 1));
            /* Both markers double physical damage against these lawful
             * defenders, but cannot change even one point of elemental dice.
             * Seed replay is exact: alignment markers consume no random dice. */
            if (!marker) elemental_delta = loss[1] - loss[0];
            else assert(loss[1] - loss[0] == elemental_delta);
          }
        }
    HBlinded = 0; u.ualign.type = old_alignment;
    mongone(a); mongone(defender);
    puts("PASS Step 14 T2/T3/Primordial nine combat paths, exact elemental independence from Anarchic/Concordant doubling, stacked shots and native resistance");
}

static void
step14_shade_tests(void)
{
    struct monst *a = makemon(&mons[PM_HUMAN], 31, 10, NO_MINVENT);
    struct monst *defender = makemon(&mons[PM_SHADE], 32, 10, NO_MINVENT);
    int path, source, seed, pass, loss[2];
    assert(a && defender); a->m_lev = 100;
    assert(!resists_fire(defender));
    HBlinded = 1;
    /* hmon melee/throw/shot, monster melee and monster throw/shot impact.
     * On shot paths also exercise a completely ordinary arrow with an
     * enhanced launcher: its zero physical dmgval cannot suppress fire. */
    for (path = 0; path < 6; ++path) {
        boolean shot = path == 2 || path == 5;
        for (source = 0; source < (shot ? 2 : 1); ++source)
            for (seed = 1; seed <= 12; ++seed) {
                for (pass = 0; pass < 2; ++pass) {
                    struct obj *o = item(shot ? ARROW : DAGGER), *bow = item(BOW);
                    struct obj *enhanced = source ? bow : o;
                    o->spe = 50; /* reliable projectile accuracy; shade physical damage stays zero */
                    assert(enhancement_set(enhanced, pass ? OEP_FIRE_III : 0,
                                           OQ_STANDARD, FALSE));
                    defender->mhp = defender->mhpmax = 2000;
                    assert(dmgval(o, defender) == 0);
                    if (pass && !source)
                        assert(!shade_miss(a, defender, o, FALSE, FALSE));
                    init_isaac64(seed, rn2);
                    if (path < 3) {
                        if (shot) uwep = bow;
                        (void) hmon(defender, o, path == 0 ? HMON_MELEE : HMON_THROWN, 10);
                        uwep = NULL;
                    } else if (path == 3) {
                        (void) step13_mdamagem(a, defender, o);
                    } else {
                        gm.marcher = a; gm.mtarget = defender;
                        gb.bhitpos.x = defender->mx; gb.bhitpos.y = defender->my;
                        (void) step13_ohitmon(defender, o, bow,
                                              shot ? ENHANCE_AMMO : ENHANCE_THROWN);
                        gm.marcher = gm.mtarget = NULL; o = NULL;
                    }
                    loss[pass] = 2000 - defender->mhp;
                    if (o) obfree(o, NULL);
                    obfree(bow, NULL);
                }
                assert(loss[0] == 0);
                assert(loss[1] >= 5 && loss[1] <= 30);
            }
    }
    HBlinded = 0;
    mongone(a); mongone(defender);
    puts("PASS Step 14 zero physical shade damage still permits elemental melee/throw/shot damage, including launcher-only fire");
}

static void
step14_combat_tests(void)
{
    step14_element_tests();
    step14_armor_tests();
    step14_combat_path_tests();
    step14_shade_tests();
}

/* Knowledge policy belongs to the hero hit caller, not to the common damage
 * calculator. Keep targets genuinely visible: the older nine-path mechanics
 * corpus deliberately blinds the hero and cannot prove this distinction. */
static void
step14_hero_use_observation_tests(void)
{
    struct monst *attacker=makemon(&mons[PM_HUMAN],31,10,NO_MINVENT);
    struct monst *defender=makemon(&mons[PM_HUMAN],32,11,NO_MINVENT);
    const uint32 effects=OEP_FIRE_II|OEP_PRIMORDIAL;
    int path,condition,pass,loss[3],tail[3],cases=0,survivors=0;
    int oldx=u.ux,oldy=u.uy,oldhp=u.uhp,oldhpmax=u.uhpmax,oldac=u.uac;
    long blind=HBlinded,fire=HFire_resistance,cold=HCold_resistance,
         shock=HShock_resistance,conduct=u.uconduct.weaphit;
    int viz_attacker=gv.viz_array[10][31],viz_hero=gv.viz_array[10][32],
        viz_defender=gv.viz_array[11][32];
    boolean mon_moving=svc.context.mon_moving;
    assert(attacker && defender && !gi.invent && !uwep);
    attacker->m_lev=100;attacker->minvis=defender->minvis=0;
    u.ux=32;u.uy=10;u.uconduct.weaphit=10;
    gv.viz_array[10][31]=gv.viz_array[10][32]=
        gv.viz_array[11][32]=IN_SIGHT|COULD_SEE;

    /* Hero melee/throw/shot, monster melee against hero/monster, and actual
     * monster m_throw flights (throw/shot) against hero/monster. */
    for(path=0;path<9;++path)for(condition=0;condition<4;++condition) {
        boolean shot=path==2||path==6||path==8;
        boolean hero_target=path==3||path==5||path==6;
        uint32 learned=condition==0?effects
            :condition==1?OEP_PRIMORDIAL:0;
        int low=condition==2?0:condition==1?10:18;
        int high=condition==2?0:condition==1?60:102;
        for(pass=0;pass<3;++pass) {
            struct obj *weapon=item(shot?ARROW:DAGGER),*bow=item(BOW);
            struct obj *landed;
            struct obj weapon_state,bow_state;
            unsigned projectile_id;
            weapon->spe=50; /* certain native projectile hit; not enhancement */
            assert(enhancement_set(weapon,pass?effects:0,OQ_FINE,FALSE));
            assert(enhancement_set(bow,pass?effects:0,OQ_EXCEPTIONAL,FALSE));
            if(pass==2) {
                enhancement_identify(weapon);enhancement_identify(bow);
            }
            weapon_state=*weapon;bow_state=*bow;
            u.uhp=u.uhpmax=defender->mhp=defender->mhpmax=2000;
            u.uac=10;
            HBlinded=condition==3?1:0;
            defender->mintrinsics=condition==2?MR_FIRE|MR_COLD|MR_ELEC
                :condition==1?MR_FIRE:0;
            HFire_resistance=condition==1||condition==2?FROMOUTSIDE:0;
            HCold_resistance=HShock_resistance=condition==2?FROMOUTSIDE:0;
            if(condition!=3) {
                assert(!Blind && canseemon(attacker) && canseemon(defender));
            }
            if(path<3) {
                bow=addinv(bow);
                if(shot)setuwep(bow);
                if(path==0) {
                    weapon=addinv(weapon);setuwep(weapon);
                }
            } else {
                if(path>=5)weapon->quan=2;
                add_to_minv(attacker,weapon);add_to_minv(attacker,bow);
                MON_WEP(attacker)=shot?bow:weapon;
                MON_WEP(attacker)->owornmask=W_WEP;
                attacker->misc_worn_check=W_WEP;
            }
            svc.context.mon_moving=path>=3;
            init_isaac64(150013UL,rn2);
            if(path<3)
                (void)hmon(defender,weapon,path==0?HMON_MELEE:HMON_THROWN,10);
            else if(path==3)(void)step13_hitmu(attacker,weapon);
            else if(path==4)(void)step13_mdamagem(attacker,defender,weapon);
            else {
                gm.marcher=attacker;gm.mtarget=hero_target?NULL:defender;
                m_throw(attacker,attacker->mx,attacker->my,1,
                        hero_target?0:1,1,weapon);
                gm.marcher=gm.mtarget=NULL;
                projectile_id=svc.context.objsplit.child_oid;
                assert(weapon->quan==1 && weapon->where==OBJ_MINVENT
                       && weapon->ocarry==attacker);
                assert(bow->where==OBJ_MINVENT && bow->ocarry==attacker);
                assert(MON_WEP(attacker)==(shot?bow:weapon));
                assert(!gt.thrownobj);
                /* Reacquire by ID: native flight may destroy ammunition. */
                landed=find_oid(projectile_id);
                if(landed) {
                    assert(landed->where==OBJ_FLOOR && landed->quan==1);
                    SAME(landed,&weapon_state);++survivors;
                    obj_extract_self(landed);obfree(landed,NULL);
                }
            }
            tail[pass]=rn2(1000000);
            loss[pass]=2000-(hero_target?u.uhp:defender->mhp);
            assert(weapon->o_enh_props==weapon_state.o_enh_props
                   && weapon->o_enh_quality==weapon_state.o_enh_quality
                   && weapon->o_enh_flags==weapon_state.o_enh_flags);
            assert(bow->o_enh_props==bow_state.o_enh_props
                   && bow->o_enh_quality==bow_state.o_enh_quality
                   && bow->o_enh_flags==bow_state.o_enh_flags);
            if(path<3 && pass==1) {
                assert(weapon->o_enh_known==learned);
                assert(bow->o_enh_known==(shot?learned:0));
            } else {
                assert(weapon->o_enh_known==weapon_state.o_enh_known);
                assert(bow->o_enh_known==bow_state.o_enh_known);
            }
            if(path<3) {
                setuwep(NULL);
                if(carried(weapon))freeinv(weapon);
                freeinv(bow);
            } else {
                assert(weapon->where==OBJ_MINVENT && weapon->ocarry==attacker);
                assert(bow->where==OBJ_MINVENT && bow->ocarry==attacker);
                setmnotwielded(attacker,MON_WEP(attacker));
                attacker->misc_worn_check=0;
                obj_extract_self(weapon);obj_extract_self(bow);
                assert(!attacker->minvent);
            }
            obfree(weapon,NULL);obfree(bow,NULL);++cases;
        }
        assert(loss[0]>0);
        assert(loss[1]-loss[0]>=low*(shot?2:1));
        assert(loss[1]-loss[0]<=high*(shot?2:1));
        assert(loss[1]==loss[2] && tail[1]==tail[2]);
    }
    assert(cases==108 && survivors>0);
    defender->mintrinsics=0;
    HBlinded=blind;HFire_resistance=fire;HCold_resistance=cold;
    HShock_resistance=shock;svc.context.mon_moving=mon_moving;
    u.ux=oldx;u.uy=oldy;u.uhp=oldhp;u.uhpmax=oldhpmax;u.uac=oldac;
    u.uconduct.weaphit=conduct;
    mongone(attacker);mongone(defender);
    gv.viz_array[10][31]=viz_attacker;gv.viz_array[10][32]=viz_hero;
    gv.viz_array[11][32]=viz_defender;
    printf("PASS hero-use-only elemental observation: %d native impacts, nine visible/blind combat paths, resistance/Primordial, knowledge-independent damage/RNG, %d surviving monster projectiles\n",cases,survivors);
}
