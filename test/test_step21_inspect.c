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
    int i, j;
    short typ, other;
    assert(ART_MIRRORED_MASK - ART_HOLOGRAPHIC_VOID_LILY + 1 == 22);
    for (i = ART_HOLOGRAPHIC_VOID_LILY; i <= ART_MIRRORED_MASK; ++i) {
        init_artifacts();
        typ = 0;
        assert(artifact_name(artiname(i), &typ, FALSE));
        /* Native history excludes competing identities, so the test does
           not depend on winning a rare roll among multiple MASK artifacts. */
        for (j = 1; j <= ART_MIRRORED_MASK; ++j) {
            if (j == i) continue;
            other = 0;
            assert(artifact_name(artiname(j), &other, FALSE));
            duplicate = mksobj(other, FALSE, FALSE);
            artifact_exists(duplicate, artiname(j), TRUE, ONAME_WISH);
            obfree(duplicate, NULL);
        }
        obj = mk_artifact(mksobj(typ, FALSE, FALSE), A_NONE, 99, FALSE);
        assert(obj && obj->oartifact == i);
        assert(!enhancement_eligible(obj) && !socket_capacity(obj));
        duplicate = mk_artifact(mksobj(typ, FALSE, FALSE), A_NONE, 99, FALSE);
        assert(duplicate && duplicate->oartifact != i && !mergable(obj, duplicate));
        obfree(duplicate, (struct obj *) 0);
        obfree(obj, (struct obj *) 0);
        assert(exist_artifact(typ, artiname(i)));
    }
    puts("PASS Step 21 all 22 native artifact acquisitions, Forge eligibility exclusion and retained uniqueness after destruction");
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

static void
step21_elven_ring_tests(void)
{
    short typ = 0;
    struct obj *obj;
    assert(artifact_name("Nenya", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "Nenya", TRUE, ONAME_WISH);
    assert(obj->oartifact && !EDetect_monsters && !EWwalking && !EEnergy_regeneration);
    setworn(obj, W_RINGL);
    assert((EDetect_monsters & W_RINGL) && (EWwalking & W_RINGL)
           && (EEnergy_regeneration & W_RINGL) && artifact_waterproof());
    setworn((struct obj *) 0, W_RINGL);
    assert(!EDetect_monsters && !EWwalking && !EEnergy_regeneration
           && !artifact_waterproof());
    inspect_discard(obj);
    assert(artifact_name("Vilya", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "Vilya", TRUE, ONAME_WISH);
    assert(obj->oartifact && !ERegeneration && !EShock_resistance);
    setworn(obj, W_RINGR);
    assert((ERegeneration & W_RINGR) && (EShock_resistance & W_RINGR)
           && (EWarning & W_RINGR) && (EStealth & W_RINGR)
           && (u.uprops[MAGICAL_BREATHING].extrinsic & W_RINGR));
    setworn((struct obj *) 0, W_RINGR);
    assert(!ERegeneration && !EShock_resistance && !EWarning && !EStealth
           && !u.uprops[MAGICAL_BREATHING].extrinsic);
    inspect_discard(obj);
    puts("PASS Step 21 Nenya/Vilya carried controls, worn properties and removal");
}

static void
step21_gravity_spoon_tests(void)
{
    short typ = 0;
    struct obj *obj, *missile;
    int hp, i;
    assert(artifact_name("The Lodestone of Gravity", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "The Lodestone of Gravity", TRUE, ONAME_WISH);
    missile = mksobj(DART, FALSE, FALSE);
    missile->opoisoned = 1;
    hp = u.uhp;
    for (i = 0; i < 100; ++i)
        assert(!thitu(1000, 20, &missile, (const char *) 0));
    assert(u.uhp == hp && missile && missile->opoisoned);
    obfree(missile, NULL);
    inspect_discard(obj);
    assert(artifact_name("The Iron Spoon of Liberation", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "The Iron Spoon of Liberation", TRUE, ONAME_WISH);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    assert(is_pick(obj) && confers_luck(obj) && (ESearching & W_ART)
           && (u.uprops[FREE_ACTION].extrinsic & W_ART));
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME);
    assert((HPasses_walls & TIMEOUT) >= 51 && (HPasses_walls & TIMEOUT) <= 150);
    assert(obj->age > svm.moves + (HPasses_walls & TIMEOUT));
    HPasses_walls = 0;
    inspect_discard(obj);
    assert(!ESearching && !u.uprops[FREE_ACTION].extrinsic);
    puts("PASS Step 21 100 poisoned missile deflections and Spoon carry/dig/luck/phasing duration/cooldown/removal");
}

static void
step21_saddle_tests(void)
{
    short typ = 0;
    struct obj *obj;
    struct monst *steed;
    assert(artifact_name("The Hellrider's Saddle", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "The Hellrider's Saddle", TRUE, ONAME_WISH);
    assert(obj->oartifact && !Antimagic && !Reflecting);
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME && Invis);
    freeinv(obj);
    assert(!Invis);
    steed = makemon(&mons[PM_PONY], u.ux, u.uy, MM_EDOG | MM_ADJACENTOK);
    assert(steed);
    initedog(steed, FALSE);
    (void) mpickobj(steed, obj);
    obj->owornmask = W_SADDLE;
    steed->misc_worn_check |= W_SADDLE;
    u.usteed = steed;
    assert(Antimagic && Reflecting && Invis);
    assert(resists_magm(steed) && mon_reflects(steed, NULL)
           && resists_magm(&gy.youmonst) && ureflects(NULL, NULL));
    cmdq_add_key(CQ_CANNED, '-');
    assert(doinvoke() == ECMD_TIME && !Invis && obj->age > svm.moves);
    u.usteed = NULL;
    assert(!Antimagic && !Reflecting && !resists_magm(steed)
           && !mon_reflects(steed, NULL));
    obj->owornmask = 0;
    steed->misc_worn_check &= ~W_SADDLE;
    obj_extract_self(obj);
    obj = addinv(obj);
    inspect_discard(obj);
    puts("PASS Step 21 Saddle dynamic rider/steed defenses, independent accessible invisibility and mounted invoke selector");
}

static void
step21_mirror_tests(void)
{
    short typ = 0;
    struct obj *obj;
    int hp = u.uhp;
    long cooldown;
    aligntyp oldalign = u.ualign.type;
    assert(artifact_name("The Mirrored Mask", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "The Mirrored Mask", TRUE, ONAME_WISH);
    assert(!Reflecting && !Half_spell_damage);
    setworn(obj, W_TOOL);
    assert(Reflecting && Half_spell_damage && ureflects(NULL, NULL));
    setnotworn(obj);
    assert(!Reflecting && !Half_spell_damage);
    inspect_discard(obj);
    assert(artifact_name("The Hand-Mirror of Cthylla", &typ, FALSE));
    u.ualign.type = A_CHAOTIC;
    obj = inspect_item(typ);
    artifact_exists(obj, "The Hand-Mirror of Cthylla", TRUE, ONAME_WISH);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    assert(Antimagic && Teleport_control && Searching);
    u.uhp = 1000;
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, '.');
    assert(use_mirror(obj) == ECMD_TIME);
    assert(u.uhp <= 985 && u.uhp >= 775 && obj->age > svm.moves);
    cooldown = obj->age;
    u.uhp = 1000;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_key(CQ_CANNED, '.');
    assert(doinvoke() == ECMD_TIME && u.uhp == 1000 && obj->age == cooldown);
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_key(CQ_CANNED, '.');
    assert(doinvoke() == ECMD_TIME && u.uhp <= 985 && obj->age > svm.moves);
    u.uhp = hp;
    u.ualign.type = oldalign;
    inspect_discard(obj);
    assert(!Antimagic && !Teleport_control && !Searching);
    puts("PASS Step 21 worn Mask source lifecycle and Mirror dual-command self damage/shared cooldown");
}

static void step21_status_init(void) { }
static void step21_status_field(int field, const char *name, const char *fmt, boolean on)
{ (void) field; (void) name; (void) fmt; (void) on; }
static void step21_status_update(int field, genericptr_t ptr, int chg,
                                 int percent, int color, unsigned long *mask)
{ (void) field; (void) ptr; (void) chg; (void) percent; (void) color; (void) mask; }

static void
step21_book_tests(void)
{
    short typ = 0;
    struct obj *obj;
    int page, i, oldpw = u.uen, oldhp = u.uhp, oldhunger = u.uhunger;
    struct spell known[MAXSPELL];
    windowprocs.win_status_init = step21_status_init;
    windowprocs.win_status_enablefield = step21_status_field;
    windowprocs.win_status_update = step21_status_update;
    if (!gb.blinit) status_initialize(FALSE);
    assert(artifact_name("The Book of Infinite Spells", &typ, FALSE));
    obj = inspect_item(typ);
    artifact_exists(obj, "The Book of Infinite Spells", TRUE, ONAME_WISH);
    assert(obj->oartifact_state >= SPE_DIG && obj->oartifact_state < SPE_BLANK_PAPER);
    page = obj->oartifact_state;
    fully_identify_obj(obj);
    inspect_run(obj);
    assert(obj->oartifact_state == page);
    inspect_window_setup();
    windowprocs.win_select_menu = step21_select;
    obj->age = 0; step21_menu_choice = 1;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME && obj->age > svm.moves);
    assert(obj->oartifact_state >= SPE_DIG && obj->oartifact_state < SPE_BLANK_PAPER);
    obj->oartifact_state = SPE_HEALING; /* deterministic supported-page fixture */
    memcpy(known, svs.spl_book, sizeof known);
    memset(svs.spl_book, 0, sizeof known);
    u.uen = 0; u.uhunger = 1000; u.uhp = u.uhpmax - 100;
    u.mith_timers[MITH_NAEN] = 1; /* native guaranteed casting chance */
    obj->age = 0; step21_menu_choice = 2;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_key(CQ_CANNED, '.');
    assert(doinvoke() == ECMD_TIME);
    assert(u.uen == 0 && u.uhp > u.uhpmax - 100);
    assert(obj->oartifact_state == SPE_HEALING && obj->age > svm.moves);
    for (i = 0; i < MAXSPELL; ++i) assert(!svs.spl_book[i].sp_id);
    obj->blessed = 1;
    obj->spestudied = 20; /* beyond ordinary book depletion */
    assert(study_book(obj) && go.occupation);
    for (i = 0; i < 1000 && (*go.occupation)(); ++i) ;
    assert(i < 1000 && svs.spl_book[0].sp_id == SPE_HEALING);
    assert(obj->otyp == SPE_BLANK_PAPER && obj->oartifact_state == SPE_HEALING
           && obj->spestudied == 20 && obj->where == OBJ_INVENT);
    go.occupation = NULL;
    HStun = 10;
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(!(doinvoke() & ECMD_TIME) && obj->age == 0);
    HStun = 0;
    memcpy(svs.spl_book, known, sizeof known);
    u.uen = oldpw; u.uhp = oldhp; u.uhunger = oldhunger;
    u.mith_timers[MITH_NAEN] = 0;
    inspect_discard(obj);
    step15b_window_setup();
    puts("PASS Step 21 Book supported initial/random pages, inspect stability, zero-Pw unmemorized native casting and shared cooldown");
}

static void
step21_mask_combat_tests(void)
{
    struct obj *mask, *lily;
    struct monst *caster;
    struct attack attack = { AT_MAGC, AD_FIRE, 1, 6 };
    int i, reflected = 0, hit = 0, hp = u.uhp, hpmax = u.uhpmax;
    struct prop saved_props[LAST_PROP + 1];
    memcpy(saved_props, u.uprops, sizeof saved_props);
    memset(u.uprops, 0, sizeof saved_props);
    u.uhpmax = 10000;
    inspect_window_setup();
    mask = inspect_item(MASK);
    artifact_exists(mask, "The Mirrored Mask", TRUE, ONAME_WISH);
    setworn(mask, W_TOOL);
    lily = mksobj(CREDIT_CARD, FALSE, FALSE);
    artifact_exists(lily, "The Holographic Void Lily", TRUE, ONAME_WISH);
    lily = addinv(lily);
    assert((EReflecting & (W_ART | W_TOOL)) == (W_ART | W_TOOL));
    assert((EHalf_spell_damage & (W_ART | W_TOOL)) == (W_ART | W_TOOL));
    caster = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK);
    assert(caster);
    caster->m_lev = 20; caster->mpeaceful = 0; caster->msleeping = 0;
    caster->mux = u.ux; caster->muy = u.uy;
    for (i = 0; i < 400; ++i) {
        int before;
        caster->mhp = caster->mhpmax = 10000; caster->mspec_used = 0;
        u.uhp = 10000;
        before = castmu(caster, &attack, TRUE, TRUE);
        if (before == M_ATTK_MISS) continue;
        if (caster->mhp < 10000) {
            ++reflected;
            assert(u.uhp == 10000); /* no duplicate hit */
        } else {
            ++hit;
            assert(u.uhp < 10000 && u.uhp >= 9967); /* 11d6, halved once */
        }
    }
    assert(reflected > 120 && reflected < 250 && hit > 120);
    setnotworn(mask);
    assert(Reflecting && Half_spell_damage);
    setworn(mask, W_TOOL);
    freeinv(lily); obfree(lily, NULL);
    assert(Reflecting && Half_spell_damage);
    setnotworn(mask);
    assert(!Reflecting && !Half_spell_damage);
    inspect_discard(mask);
    u.uhp = hp;
    memcpy(u.uprops, saved_props, sizeof saved_props);
    u.uhpmax = hpmax;
    mongone(caster);
    puts("PASS Step 21 Mask 400 native cast attempts, independent Lily source, single damage resolution and both removal orders");
}

static void
step21_persistence_tests(void)
{
    NHFILE *f;
    struct obj *obj;
    unsigned ids[22];
    int i, seen[22] = { 0 }, lights[22] = { 0 };
    short typ;
    light_source *ls;
    assert(!gi.invent);
    for (i = 0; i < 22; ++i) {
        typ = 0;
        assert(artifact_name(artiname(ART_HOLOGRAPHIC_VOID_LILY + i), &typ, FALSE));
        obj = mksobj(typ, TRUE, FALSE);
        artifact_exists(obj, artiname(ART_HOLOGRAPHIC_VOID_LILY + i), TRUE, ONAME_WISH);
        assert(obj->quan == 1);
        obj->age = svm.moves + 77;
        if (obj->oartifact == ART_BOOK_OF_INFINITE_SPELLS) obj->oartifact_state = SPE_HEALING;
        if (obj->oartifact == ART_HELLRIDER_S_SADDLE) obj->oartifact_state = 1;
        ids[i] = obj->o_id;
        place_object(obj, u.ux, u.uy);
    }
    f = get_freeing_nhfile();
    file_mode(f, WRITING | FREEING, open("step21-level.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    savelev(f, ledger_no(&u.uz)); close_nhfile(f);
    f = get_freeing_nhfile();
    file_mode(f, READING, open("step21-level.tmp", O_RDONLY | O_BINARY));
    getlev(f, 0, ledger_no(&u.uz)); close_nhfile(f);
    for (obj = fobj; obj; obj = obj->nobj) {
        for (i = 0; i < 22; ++i) if (obj->o_id == ids[i]) {
            ++seen[i];
            assert(obj->oartifact == ART_HOLOGRAPHIC_VOID_LILY + i && obj->quan == 1);
            assert(obj->age == svm.moves + 77 && obj->where == OBJ_FLOOR);
            if (obj->oartifact == ART_BOOK_OF_INFINITE_SPELLS) assert(obj->oartifact_state == SPE_HEALING);
            if (obj->oartifact == ART_HELLRIDER_S_SADDLE) assert(obj->oartifact_state == 1);
            if (artifact_permanent_light(obj)) assert(obj->lamplit && !obj->timed);
        }
    }
    for (ls = gl.light_base; ls; ls = ls->next) if (ls->type == LS_OBJECT) {
        for (i = 0; i < 22; ++i) if (ls->id.a_obj && ls->id.a_obj->o_id == ids[i]) {
            ++lights[i];
            assert(ls->range == arti_light_radius(ls->id.a_obj));
        }
    }
    for (i = 0; i < 22; ++i) {
        assert(seen[i] == 1);
        assert(lights[i] == (ART_HOLOGRAPHIC_VOID_LILY + i == ART_BRASS_LANTERN_OF_ZORK
                             || ART_HOLOGRAPHIC_VOID_LILY + i == ART_CANDLE_OF_ETERNAL_FLAME
                             || ART_HOLOGRAPHIC_VOID_LILY + i == ART_GLITTERSTONE));
    }
    puts("PASS Step 21 native level unload/reload all22 identities, singleton quantities, ages, Book page, Saddle toggle and exact light-source relinking");
}
