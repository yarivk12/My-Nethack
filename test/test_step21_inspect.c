/* Included after the native inspection harness, exercising the real command. */
extern void step21_forge_guard_tests(void);
extern int step21_test_reflected_spell(struct monst *, int, int);
#define MCASTU_ENUM
enum step21_spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM
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
step21_all_inspect_tests(void)
{
    struct obj *obj;
    short typ;
    int art;
    boolean timed;
    long moves = svm.moves;
    inspect_window_setup();
    for (art = ART_HOLOGRAPHIC_VOID_LILY; art <= ART_MIRRORED_MASK; ++art) {
        typ = 0; assert(artifact_name(artiname(art), &typ, FALSE));
        obj = inspect_item(typ); artifact_exists(obj, artiname(art), TRUE, ONAME_WISH);
        obj->age = svm.moves + 5;
        obj->known = obj->dknown = 0;
        inspect_run(obj);
        assert(!strstr(inspect_screen, "Cooldown:") && !strstr(inspect_screen, "Carried:")
               && !strstr(inspect_screen, "Stored spell:"));
        obj->known = obj->dknown = 1; discover_artifact(art);
        timed = art != ART_RING_OF_P_HUL && art != ART_MASK_OF_TLALOC
            && art != ART_BRASS_LANTERN_OF_ZORK && art != ART_NENYA
            && art != ART_VILYA && art != ART_LYRE_OF_ORPHEUS
            && art != ART_LODESTONE_OF_GRAVITY && art != ART_MIRRORED_MASK;
        inspect_run(obj);
        assert(timed ? strstr(inspect_screen, "Cooldown: 5 turns remaining") != NULL
                     : strstr(inspect_screen, "Cooldown:") == NULL);
        assert(obj->age == svm.moves + 5);
        svm.moves += 6;
        inspect_run(obj);
        assert(timed ? strstr(inspect_screen, "Cooldown: Ready") != NULL
                     : strstr(inspect_screen, "Cooldown:") == NULL);
        svm.moves = moves;
        inspect_discard(obj);
    }
    step15b_window_setup();
    puts("PASS Step 21 all22 unidentified/identified native inspection, explicit timed/untimed contract, positive countdown, expired Ready and read-only clocks");
}

static void
step21_ordinary_base_tests(void)
{
    struct prop saved[LAST_PROP + 1];
    struct obj *obj;
    short typ;
    int art, prop, native;
    long slot;
    memcpy(saved, u.uprops, sizeof saved);
    for (art = ART_HOLOGRAPHIC_VOID_LILY; art <= ART_MIRRORED_MASK; ++art) {
        memset(u.uprops, 0, sizeof saved);
        typ = 0; assert(artifact_name(artiname(art), &typ, FALSE));
        obj = inspect_item(typ); assert(!obj->oartifact);
        for (prop = 1; prop <= LAST_PROP; ++prop) assert(!u.uprops[prop].extrinsic);
        slot = obj->oclass == RING_CLASS ? W_RINGL
             : obj->oclass == AMULET_CLASS ? W_AMUL
             : typ == MASK ? W_TOOL : typ == T_SHIRT ? W_ARMU
             : typ == OILSKIN_CLOAK ? W_ARMC : typ == JACKET ? W_ARM : 0;
        native = objects[typ].oc_oprop;
        if (slot) setworn(obj, slot);
        for (prop = 1; prop <= LAST_PROP; ++prop)
            assert(u.uprops[prop].extrinsic == (slot && prop == native ? slot : 0));
        if (slot) setnotworn(obj);
        for (prop = 1; prop <= LAST_PROP; ++prop) assert(!u.uprops[prop].extrinsic);
        inspect_discard(obj);
    }
    memcpy(u.uprops, saved, sizeof saved);
    puts("PASS Step 21 every ordinary base has only its native equipment property, no artifact carry/slot leakage, clean removal and native inventory construction");
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
    boolean oldwiz = wizard;
    char wish[BUFSZ];
    assert(NUMMONS == 531 && NUM_OBJECTS == 569 && NROFARTIFACTS == 71);
    assert(AFTER_LAST_ARTIFACT == 72 && MAX_GLYPH == 13071 && NROFARTIFACTS <= 127);
    assert(ART_MIRRORED_MASK - ART_HOLOGRAPHIC_VOID_LILY + 1 == 22);
    for (i = ART_HOLOGRAPHIC_VOID_LILY; i <= ART_MIRRORED_MASK; ++i) {
        init_artifacts();
        wizard = TRUE;
        Snprintf(wish, sizeof wish, "uncursed %s", artiname(i));
        obj = readobjnam(wish, NULL);
        assert(obj && obj->oartifact == i && obj->quan == 1);
        obfree(obj, NULL);
        wizard = oldwiz;
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
    {
        extern int step21_create_count;
        extern boolean step21_create_neverask;
        int sample, maximum = 0;
        for (sample = 1; sample <= 100 && maximum < 10; ++sample) {
            obj->age = 0; oldhead = fmon;
            init_isaac64(21500UL + sample, rn2);
            cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doinvoke() == ECMD_TIME);
            assert(step21_create_count >= 1 && step21_create_count <= 10
                   && step21_create_neverask);
            maximum = max(maximum, step21_create_count);
            for (mon = fmon; mon && mon != oldhead; mon = mon->nmon)
                assert(!mon->mtame);
            for (mon = fmon; mon && mon != oldhead; ) {
                struct monst *next = mon->nmon;
                mongone(mon); mon = next;
            }
        }
        assert(maximum == 10);
        {
            struct monst *occupants[COLNO][ROWNO];
            struct monst blocked = cg.zeromonst;
            coordxy x, y;
            memcpy(occupants, svl.level.monsters, sizeof occupants);
            blocked.data = &mons[PM_GOBLIN]; blocked.mhp = 100;
            /* Fully occupied placement grid: use real native creation failure. */
            for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
                svl.level.monsters[x][y] = &blocked;
            oldhead = fmon; obj->age = 0;
            cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doinvoke() == ECMD_TIME && fmon == oldhead && obj->age > svm.moves);
            memcpy(svl.level.monsters, occupants, sizeof occupants);
        }
    }
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
        assert(DEADMONSTER(mon) || !mon->msleeping);
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
    pet->minvis = 0;
    if ((mon = m_at(u.ux + 1, u.uy)) != NULL && mon != pet) mongone(mon);
    remove_monster(pet->mx, pet->my);
    levl[u.ux + 1][u.uy].typ = ROOM;
    levl[u.ux + 1][u.uy].lit = 1;
    place_monster(pet, u.ux + 1, u.uy);
    make_blinded(0L, FALSE);
    vision_reset(); vision_recalc(0);
    assert(canseemon(pet));
    artifact_detect_pets();
    assert(pet->mtame == 20);
    artifact_detect_pets();
    assert(pet->mtame == 20);
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
    {
        struct trap *tr = maketrap(u.ux + 1, u.uy + 1, PIT);
        assert(tr); tr->tseen = 0;
        levl[u.ux][u.uy + 2].typ = SDOOR;
        block_point(u.ux, u.uy + 2);
        levl[u.ux + 5][u.uy].typ = SDOOR;
        block_point(u.ux + 5, u.uy);
        vision_recalc(0);
        artifact_lantern_detect();
        assert(tr->tseen && levl[u.ux][u.uy + 2].typ == DOOR);
        assert(levl[u.ux + 5][u.uy].typ == SDOOR);
        levl[u.ux + 1][u.uy].typ = STONE;
        block_point(u.ux + 1, u.uy);
        vision_recalc(0);
        assert(!artifact_lantern_reveals(mx, my) && !canseemon(mon));
        levl[u.ux + 1][u.uy].typ = ROOM;
        unblock_point(u.ux + 1, u.uy);
        vision_recalc(0);
        assert(canseemon(mon));
    }
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
    {
        const short physical[] = { ARROW, CROSSBOW_BOLT, DART, DAGGER, BOULDER };
        struct monst *thrower;
        struct trap *underfoot = t_at(u.ux, u.uy);
        int k, oldac = u.uac;
        long fumbling = HFumbling;
        coordxy xx = u.ux - 1, yy = u.uy;
        if (underfoot) deltrap(underfoot);
        u.utrap = 0;
        levl[u.ux][u.uy].typ = ROOM;
        if ((thrower = m_at(xx, yy)) != NULL) mongone(thrower);
        levl[xx][yy].typ = ROOM;
        thrower = makemon(&mons[PM_GOBLIN], xx, yy, NO_MINVENT);
        assert(thrower);
        u.uac = 100;
        HFumbling = FROMOUTSIDE; /* exercise deflection, not random hand-catching */
        for (k = 0; k < SIZE(physical); ++k) {
            missile = mksobj(physical[k], FALSE, FALSE);
            assert(!thitu(1000, 20, &missile, NULL) && u.uhp == hp);
            obfree(missile, NULL);
            missile = mksobj(physical[k], FALSE, FALSE);
            missile->quan = 1; missile->owt = weight(missile);
            (void) mpickobj(thrower, missile);
            m_throw(thrower, xx, yy, 1, 0, 1, missile);
            assert(u.uhp == hp && !thrower->minvent);
        }
        for (k = 0; k < 40; ++k) {
            struct trap *firing = maketrap(u.ux, u.uy, k & 1 ? ARROW_TRAP : DART_TRAP);
            assert(firing);
            dotrap(firing, FORCETRAP);
            assert(u.uhp == hp && !Sick);
            if (t_at(u.ux, u.uy)) deltrap(t_at(u.ux, u.uy));
        }
        {
            struct trap *rolling = maketrap(u.ux, u.uy, ROLLING_BOULDER_TRAP);
            struct obj *boulder, *floorobj;
            unsigned id;
            int present = 0;
            coordxy tx;
            assert(rolling);
            for (tx = u.ux - 2; tx <= u.ux + 2; ++tx) {
                levl[tx][u.uy].typ = ROOM;
                if (m_at(tx, u.uy)) mongone(m_at(tx, u.uy));
                if (tx != u.ux && t_at(tx, u.uy)) deltrap(t_at(tx, u.uy));
                while ((boulder = sobj_at(BOULDER, tx, u.uy)) != NULL) {
                    obj_extract_self(boulder); obfree(boulder, NULL);
                }
            }
            rolling->launch.x = u.ux - 2; rolling->launch.y = u.uy;
            rolling->launch2.x = u.ux + 2; rolling->launch2.y = u.uy;
            boulder = mksobj(BOULDER, FALSE, FALSE); id = boulder->o_id;
            place_object(boulder, rolling->launch.x, rolling->launch.y);
            dotrap(rolling, FORCETRAP);
            assert(u.uhp == hp);
            for (floorobj = fobj; floorobj; floorobj = floorobj->nobj)
                if (floorobj->o_id == id) { ++present; assert(floorobj->quan == 1); }
            assert(present == 1 && boulder->where == OBJ_FLOOR);
            obj_extract_self(boulder); obfree(boulder, NULL);
            deltrap(rolling);
        }
        missile = mksobj(ACID_VENOM, FALSE, FALSE);
        assert(thitu(1000, 20, &missile, NULL) && u.uhp < hp);
        if (missile) obfree(missile, NULL);
        u.uhp = hp; u.uac = oldac; HFumbling = fumbling;
        mongone(thrower);
    }
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
    {
        int low = 151, high = 0, duration;
        long oldage;
        obj->age = 0; oldage = HPasses_walls;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        (void) doinvoke();
        assert(HPasses_walls == oldage && obj->age == 0);
        for (i = 0; i < 1000; ++i) {
            obj->age = 0; HPasses_walls = 0;
            cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doinvoke() == ECMD_TIME);
            duration = HPasses_walls & TIMEOUT;
            low = min(low, duration); high = max(high, duration);
            assert(duration >= 51 && duration <= 150 && obj->age > svm.moves + duration);
        }
        assert(low == 51 && high == 150);
        obj->blessed = 1; obj->cursed = 0;
        assert(stone_luck(TRUE) == 1 && stone_luck(FALSE) == 1);
        obj->blessed = 0;
        assert(stone_luck(TRUE) == 1 && stone_luck(FALSE) == 0);
        obj->cursed = 1;
        assert(stone_luck(TRUE) == -1 && stone_luck(FALSE) == -1);
    }
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
    assert(mount_steed(steed, TRUE) && u.usteed == steed);
    assert(Antimagic && Reflecting && Invis);
    assert(resists_magm(steed) && mon_reflects(steed, NULL)
           && resists_magm(&gy.youmonst) && ureflects(NULL, NULL));
    {
        struct obj *destroyed_armor = NULL;
        int steedhp = steed->mhp, herohp = u.uhp;
        struct monst *caster = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy,
                                      NO_MINVENT | MM_ADJACENTOK);
        struct attack missile = { AT_MAGC, AD_MAGM, 5, 6 };
        int attempt;
        assert(caster);
        caster->mpeaceful = caster->msleeping = 0; caster->m_lev = 20;
        (void) zhitm(steed, BZ_M_SPELL(BZ_OFS_AD(AD_MAGM)), 5, &destroyed_armor);
        assert(steed->mhp == steedhp && u.uhp == herohp);
        for (attempt = 0; attempt < 20; ++attempt) {
            caster->mspec_used = 0;
            (void) castmu(caster, &missile, TRUE, TRUE);
            assert(u.uhp == herohp && steed->mhp == steedhp);
        }
        mongone(caster);
    }
    cmdq_add_key(CQ_CANNED, '-');
    assert(doinvoke() == ECMD_TIME && !Invis && obj->age > svm.moves);
    assert(newcham(steed, &mons[PM_HORSE], NO_NC_FLAGS));
    assert(u.usteed == steed && Antimagic && Reflecting && resists_magm(steed));
    HInvis = FROMOUTSIDE;
    dismount_steed(DISMOUNT_GENERIC);
    assert(!u.usteed && Invis);
    HInvis = 0;
    assert(!Antimagic && !Reflecting && !resists_magm(steed)
           && !mon_reflects(steed, NULL));
    obj->owornmask = 0;
    steed->misc_worn_check &= ~W_SADDLE;
    obj_extract_self(obj);
    obj = addinv(obj);
    freeinv(obj);
    (void) mpickobj(steed, obj);
    obj->owornmask = W_SADDLE; steed->misc_worn_check |= W_SADDLE;
    obj->oartifact_state = 1;
    u.utrap = 0; steed->mtrapped = 0;
    levl[u.ux][u.uy].typ = ROOM;
    levl[steed->mx][steed->my].typ = ROOM;
    assert(mount_steed(steed, TRUE) && Invis && Antimagic);
    HInvis = FROMOUTSIDE;
    monkilled(steed, "", AD_PHYS);
    assert(!u.usteed && !Antimagic && !Reflecting && Invis);
    HInvis = 0;
    assert(!Invis && !gi.invent);
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
    {
        struct monst *mon;
        coordxy x = u.ux + 2, y = u.uy;
        int mode;
        for (mode = 1; mode <= 2; ++mode) {
            levl[u.ux + mode][y].typ = ROOM;
            levl[u.ux + mode][y].lit = 1;
            unblock_point(u.ux + mode, y);
            if ((mon = m_at(u.ux + mode, y)) != NULL) mongone(mon);
        }
        mon = makemon(&mons[PM_GOBLIN], x, y, NO_MINVENT);
        assert(mon); mon->mhp = mon->mhpmax = 10000;
        mon->msleeping = 0; mon->mcansee = 1; mon->minvis = 0;
        vision_reset(); vision_recalc(0);
        obj->age = 0;
        cmdq_add_dir(CQ_CANNED, 1, 0, 0);
        assert(use_mirror(obj) == ECMD_TIME && obj->age > svm.moves);
        assert(mon->mhp >= 9775 && mon->mhp <= 9985 && mon->mflee);
        for (mode = 0; mode < 4; ++mode) {
            mon->mhp = 10000; mon->msleeping = mode == 0;
            mon->mcansee = mode != 1; mon->minvis = mode == 2;
            if (mode == 3) {
                levl[u.ux + 1][y].typ = STONE;
                block_point(u.ux + 1, y);
            }
            obj->age = 0;
            cmdq_add_dir(CQ_CANNED, 1, 0, 0);
            assert(use_mirror(obj) == ECMD_TIME && !obj->age && mon->mhp == 10000);
        }
        levl[u.ux + 1][y].typ = ROOM; unblock_point(u.ux + 1, y);
        mongone(mon);
        make_blinded(10L, FALSE); obj->age = 0;
        cmdq_add_key(CQ_CANNED, '.');
        assert(use_mirror(obj) == ECMD_TIME && !obj->age);
        make_blinded(0L, FALSE);
        cmdq_add_key(CQ_CANNED, '\033');
        assert(use_mirror(obj) == ECMD_CANCEL && !obj->age);
    }
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
    for (i = 0; i < 100; ++i) {
        obj->age = 0; cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doinvoke() == ECMD_TIME);
        assert(obj->oartifact_state >= SPE_DIG && obj->oartifact_state < SPE_BLANK_PAPER);
    }
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
    for (i = 0; i < 100; ++i) {
        u.uhunger = 1000; u.uen = 0; obj->age = 0;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_key(CQ_CANNED, '.');
        assert(doinvoke() == ECMD_TIME);
        assert(obj->oartifact_state == SPE_HEALING && u.uen == 0
               && obj->age > svm.moves && !svs.spl_book[0].sp_id);
    }
    obj->blessed = 1;
    obj->spestudied = 20; /* beyond ordinary book depletion */
    for (page = 0; page < 100; ++page) {
        memset(svs.spl_book, 0, sizeof known);
        assert(study_book(obj) && go.occupation);
        for (i = 0; i < 1000 && (*go.occupation)(); ++i) ;
        assert(i < 1000 && svs.spl_book[0].sp_id == SPE_HEALING);
        assert(obj->oartifact_state == SPE_HEALING && obj->spestudied == 20);
        go.occupation = NULL;
    }
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

static void step21_no_animation_delay(void) { }

static void
step21_mask_ray_tests(void)
{
    extern int step21_ray_checks, step21_ray_redirects, step21_ray_contacts;
    struct obj *mask, *lily, *amulet;
    struct monst *caster;
    struct prop saved[LAST_PROP + 1];
    int i, mode, successes = 0, failures = 0, rebounds = 0;
    int oldac = u.uac, hp = u.uhp, maxhp = u.uhpmax;
    coordxy x = u.ux, y = u.uy;
    void (*old_delay)(void) = windowprocs.win_delay_output;
    windowprocs.win_delay_output = step21_no_animation_delay;
    memcpy(saved, u.uprops, sizeof saved);
    memset(u.uprops, 0, sizeof saved);
    for (i = -3; i <= 3; ++i) {
        assert(isok(x + i, y));
        if (m_at(x + i, y)) mongone(m_at(x + i, y));
        levl[x + i][y].typ = ROOM;
        if (t_at(x + i, y)) deltrap(t_at(x + i, y));
    }
    levl[x - 2][y].typ = STONE; levl[x + 2][y].typ = STONE;
    u.utrap = 0; u.uac = 100; u.uhp = u.uhpmax = 10000;
    mask = inspect_item(MASK);
    artifact_exists(mask, "The Mirrored Mask", TRUE, ONAME_WISH);
    setworn(mask, W_TOOL);
    lily = mksobj(CREDIT_CARD, FALSE, FALSE);
    artifact_exists(lily, "The Holographic Void Lily", TRUE, ONAME_WISH);
    lily = addinv(lily);
    caster = makemon(&mons[PM_GOBLIN], x - 1, y, NO_MINVENT);
    assert(caster); caster->mpeaceful = 0; caster->msleeping = 0;
    for (mode = 0; mode < 6; ++mode) {
        amulet = NULL;
        if (mode == 4) {
            amulet = mksobj(AMULET_OF_REFLECTION, FALSE, FALSE);
            (void) mpickobj(caster, amulet);
            amulet->owornmask = W_AMUL; caster->misc_worn_check |= W_AMUL;
        }
        if (mode == 5) {
            /* Remove generic Reflection so only fire resistance protects it. */
            amulet = which_armor(caster, W_AMUL);
            assert(amulet);
            caster->misc_worn_check &= ~W_AMUL;
            amulet->owornmask = 0; obj_extract_self(amulet); obfree(amulet, NULL);
            assert(newcham(caster, &mons[PM_FIRE_ELEMENTAL], NO_NC_FLAGS));
            assert(resists_fire(caster));
        }
        for (i = 1; i <= 200; ++i) {
            int type = mode == 2 ? BZ_M_BREATH(BZ_OFS_AD(AD_FIRE))
                     : mode == 3 ? BZ_M_WAND(BZ_OFS_AD(AD_FIRE))
                     : BZ_M_SPELL(BZ_OFS_AD(AD_FIRE));
            caster->mhp = caster->mhpmax = 10000;
            init_isaac64(21000UL + i, rn2);
            gb.buzzer = caster;
            dobuzz(type, 1, caster->mx, caster->my, 1, 0,
                   FALSE, FALSE, FALSE, mode != 1);
            gb.buzzer = NULL;
            assert(u.uhp == 10000 && step21_ray_checks <= 1);
            if (mode > 0 && mode < 4) assert(!step21_ray_checks && !step21_ray_redirects);
            if (mode == 0 && step21_ray_checks) {
                if (step21_ray_redirects) ++successes; else ++failures;
                if (step21_ray_contacts > 1) ++rebounds;
            }
            if (mode >= 4) assert(caster->mhp == 10000);
        }
    }
    assert(successes > 50 && failures > 50 && rebounds > 0);
    mongone(caster);
    for (mode = 0; mode < 2; ++mode) {
        for (i = 1; i <= 100; ++i) {
            boolean redirected, dead;
            caster = makemon(&mons[PM_GOBLIN], x - 1, y, NO_MINVENT);
            assert(caster); caster->mpeaceful = caster->msleeping = 0;
            caster->mhp = caster->mhpmax = 100;
            if (mode) {
                amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
                (void) mpickobj(caster, amulet);
                amulet->owornmask = W_AMUL; caster->misc_worn_check |= W_AMUL;
            }
            init_isaac64(21000UL + i, rn2); gb.buzzer = caster;
            dobuzz(BZ_M_SPELL(BZ_OFS_WAN(WAN_DEATH)), 1, caster->mx, caster->my,
                   1, 0, FALSE, FALSE, FALSE, TRUE);
            gb.buzzer = NULL;
            redirected = step21_ray_redirects != 0; dead = DEADMONSTER(caster);
            assert(u.uhp == 10000);
            if (redirected) {
                assert(mode ? !dead && !which_armor(caster, W_AMUL) : dead);
            }
            if (!dead) mongone(caster);
            if (redirected) break;
        }
        assert(i <= 100);
    }
    freeinv(lily); obfree(lily, NULL);
    setnotworn(mask); inspect_discard(mask);
    levl[x - 2][y].typ = levl[x + 2][y].typ = ROOM;
    memcpy(u.uprops, saved, sizeof saved);
    u.uac = oldac; u.uhp = hp; u.uhpmax = maxhp;
    windowprocs.win_delay_output = old_delay;
    puts("PASS Step 21 seeded native Mask rays: both coin outcomes, Lily coexistence, bouncing contacts roll once, caster Reflection/resistance/death/lifesaving, incidental spells/breath/wand exclusions");
}

static void
step21_reflected_death_tests(void)
{
    struct monst *caster;
    struct obj *armor, *amulet;
    int result, lifesave;
    for (lifesave = 0; lifesave < 2; ++lifesave) {
        caster = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, NO_MINVENT | MM_ADJACENTOK);
        assert(caster);
        caster->mhp = caster->mhpmax = 100; caster->m_lev = 50;
        armor = mksobj(PLATE_MAIL, FALSE, FALSE);
        assert(enhancement_set(armor, OEP_DR_III, 0, FALSE));
        (void) mpickobj(caster, armor); armor->owornmask = W_ARM;
        caster->misc_worn_check |= W_ARM;
        if (lifesave) {
            amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
            (void) mpickobj(caster, amulet); amulet->owornmask = W_AMUL;
            caster->misc_worn_check |= W_AMUL;
        }
        result = step21_test_reflected_spell(caster, MCAST_DEATH_TOUCH, 1);
        if (lifesave) {
            assert(!DEADMONSTER(caster) && !(result & M_ATTK_AGR_DIED));
            assert(!which_armor(caster, W_AMUL));
            mongone(caster);
        } else {
            assert(DEADMONSTER(caster) && (result & M_ATTK_AGR_DIED));
        }
    }
    puts("PASS Step 21 reflected death bypasses ordinary DR but consumes native monster life saving");
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

static void
step21_consumption_tests(void)
{
    const char *names[] = { "The Glitterstone", "The Lodestone of Gravity" };
    struct obj *obj, *remainder, *pen, *paper;
    struct permonst *oldform = gy.youmonst.data;
    short typ;
    int i;
    for (i = 0; i < 2; ++i) {
        typ = 0; assert(artifact_name(names[i], &typ, FALSE));
        obj = inspect_item(typ);
        obj->quan = 7; obj->owt = weight(obj);
        artifact_exists(obj, names[i], TRUE, ONAME_WISH);
        assert(obj->quan == 1 && obj->oartifact);
        remainder = obj->nobj;
        assert(remainder && remainder->quan == 6 && !remainder->oartifact
               && remainder->where == OBJ_INVENT && !mergable(obj, remainder));
        freeinv(remainder); obfree(remainder, NULL);
        inspect_discard(obj);
        obj = mksobj(typ, TRUE, FALSE);
        obj->quan = 2;
        artifact_exists(obj, names[i], TRUE, ONAME_GIFT);
        assert(obj->quan == 1 && obj->oartifact);
        obfree(obj, NULL);
    }
    obj = inspect_item(SPE_BLANK_PAPER);
    artifact_exists(obj, "The Book of Infinite Spells", TRUE, ONAME_WISH);
    obj->oartifact_state = SPE_HEALING; obj->age = svm.moves + 100;
    gy.youmonst.data = &mons[PM_FIRE_ELEMENTAL];
    assert(!is_edible(obj));
    gy.youmonst.data = oldform;
    pen = addinv(mksobj(MAGIC_MARKER, TRUE, FALSE));
    paper = addinv(mksobj(SPE_BLANK_PAPER, FALSE, FALSE));
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_key(CQ_CANNED, '\033');
    (void) dowrite(pen);
    cmdq_clear(CQ_CANNED);
    assert(obj->where == OBJ_INVENT && obj->oartifact_state == SPE_HEALING
           && obj->age == svm.moves + 100 && obj->oartifact == ART_BOOK_OF_INFINITE_SPELLS);
    freeinv(pen); obfree(pen, NULL);
    freeinv(paper); obfree(paper, NULL);
    inspect_discard(obj);
    obj = inspect_item(PICK_AXE);
    artifact_exists(obj, "The Iron Spoon of Liberation", TRUE, ONAME_WISH);
    gy.youmonst.data = &mons[PM_RUST_MONSTER];
    assert(!is_edible(obj));
    gy.youmonst.data = oldform;
    inspect_discard(obj);
    puts("PASS Step 21 artifact stack conversion preserves ordinary remainder; initialized gems remain singleton; marker/eating consumption excluded");
}

static void
step21_phasing_expiry_tests(void)
{
    struct prop saved[LAST_PROP + 1];
    struct permonst *oldform = gy.youmonst.data;
    coordxy x, y, oldx = u.ux, oldy = u.uy;
    memcpy(saved, u.uprops, sizeof saved);
    memset(u.uprops, 0, sizeof saved);
    for (x = oldx - 2; x <= oldx + 2; ++x)
        for (y = oldy - 2; y <= oldy + 2; ++y) {
            levl[x][y].typ = ROOM;
            levl[x][y].wall_info = 0;
            unblock_point(x, y);
        }
    levl[oldx][oldy].typ = STONE;
    HPasses_walls = 1;
    nh_timeout();
    assert(!Passes_walls && !IS_OBSTRUCTED(levl[u.ux][u.uy].typ));
    assert(u.ux != oldx || u.uy != oldy);
    levl[oldx][oldy].typ = ROOM;
    levl[oldx + 2][oldy].typ = STONE;
    levl[oldx + 2][oldy].wall_info = W_NONPASSWALL;
    HPasses_walls = 100;
    assert(!may_passwall(oldx + 2, oldy));
    levl[oldx + 2][oldy].wall_info = 0;
    assert(may_passwall(oldx + 2, oldy));
    levl[u.ux + 1][u.uy].typ = STONE;
    levl[u.ux + 1][u.uy].wall_info = W_NONPASSWALL;
    assert(!test_move(u.ux, u.uy, 1, 0, TEST_MOVE));
    levl[u.ux + 1][u.uy].wall_info = 0;
    assert(test_move(u.ux, u.uy, 1, 0, TEST_MOVE));
    levl[u.ux + 1][u.uy].typ = ROOM;
    oldx = u.ux; oldy = u.uy;
    levl[oldx][oldy].typ = DOOR;
    levl[oldx][oldy].doormask = D_CLOSED;
    gy.youmonst.data = &mons[PM_ACID_BLOB];
    HPasses_walls = 1;
    nh_timeout();
    assert(!Passes_walls && u.ux == oldx && u.uy == oldy);
    gy.youmonst.data = oldform;
    levl[oldx][oldy].typ = ROOM;
    HPasses_walls = FROMOUTSIDE | 1;
    nh_timeout();
    assert(Passes_walls && !(HPasses_walls & TIMEOUT));
    {
        dest_area down = svd.dndest, up = svu.updest;
        struct rm terrain[COLNO][ROWNO];
        memcpy(terrain, levl, sizeof terrain);
        memset(&svd.dndest, 0, sizeof svd.dndest);
        memset(&svu.updest, 0, sizeof svu.updest);
        /* Constrain all legal exits to this small native teleport region. */
        svd.dndest.nlx = oldx - 1; svd.dndest.nhx = oldx + 1;
        svd.dndest.nly = oldy - 1; svd.dndest.nhy = oldy + 1;
        for (x = oldx - 1; x <= oldx + 1; ++x)
            for (y = oldy - 1; y <= oldy + 1; ++y) {
                levl[x][y].typ = STONE;
                if (m_at(x, y)) mongone(m_at(x, y));
                if (t_at(x, y)) deltrap(t_at(x, y));
            }
        levl[oldx - 1][oldy].typ = LAVAPOOL;
        levl[oldx + 1][oldy].typ = POOL;
        levl[oldx][oldy - 1].typ = ROOM;
        assert(maketrap(oldx, oldy - 1, SPIKED_PIT));
        HPasses_walls = 1;
        nh_timeout();
        assert(HPasses_walls == 1 && u.ux == oldx && u.uy == oldy);
        /* An ordinary floor outside the restricted region was not used. */
        levl[oldx][oldy + 1].typ = ROOM;
        nh_timeout();
        assert(!Passes_walls && u.ux == oldx && u.uy == oldy + 1);
        deltrap(t_at(oldx, oldy - 1));
        memcpy(levl, terrain, sizeof terrain);
        svd.dndest = down; svu.updest = up;
    }
    memcpy(u.uprops, saved, sizeof saved);
    puts("PASS Step 21 native timeout rock exit, protected walls, amorphous doors, independent source, restricted-region/hazard rejection and no-exit one-turn extension");
}

static void
step21_summon_heal_charge_tests(void)
{
    const char *summoners[] = { "Ehecailacocozcatl", "The Candle of Eternal Flame", "The Storm Whistle" };
    const int species[] = { PM_AIR_ELEMENTAL, PM_FIRE_ELEMENTAL, PM_WATER_ELEMENTAL };
    struct obj *obj, *target;
    struct monst *head, *mon;
    aligntyp oldalign = u.ualign.type;
    short typ;
    int i, buc, count, hp = u.uhp, hpmax = u.uhpmax, con = ABASE(A_CON);
    struct prop props[LAST_PROP + 1];
    memcpy(props, u.uprops, sizeof props);
    memset(u.uprops, 0, sizeof props);
    u.ualign.type = A_LAWFUL;
    for (i = 0; i < 3; ++i) {
        typ = 0; assert(artifact_name(summoners[i], &typ, FALSE));
        obj = inspect_item(typ);
        artifact_exists(obj, summoners[i], TRUE, ONAME_WISH);
        head = fmon;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doinvoke() == ECMD_TIME && obj->age > svm.moves);
        count = 0;
        for (mon = fmon; mon && mon != head; mon = mon->nmon) {
            ++count;
            assert(mon->data == &mons[species[i]] && mon->mtame && has_edog(mon));
        }
        assert(count == 1);
        inspect_discard(obj);
    }
    obj = inspect_item(AMULET_OF_LIFE_SAVING);
    artifact_exists(obj, "The Talisman of Buer", TRUE, ONAME_WISH);
    u.uhpmax = 1000; u.uhp = 800;
    make_sick(20L, "a test illness", FALSE, SICK_ALL);
    make_slimed(20L, NULL);
    make_blinded(20L, FALSE);
    cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME);
    assert(u.uhp == 900 && !Sick && !Slimed && !BlindedTimeout);
    setworn(obj, W_AMUL);
    assert(Lifesaved && Regeneration && Fixed_abil);
    u.umonnum = PM_TROLL; u.mhmax = 100; u.mh = 1; u.mtimedone = 100;
    set_uasmon();
    obj->age = 0; cmdq_add_key(CQ_CANNED, obj->invlet);
    assert(doinvoke() == ECMD_TIME && u.mh == 51 && u.uhp == 900);
    losehp(1000, "a test polymorph injury", KILLED_BY);
    assert(!Upolyd && uamul == obj && Lifesaved); /* normal rehumanization */
    u.uhp = 0;
    done(DIED);
    assert(u.uhp > 0 && !uamul && !gi.invent && !Lifesaved && !ERegeneration && !Fixed_abil);
    assert(exist_artifact(AMULET_OF_LIFE_SAVING, "The Talisman of Buer"));
    ABASE(A_CON) = con;
    u.uhpmax = hpmax; u.uhp = hp;
    u.ualign.type = A_NEUTRAL;
    obj = inspect_item(AMBER);
    artifact_exists(obj, "The Glitterstone", TRUE, ONAME_WISH);
    for (buc = -1; buc <= 1; ++buc) {
        obj->cursed = buc < 0; obj->blessed = buc > 0; obj->age = 0;
        target = addinv(mksobj(WAN_LIGHT, FALSE, FALSE));
        target->spe = 0; target->recharged = 0;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_key(CQ_CANNED, target->invlet);
        assert(doinvoke() == ECMD_TIME && obj->age > svm.moves);
        assert(target->recharged == 1);
        assert(buc < 0 ? target->spe == 0
               : buc > 0 ? (target->spe >= 11 && target->spe <= 15)
                         : (target->spe >= 1 && target->spe <= 15));
        freeinv(target); obfree(target, NULL);
    }
    {
        int role = gu.urole.mnum, race = gu.urace.mnum;
        int variant, baseline[3];
        for (variant = 0; variant < 4; ++variant) {
            gu.urole.mnum = variant & 1 ? PM_WIZARD : PM_ROGUE;
            gu.urace.mnum = variant & 2 ? PM_GNOME : PM_HUMAN;
            for (buc = -1; buc <= 1; ++buc) {
                obj->cursed = buc < 0; obj->blessed = buc > 0; obj->age = 0;
                target = addinv(mksobj(WAN_LIGHT, FALSE, FALSE));
                target->spe = 0; target->recharged = 0;
                cmdq_add_key(CQ_CANNED, obj->invlet);
                cmdq_add_key(CQ_CANNED, target->invlet);
                init_isaac64(21300UL, rn2);
                assert(doinvoke() == ECMD_TIME && target->recharged == 1);
                if (!variant) baseline[buc + 1] = target->spe;
                else assert(target->spe == baseline[buc + 1]);
                freeinv(target); obfree(target, NULL);
            }
        }
        gu.urole.mnum = role; gu.urace.mnum = race;
        target = addinv(mksobj(WAN_LIGHT, FALSE, FALSE));
        target->spe = 0; target->recharged = 7;
        obj->age = 0; u.uhp = u.uhpmax = 1000;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_key(CQ_CANNED, target->invlet);
        assert(doinvoke() == ECMD_TIME && gi.invent == obj && !obj->nobj);
        assert(u.uhp < 1000); /* native repeated-recharge explosion consumed target */
        u.uhp = hp; u.uhpmax = hpmax;
    }
    obj->age = 0;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_key(CQ_CANNED, '\033');
    assert(doinvoke() == ECMD_CANCEL && obj->age == 0);
    inspect_discard(obj);
    u.ualign.type = oldalign;
    memcpy(u.uprops, props, sizeof props);
    puts("PASS Step 21 all three single tame elementals, Buer healing/cures/polymorph rehumanization/consumptive lifesaving, Glitterstone BUC role/race equivalence, recharge explosion and cancellation");
}

extern void step21_test_regeneration(int);
static char step21_water_yn(const char *prompt, const char *choices, char def)
{ (void) prompt; (void) choices; (void) def; assert(0 && "unexpected water fixture prompt"); return 'n'; }
static void
step21_water_and_oilskin_tests(void)
{
    const short types[] = { RIN_WATER_WALKING, RIN_REGENERATION, AMULET_OF_MAGICAL_BREATHING };
    const char *names[] = { "Nenya", "Vilya", "Ehecailacocozcatl" };
    struct prop saved[LAST_PROP + 1];
    struct obj *obj;
    struct monst *wrapper;
    struct attack wrap = { AT_TUCH, AD_WRAP, 1, 6 };
    coordxy x = u.ux, y = u.uy, xx, yy;
    int i, tick, hp = u.uhp, hpmax = u.uhpmax, ordinary;
    boolean moved = u.umoved;
    memcpy(saved, u.uprops, sizeof saved);
    memset(u.uprops, 0, sizeof saved);
    windowprocs.win_yn_function = step21_water_yn;
    for (i = 0; i < 3; ++i) {
        u.ux = x; u.uy = y; u.utrap = 0; gm.multi = 0; set_uinwater(0);
        for (xx = x - 1; xx <= x + 1; ++xx)
            for (yy = y - 1; yy <= y + 1; ++yy) {
                levl[xx][yy].typ = ROOM;
                if (m_at(xx, yy)) mongone(m_at(xx, yy));
                if (t_at(xx, yy)) deltrap(t_at(xx, yy));
            }
        obj = inspect_item(types[i]); artifact_exists(obj, names[i], TRUE, ONAME_WISH);
        setworn(obj, i == 2 ? W_AMUL : W_RINGL);
        if (i == 1) {
            u.uhp = 100; u.uhpmax = 1000; u.umoved = TRUE;
            for (tick = 0; tick < 20; ++tick) {
                int before = u.uhp;
                step21_test_regeneration(EXT_ENCUMBER);
                assert(u.uhp > before);
            }
        }
        levl[x][y].typ = POOL;
        assert(!pooleffects(TRUE));
        assert(u.ux == x && u.uy == y);
        assert(i == 0 ? !u.uinwater && Wwalking : u.uinwater && Breathless);
        gm.multi = 0;
        if (i == 2) Amulet_off(); else Ring_off(obj);
        assert(!Wwalking && !Breathless);
        assert(!is_pool(u.ux, u.uy) && !u.uinwater);
        if (i == 1) {
            int before = u.uhp;
            step21_test_regeneration(EXT_ENCUMBER);
            assert(u.uhp == before);
        }
        inspect_discard(obj);
        levl[x][y].typ = ROOM;
    }
    wrapper = makemon(&mons[PM_GIANT_EEL], u.ux + 1, u.uy, NO_MINVENT | MM_ADJACENTOK);
    assert(wrapper);
    for (ordinary = 0; ordinary < 2; ++ordinary) {
        obj = inspect_item(OILSKIN_CLOAK);
        if (!ordinary) artifact_exists(obj, "The Cloak of the Unheld One", TRUE, ONAME_WISH);
        obj->cursed = 0; setworn(obj, W_ARMC);
        assert(u_slip_free(wrapper, &wrap));
        setnotworn(obj); assert(!u_slip_free(wrapper, &wrap));
        inspect_discard(obj);
    }
    mongone(wrapper);
    u.uhp = hp; u.uhpmax = hpmax; u.umoved = moved;
    memcpy(u.uprops, saved, sizeof saved);
    step15b_window_setup();
    puts("PASS Step 21 Nenya water walking, Vilya/amulet underwater breathing, native pool escape on removal, Vilya injured HP ticks and removal, Cloak/ordinary oilskin wrapping protection");
}

static void
step21_source_water_regen_tests(void)
{
    struct prop saved[LAST_PROP + 1];
    struct obj *obj, *paper, *bag, *floorpaper;
    short typ;
    int art, prop, i, before, pw = u.uen, pwmax = u.uenmax;
    boolean seeded[LAST_PROP + 1];
    long slot, oldmoves = svm.moves;
    memcpy(saved, u.uprops, sizeof saved);
    for (art = ART_HOLOGRAPHIC_VOID_LILY; art <= ART_MIRRORED_MASK; ++art) {
        memset(u.uprops, 0, sizeof saved);
        typ = 0; assert(artifact_name(artiname(art), &typ, FALSE));
        obj = inspect_item(typ);
        artifact_exists(obj, artiname(art), TRUE, ONAME_WISH);
        set_artifact_intrinsic(obj, TRUE, W_ART);
        /* Every direct-carried source must stop while inside a carried bag. */
        freeinv(obj);
        bag = addinv(mksobj(SACK, FALSE, FALSE));
        add_to_container(bag, obj);
        for (prop = 1; prop <= LAST_PROP; ++prop) assert(!u.uprops[prop].extrinsic);
        obj_extract_self(obj); obj = addinv(obj);
        freeinv(bag); obfree(bag, NULL);
        slot = obj->oclass == RING_CLASS ? W_RINGL
             : obj->oclass == AMULET_CLASS ? W_AMUL
             : typ == MASK ? W_TOOL : typ == T_SHIRT ? W_ARMU
             : typ == OILSKIN_CLOAK ? W_ARMC : typ == JACKET ? W_ARM : 0;
        if (slot) setworn(obj, slot);
        memset(seeded, 0, sizeof seeded);
        for (prop = 1; prop <= LAST_PROP; ++prop)
            if (u.uprops[prop].extrinsic) {
                long granted = u.uprops[prop].extrinsic;
                seeded[prop] = TRUE;
                u.uprops[prop].intrinsic = FROMOUTSIDE;
                u.uprops[prop].intrinsic = 0;
                assert(u.uprops[prop].extrinsic == granted);
                u.uprops[prop].intrinsic = FROMOUTSIDE;
            }
        if (slot) setnotworn(obj);
        inspect_discard(obj);
        for (prop = 1; prop <= LAST_PROP; ++prop) {
            assert(!u.uprops[prop].extrinsic);
            assert(u.uprops[prop].intrinsic == (seeded[prop] ? FROMOUTSIDE : 0));
        }
    }
    memset(u.uprops, 0, sizeof saved);
    typ = 0; assert(artifact_name("Nenya", &typ, FALSE));
    obj = inspect_item(typ); artifact_exists(obj, "Nenya", TRUE, ONAME_WISH);
    setworn(obj, W_RINGL);
    paper = addinv(mksobj(SCR_IDENTIFY, FALSE, FALSE));
    bag = addinv(mksobj(SACK, FALSE, FALSE));
    floorpaper = mksobj(SCR_IDENTIFY, FALSE, FALSE);
    add_to_container(bag, floorpaper);
    assert(water_damage(paper, NULL, TRUE) == ER_NOTHING && paper->otyp == SCR_IDENTIFY);
    assert(water_damage(bag, NULL, TRUE) == ER_NOTHING && floorpaper->otyp == SCR_IDENTIFY);
    floorpaper = mksobj(SCR_IDENTIFY, FALSE, FALSE);
    place_object(floorpaper, u.ux, u.uy);
    (void) water_damage(floorpaper, NULL, TRUE);
    assert(floorpaper->otyp == SCR_BLANK_PAPER);
    obj_extract_self(floorpaper); obfree(floorpaper, NULL);
    assert(Wwalking && !EMagical_breathing && !Half_spell_damage);
    setnotworn(obj);
    (void) water_damage(paper, NULL, TRUE);
    assert(paper->otyp == SCR_BLANK_PAPER);
    freeinv(paper); obfree(paper, NULL); freeinv(bag); obfree(bag, NULL);
    inspect_discard(obj);
    obj = inspect_item(CREDIT_CARD);
    artifact_exists(obj, "The Holographic Void Lily", TRUE, ONAME_WISH);
    set_artifact_intrinsic(obj, TRUE, W_ART);
    u.uen = 0; u.uenmax = 10000;
    for (i = 0; i < 100; ++i) {
        before = u.uen; ++svm.moves; step21_test_regeneration(EXT_ENCUMBER);
        assert(u.uen > before);
    }
    HEnergy_regeneration = FROMOUTSIDE;
    inspect_discard(obj);
    before = u.uen; step21_test_regeneration(EXT_ENCUMBER); assert(u.uen > before);
    HEnergy_regeneration = 0;
    before = u.uen; step21_test_regeneration(EXT_ENCUMBER); assert(u.uen == before);
    u.uen = pw; u.uenmax = pwmax; svm.moves = oldmoves;
    memcpy(u.uprops, saved, sizeof saved);
    puts("PASS Step 21 all22 independent intrinsic source preservation and complete extrinsic removal; Nenya direct/nested/floor water scope; Lily 100-turn native fast Pw regeneration and independent source");
}

static char step21_yes(const char *prompt, const char *choices, char def)
{ (void) prompt; (void) choices; (void) def; return 'y'; }

static void
step21_combined_source_tests(void)
{
    const char *names[] = { "The Silver Key", "Nenya", "The Glitterstone", "The Holographic Void Lily" };
    struct obj *items[4], *crystal, *armor;
    struct prop saved[LAST_PROP + 1];
    short typ;
    int order, i, j, before, pw = u.uen, maxpw = u.uenmax, hp = u.uhp;
    memcpy(saved, u.uprops, sizeof saved);
    for (order = 0; order < 2; ++order) {
        memset(u.uprops, 0, sizeof saved);
        for (i = 0; i < 4; ++i) {
            typ = 0; assert(artifact_name(names[i], &typ, FALSE));
            items[i] = mksobj(typ, FALSE, FALSE);
            artifact_exists(items[i], names[i], TRUE, ONAME_WISH);
            items[i] = addinv(items[i]);
        }
        setworn(items[1], W_RINGL);
        u.uenmax = 10000; u.uen = 0;
        for (i = 0; i < 4; ++i) {
            assert(Energy_regeneration);
            before = u.uen; step21_test_regeneration(EXT_ENCUMBER);
            assert(u.uen > before && u.uen - before <= (ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1);
            j = order ? 3 - i : i;
            if (j == 1) setnotworn(items[j]);
            freeinv(items[j]); obfree(items[j], NULL);
        }
        assert(!Energy_regeneration);
    }
    memset(u.uprops, 0, sizeof saved);
    crystal = inspect_item(CRYSTAL_BALL);
    artifact_exists(crystal, "The Earth Crystal", TRUE, ONAME_WISH);
    set_artifact_intrinsic(crystal, TRUE, W_ART);
    armor = addinv(mksobj(PLATE_MAIL, FALSE, FALSE));
    assert(enhancement_set(armor, OEP_DR_III, 0, FALSE));
    setworn(armor, W_ARM);
    u.uhp = 500; losehp_physical(Maybe_Half_Phys(100), "test damage", KILLED_BY);
    assert(u.uhp == 465); /* native half, then 30% DR, one resolution */
    u.uhp = 500; losehp(100, "test magic", KILLED_BY);
    assert(u.uhp == 430); /* Earth Crystal does not halve magic */
    HHalf_physical_damage = FROMOUTSIDE;
    freeinv(crystal); obfree(crystal, NULL);
    assert(Half_physical_damage);
    setnotworn(armor); freeinv(armor); obfree(armor, NULL);
    u.uhp = 500; losehp_physical(Maybe_Half_Phys(100), "test damage", KILLED_BY);
    assert(u.uhp == 450);
    u.uhp = hp; u.uen = pw; u.uenmax = maxpw;
    memcpy(u.uprops, saved, sizeof saved);
    puts("PASS Step 21 Silver Key/Nenya/Glitterstone/Lily simultaneous nonstacking native regeneration and both source-removal orders; Earth Crystal plus DR exact physical/magic pipeline");
}

static void
step21_base_command_tests(void)
{
    const short types[] = { BRASS_LANTERN, MAGIC_CANDLE, MAGIC_WHISTLE, MAGIC_HARP };
    const char *names[] = { "The Brass Lantern of Zork", "The Candle of Eternal Flame",
                          "The Storm Whistle", "The Lyre of Orpheus" };
    struct obj *obj;
    struct monst *pet;
    int i, ordinary;
    aligntyp oldalign = u.ualign.type;
    windowprocs.win_yn_function = step21_yes;
    for (ordinary = 0; ordinary < 2; ++ordinary) for (i = 0; i < 4; ++i) {
        obj = inspect_item(types[i]);
        if (!ordinary) artifact_exists(obj, names[i], TRUE, ONAME_WISH);
        obj->age = 1000; obj->spe = 3; obj->cursed = 0;
        u.ualign.type = i == 0 || i == 3 ? A_NEUTRAL : A_LAWFUL;
        pet = NULL;
        if (i == 2) {
            coordxy x = u.ux < COLNO / 2 ? COLNO - 3 : 2, y = 10;
            if (m_at(x, y)) mongone(m_at(x, y));
            levl[x][y].typ = ROOM;
            pet = makemon(&mons[PM_LITTLE_DOG], x, y, MM_EDOG);
            assert(pet); initedog(pet, FALSE); pet->msleeping = 0;
            assert(dist2(pet->mx, pet->my, u.ux, u.uy) > 25);
        }
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doapply() & ECMD_TIME);
        assert(obj->where == OBJ_INVENT && obj->quan == 1);
        if (pet) {
            assert(pet->mtame && dist2(pet->mx, pet->my, u.ux, u.uy) <= 2);
            assert(m_at(pet->mx, pet->my) == pet);
            mongone(pet);
        }
        if (i < 2) {
            assert(obj->lamplit);
            cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doapply() & ECMD_TIME);
            assert(ordinary ? !obj->lamplit : obj->lamplit);
        }
        if (i == 3) assert(obj->spe == 2); /* native magic-harp charge */
        if (!ordinary) assert(obj->age == 1000);
        inspect_discard(obj);
    }
    u.ualign.type = oldalign;
    step15b_window_setup();
    puts("PASS Step 21 native apply Lantern/Candle/Whistle/Lyre and ordinary counterparts: light lifecycle, harp charge and invocation-age independence");
}

static void
step21_engraving_getlin(const char *prompt, char *answer)
{ (void) prompt; Strcpy(answer, "test"); }

static char step21_scry_symbol(const char *prompt, const char *choices, char def)
{ (void) prompt; (void) choices; (void) def; return '?'; }

static void
step21_crystal_apply_tests(void)
{
    struct obj *obj;
    int ordinary, intelligence = ABASE(A_INT), maxint = AMAX(A_INT);
    aligntyp align = u.ualign.type;
    ABASE(A_INT) = AMAX(A_INT) = 25;
    u.ualign.type = A_CHAOTIC;
    windowprocs.win_yn_function = step21_scry_symbol;
    for (ordinary = 0; ordinary < 2; ++ordinary) {
        obj = inspect_item(CRYSTAL_BALL);
        if (!ordinary) artifact_exists(obj, "The Earth Crystal", TRUE, ONAME_WISH);
        obj->spe = 3; obj->age = svm.moves + 100; obj->cursed = 0;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doapply() == ECMD_TIME && obj->spe == 2);
        assert(obj->age == svm.moves + 100);
        obj->spe = -1;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doapply() == ECMD_TIME);
        if (ordinary) assert(!gi.invent);
        else {
            assert(gi.invent == obj && obj->oartifact == ART_EARTH_CRYSTAL);
            inspect_discard(obj);
        }
        nomul(0);
    }
    ABASE(A_INT) = intelligence; AMAX(A_INT) = maxint;
    u.ualign.type = align;
    step15b_window_setup();
    puts("PASS Step 21 Earth Crystal native scrying uses charges independently of invocation; canceled artifact persists while ordinary canceled ball implodes");
}

static void
step21_pick_command_tests(void)
{
    struct obj *obj;
    struct trap *trap;
    int ordinary, i;
    coordxy x = u.ux + 1, y = u.uy;
    aligntyp oldalign = u.ualign.type;
    for (ordinary = 0; ordinary < 2; ++ordinary) {
        obj = inspect_item(PICK_AXE);
        if (!ordinary) {
            artifact_exists(obj, "The Iron Spoon of Liberation", TRUE, ONAME_WISH);
            assert(obj->oartifact == ART_IRON_SPOON_OF_LIBERATION);
        }
        obj->age = svm.moves + 100; obj->spe = 3;
        u.ualign.type = A_NEUTRAL;
        u.utrap = 0;
        if ((trap = t_at(u.ux, u.uy)) != NULL) deltrap(trap);
        if (m_at(x, y)) mongone(m_at(x, y));
        levl[u.ux][u.uy].typ = ROOM;
        levl[x][y].typ = STONE; levl[x][y].wall_info = 0;
        setworn(obj, W_WEP);
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0);
        assert(doapply() == ECMD_TIME && go.occupation);
        for (i = 0; i < 1000 && (*go.occupation)(); ++i) ;
        assert(i < 1000 && !IS_OBSTRUCTED(levl[x][y].typ));
        go.occupation = NULL;
        levl[x][y].typ = STONE; levl[x][y].wall_info = W_NONDIGGABLE;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        cmdq_add_dir(CQ_CANNED, 1, 0, 0);
        (void) doapply();
        if (go.occupation) (void) (*go.occupation)();
        assert(levl[x][y].typ == STONE);
        go.occupation = NULL;
        del_engr_at(u.ux, u.uy);
        windowprocs.win_getlin = step21_engraving_getlin;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        (void) doengrave();
        assert(go.occupation);
        for (i = 0; i < 1000 && (*go.occupation)(); ++i) ;
        assert(i < 1000 && engr_at(u.ux, u.uy));
        /* Native picks write in dust, with the ordinary chance of typos. */
        assert(engr_at(u.ux, u.uy)->engr_type == DUST);
        assert(strlen(engr_at(u.ux, u.uy)->engr_txt[actual_text]) == 4);
        assert(obj->age == svm.moves + 100 && obj->quan == 1);
        go.occupation = NULL; del_engr_at(u.ux, u.uy);
        setnotworn(obj); inspect_discard(obj);
    }
    levl[x][y].typ = ROOM; levl[x][y].wall_info = 0;
    u.ualign.type = oldalign;
    step15b_window_setup();
    puts("PASS Step 21 Spoon and ordinary pick native apply digging, protected rock rejection and actual engraving during invocation cooldown");
}

static void
step21_card_cloak_tests(void)
{
    struct obj *obj;
    coordxy x, y;
    int i, worn;
    boolean oldwizard = wizard, oldno = svl.level.flags.noteleport;
    aligntyp oldalign = u.ualign.type;
    d_level oldlevel = u.uz;
    long oldcontrol = HTeleport_control;
    windowprocs.win_yn_function = step21_yes;
    obj = inspect_item(CREDIT_CARD);
    artifact_exists(obj, "The Holographic Void Lily", TRUE, ONAME_WISH);
    x = u.ux + 1; y = u.uy;
    u.ualign.type = A_CHAOTIC;
    if (m_at(x, y)) mongone(m_at(x, y));
    u.utrap = 0;
    reset_pick(); go.occupation = NULL;
    levl[x][y].typ = DOOR; levl[x][y].doormask = D_LOCKED;
    obj->age = svm.moves + 100;
    cmdq_add_key(CQ_CANNED, obj->invlet);
    cmdq_add_dir(CQ_CANNED, 1, 0, 0);
    assert(doapply() == ECMD_TIME);
    assert(go.occupation);
    for (i = 0; i < 1000 && go.occupation && (*go.occupation)(); ++i) ;
    assert(i < 1000 && !(levl[x][y].doormask & D_LOCKED));
    assert(obj->age == svm.moves + 100 && obj->where == OBJ_INVENT);
    go.occupation = NULL; inspect_discard(obj);
    obj = inspect_item(OILSKIN_CLOAK);
    artifact_exists(obj, "The Cloak of the Unheld One", TRUE, ONAME_WISH);
    u.ualign.type = A_NEUTRAL; wizard = FALSE; HTeleport_control = 0;
    for (worn = 0; worn < 2; ++worn) {
        if (worn) setworn(obj, W_ARMC);
        x = u.ux; y = u.uy;
        svl.level.flags.noteleport = 1; obj->age = 0;
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doinvoke() == ECMD_TIME && obj->age > svm.moves);
        assert(u.ux == x && u.uy == y && on_level(&u.uz, &oldlevel));
        svl.level.flags.noteleport = 0;
        for (i = 0; i < 50 && u.ux == x && u.uy == y; ++i) {
            obj->age = 0; cmdq_add_key(CQ_CANNED, obj->invlet);
            assert(doinvoke() == ECMD_TIME && obj->age > svm.moves);
        }
        assert(i < 50 && obj->where == OBJ_INVENT && on_level(&u.uz, &oldlevel));
        if (worn) setnotworn(obj);
    }
    inspect_discard(obj);
    wizard = oldwizard; u.ualign.type = oldalign;
    HTeleport_control = oldcontrol; svl.level.flags.noteleport = oldno;
    step15b_window_setup();
    puts("PASS Step 21 Void Lily native credit-card unlocking during invoke cooldown; Cloak carried/worn native hero teleport and no-teleport restriction");
}

static void
step21_quake_protection_tests(void)
{
    struct obj *obj;
    struct trap *trap;
    coordxy x = u.ux, y = u.uy;
    int i, oldlevel = u.ulevel;
    long oldlev = HLevitation;
    aligntyp oldalign = u.ualign.type;
    assert(isok(x + 6, y));
    for (i = 1; i <= 6; ++i) {
        if ((trap = t_at(x + i, y)) != NULL) deltrap(trap);
        levl[x + i][y].typ = ROOM;
        levl[x + i][y].flags = 0;
    }
    levl[x + 1][y].typ = DEADTREE;
    levl[x + 1][y].wall_info = W_NONDIGGABLE;
    levl[x + 2][y].typ = DEADTREE;
    levl[x + 3][y].typ = ALTAR;
    levl[x + 3][y].altarmask = AM_SANCTUM | AM_LAWFUL;
    levl[x + 4][y].typ = STAIRS;
    assert(maketrap(x + 5, y, MAGIC_PORTAL));
    assert(maketrap(x + 6, y, VIBRATING_SQUARE));
    obj = inspect_item(CRYSTAL_BALL);
    artifact_exists(obj, "The Earth Crystal", TRUE, ONAME_WISH);
    u.ualign.type = A_CHAOTIC; u.ulevel = 30;
    HLevitation = FROMOUTSIDE;
    for (i = 0; i < 40; ++i) {
        obj->age = 0; cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doinvoke() == ECMD_TIME);
        assert(!t_at(x + 1, y) && levl[x + 1][y].typ == DEADTREE);
        assert(levl[x + 3][y].typ == ALTAR && !t_at(x + 3, y));
        assert(levl[x + 4][y].typ == STAIRS && !t_at(x + 4, y));
        assert(t_at(x + 5, y)->ttyp == MAGIC_PORTAL);
        assert(t_at(x + 6, y)->ttyp == VIBRATING_SQUARE);
    }
    assert(t_at(x + 2, y) && is_pit(t_at(x + 2, y)->ttyp));
    inspect_discard(obj);
    u.ualign.type = oldalign; u.ulevel = oldlevel; HLevitation = oldlev;
    puts("PASS Step 21 40 native Crystal quakes preserve nondiggable terrain/high altar/stairs/portal/Invocation square while ordinary terrain receives pits");
}

static char step21_level_answer[BUFSZ];
static void step21_level_getlin(const char *prompt, char *answer)
{ (void) prompt; Strcpy(answer, step21_level_answer); }

static char step21_no(const char *prompt, const char *choices, char def)
{ (void) prompt; (void) choices; (void) def; return 'n'; }

static void
step21_progression_tests(void)
{
    d_level origin = u.uz, previous_target = u.utolev, scheduled;
    int pending = u.utotype;
    boolean oldwizard = wizard, invoked = u.uevent.invoked;
    boolean entered = u.uevent.gehennom_entered;
    long control = HTeleport_control;
    struct obj *lyre;
    int choice;
    lyre = inspect_item(MAGIC_HARP);
    artifact_exists(lyre, "The Lyre of Orpheus", TRUE, ONAME_WISH);
    wizard = FALSE; HTeleport_control = FROMOUTSIDE;
    windowprocs.win_getlin = step21_level_getlin;
    u.uz = valley_level; ++u.uz.dlevel;
    assert(Inhell);
    for (choice = 0; choice < 2; ++choice) {
        u.uevent.invoked = choice;
        Sprintf(step21_level_answer, "%d", depth(&sanctum_level));
        level_tele();
        assert(choice ? on_level(&u.utolev, &sanctum_level)
                      : u.utolev.dnum == sanctum_level.dnum
                        && u.utolev.dlevel == sanctum_level.dlevel - 1);
        /* Scheduling is not a committed transition and must not migrate pets. */
        assert(!gm.mydogs);
        u.utotype = 0;
    }
    u.uz = valley_level; u.uevent.gehennom_entered = 0;
    windowprocs.win_yn_function = step21_no;
    stairway_free_all();
    scheduled = valley_level; ++scheduled.dlevel;
    if (t_at(u.ux, u.uy)) deltrap(t_at(u.ux, u.uy));
    levl[u.ux][u.uy].typ = STAIRS;
    stairway_add(u.ux, u.uy, FALSE, FALSE, &scheduled);
    assert(dodown() == ECMD_OK && on_level(&u.uz, &valley_level));
    assert(!u.uevent.gehennom_entered && !gm.mydogs);
    u.uz = stronghold_level;
    assert(Can_fall_thru(&u.uz)); /* Castle's native route into Gehennom remains */
    u.uz = origin; u.utolev = previous_target; u.utotype = pending;
    wizard = oldwizard; HTeleport_control = control;
    u.uevent.invoked = invoked; u.uevent.gehennom_entered = entered;
    stairway_free_all(); levl[u.ux][u.uy].typ = ROOM;
    inspect_discard(lyre); step15b_window_setup();
    puts("PASS Step 21 native pre/post-Invocation Sanctum level-teleport restriction, declined Gehennom gate and preserved Castle fall route with carried Lyre");
}

static void
step21_transition_tests(void)
{
    struct obj *lyre, *keepsake;
    struct monst *pet, *mon;
    struct trap *trap;
    d_level from, dest;
    unsigned mid, oid;
    unsigned petids[6], keptids[6], excluded[4] = {0, 0, 0, 0};
    NHFILE *file;
    int route, count, hp, tame;
    coordxy x, y;
    const char *routes[] = { "stairs", "ladder", "hole", "trap door", "level teleport", "portal" };
    memset(u.uprops, 0, sizeof u.uprops);
    u.uhp = u.uhpmax = 1000;
    u.ustuck = u.usteed = NULL;
    u.utrap = 0;
    Strcpy(gl.lock, "step21-transition");
    windowprocs.win_getlin = step21_level_getlin;
    lyre = inspect_item(MAGIC_HARP);
    artifact_exists(lyre, "The Lyre of Orpheus", TRUE, ONAME_WISH);
    for (route = 0; route < 6; ++route) {
        from = u.uz; dest = from; ++dest.dlevel;
        if (route == 5) {
            goto_level(&dest, FALSE, FALSE, FALSE);
            trap = maketrap(u.ux, u.uy, MAGIC_PORTAL);
            assert(trap); trap->dst = from;
            mon = makemon(&mons[PM_GOBLIN], u.ux, u.uy, NO_MINVENT | MM_ADJACENTOK);
            assert(mon);
            remove_monster(mon->mx, mon->my);
            place_monster(mon, u.ux, u.uy); /* occupy paired arrival square */
            goto_level(&from, FALSE, FALSE, FALSE);
        }
        /* A genuinely distant pet, carrying persistent state. */
        x = y = 0;
        for (x = 2; x < COLNO - 1; ++x) {
            for (y = 1; y < ROWNO - 1; ++y)
                if (dist2(x, y, u.ux, u.uy) > 100 && !m_at(x, y)
                    && goodpos(x, y, &gy.youmonst, 0)) break;
            if (y < ROWNO - 1) break;
        }
        if (x >= COLNO - 1) {
            x = u.ux < COLNO / 2 ? COLNO - 3 : 2; y = 10;
            if ((mon = m_at(x, y)) != NULL) mongone(mon);
            levl[x][y].typ = ROOM; unblock_point(x, y);
        }
        pet = makemon(&mons[PM_LITTLE_DOG], x, y, MM_EDOG);
        assert(pet); initedog(pet, FALSE);
        pet->mhp = pet->mhpmax = 40; pet->mconf = 1;
        keepsake = mksobj(DIAMOND, FALSE, FALSE); oid = keepsake->o_id;
        (void) mpickobj(pet, keepsake);
        mid = pet->m_id; hp = pet->mhp; tame = pet->mtame;
        petids[route] = mid; keptids[route] = oid;
        if (route == 5) {
            int n;
            for (n = 0; n < 4; ++n) {
                mon = makemon(&mons[PM_LITTLE_DOG], x, y, MM_EDOG | MM_ADJACENTOK);
                assert(mon);
                excluded[n] = mon->m_id;
                if (n == 0) { initedog(mon, FALSE); mon->msleeping = 1; }
                else if (n == 3) { initedog(mon, FALSE); mongone(mon); }
                else { mon->mtame = 0; mon->mpeaceful = n == 1; }
            }
        }
        if (route == 0) {
            d_level denied = earth_level, oldlevel;
            boolean oldwizard = wizard;
            /* All denial paths must leave the remote pet on this level. */
            wizard = FALSE; u.uhave.amulet = 0;
            goto_level(&denied, FALSE, FALSE, FALSE);
            assert(on_level(&u.uz, &from) && m_at(x, y) == pet && !gm.mydogs);
            oldlevel = u.uz; u.uz = qstart_level;
            denied = u.uz; ++denied.dlevel;
            goto_level(&denied, TRUE, FALSE, FALSE);
            assert(on_level(&u.uz, &qstart_level) && m_at(x, y) == pet && !gm.mydogs);
            u.uz = oldlevel;
            HTeleport_control = FROMOUTSIDE;
            Strcpy(step21_level_answer, "\033"); level_tele();
            assert(on_level(&u.uz, &from) && m_at(x, y) == pet && !gm.mydogs);
            HTeleport_control = 0;
            Strcpy(gl.lock, "missing-step21-directory/level");
            goto_level(&dest, FALSE, FALSE, FALSE);
            assert(on_level(&u.uz, &from) && m_at(x, y) == pet && !gm.mydogs);
            Strcpy(gl.lock, "step21-transition");
            wizard = oldwizard;
            puts("PASS Step 21 denied quest/Amulet progression, canceled level teleport and failed level-file creation preserve remote pet without migration");
        }
        if ((trap = t_at(u.ux, u.uy)) != NULL) deltrap(trap);
        stairway_free_all();
        levl[u.ux][u.uy].typ = ROOM;
        HStun = HConfusion = 0;
        if (route < 2) {
            levl[u.ux][u.uy].typ = route ? LADDER : STAIRS;
            stairway_add(u.ux, u.uy, FALSE, route == 1, &dest);
            assert(dodown() == ECMD_TIME);
        } else if (route < 4) {
            trap = maketrap(u.ux, u.uy, route == 2 ? HOLE : TRAPDOOR);
            assert(trap); trap->dst = dest;
            fall_through(TRUE, 0);
            assert(on_level(&u.uz, &from) && fmon && !gm.mydogs);
            deferred_goto();
        } else if (route == 4) {
            HTeleport_control = FROMOUTSIDE;
            Sprintf(step21_level_answer, "%d", depth(&dest));
            level_tele(); deferred_goto();
            HTeleport_control = 0;
        } else {
            u.uz0 = u.uz;
            trap = maketrap(u.ux, u.uy, MAGIC_PORTAL);
            assert(trap); trap->dst = dest;
            domagicportal(trap);
            assert(on_level(&u.uz, &from) && !gm.mydogs);
            deferred_goto();
        }
        assert(on_level(&u.uz, &dest));
        count = 0;
        for (mon = fmon; mon; mon = mon->nmon) if (mon->m_id == mid) {
            ++count;
            assert(mon->mtame == tame && mon->mhp == hp && mon->mconf);
            assert(mon->minvent && mon->minvent->o_id == oid);
            assert(isok(mon->mx, mon->my) && m_at(mon->mx, mon->my) == mon);
        }
        assert(count == 1 && !gm.mydogs);
        if (route == 5) for (mon = fmon; mon; mon = mon->nmon)
            assert(mon->m_id != excluded[0] && mon->m_id != excluded[1]
                   && mon->m_id != excluded[2] && mon->m_id != excluded[3]);
        printf("PASS Step 21 native %s entrypoint: remote pet committed once with identity, HP, tameness, status and inventory\n", routes[route]);
    }
    file = get_freeing_nhfile();
    file_mode(file, WRITING | FREEING, open("step21-pets.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    savelev(file, ledger_no(&u.uz)); close_nhfile(file);
    file = get_freeing_nhfile();
    file_mode(file, READING, open("step21-pets.tmp", O_RDONLY | O_BINARY));
    getlev(file, 0, ledger_no(&u.uz)); close_nhfile(file);
    for (route = 0; route < 6; ++route) {
        count = 0;
        for (mon = fmon; mon; mon = mon->nmon) if (mon->m_id == petids[route]) {
            ++count;
            assert(mon->minvent && mon->minvent->o_id == keptids[route] && mon->mtame);
        }
        assert(count == 1);
    }
    puts("PASS Step 21 multiple following pets survive all transitions and native level reload exactly once; occupied portal arrival and sleeping/hostile/peaceful exclusions");
    inspect_discard(lyre);
}

static void
step21_shirt_conditions_tests(void)
{
    struct obj *obj;
    struct prop props[LAST_PROP + 1];
    int i, saw = 0, strength = ABASE(A_STR);
    memcpy(props, u.uprops, sizeof props);
    memset(u.uprops, 0, sizeof props);
    obj = inspect_item(T_SHIRT);
    artifact_exists(obj, "The Tie-Dye Shirt of Shambhala", TRUE, ONAME_WISH);
    obj->blessed = 1;
    ABASE(A_STR) = AMAX(A_STR) - 1;
    make_sick(20L, "a test illness", FALSE, SICK_ALL);
    make_blinded(20L, FALSE); make_hallucinated(20L, FALSE, 0L);
    make_confused(20L, FALSE); make_stunned(20L, FALSE); make_vomiting(20L, FALSE);
    HDeaf = 20; Stoned = 20; Slimed = 20;
    for (i = 0; i < 100 && (Sick || BlindedTimeout || HHallucination
                            || HConfusion || HStun || Vomiting); ++i) {
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doread() == ECMD_TIME);
    }
    assert(i < 100 && !Sick && !BlindedTimeout && !HHallucination
           && !HConfusion && !HStun && !Vomiting);
    assert(HDeaf == 20 && Stoned == 20 && Slimed == 20 && ABASE(A_STR) == AMAX(A_STR) - 1);
    HDeaf = Stoned = Slimed = 0;
    obj->blessed = 0; obj->cursed = 1;
    for (i = 0; i < 100; ++i) {
        make_sick(0L, NULL, FALSE, SICK_ALL);
        make_blinded(0L, FALSE); make_hallucinated(0L, FALSE, 0L);
        make_confused(0L, FALSE); make_stunned(0L, FALSE);
        cmdq_add_key(CQ_CANNED, obj->invlet);
        assert(doread() == ECMD_TIME);
        if (Sick) saw |= 1;
        if (BlindedTimeout) saw |= 2;
        if (HHallucination) saw |= 4;
        if (HConfusion) saw |= 8;
        if (HStun) saw |= 16;
        assert(!Vomiting && !HDeaf && !Stoned && !Slimed);
        assert(ABASE(A_STR) == AMAX(A_STR) - 1);
    }
    assert(saw == 31);
    inspect_discard(obj);
    make_sick(0L, NULL, FALSE, SICK_ALL);
    memcpy(u.uprops, props, sizeof props);
    ABASE(A_STR) = strength;
    puts("PASS Step 21 Shirt all six donor cures, excluded conditions/attributes and 100 cursed readings covering exactly five outcomes; Book 100 no-page-flip native casts");
}
