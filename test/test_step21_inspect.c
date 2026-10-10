/* Included after the native inspection harness, exercising the real command. */
static void
step21_inspect_tests(void)
{
    struct obj *obj;
    long saved_moves = svm.moves;
    inspect_window_setup();
    obj = inspect_item(CRYSTAL_BALL);
    artifact_exists(obj, "The Orb of Fate", TRUE, ONAME_WISH);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    discover_artifact(obj->oartifact);
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: 37 turns remaining"));
    assert(obj->age == svm.moves + 37);
    svm.moves += 37;
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: Ready"));
    ++svm.moves;
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: Ready"));
    inspect_discard(obj);
    obj = inspect_item(UNIVERSAL_KEY);
    artifact_exists(obj, "The Silver Key", TRUE, ONAME_WISH);
    discover_artifact(obj->oartifact);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    inspect_discard(obj);
    obj = inspect_item(BRASS_LANTERN);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    inspect_discard(obj);
    svm.moves = saved_moves;
    step15b_window_setup();
    puts("PASS Step 21 invocation cooldown inspection, identity, expiry and fuel exclusions");
}

static void
step21_carried_artifact_tests(void)
{
    struct obj *lily, *crystal;
    inspect_window_setup();
    lily = inspect_item(CREDIT_CARD);
    artifact_exists(lily, "The Holographic Void Lily", TRUE, ONAME_WISH);
    assert(lily->oartifact);
    set_artifact_intrinsic(lily, TRUE, W_ART);
    assert(EReflecting & W_ART);
    assert(EHalf_spell_damage & W_ART);
    assert(EEnergy_regeneration & W_ART);
    assert(ureflects((const char *) 0, (const char *) 0));
    crystal = mksobj(CRYSTAL_BALL, FALSE, FALSE);
    artifact_exists(crystal, "The Earth Crystal", TRUE, ONAME_WISH);
    assert(crystal->oartifact);
    crystal = addinv(crystal);
    assert(EHalf_physical_damage & W_ART);
    assert(!mergable(crystal, lily));
    assert(!enhancement_eligible(lily) && !socket_capacity(lily));
    assert(!enhancement_eligible(crystal) && !socket_capacity(crystal));
    freeinv(crystal);
    assert(!(EHalf_physical_damage & W_ART));
    assert(EHalf_spell_damage & W_ART);
    obfree(crystal, (struct obj *) 0);
    inspect_discard(lily);
    assert(!(EReflecting & W_ART));
    assert(!(EHalf_spell_damage & W_ART));
    assert(!(EEnergy_regeneration & W_ART));
    step15b_window_setup();
    puts("PASS Step 21 Lily and Crystal carried defenses, reflection resolver, removal and Forge exclusion");
}

static void
step21_native_acquisition_tests(void)
{
    struct obj *obj, *duplicate;
    int i;
    const int types[] = { CREDIT_CARD, CRYSTAL_BALL, OILSKIN_CLOAK,
        RIN_FREE_ACTION, MASK, AMULET_OF_MAGICAL_BREATHING, MAGIC_WHISTLE };
    const int ids[] = { ART_HOLOGRAPHIC_VOID_LILY, ART_EARTH_CRYSTAL,
        ART_CLOAK_OF_THE_UNHELD_ONE, ART_RING_OF_P_HUL, ART_MASK_OF_TLALOC,
        ART_EHECAILACOCOZCATL, ART_STORM_WHISTLE };
    init_artifacts();
    for (i = 0; i < SIZE(types); ++i) {
        obj = mk_artifact(mksobj(types[i], FALSE, FALSE), A_NONE, 99, FALSE);
        assert(obj && obj->oartifact == ids[i]);
        duplicate = mk_artifact(mksobj(types[i], FALSE, FALSE), A_NONE, 99, FALSE);
        assert(duplicate && duplicate->oartifact != ids[i]);
        obfree(duplicate, (struct obj *) 0);
        obfree(obj, (struct obj *) 0);
        assert(exist_artifact(types[i], artiname(ids[i])));
    }
    puts("PASS Step 21 native random artifact acquisition and retained uniqueness after destruction");
}

static void
step21_equipment_tests(void)
{
    struct obj *obj, *other, *paper;
    inspect_window_setup();
    obj = inspect_item(OILSKIN_CLOAK);
    artifact_exists(obj, "The Cloak of the Unheld One", TRUE, ONAME_WISH);
    assert(obj->oartifact);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    assert(ESleep_resistance & W_ART);
    assert(!u.uprops[FREE_ACTION].extrinsic && !EAntimagic);
    setworn(obj, W_WEP);
    assert(!u.uprops[FREE_ACTION].extrinsic && !EAntimagic);
    setworn((struct obj *) 0, W_WEP);
    setworn(obj, W_ARMC);
    assert((u.uprops[FREE_ACTION].extrinsic & W_ARMC) && (EAntimagic & W_ARMC));
    assert(resists_magm(&gy.youmonst) && magic_negation(&gy.youmonst) == 3);
    setworn((struct obj *) 0, W_ARMC);
    assert((ESleep_resistance & W_ART) && !u.uprops[FREE_ACTION].extrinsic && !EAntimagic);
    inspect_discard(obj);
    obj = inspect_item(MASK);
    artifact_exists(obj, "The Mask of Tlaloc", TRUE, ONAME_WISH);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    assert((ECold_resistance & W_ART) && (EShock_resistance & W_ART)
           && (EAntimagic & W_ART) && !(EHalf_spell_damage & W_ART));
    assert(!artifact_waterproof());
    other = mksobj(CRYSTAL_BALL, FALSE, FALSE);
    artifact_exists(other, "The Orb of Detection", TRUE, ONAME_WISH);
    other = addinv(other);
    assert(EAntimagic & W_ART);
    freeinv(obj);
    assert(EAntimagic & W_ART);
    assert(!(ECold_resistance & W_ART) && !(EShock_resistance & W_ART));
    obj = addinv(obj);
    freeinv(other);
    obfree(other, (struct obj *) 0);
    assert(EAntimagic & W_ART);
    Blindf_on(obj);
    assert(!Blind && artifact_waterproof());
    paper = addinv(mksobj(SCR_IDENTIFY, FALSE, FALSE));
    assert(water_damage(paper, (const char *) 0, TRUE) == ER_NOTHING);
    assert(paper->otyp == SCR_IDENTIFY);
    Blindf_off(obj);
    assert(!artifact_waterproof() && (EAntimagic & W_ART));
    freeinv(paper); obfree(paper, (struct obj *) 0);
    inspect_discard(obj);
    obj = inspect_item(AMULET_OF_MAGICAL_BREATHING);
    artifact_exists(obj, "Ehecailacocozcatl", TRUE, ONAME_WISH);
    assert(!EReflecting && !EEnergy_regeneration);
    setworn(obj, W_WEP);
    assert(!EReflecting);
    setworn((struct obj *) 0, W_WEP);
    setworn(obj, W_AMUL);
    assert((EReflecting & W_AMUL) && EMagical_breathing
           && !EEnergy_regeneration && ureflects((const char *) 0, (const char *) 0));
    setworn((struct obj *) 0, W_AMUL);
    assert(!EReflecting && !EMagical_breathing);
    inspect_discard(obj);
    assert(!(ESleep_resistance & W_ART));
    obj = inspect_item(RIN_FREE_ACTION);
    artifact_exists(obj, "The Ring of P'hul", TRUE, ONAME_WISH);
    assert(obj->oartifact);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    assert(ESick_resistance & W_ART);
    assert(!u.uprops[FREE_ACTION].extrinsic && !EAntimagic);
    setworn(obj, W_RINGL);
    assert((u.uprops[FREE_ACTION].extrinsic & W_RINGL) && (EAntimagic & W_RINGL));
    setworn((struct obj *) 0, W_RINGL);
    assert(!u.uprops[FREE_ACTION].extrinsic && !EAntimagic && (ESick_resistance & W_ART));
    inspect_discard(obj);
    step15b_window_setup();
    puts("PASS Step 21 Cloak and Ring direct carry, correct worn slots, antimagic resolver, MC3 and source cleanup");
    puts("PASS Step 21 Tlaloc carried defense source orders, worn water protection, and Ehecailacocozcatl slot/reflection/breathing");
}

static void
step21_native_invocation_tests(void)
{
    struct obj *obj;
    struct monst *mon, *oldhead;
    aligntyp oldalign = u.ualign.type;
    long deadline;
    int count = 0;
    u.ualign.type = A_CHAOTIC;
    obj = inspect_item(CREDIT_CARD);
    artifact_exists(obj, "The Holographic Void Lily", TRUE, ONAME_WISH);
    obj->age = 0;
    oldhead = fmon;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME);
    for (mon = fmon; mon && mon != oldhead; mon = mon->nmon) {
        ++count;
        assert(!mon->mtame);
    }
    assert(count > 0 && obj->age > svm.moves);
    deadline = obj->age;
    oldhead = fmon;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME);
    assert(fmon == oldhead && obj->age > deadline);
    inspect_discard(obj);
    obj = inspect_item(CRYSTAL_BALL);
    artifact_exists(obj, "The Earth Crystal", TRUE, ONAME_WISH);
    obj->age = 0;
    for (mon = fmon; mon; mon = mon->nmon)
        mon->msleeping = 1;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME);
    assert(obj->age > svm.moves);
    for (mon = fmon; mon; mon = mon->nmon)
        assert(!mon->msleeping);
    inspect_discard(obj);
    u.ualign.type = oldalign;
    puts("PASS Step 21 native Lily summon/non-tame disposition/cooldown and Crystal earthquake/wakeup");
}
