/* Step 11 native probe. Linked to the actual generator and codecs only in
 * explicitly instrumented builds. No replacement RNG, Lua, or constructors. */
#include "hack.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern void step11_generate(void);
extern void step11_repeat_backends(void);
extern void step12_library_tables(void);
extern void step12_library_validate(struct mkroom *);
extern void step12_library_fixture(int, int, int, boolean);
extern void step12_library_loose_case(int, int);
extern const struct shclass shtypes[];
static int baby_requests, adult_requests;
static int library_messages;

static void
capture_library_message(const char *message)
{
    if (!strcmp(message, "You enter a library!")) {
        ++library_messages;
        puts(message);
    }
}

void
step11_monster_request(int pm, boolean waiting)
{
    if (pm >= PM_BABY_GRAY_DRAGON && pm <= PM_BABY_YELLOW_DRAGON) {
        ++baby_requests;
        assert(waiting);
    } else if (pm >= PM_GRAY_DRAGON && pm <= PM_YELLOW_DRAGON) {
        ++adult_requests;
        assert(waiting);
    }
}
#ifdef USE_ISAAC64
extern void init_isaac64(unsigned long, int (*)(int));
#endif

static void
selector_gate(void)
{
    struct custom_descriptor entries[40];
    struct custom_context c = { TRUE, 50, 50, 50, 197 };
    int i, j, k, outcome, counts[256], eligible;
    const int boundaries[] = { 0, 1, 4, 5, 12, 13, 14, 15, 20, 21, 22,
                               29, 30, 31, 59, 60, 99, 100, 149, 150,
                               195, 196, 197, 198, 199, 200 };
    assert(custom_registry_count == 8); /* Step 12 appends Library. */
    assert(!custom_validate(custom_registry, custom_registry_count));
    for (i = 0; i < SIZE(boundaries); ++i)
        for (j = 0; j < SIZE(boundaries); ++j) {
            c.dlevel = c.logical_depth = boundaries[i];
            c.difficulty = boundaries[j];
            memset(counts, 0, sizeof counts);
            for (outcome = 0; outcome < 10000; ++outcome) {
                k = custom_select(custom_registry, custom_registry_count,
                                  &c, outcome);
                assert(k >= 0 && k < 256);
                ++counts[k];
            }
            eligible = 0;
            for (k = 0; k < custom_registry_count; ++k) {
                int want = custom_eligible(&custom_registry[k], &c) ? 300 : 0;
                if (custom_registry[k].id == CUSTOM_LIBRARY)
                    assert(want == (c.dlevel >= 5 && c.dlevel <= 199 ? 300 : 0));
                else if (k < 3)
                    assert(want == (c.dlevel >= 30 && c.dlevel <= 199
                                   && c.logical_depth < c.medusa_depth ? 300 : 0));
                else
                    assert(want == ((k == 3 ? c.difficulty >= 14
                                     : k == 4 ? c.difficulty <= 14
                                     : k == 5 ? c.difficulty >= 13
                                              : c.difficulty >= 21) ? 300 : 0));
                assert(counts[custom_registry[k].id] == want);
                eligible += want;
            }
            assert(counts[0] == 10000 - eligible);
        }
    c.dlevel = c.logical_depth = c.difficulty = 50;
    assert(!custom_eligible(&custom_registry[4], &c));
    c.difficulty = 14;
    assert(custom_eligible(&custom_registry[4], &c));
    c.dlevel = c.logical_depth = 1;
    c.difficulty = 21;
    assert(custom_eligible(&custom_registry[6], &c));
    assert(!custom_eligible(&custom_registry[0], &c));
    c.ordinary_dod = FALSE;
    for (i = 0; i < 10000; ++i) {
        assert(!custom_select(custom_registry, custom_registry_count, &c, i));
        assert(!custom_select(NULL, 0, &c, i));
    }
    c.ordinary_dod = TRUE;
    c.dlevel = c.logical_depth = c.difficulty = 50;
    for (i = 0; i < custom_registry_count; ++i)
        entries[i] = custom_registry[custom_registry_count - 1 - i];
    memset(counts, 0, sizeof counts);
    for (i = 0; i < 10000; ++i)
        ++counts[custom_select(entries, custom_registry_count, &c, i)];
    for (i = 0; i < custom_registry_count; ++i)
        assert(counts[entries[i].id] == (custom_eligible(&entries[i], &c) ? 300 : 0));
    entries[0].probability = 137; /* justified non-production override */
    memset(counts, 0, sizeof counts);
    for (i = 0; i < 10000; ++i)
        ++counts[custom_select(entries, custom_registry_count, &c, i)];
    assert(counts[entries[0].id] == 137);
    entries[1].id = entries[0].id;
    assert(custom_validate(entries, custom_registry_count));
    entries[1] = custom_registry[0];
    entries[0].probability = 10001;
    assert(custom_validate(entries, 1));
    entries[0].probability = -1;
    assert(custom_validate(entries, 1));
    entries[0].probability = 0;
    entries[0].min_dl = 40; entries[0].max_dl = 30;
    assert(custom_validate(entries, 1));
    entries[0] = custom_registry[0]; entries[0].target = "missing";
    assert(custom_validate(entries, 1));
    for (i = 0; i < 34; ++i) {
        entries[i] = custom_registry[0];
        entries[i].id = i + 1;
    }
    assert(custom_select(entries, 34, &c, 9999) == -1);
    assert(custom_select(entries, 34, &c, 0) == -1);
    memset(counts, 0, sizeof counts);
    for (i = 0; i < 100000; ++i)
        ++counts[custom_select(custom_registry, custom_registry_count,
                               &c, rn2(10000))];
    for (i = 0; i <= CUSTOM_LIBRARY; ++i)
        printf("SELECTOR|id=%d|count=%d|draws=100000\n", i, counts[i]);
    puts("PASS exhaustive production selector: boundaries, permutations, empty, invalid, overrides, overflow");
}

static boolean
owns(struct mkroom *r, int x, int y)
{
    if (x < r->lx || x > r->hx || y < r->ly || y > r->hy)
        return FALSE;
    return (boolean) (!r->irregular
        || (levl[x][y].roomno == (r - svr.rooms) + ROOMOFFSET
            && !levl[x][y].edge));
}

static void
validate_room(struct mkroom *r)
{
    int x, y, cells = 0, reached = 0, change, monsters = 0, objects = 0;
    int gold = 0, chests = 0, teleports = 0, escapes = 0, books = 0;
    int coffers = 0, itemunits = 0;
    int queens = 0, bees = 0, adult = 0, baby = 0, eggs = 0, traps = 0;
    boolean seen[COLNO][ROWNO] = { { FALSE } };
    struct monst *mon;
    struct obj *obj;
    struct trap *trap;
    assert(r->custom_id > 0 && r->custom_id <= CUSTOM_LIBRARY);
    assert(r->needfill == 0);
    assert(!has_upstairs(r) && !has_dnstairs(r));
    for (x = r->lx; x <= r->hx; ++x)
        for (y = r->ly; y <= r->hy; ++y)
            if (owns(r, x, y)) {
                if (!cells) seen[x][y] = TRUE;
                ++cells;
            }
    do {
        change = 0;
        for (x = r->lx; x <= r->hx; ++x)
            for (y = r->ly; y <= r->hy; ++y)
                if (owns(r, x, y) && !seen[x][y]
                    && (seen[x-1][y] || seen[x+1][y]
                        || seen[x][y-1] || seen[x][y+1])) {
                    seen[x][y] = TRUE; ++change;
                }
    } while (change);
    for (x = r->lx; x <= r->hx; ++x)
        for (y = r->ly; y <= r->hy; ++y)
            reached += seen[x][y];
    assert(reached == cells);
    for (mon = fmon; mon; mon = mon->nmon)
        if (!DEADMONSTER(mon) && owns(r, mon->mx, mon->my)) {
            ++monsters;
            queens += (mon->mnum == PM_QUEEN_BEE);
            bees += (mon->mnum == PM_KILLER_BEE);
            adult += (mon->mnum >= PM_GRAY_DRAGON && mon->mnum <= PM_YELLOW_DRAGON);
            baby += (mon->mnum >= PM_BABY_GRAY_DRAGON && mon->mnum <= PM_BABY_YELLOW_DRAGON);
            if (r->custom_id == CUSTOM_DRAGON_HALL
                && mon->mnum >= PM_BABY_GRAY_DRAGON
                && mon->mnum <= PM_YELLOW_DRAGON)
                assert(mon->mstrategy & STRAT_WAITFORU);
        }
    for (obj = fobj; obj; obj = obj->nobj)
        if (owns(r, obj->ox, obj->oy)) {
            ++objects;
            itemunits += (int) obj->quan;
            gold += obj->otyp == GOLD_PIECE;
            chests += obj->otyp == CHEST;
            if (obj->otyp == CHEST && obj->spe == 2) {
                struct obj *item;
                for (item = obj->cobj; item; item = item->nobj)
                    coffers += item->otyp == GOLD_PIECE;
            }
            if (obj->oclass == SPBOOK_CLASS) books += (int) obj->quan;
            eggs += obj->otyp == EGG;
            escapes += obj->otyp == PICK_AXE || obj->otyp == DWARVISH_MATTOCK
                || obj->otyp == WAN_DIGGING || obj->otyp == SCR_TELEPORTATION
                || obj->otyp == RIN_TELEPORTATION;
        }
    for (trap = gf.ftrap; trap; trap = trap->ntrap)
        if (owns(r, trap->tx, trap->ty)) {
            ++traps;
            teleports += trap->ttyp == TELEP_TRAP;
            escapes += trap->ttyp == TELEP_TRAP || trap->ttyp == HOLE
                || trap->ttyp == TRAPDOOR;
        }
    switch (r->custom_id) {
    case CUSTOM_LIBRARY:
        step12_library_validate(r);
        break;
    case CUSTOM_WIZARD_STUDY:
        assert(cells == 9 && !r->needjoining && !r->doorct);
        assert(teleports == 1 && books >= 3 && itemunits >= 7);
        break;
    case CUSTOM_STOREROOM:
        assert(cells == 4 && !r->needjoining && !r->doorct);
        assert(chests >= 1 && chests <= 3 && escapes >= 1);
        break;
    case CUSTOM_HONEYCOMB:
        assert(r->irregular && r->needjoining && cells == 68);
        assert(queens == 1 && bees > 0);
        break;
    case CUSTOM_DRAGON_HALL:
        assert(r->irregular && r->needjoining && cells > 90);
        assert(gold > 0 && adult <= 10 && baby <= 6 && traps >= 2);
        break;
    case CUSTOM_GIANT_COURT:
        assert(monsters > 0 && coffers == 1);
        break;
    case CUSTOM_REAL_ZOO:
    case CUSTOM_DRAGON_LAIR:
        assert(monsters > 0 && gold > 0);
        break;
    }
    printf("CONTENT|id=%u|cells=%d|monsters=%d|objects=%d|gold=%d|chests=%d|eggs=%d|traps=%d|adult=%d|baby=%d\n",
           r->custom_id,cells,monsters,objects,gold,chests,eggs,traps,adult,baby);
}

static void
free_level(void)
{
    NHFILE *file = get_freeing_nhfile();
    if (iflags.purge_monsters) dmonsfree();
    savelev(file, -1);
    close_nhfile(file);
}

static void
file_mode(NHFILE *f, int mode, int fd)
{
    init_nhfile(f);
    f->mode = mode; f->ftype = NHF_LEVELFILE;
    f->structlevel = TRUE; f->fieldlevel = FALSE; f->addinfo = FALSE;
    f->fnidx = historical; f->fd = fd;
}

static void
level_roundtrip(boolean bones)
{
    NHFILE *f = get_freeing_nhfile();
    int i, n = svn.nroom;
    unsigned char ids[MAXNROFROOMS];
    schar types[MAXNROFROOMS];
    for (i = 0; i < n; ++i) {
        ids[i] = svr.rooms[i].custom_id;
        types[i] = svr.rooms[i].rtype;
    }
    file_mode(f, WRITING | FREEING,
              open("step11-level.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
                   _S_IREAD | _S_IWRITE));
    assert(f->fd >= 0);
    if (bones) {
        f->ftype = NHF_BONESFILE;
        f->mode = WRITING; /* a bones writer does not free the game's fruits */
        savefruitchn(f);
        f->mode = WRITING | FREEING;
    }
    savelev(f, ledger_no(&u.uz));
    close_nhfile(f);
    assert(!custom_generation.rolled);
    f = get_freeing_nhfile();
    file_mode(f, READING, open("step11-level.tmp", O_RDONLY | O_BINARY));
    if (bones) f->ftype = NHF_BONESFILE;
    assert(f->fd >= 0);
    getlev(f, 0, ledger_no(&u.uz));
    close_nhfile(f);
    assert(svn.nroom == n && !custom_generation.rolled);
    for (i = 0; i < n; ++i) {
        assert(ids[i] == svr.rooms[i].custom_id);
        assert(types[i] == svr.rooms[i].rtype);
        if (ids[i]) validate_room(&svr.rooms[i]);
    }
    puts("PASS actual savelev/getlev identity and payload roundtrip");
}

static void
metadata_roundtrip(void)
{
    NHFILE *f;
    int i;
    clear_level_structures();
    svr.rooms[0].custom_id = CUSTOM_DRAGON_HALL;
    gs.subrooms[0].custom_id = CUSTOM_HONEYCOMB;
    add_room(40, 2, 50, 12, TRUE, OROOM, FALSE);
    add_subroom(&svr.rooms[0], 43, 5, 46, 8, TRUE, OROOM, FALSE);
    assert(!svr.rooms[0].custom_id && !gs.subrooms[0].custom_id);
    svr.rooms[0].custom_id = CUSTOM_WIZARD_STUDY;
    add_room(5, 2, 15, 12, TRUE, OROOM, FALSE);
    sort_rooms();
    assert(!svr.rooms[0].custom_id);
    assert(svr.rooms[1].custom_id == CUSTOM_WIZARD_STUDY);
    assert(svr.rooms[1].sbrooms[0] == &gs.subrooms[0]);
    f = get_freeing_nhfile();
    file_mode(f, WRITING, open("step11-rooms.tmp",
              O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    assert(f->fd >= 0);
    save_rooms(f); close_nhfile(f);
    clear_level_structures();
    f = get_freeing_nhfile();
    file_mode(f, READING, open("step11-rooms.tmp", O_RDONLY | O_BINARY));
    assert(f->fd >= 0);
    rest_rooms(f); close_nhfile(f);
    assert(svn.nroom == 2 && gn.nsubroom == 1);
    assert(!svr.rooms[0].custom_id && !gs.subrooms[0].custom_id);
    assert(svr.rooms[1].custom_id == CUSTOM_WIZARD_STUDY);
    assert(svr.rooms[1].sbrooms[0] == &gs.subrooms[0]);
    clear_level_structures();
    for (i = 0; i < SIZE(svr.rooms); ++i) assert(!svr.rooms[i].custom_id);
    puts("PASS real constructors, room sorting, recursive subroom codec and reuse");
}

static void
escape_check(void)
{
    int i;
    /* Real games start at move 1. Move 0 satisfies the native stasis timer's
     * inclusive comparison even though no stasis effect has been applied. */
    if (!svm.moves) svm.moves = 1;
    for (i = 0; i < svn.nroom; ++i) {
        struct mkroom *r = &svr.rooms[i];
        struct trap *t;
        if (r->custom_id != CUSTOM_WIZARD_STUDY
            && r->custom_id != CUSTOM_STOREROOM) continue;
        assert(!svl.level.flags.noteleport);
        assert(!(levl[r->lx - 1][r->ly].wall_info & W_NONDIGGABLE));
        for (t = gf.ftrap; t; t = t->ntrap)
            if (owns(r, t->tx, t->ty)) {
                if (t->ttyp == HOLE || t->ttyp == TRAPDOOR)
                    assert(Can_fall_thru(&u.uz));
                if (t->ttyp == TELEP_TRAP) {
                    int attempt;
                    boolean was_wizard = wizard;
                    wizard = FALSE; /* avoid interactive wizard destination UI */
                    u.ux = t->tx; u.uy = t->ty;
                    assert(!Antimagic && !Teleport_control);
                    assert(!noteleport_level(&gy.youmonst));
                    for (attempt = 0; attempt < 20 && owns(r,u.ux,u.uy); ++attempt)
                        tele_trap(t);
                    wizard = was_wizard;
                    assert(!owns(r,u.ux,u.uy));
                    puts("PASS actual teleport trap escapes disconnected room");
                }
            }
    }
}

static void
coexistence_and_entry(void)
{
    int i, j, vanilla = 0, shops = 0;
    for (i = 0; i < svn.nroom; ++i) {
        struct mkroom *r = &svr.rooms[i];
        if (r->rtype >= SHOPBASE) {
            struct monst *shk = shop_keeper(i + ROOMOFFSET);
            struct obj *obj;
            int stock = 0;
            assert(!r->custom_id && shk && shk->isshk);
            for (obj = fobj; obj; obj = obj->nobj)
                if (owns(r, obj->ox, obj->oy) && obj->otyp != GOLD_PIECE)
                    ++stock;
            assert(stock > 0);
            for (obj = fobj; obj; obj = obj->nobj)
                if (owns(r, obj->ox, obj->oy) && !obj->no_charge
                    && obj->otyp != GOLD_PIECE && !Has_contents(obj)) {
                    int oldbill = ESHK(shk)->billct;
                    u.ux = obj->ox; u.uy = obj->oy;
                    u.ushops[0] = (char) (i + ROOMOFFSET); u.ushops[1] = '\0';
                    u_entered_shop(u.ushops); /* reconnect restored bill pointer */
                    addtobill(obj, FALSE, FALSE, TRUE);
                    assert(obj->unpaid && ESHK(shk)->billct == oldbill + 1);
                    subfrombill(obj, shk);
                    assert(!obj->unpaid && ESHK(shk)->billct == oldbill);
                    u.ushops[0] = '\0';
                    break;
                }
            assert(obj); /* actual native billing, not just stock presence */
            ++shops;
        }
        if (!r->custom_id) continue;
        for (j = 0; j < svn.nroom; ++j)
            if (!svr.rooms[j].custom_id && svr.rooms[j].rtype == r->rtype)
                ++vanilla;
        if (r->rtype != THEMEROOM || r->custom_id == CUSTOM_LIBRARY) {
            int rt = r->rtype, old_vanilla = vanilla;
            unsigned id = r->custom_id;
            int x, y;
            for (x = r->lx; x <= r->hx; ++x)
                for (y = r->ly; y <= r->hy; ++y)
                    if (owns(r,x,y)) { u.ux=x; u.uy=y; goto entered; }
 entered:
            memset(u.urooms, 0, sizeof u.urooms);
            memset(u.ushops, 0, sizeof u.ushops);
            {
                void (*saved_print)(const char *) = windowprocs.win_raw_print;
                boolean saved_init = iflags.window_inited;
                int before = library_messages;
                iflags.window_inited = FALSE;
                windowprocs.win_raw_print = capture_library_message;
                check_special_room(FALSE);
                assert(r->rtype == OROOM && r->custom_id == id);
                check_special_room(FALSE);
                assert(r->rtype == OROOM && r->custom_id == id);
                if (id == CUSTOM_LIBRARY)
                    assert(library_messages == before + (rt == THEMEROOM ? 1 : 0));
                windowprocs.win_raw_print = saved_print;
                iflags.window_inited = saved_init;
            }
            if (old_vanilla) {
                if (rt == COURT) assert(svl.level.flags.has_court);
                if (rt == ZOO) assert(svl.level.flags.has_zoo);
                if (rt == BEEHIVE) assert(svl.level.flags.has_beehive);
            }
        }
    }
    {
        int other = 0;
        for (i = 0; i < svn.nroom; ++i)
            if (!svr.rooms[i].custom_id && svr.rooms[i].rtype > OROOM
                && svr.rooms[i].rtype != THEMEROOM && svr.rooms[i].rtype < SHOPBASE)
                ++other;
        printf("COEXIST|samebase=%d|shops=%d|vanilla=%d\n",vanilla,shops,other);
    }
}

static void
setup_large_shop_host(void)
{
    struct mkroom *room;

    clear_level_structures();
    add_room(10, 5, 15, 10, TRUE, OROOM, FALSE);
    room = &svr.rooms[0];
    levl[9][7].typ = DOOR;
    levl[9][7].doormask = D_ISOPEN;
    add_door(9, 7, room);
    assert(room->doorct == 1);
    assert((room->hx - room->lx + 1) * (room->hy - room->ly + 1) > 20);
}

#ifdef USE_ISAAC64
static unsigned long
seed_for_shop_type(int target)
{
    unsigned long seed;

    for (seed = 1; seed < 100000UL; ++seed) {
        int i, roll;

        init_isaac64(seed, rn2);
        roll = rnd(100);
        for (i = 0; (roll -= shtypes[i].prob) > 0; ++i)
            continue;
        if (i == target)
            return seed;
    }
    assert(0); /* every positive configured probability must be reachable */
    return 0;
}

static void
shop_type_gate(void)
{
    int selected;

    for (selected = 0; selected < FODDERSHOP - SHOPBASE + 1; ++selected) {
        unsigned long seed = seed_for_shop_type(selected);
        int final_type;

        setup_large_shop_host();
        init_isaac64(seed, rn2);
        do_mkroom(SHOPBASE);
        final_type = svr.rooms[0].rtype;
        printf("SHOP_TYPE|selected=%d|final=%d|large=1|seed=%lu\n",
               SHOPBASE + selected, final_type, seed);
        assert(final_type == SHOPBASE + selected);
        assert(svr.rooms[0].needfill == FILL_NORMAL);
    }

    setup_large_shop_host();
    init_isaac64(seed_for_shop_type(WANDSHOP - SHOPBASE), rn2);
    do_mkroom(SHOPBASE);
    assert(svr.rooms[0].rtype == WANDSHOP);
    fill_special_room(&svr.rooms[0]);
    assert(svr.rooms[0].resident != 0);
    assert(ESHK(svr.rooms[0].resident)->shoptype == WANDSHOP);
    coexistence_and_entry();
    puts("PASS final Step 5 shop types preserve all configured selections in "
         "large rooms; native stocking and billing work");
}
#endif

static void
accepted_bones(void)
{
    char *id, why[BUFSZ], count;
    unsigned feature = custom_generation.selected;
    NHFILE *f = create_bonesfile(&u.uz, &id, why);
    int i, found = 0;
    unsigned long seed;
    assert(f);
    count = (char) (strlen(id) + 1);
    f->mode = WRITING;
    store_version(f);
    Sfo_char(f, svn.nhuuid, "ancestor-nhuuid", sizeof svn.nhuuid);
    Sfo_char(f, &count, "bones_count", 1);
    Sfo_char(f, id, "bonesid", count);
    savefruitchn(f);
    savelev(f, ledger_no(&u.uz));
    close_nhfile(f);
    commit_bonesfile(&u.uz);
    free_level();
    wizard = FALSE;
    flags.bones = TRUE;
#ifdef USE_ISAAC64
    /* Control the actual bones probability draw, then let mklev/getbones
     * run normally. No mocked loader or replacement random function. */
    for (seed = 1; seed < 1000; ++seed) {
        init_isaac64(seed, rn2);
        if (!rn2(3)) break;
    }
    assert(seed < 1000);
    init_isaac64(seed, rn2);
#endif
    mklev();
    assert(!custom_generation.rolled && !custom_generation.emissions);
    for (i = 0; i < svn.nroom; ++i)
        if (svr.rooms[i].custom_id == feature) ++found;
    assert(found == 1);
    puts("PASS accepted bones through actual mklev/getbones: no selector or emission");
}

int
step11_test_main(void)
{
    const char *mode = getenv("STEP11_MODE"), *v = getenv("STEP11_SEED");
    const char *min_v = getenv("STEP11_MIN_DL");
    const char *max_v = getenv("STEP11_MAX_DL");
    unsigned long seed = v ? strtoul(v, NULL, 10) : 110001UL;
    int i, dl, samples = 0;
    int min_dl = min_v ? atoi(min_v) : 0;
    int max_dl = max_v ? atoi(max_v) : 0;
    boolean bone_test = mode && !strcmp(mode, "bones");
    boolean escape_test = mode && !strcmp(mode, "escape");
    boolean forced = mode && (!strcmp(mode, "forced") || bone_test || escape_test);
    boolean clean = getenv("STEP11_CLEAN_FAILURE") != NULL;
    setvbuf(stdout, NULL, _IONBF, 0);
    has_strong_rngseed = FALSE;
#ifdef USE_ISAAC64
    init_isaac64(seed, rn2);
    init_isaac64(seed ^ 0x5a5aUL, rn2_on_display_rng);
#endif
    if (mode && !strcmp(mode, "selector")) {
        selector_gate();
        return 0;
    }
    init_objects();
    flags.pantheon = -1;
    flags.initrole = flags.initrace = flags.initgend = flags.initalign = ROLE_NONE;
    Strcpy(svp.plname, "step11-probe");
    svp.pl_character[0] = '\0';
    role_init(); init_dungeons(); init_artifacts();
    svc.context.current_fruit = fruitadd("slime mold", (struct fruit *) 0);
    u.ulevel = 1; u.uhp = u.uhpmax = 100;
    l_nhcore_init(); vision_init();
    flags.bones = FALSE;
    wizard = forced;
    /* Billing and greetings need a real hero form; generation-only probes
     * historically never entered a shop and did not need this setup. */
    gy.youmonst.data = &mons[gu.urole.mnum];
    u.umonnum = u.umonster = gu.urole.mnum;
    for (i = 0; i < A_MAX; ++i) ABASE(i) = AMAX(i) = 12;
    if (mode && !strcmp(mode, "library")) {
        const int depths[] = {5, 29, 30, 59, 60, 99, 100, 149, 150, 199};
        int j, k;
        step12_library_tables();
        for (i = 0; i < SIZE(depths); ++i)
            for (j = 0; j < 100; j += 5)
                for (k = 0; k < 2; ++k) {
                    step12_library_fixture(depths[i], k ? 299 : 0, j, k);
                    free_level();
                }
        for (j = 0; j < 20; ++j)
            for (k = 0; k < 2; ++k) {
                step12_library_loose_case(j, k);
                free_level();
            }
        puts("PASS Library deterministic native fixtures");
        return 0;
    }
    if (mode && !strcmp(mode, "metadata")) {
        metadata_roundtrip();
        return 0;
    }
#ifdef USE_ISAAC64
    if (mode && !strcmp(mode, "shop-types")) {
        shop_type_gate();
        return 0;
    }
#endif
    if (mode && !strcmp(mode, "excluded")) {
        s_level *slev;
        branch *br;
        for (slev = svs.sp_levchn; slev; slev = slev->next) {
            if (slev->dlevel.dnum != medusa_level.dnum
                && strncmp(slev->proto,"moria",5)
                && strncmp(slev->proto,"tomb",4)
                && strcmp(slev->proto,"moloch")
                && strncmp(slev->proto,"drgn",4)
                && strncmp(slev->proto,"sheol",5)) continue;
            u.uz = slev->dlevel;
            gi.in_mklev = TRUE;
            step11_generate();
            assert(!custom_generation.rolled && !custom_generation.emissions);
            for (i = 0; i < svn.nroom; ++i) assert(!svr.rooms[i].custom_id);
            printf("EXCLUDED|%s\n", slev->proto);
            free_level();
        }
        for (br = svb.branches; br; br = br->next) {
            if (br->end1.dnum != medusa_level.dnum) continue;
            u.uz = br->end1;
            gi.in_mklev = TRUE;
            step11_generate();
            assert(!custom_generation.rolled && !custom_generation.emissions);
            free_level();
        }
        /* Exercise branch proto/filler and quest/endgame paths as actual
         * generation, selecting an interior without an explicit special
         * where the branch provides one. */
        for (dl = 0; dl < svn.n_dgns; ++dl) {
            int levelnum;
            if (dl == medusa_level.dnum || dl == tutorial_dnum) continue;
            u.uz.dnum = dl; u.uz.dlevel = 1;
            for (levelnum = 1; levelnum <= svd.dungeons[dl].num_dunlevs; ++levelnum) {
                u.uz.dlevel = levelnum;
                if (!Is_special(&u.uz) && !Is_branchlev(&u.uz)) break;
            }
            if (levelnum > svd.dungeons[dl].num_dunlevs) u.uz.dlevel = 1;
            gi.in_mklev = TRUE;
            step11_generate();
            assert(!custom_generation.rolled && !custom_generation.emissions);
            for (i = 0; i < svn.nroom; ++i) assert(!svr.rooms[i].custom_id);
            printf("EXCLUDED_BRANCH|%s|proto=%s|filler=%s|maze=%d\n",
                   svd.dungeons[dl].dname, svd.dungeons[dl].proto,
                   svd.dungeons[dl].fill_lvl, svl.level.flags.is_maze_lev);
            free_level();
        }
        puts("PASS excluded special, Big Room, Rogue, Medusa, Castle, branch parents/interiors, proto/filler, quest/endgame and maze paths");
        return 0;
    }
    for (dl = 1; dl <= 195; ++dl) {
        if (min_v && max_v) {
            if (dl < min_dl || dl > max_dl)
                continue;
        } else if (!forced && dl > 14 && (dl < 40 || dl > 57)) {
            continue;
        }
        if (forced && dl < 2) continue;
        u.uz.dnum = medusa_level.dnum; u.uz.dlevel = dl;
        if (forced && (Is_special(&u.uz) || Is_branchlev(&u.uz))) continue;
        gi.in_mklev = TRUE;
        baby_requests = adult_requests = 0;
        step11_generate();
        step11_repeat_backends();
        assert(!custom_generation.rolled || custom_generation.context.ordinary_dod);
        if (custom_generation.emissions) {
            assert(custom_generation.emissions == 1);
            assert(custom_generation.progress == CUSTOM_COMPLETE);
            if (custom_generation.selected == CUSTOM_DRAGON_HALL) {
                assert(baby_requests >= 4 && baby_requests <= 6);
                assert(adult_requests >= 6 && adult_requests <= 10);
                printf("REQUESTS|baby=%d|adult=%d\n",baby_requests,adult_requests);
            }
        }
        printf("LEVEL|seed=%lu|dl=%d|diff=%d|rolled=%d|selected=%u|attempts=%d|emissions=%d|state=%d|vanilla_attempt=%d|vanilla_placements=%d|shop_candidate=%d|shop_result=%d|shop_success=%d|shop_selected_type=%d|shop_type=%d|shop_large=%d|shop_room_candidates=%d|shop_stairs_blocked=%d|shop_door_mismatch=%d|shop_invalid_shapes=%d",
               seed,dl,custom_generation.context.difficulty,custom_generation.rolled,
               custom_generation.selected,custom_generation.attempts,
               custom_generation.emissions,custom_generation.progress,
               custom_generation.vanilla_attempt,custom_generation.vanilla_placements,
               custom_generation.shop_candidate,
               custom_generation.vanilla_attempt == SHOPBASE,
               custom_generation.vanilla_attempt == SHOPBASE
                   && custom_generation.vanilla_placements > 0,
               custom_generation.shop_selected_type,
               custom_generation.shop_type, custom_generation.shop_large,
               custom_generation.shop_room_candidates,
               custom_generation.shop_stairs_blocked,
               custom_generation.shop_door_mismatch,
               custom_generation.shop_invalid_shapes);
        for (i = 0; i < custom_registry_count; ++i)
            printf("|e%d=%d", i+1, custom_eligible(&custom_registry[i], &custom_generation.context));
        puts("");
        for (i = 0; i < svn.nroom; ++i)
            if (svr.rooms[i].custom_id) {
                if (!min_v || !max_v || svr.rooms[i].custom_id == CUSTOM_LIBRARY)
                    validate_room(&svr.rooms[i]);
                ++samples;
            }
        if (bone_test && custom_generation.emissions) {
            accepted_bones();
            return 0;
        }
        if (escape_test && custom_generation.emissions) {
            escape_check();
            puts("PASS disconnected escape provisions and native restrictions");
            return 0;
        }
        if (forced && custom_generation.emissions) {
            boolean library = custom_generation.selected == CUSTOM_LIBRARY;
            level_roundtrip(FALSE);
            coexistence_and_entry();
            level_roundtrip(FALSE);
            level_roundtrip(TRUE);
            if (library)
                coexistence_and_entry(); /* restored/revisited Library stays discovered */
        }
        if (clean && custom_generation.selected) {
            assert(custom_generation.progress == CUSTOM_CLEAN_FAILURE);
            assert(custom_generation.attempts == 3 && !custom_generation.emissions);
            ++samples;
        }
        free_level();
        if (forced && samples >= 3) break;
    }
    if (forced) assert(samples == 3);
    puts("PASS native generation corpus");
    return 0;
}
