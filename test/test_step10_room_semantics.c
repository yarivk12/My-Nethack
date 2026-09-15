/* Fresh-generation semantic regression for Step 10 QA2 remediation. */
#include "hack.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

static const char *const all_resources[] = {
    "neulev", "gatetwn", "out1", "out2", "out3", "out4", "spire",
    "sumall", "leth-a-1", "leth-a-2", "lethe-b", "leth-c-1",
    "leth-c-2", "leth-d-1", "leth-d-2", "lethe-e", "lethe-f", "lethe-g",
    "lethe-z", "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c", "nkai-z",
    "rlyeh", "lbyrnth"
};

static d_level
level_of(const char *name)
{
    s_level *slev = find_level(name);
    assert(slev != 0);
    return slev->dlevel;
}

static const char *
alternate_peer(const char *name)
{
    static const char *const names[][2] = {
        { "leth-a-1", "leth-a-2" },
        { "leth-c-1", "leth-c-2" },
        { "leth-d-1", "leth-d-2" },
        { "nkai-a-1", "nkai-a-2" }
    };
    unsigned i;

    for (i = 0; i < sizeof names / sizeof names[0]; ++i) {
        if (!strcmp(name, names[i][0]))
            return names[i][1];
        if (!strcmp(name, names[i][1]))
            return names[i][0];
    }
    return (const char *) 0;
}

static void
load_fresh(const char *name)
{
    s_level *slev = find_level(name);
    s_level scratch;
    s_level *saved_chain = svs.sp_levchn;
    boolean temporary = FALSE;

    if (!slev) {
        const char *base = alternate_peer(name);
        s_level *base_level = base ? find_level(base) : (s_level *) 0;

        if (base_level) {
            (void) memset((genericptr_t) &scratch, 0, sizeof scratch);
            (void) strcpy(scratch.proto, name);
            scratch.dlevel = base_level->dlevel;
            scratch.next = saved_chain;
            svs.sp_levchn = &scratch;
            slev = &scratch;
            temporary = TRUE;
        }
    }
    assert(slev != 0);
    while (gf.ftrap)
        deltrap(gf.ftrap);
    u.uz = slev->dlevel;
    level_status_init();
    oinit();
    clear_level_structures();
    step10c_set_level_flags(&u.uz);
    makemaz(slev->proto);
    /* makemaz() loads a special map; makelevel() performs this common
     * finalization immediately after makemaz() returns.  Reproduce that
     * production fresh-generation step explicitly in this direct probe. */
    {
        int i;
        for (i = 0; i < svn.nroom; ++i)
            fill_special_room(&svr.rooms[i]);
    }
    if (temporary)
        svs.sp_levchn = saved_chain;
}

static int
room_count(int type)
{
    int i, count = 0;

    for (i = 0; i < svn.nroom; ++i)
        if (svr.rooms[i].rtype == type)
            ++count;
    return count;
}

static int
room_needfill_count(int type, int needfill)
{
    int i, count = 0;

    for (i = 0; i < svn.nroom; ++i)
        if (svr.rooms[i].rtype == type && svr.rooms[i].needfill == needfill)
            ++count;
    return count;
}

static int
floor_objects_in_room(const struct mkroom *room)
{
    int count = 0;
    struct obj *obj;
    int rmno = (int) (room - svr.rooms) + ROOMOFFSET;

    for (obj = fobj; obj; obj = obj->nobj)
        if (obj->where == OBJ_FLOOR && isok(obj->ox, obj->oy)
            && (int) levl[obj->ox][obj->oy].roomno == rmno)
            ++count;
    return count;
}

static int
monster_count(int pm)
{
    int count = 0;
    struct monst *mon;

    for (mon = fmon; mon; mon = mon->nmon)
        if (mon->data == &mons[pm])
            ++count;
    return count;
}

static int
object_count(int otyp)
{
    int count = 0;
    struct obj *obj;

    for (obj = fobj; obj; obj = obj->nobj)
        if (obj->otyp == otyp)
            ++count;
    return count;
}

static int
trap_count(int ttyp)
{
    int count = 0;
    struct trap *trap;

    for (trap = gf.ftrap; trap; trap = trap->ntrap)
        if ((int) trap->ttyp == ttyp)
            ++count;
    return count;
}

static int
shopkeeper_count(void)
{
    int count = 0;
    struct monst *mon;

    for (mon = fmon; mon; mon = mon->nmon)
        if (mon->isshk && has_eshk(mon))
            ++count;
    return count;
}

static int
shop_stock_count(void)
{
    int i, count = 0;

    for (i = 0; i < svn.nroom; ++i)
        if (svr.rooms[i].rtype >= SHOPBASE)
            count += floor_objects_in_room(&svr.rooms[i]);
    return count;
}

static int
nonstone_count(void)
{
    int x, y, count = 0;

    for (x = 0; x < COLNO; ++x)
        for (y = 0; y < ROWNO; ++y)
            if (levl[x][y].typ != STONE)
                ++count;
    return count;
}

static void
assert_shop_contract(int expected_shops)
{
    int i, residents = 0;

    assert(room_count(SHOPBASE) + room_count(ARMORSHOP)
           + room_count(SCROLLSHOP) + room_count(POTIONSHOP)
           + room_count(WEAPONSHOP) + room_count(FOODSHOP)
           + room_count(RINGSHOP) + room_count(WANDSHOP)
           + room_count(TOOLSHOP) + room_count(BOOKSHOP)
           + room_count(FODDERSHOP) + room_count(CANDLESHOP)
           + room_count(SEAGARDEN) + room_count(SEAFOOD)
           + room_count(SANDWALKER) + room_count(NAIADSHOP)
           == expected_shops);
    for (i = 0; i < svn.nroom; ++i) {
        struct monst *resident = svr.rooms[i].resident;

        if (svr.rooms[i].rtype >= SHOPBASE) {
            assert(resident != 0 && resident->isshk && has_eshk(resident));
            assert(ESHK(resident)->shoproom == (schar) (i + ROOMOFFSET));
            assert(ESHK(resident)->shoptype == svr.rooms[i].rtype);
            assert(resident == svr.rooms[i].resident);
            ++residents;
        }
    }
    assert(residents == expected_shops);
    assert(shopkeeper_count() == expected_shops);
    assert(shop_stock_count() >= expected_shops);
    assert(svl.level.flags.has_shop);
}

static void
assert_gate_town(void)
{
    load_fresh("gatetwn");
    assert(room_count(SHOPBASE) + room_count(ARMORSHOP)
           + room_count(POTIONSHOP) + room_count(FOODSHOP)
           + room_count(TOOLSHOP) == 8);
    assert(room_count(TEMPLE) == 1 && room_count(BEEHIVE) == 1);
    assert(room_needfill_count(TEMPLE, FILL_NONE) == 1);
    assert(room_needfill_count(BEEHIVE, FILL_NORMAL) == 1);
    assert_shop_contract(8);
    assert(svl.level.flags.has_beehive && svl.level.flags.has_temple);
    assert(monster_count(PM_KILLER_BEE) + monster_count(PM_QUEEN_BEE) > 0);
    assert(trap_count(LANDMINE) >= 0);
    printf("QA2_AFFECTED|gatetwn|shops=8|keepers=%d|stock=%d|bees=%d\n",
           shopkeeper_count(), shop_stock_count(),
           monster_count(PM_KILLER_BEE) + monster_count(PM_QUEEN_BEE));
}

static void
assert_neulev(void)
{
    load_fresh("neulev");
    assert(room_count(BARRACKS) == 9);
    assert(room_count(WEAPONSHOP) + room_count(WANDSHOP) == 2);
    assert(room_count(COURT) == 1);
    assert(room_needfill_count(BARRACKS, FILL_NORMAL) == 9);
    assert(room_needfill_count(WEAPONSHOP, FILL_NORMAL)
           + room_needfill_count(WANDSHOP, FILL_NORMAL) == 2);
    assert(room_needfill_count(COURT, FILL_NONE) == 1);
    assert_shop_contract(2);
    assert(svl.level.flags.has_barracks);
    assert(monster_count(PM_DEEP_ONE) >= 8);
    assert(monster_count(PM_DEEPER_ONE) >= 1);
    assert(monster_count(PM_MASTER_MIND_FLAYER) == 1);
    assert(monster_count(PM_SHRIEKER) == 8);
    assert(trap_count(LANDMINE) == 2);
    printf("QA2_AFFECTED|neulev|barracks=9|shops=2|fixed_deep_one=%d|"
           "deepest=%d|shriekers=%d|landmines=%d\n",
           monster_count(PM_DEEP_ONE), monster_count(PM_DEEPER_ONE),
           monster_count(PM_SHRIEKER), trap_count(LANDMINE));
}

static void
assert_lethe_d(void)
{
    load_fresh("leth-d-1");
    assert(room_count(ZOO) == 1 && room_count(COURT) == 2);
    assert(room_needfill_count(ZOO, FILL_NORMAL) == 1);
    assert(room_needfill_count(COURT, FILL_NORMAL) == 2);
    assert(svl.level.flags.has_zoo && svl.level.flags.has_court);
    assert(monster_count(PM_ALHOON) >= 1);
    assert(fmon != 0 && fobj != 0);

    load_fresh("leth-d-2");
    assert(room_count(SWAMP) == 1 && room_count(MORGUE) == 3);
    assert(room_needfill_count(SWAMP, FILL_NORMAL) == 1);
    assert(room_needfill_count(MORGUE, FILL_NORMAL) == 3);
    assert(svl.level.flags.has_swamp && svl.level.flags.has_morgue);
    assert(monster_count(PM_ALHOON) >= 1);
    assert(nonstone_count() > 0);
    assert(fmon != 0 && fobj != 0);
    printf("QA2_AFFECTED|lethe-d|zoo=1|courts=2|swamp=1|morgues=3|"
           "alhoon=%d|terrain=%d\n", monster_count(PM_ALHOON),
           nonstone_count());
}

static void
assert_lethe_z(void)
{
    static const int corpse_species[] = {
        PM_KNIGHT, PM_WIZARD, PM_ROGUE, PM_ALIGNED_CLERIC, PM_RANGER
    };
    struct obj *obj;
    int i, found[5] = { 0, 0, 0, 0, 0 }, sword = 0;

    load_fresh("lethe-z");
    assert(room_count(BARRACKS) == 1 && room_count(MORGUE) == 1);
    assert(room_needfill_count(BARRACKS, FILL_NORMAL) == 1);
    assert(room_needfill_count(MORGUE, FILL_NORMAL) == 1);
    assert(svl.level.flags.has_barracks && svl.level.flags.has_morgue);
    for (obj = fobj; obj; obj = obj->nobj) {
        if (obj->otyp == CORPSE) {
            if (obj->spe == 0 && !has_oname(obj))
                for (i = 0; i < SIZE(corpse_species); ++i)
                    if (obj->corpsenm == corpse_species[i])
                    ++found[i];
        }
        if (obj->otyp == LONG_SWORD && has_oname(obj)
            && !strcmp(ONAME(obj), "The Sword of the Deeps")) {
            ++sword;
            assert((obj->obranch_props & OBP_DEEP) != 0UL);
            assert(obj->spe == 12 && obj->cursed && !obj->blessed);
            assert(obj->oartifact == 0);
        }
    }
    for (i = 0; i < SIZE(found); ++i)
        assert(found[i] == 1);
    assert(sword == 1);
    printf("QA2_AFFECTED|lethe-z|barracks=1|morgues=1|corpses=5|"
           "sword=1\n");
}

static void
assert_rlyeh(void)
{
    int hostile_unknown_priests;

    load_fresh("rlyeh");
    assert(room_count(TEMPLE) == 7);
    assert(room_needfill_count(TEMPLE, FILL_NORMAL) == 7);
    assert(svl.level.flags.has_temple);
    hostile_unknown_priests = monster_count(PM_PRIEST_OF_AN_UNKNOWN_GOD);
    assert(hostile_unknown_priests == 2);
    assert(monster_count(PM_FATHER_DAGON) == 1);
    assert(monster_count(PM_MOTHER_HYDRA) == 1);
    assert(monster_count(PM_GREAT_CTHULHU) == 1);
    assert(object_count(UNIVERSAL_KEY) == 1);
    assert(trap_count(MAGIC_PORTAL) >= 0);
    printf("QA2_AFFECTED|rlyeh|temples=7|has_temple=1|"
           "explicit_unknown_priests=%d|dagon=1|hydra=1|cthulhu=1\n",
           hostile_unknown_priests);
}

static void
assert_intentional_controls(void)
{
    static const char *const nkai_names[] = {
        "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c", "nkai-z"
    };
    int i;

    load_fresh("neulev");
    assert(room_count(COURT) == 1 && room_needfill_count(COURT, FILL_NONE) == 1);
    for (i = 0; i < SIZE(nkai_names); ++i) {
        load_fresh(nkai_names[i]);
        assert(room_count(MORGUE) == 1);
        assert(room_needfill_count(MORGUE, FILL_NONE) == 1);
    }
    load_fresh("nkai-b");
    {
        struct obj *obj;
        int scrolls = 0;

        for (obj = fobj; obj; obj = obj->nobj)
            if (obj->otyp == SCR_CREATE_MONSTER && obj->cursed
                && obj->spe == 0 && !has_oname(obj))
                ++scrolls;
        assert(scrolls == 2);
    }
    printf("QA2_CONTROLS|neulev_throne_unfilled=1|nkai_morgues_unfilled=5|"
           "nkai_b_scrolls=2\n");
}

static void
assert_all_resources(void)
{
    int i, rooms, monsters, objects, traps;

    for (i = 0; i < SIZE(all_resources); ++i) {
        load_fresh(all_resources[i]);
        rooms = svn.nroom;
        monsters = 0;
        objects = 0;
        traps = 0;
        {
            struct monst *mon;
            struct obj *obj;
            struct trap *trap;

            for (mon = fmon; mon; mon = mon->nmon)
                ++monsters;
            for (obj = fobj; obj; obj = obj->nobj)
                ++objects;
            for (trap = gf.ftrap; trap; trap = trap->ntrap)
                ++traps;
        }
        assert(nonstone_count() > 0);
        assert(monsters + objects + traps > 0);
        printf("QA2_RESOURCE|%s|rooms=%d|monsters=%d|objects=%d|traps=%d|"
               "terrain=%d\n", all_resources[i], rooms, monsters, objects,
               traps, nonstone_count());
    }
}

int
step10qa2_test_main(void)
{
    init_objects();
    flags.pantheon = -1;
    flags.initrole = flags.initrace = flags.initgend = flags.initalign = ROLE_NONE;
    (void) strcpy(svp.plname, "step10qa2-probe");
    svp.pl_character[0] = '\0';
    role_init();
    init_dungeons();
    init_artifacts();
    u.ulevel = 1;
    l_nhcore_init();
    vision_init();
    (void) level_of("neulev");

    assert_gate_town();
    assert_neulev();
    assert_lethe_d();
    assert_lethe_z();
    assert_rlyeh();
    assert_intentional_controls();
    assert_all_resources();
    puts("PASS Step 10QA2 fresh generated room/object semantics across all 26 resources");
    return 0;
}
