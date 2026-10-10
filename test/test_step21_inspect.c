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

static void
step21_utility_tests(void)
{
    struct obj *obj;
    int capacity = weight_cap();
    obj = inspect_item(T_SHIRT);
    artifact_exists(obj, "The Tie-Dye Shirt of Shambhala", TRUE, ONAME_WISH);
    assert(obj->oartifact && weight_cap() == capacity);
    setworn(obj, W_ARMU);
    obj->blessed = 1; obj->cursed = 0;
    assert(weight_cap() == capacity * 150L / 100L);
    assert(weight_cap() == capacity * 150L / 100L);
    obj->blessed = 0;
    assert(weight_cap() == capacity * 125L / 100L);
    obj->cursed = 1;
    assert(weight_cap() == capacity * 75L / 100L);
    setworn((struct obj *) 0, W_ARMU);
    assert(weight_cap() == capacity);
    inspect_discard(obj);
    obj = inspect_item(AMULET_OF_LIFE_SAVING);
    artifact_exists(obj, "The Talisman of Buer", TRUE, ONAME_WISH);
    assert(obj->oartifact && !ERegeneration);
    setworn(obj, W_WEP);
    assert(!ERegeneration);
    setworn((struct obj *) 0, W_WEP);
    setworn(obj, W_AMUL);
    assert((ERegeneration & W_AMUL) && Lifesaved && Fixed_abil);
    setworn((struct obj *) 0, W_AMUL);
    assert(!ERegeneration && !Lifesaved && !Fixed_abil);
    inspect_discard(obj);
    puts("PASS Step 21 Shirt capacity BUC/rounding/noncompounding and Talisman exact-slot passives");
}

static int step21_menu_choice;
static int
step21_select(winid win, int how, MENU_ITEM_P **picked)
{
    (void) win;
    assert(how == PICK_ONE);
    *picked = (menu_item *) alloc(sizeof **picked);
    (*picked)->item.a_int = step21_menu_choice;
    (*picked)->count = 1;
    return 1;
}

static void
step21_read_and_supply_tests(void)
{
    static const short types[] = {
        MAGIC_WHISTLE, LEASH, SADDLE, TRIPE_RATION, APPLE, BANANA
    };
    struct obj *obj, *armor, *supply;
    int i, buc;
    long olddeaf = HDeaf;
    obj = inspect_item(T_SHIRT);
    artifact_exists(obj, "The Tie-Dye Shirt of Shambhala", TRUE, ONAME_WISH);
    obj->blessed = 1;
    setworn(obj, W_ARMU);
    armor = addinv(mksobj(PLATE_MAIL, FALSE, FALSE));
    setworn(armor, W_ARM);
    make_confused(30L, FALSE);
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doread() == ECMD_OK && HConfusion == 30L);
    setworn((struct obj *) 0, W_ARM);
    freeinv(armor); obfree(armor, NULL);
    armor = addinv(mksobj(CRYSTAL_PLATE_MAIL, FALSE, FALSE));
    setworn(armor, W_ARM);
    HDeaf = 42L;
    for (i = 0; i < 100 && HConfusion; ++i) {
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doread() == ECMD_TIME);
    }
    assert(!HConfusion && HDeaf == 42L);
    make_blinded(30L, FALSE);
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doread() == ECMD_TIME);
    make_blinded(0L, FALSE);
    HDeaf = olddeaf;
    setworn((struct obj *) 0, W_ARM);
    freeinv(armor); obfree(armor, NULL);
    setworn((struct obj *) 0, W_ARMU);
    inspect_discard(obj);

    obj = inspect_item(JACKET);
    artifact_exists(obj, "The Beastmaster's Duster", TRUE, ONAME_WISH);
    assert(obj->oartifact && !obj->owornmask);
    inspect_window_setup();
    windowprocs.win_select_menu = step21_select;
    for (buc = -1; buc <= 1; ++buc)
        for (i = 0; i < SIZE(types); ++i) {
            obj->blessed = buc > 0; obj->cursed = buc < 0;
            obj->bknown = 1; obj->age = 0;
            step21_menu_choice = i + 1;
            cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doinvoke() == ECMD_TIME);
            for (supply = gi.invent; supply && supply == obj;
                 supply = supply->nobj) ;
            assert(supply && supply->otyp == types[i] && supply->quan == 1);
            assert(supply->blessed == obj->blessed
                   && supply->cursed == obj->cursed && supply->bknown);
            assert(!supply->oartifact && obj->age > svm.moves);
            freeinv(supply); obfree(supply, NULL);
        }
    inspect_discard(obj);
    step15b_window_setup();
    puts("PASS Step 21 Shirt reading/opaque and transparent cover/blindness/no deafness cure; carried Duster six supplies in all beatitudes");
}

static void step21_clear(winid win) { (void) win; }
static void step21_cursor(winid win, int x, int y)
{ (void) win; (void) x; (void) y; }
static void step21_glyph(winid win, coordxy x, coordxy y,
                         const glyph_info *glyph, const glyph_info *bg)
{ (void) win; (void) x; (void) y; (void) glyph; (void) bg; }

static void
step21_light_tests(void)
{
    static const short types[] = { BRASS_LANTERN, MAGIC_CANDLE, AMBER };
    static const char *const names[] = {
        "The Brass Lantern of Zork", "The Candle of Eternal Flame",
        "The Glitterstone"
    };
    static const int radii[] = { 4, 3, 2 };
    struct obj *obj;
    int i;
    for (i = 0; i < SIZE(types); ++i) {
        obj = inspect_item(types[i]);
        artifact_exists(obj, names[i], TRUE, ONAME_WISH);
        assert(obj->oartifact && obj->lamplit && !obj->timed);
        assert(arti_light_radius(obj) == radii[i]);
        obj->age = svm.moves + 123;
        assert(!snuff_lit(obj) && obj->lamplit);
        assert(obj->age == svm.moves + 123);
        obj->blessed = 1;
        assert(arti_light_radius(obj) == radii[i]);
        obj->blessed = 0; obj->cursed = 1;
        assert(arti_light_radius(obj) == radii[i]);
        freeinv(obj);
        place_object(obj, u.ux, u.uy);
        assert(obj->lamplit && !obj->timed);
        obj_extract_self(obj);
        obj = addinv(obj);
        inspect_discard(obj);
    }
    puts("PASS Step 21 three permanent native lights, fixed radius across BUC, floor/drop lifecycle and cooldown/fuel independence");
}

static void
step21_pet_tests(void)
{
    struct monst *pet, *mon;
    struct obj *obj;
    coordxy x = 0, y = 0, xx, yy;
    struct prop oldsee = u.uprops[SEE_INVIS];
    int count;
    for (xx = 2; xx < COLNO - 1 && !x; ++xx)
        for (yy = 1; yy < ROWNO - 1; ++yy)
            if (dist2(xx, yy, u.ux, u.uy) > 25 && !m_at(xx, yy)
                && goodpos(xx, yy, &gy.youmonst, 0)) {
                x = xx; y = yy; break;
            }
    assert(x);
    pet = makemon(&mons[PM_LITTLE_DOG], x, y, MM_EDOG);
    assert(pet);
    initedog(pet, FALSE);
    pet->mtame = 19;
    pet->mstrategy = 0;
    pet->minvis = 1;
    u.uprops[SEE_INVIS].intrinsic = u.uprops[SEE_INVIS].extrinsic = 0L;
    assert(!canseemon(pet));
    inspect_window_setup();
    inspect_screen[0] = '\0';
    windowprocs.win_clear_nhwindow = step21_clear;
    windowprocs.win_curs = step21_cursor;
    windowprocs.win_print_glyph = step21_glyph;
    artifact_detect_pets();
    assert(pet->mtame == 19);
    u.uprops[SEE_INVIS] = oldsee;
    pet->minvis = 0;
    pet->msleeping = 1;
    obj = inspect_item(MAGIC_HARP);
    artifact_exists(obj, "The Lyre of Orpheus", TRUE, ONAME_WISH);
    assert(obj->oartifact && !EAntimagic);
    keepdogs(FALSE);
    assert(m_at(x, y) == pet); /* normal helplessness restriction */
    pet->msleeping = 0;
    freeinv(obj);
    keepdogs(FALSE);
    assert(m_at(x, y) == pet); /* no carried Lyre, no remote following */
    obj = addinv(obj);
    keepdogs(FALSE);
    assert(m_at(x, y) != pet && !pet->mx);
    count = 0;
    for (mon = gm.mydogs; mon; mon = mon->nmon)
        if (mon == pet) ++count;
    assert(count == 1);
    keepdogs(FALSE);
    count = 0;
    for (mon = gm.mydogs; mon; mon = mon->nmon)
        if (mon == pet) ++count;
    assert(count == 1);
    losedogs();
    assert(pet->mx && pet->mtame == 19);
    inspect_discard(obj);
    puts("PASS Step 21 nonvisible pet tameness unchanged; Lyre remote eligibility, sleep restriction, absent-source control and single native migration");
}

static void
step21_lantern_vision_tests(void)
{
    struct obj *obj, *box;
    struct monst *mon;
    struct prop saved[LAST_PROP + 1];
    coordxy x, y, mx = u.ux + 2, my = u.uy;
    boolean oldmk = gi.in_mklev;
    anything flash = cg.zeroany;
    memcpy(saved, u.uprops, sizeof saved);
    memset(u.uprops, 0, sizeof saved);
    gi.in_mklev = FALSE;
    for (x = u.ux - 5; x <= u.ux + 5; ++x)
        for (y = u.uy - 5; y <= u.uy + 5; ++y)
            if (isok(x, y)) {
                levl[x][y].typ = ROOM;
                levl[x][y].lit = 1;
                unblock_point(x, y);
            }
    if ((mon = m_at(mx, my)) != 0) mongone(mon);
    mon = makemon(&mons[PM_LITTLE_DOG], mx, my, NO_MM_FLAGS);
    assert(mon);
    mon->minvis = 1;
    vision_reset();
    vision_recalc(0);
    assert(!canseemon(mon));
    obj = inspect_item(BRASS_LANTERN);
    artifact_exists(obj, "The Brass Lantern of Zork", TRUE, ONAME_WISH);
    vision_recalc(0);
    assert(artifact_lantern_reveals(mx, my) && canseemon(mon));
    assert(glyph_is_monster(glyph_at(mx, my)));
    box = addinv(mksobj(SACK, FALSE, FALSE));
    freeinv(obj);
    add_to_container(box, obj);
    vision_recalc(0);
    assert(!artifact_lantern_reveals(mx, my) && !canseemon(mon));
    assert(!glyph_is_monster(glyph_at(mx, my)));
    obj_extract_self(obj);
    obj = addinv(obj);
    vision_recalc(0);
    assert(canseemon(mon));
    make_blinded(10L, FALSE);
    assert(!artifact_lantern_reveals(mx, my) && !canseemon(mon));
    make_blinded(0L, FALSE);
    /* A native camera source has no object pointer. */
    new_light_source(u.ux, u.uy, 1, LS_OBJECT, &flash);
    assert(artifact_lantern_reveals(mx, my));
    del_light_source(LS_OBJECT, &flash);
    freeinv(box); obfree(box, NULL);
    inspect_discard(obj);
    vision_recalc(0);
    assert(!canseemon(mon) && !glyph_is_monster(glyph_at(mx, my)));
    memcpy(u.uprops, saved, sizeof saved);
    gi.in_mklev = oldmk;
    puts("PASS Step 21 Lantern native lit-room visibility enter/exit, container occlusion, blindness, source destruction and camera null-source control");
}
