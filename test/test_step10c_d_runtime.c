/* Compile-gated native Step 10C-D production identity probe. */
#include "hack.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_ISAAC64
extern void init_isaac64(unsigned long, int (*)(int));
#endif

static d_level
level_of(const char *name)
{
    s_level *slev = find_level(name);
    assert(slev != 0);
    return slev->dlevel;
}

static void
assert_context(d_level lev, enum step10b_level_context expected)
{
    assert(step10c_level_context(&lev) == expected);
}

static void
load_map(const char *name)
{
    s_level *slev = find_level(name);

    assert(slev != 0);
    while (gf.ftrap)
        deltrap(gf.ftrap);
    u.uz = slev->dlevel;
    level_status_init();
    oinit();
    clear_level_structures();
    step10c_set_level_flags(&u.uz);
    makemaz(slev->proto);
}

static void
load_map_at(const char *name, d_level lev)
{
    s_level scratch, *saved_chain = svs.sp_levchn;
    s_level *slev = find_level(name);
    boolean temporary = (slev == 0);

    if (temporary) {
        (void) memset((genericptr_t) &scratch, 0, sizeof scratch);
        (void) strcpy(scratch.proto, name);
        scratch.dlevel = lev;
        scratch.next = saved_chain;
        svs.sp_levchn = &scratch;
        slev = &scratch;
    }
    assert(slev->dlevel.dnum == lev.dnum
           && slev->dlevel.dlevel == lev.dlevel);
    while (gf.ftrap)
        deltrap(gf.ftrap);
    u.uz = lev;
    level_status_init();
    oinit();
    clear_level_structures();
    step10c_set_level_flags(&u.uz);
    makemaz(slev->proto);
    if (temporary) {
        assert(svs.sp_levchn == &scratch);
        svs.sp_levchn = saved_chain;
    }
}

static int
magic_portals(boolean require_seen)
{
    struct trap *trap;
    int count = 0;

    for (trap = gf.ftrap; trap; trap = trap->ntrap)
        if (trap->ttyp == MAGIC_PORTAL) {
            ++count;
            if (require_seen)
                assert(trap->tseen);
        }
    return count;
}

static void
assert_all_portals_seen(void)
{
    (void) magic_portals(TRUE);
}

static unsigned long long
topology_hash(void)
{
    unsigned long long hash = 1469598103934665603ULL;
    const unsigned char *p;
    branch *br;
    s_level *slev;
    int i;

#define MIX(value) do { \
        hash ^= (unsigned long long) (value); \
        hash *= 1099511628211ULL; \
    } while (0)
    MIX(svn.n_dgns);
    for (i = 0; i < svn.n_dgns; ++i) {
        MIX(svd.dungeons[i].ledger_start);
        MIX(svd.dungeons[i].depth_start);
        MIX(svd.dungeons[i].num_dunlevs);
    }
    for (br = svb.branches; br; br = br->next) {
        MIX(br->id);
        MIX(br->type);
        MIX(br->end1.dnum);
        MIX(br->end1.dlevel);
        MIX(br->end2.dnum);
        MIX(br->end2.dlevel);
        MIX(br->end1_up);
    }
    for (slev = svs.sp_levchn; slev; slev = slev->next) {
        MIX(slev->dlevel.dnum);
        MIX(slev->dlevel.dlevel);
        for (p = (const unsigned char *) slev->proto; *p; ++p)
            MIX(*p);
    }
#undef MIX
    return hash;
}

static branch *
find_dispensary_branch(void)
{
    int neutral_dnum = level_of("gatetwn").dnum;
    branch *br;

    for (br = svb.branches; br; br = br->next)
        if (br->end1.dnum == neutral_dnum
            && br->end2.dnum == neutral_dnum
            && br->end2.dlevel == 8)
            return br;
    return (branch *) 0;
}

static void
whole_identity_matrix(void)
{
    static const char *const lost_names[] = {
        "leth-a-1", "leth-a-2", "lethe-b", "leth-c-1", "leth-c-2",
        "leth-d-1", "leth-d-2", "lethe-e", "lethe-f", "lethe-g",
        "lethe-z", "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c",
        "nkai-z", "rlyeh"
    };
    static const int lost_levels[] = {
        1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13
    };
    static const char *const neutral_names[] = {
        "gatetwn", "out1", "out2", "out3", "out4", "spire", "sumall"
    };
    d_level approach = level_of("neulev");
    int dod_dnum = approach.dnum;
    int neutral_dnum = level_of("gatetwn").dnum;
    int lost_dnum = level_of("lethe-b").dnum;
    d_level lev;
    branch *disp = find_dispensary_branch();
    int i;

    assert(dod_dnum >= 0 && neutral_dnum >= 0 && lost_dnum >= 0);
    assert(disp != 0);
    load_map("neulev");
    assert_context(approach, STEP10B_CTX_APPROACH);
    assert_all_portals_seen();

    for (i = 0; i < SIZE(neutral_names); ++i) {
        lev = level_of(neutral_names[i]);
        assert(lev.dnum == neutral_dnum);
        assert_context(lev, (enum step10b_level_context)
                       (STEP10B_CTX_GATE + i));
        load_map(neutral_names[i]);
        assert(!svl.level.flags.lethe);
        assert_all_portals_seen();
    }

    lev.dnum = neutral_dnum;
    lev.dlevel = 8;
    assert_context(lev, STEP10B_CTX_DISPENSARY);
    for (i = 2; i <= 6; ++i) {
        disp->end1.dlevel = (xint16) i;
        assert(depth(&lev) == depth(&(d_level) { neutral_dnum, 1 }) + i);
        load_map_at("lbyrnth", lev);
        assert(step10c_level_context(&u.uz) == STEP10B_CTX_DISPENSARY);
        assert(!svl.level.flags.lethe);
        printf("E5_DISPENSARY|parent_N=%d|depth=%d\n", i, depth(&lev));
    }

    for (i = 0; i < SIZE(lost_names); ++i) {
        lev.dnum = (xint16) lost_dnum;
        lev.dlevel = (xint16) lost_levels[i];
        assert_context(lev, lost_levels[i] == 13
                               ? STEP10B_CTX_RLYEH
                               : STEP10B_CTX_LOST_CITIES);
        load_map_at(lost_names[i], lev);
        assert(svl.level.flags.lethe);
        assert_all_portals_seen();
        printf("E5_LOST|name=%s|variant=%s\n", lost_names[i],
               (strstr(lost_names[i], "-1") != 0) ? "A"
                 : (strstr(lost_names[i], "-2") != 0) ? "B" : "fixed");
    }

    assert(svd.dungeons[dod_dnum].num_dunlevs == 200);
    assert(stronghold_level.dnum == dod_dnum
           && stronghold_level.dlevel == 200);
    printf("E5_MATRIX|resources=26|dispensary_parents=5|alternate_groups=4|"
           "lost_variants=8|topology_hash=%llu\n", topology_hash());
}

static void
set_file_mode(NHFILE *nhfp, int mode, int fd)
{
    init_nhfile(nhfp);
    nhfp->mode = mode;
    nhfp->ftype = NHF_SAVEFILE;
    nhfp->structlevel = TRUE;
    nhfp->fieldlevel = FALSE;
    nhfp->addinfo = FALSE;
    nhfp->fnidx = historical;
    nhfp->fd = fd;
}

static void
save_special_chain(NHFILE *nhfp)
{
    int count = 0;
    s_level *slev;

    for (slev = svs.sp_levchn; slev; slev = slev->next)
        ++count;
    Sfo_int(nhfp, &count, "step10c-e-special-count");
    for (slev = svs.sp_levchn; slev; slev = slev->next)
        Sfo_s_level(nhfp, slev, "step10c-e-special-level");
}

static void
restore_special_chain(NHFILE *nhfp)
{
    int count = 0;
    s_level *slev, *last = (s_level *) 0;

    Sfi_int(nhfp, &count, "step10c-e-special-count");
    assert(count > 0 && count < MAXLINFO);
    while (count-- > 0) {
        slev = (s_level *) alloc(sizeof *slev);
        Sfi_s_level(nhfp, slev, "step10c-e-special-level");
        slev->next = (s_level *) 0;
        if (last)
            last->next = slev;
        else
            svs.sp_levchn = slev;
        last = slev;
    }
}

static void
whole_persistence(const char *path)
{
    NHFILE *writer, *reader;
    branch *disp;
    d_level dispensary = { 0, 8 };
    unsigned long long before, after;
    int neutral_dnum, fd;

    (void) strcpy(svp.plname, "step10c-e-persistence");
    before = topology_hash();
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
              _S_IREAD | _S_IWRITE);
    assert(fd >= 0);
    writer = get_freeing_nhfile();
    assert(writer != 0);
    set_file_mode(writer, WRITING, fd);
    save_dungeon(writer, TRUE, FALSE);
    save_special_chain(writer);
    close_nhfile(writer);

    neutral_dnum = level_of("gatetwn").dnum;
    disp = find_dispensary_branch();
    assert(disp != 0);
    disp->end1.dlevel = 2;
    dispensary.dnum = (xint16) neutral_dnum;
    svd.dungeons[neutral_dnum].depth_start = 1;
    svs.sp_levchn = (s_level *) 0;

    fd = open(path, O_RDONLY | O_BINARY);
    assert(fd >= 0);
    reader = get_freeing_nhfile();
    assert(reader != 0);
    set_file_mode(reader, READING, fd);
    restore_dungeon(reader);
    restore_special_chain(reader);
    close_nhfile(reader);

    after = topology_hash();
    assert(after == before);
    disp = find_dispensary_branch();
    assert(disp != 0 && disp->end1.dlevel >= 2 && disp->end1.dlevel <= 6);
    assert(depth(&dispensary) == svd.dungeons[neutral_dnum].depth_start
           + disp->end1.dlevel);

    load_map("lethe-e");
    assert(svl.level.flags.lethe);
    load_map("lethe-z");
    assert(svl.level.flags.lethe);
    load_map("lbyrnth");
    assert(!svl.level.flags.lethe);
    {
        struct silver_key_domain domain;
        d_level target, candidate;

        assert(step10c_silver_key_domain(&domain));
        candidate = domain.approach;
        assert(silver_key_choose_destination(&candidate, 1, 0, &domain,
                                              &target));
        candidate.dnum = domain.neutral_dnum;
        candidate.dlevel = 8;
        assert(silver_key_choose_destination(&candidate, 1, 0, &domain,
                                              &target));
    }
    printf("E6_PERSISTENCE|topology_hash=%llu|dispensary_parent=%d|"
           "lethe=restored|auxiliaries=restored|silver_key=restored\n",
           after, disp->end1.dlevel);
}

int
step10c_d_test_main(void)
{
    d_level approach, neutral, lost, ordinary;
    struct silver_key_domain domain;
    int i;
    static const enum step10b_level_context neutral_contexts[] = {
        STEP10B_CTX_GATE, STEP10B_CTX_OUTLANDS_1,
        STEP10B_CTX_OUTLANDS_2, STEP10B_CTX_OUTLANDS_3,
        STEP10B_CTX_OUTLANDS_4, STEP10B_CTX_SPIRE, STEP10B_CTX_SUM
    };

    init_objects();
    flags.pantheon = -1;
    flags.initrole = flags.initrace = flags.initgend = flags.initalign = ROLE_NONE;
    (void) strcpy(svp.plname, "step10c-d-probe");
    svp.pl_character[0] = '\0';
    role_init();
    init_dungeons();
    init_artifacts();
    u.ulevel = 1;
    l_nhcore_init();
    vision_init();

    approach = level_of("neulev");
    neutral = level_of("gatetwn");
    lost = level_of("lethe-b");
    assert_context(approach, STEP10B_CTX_APPROACH);
    for (i = 1; i <= 7; ++i) {
        neutral.dlevel = (xint16) i;
        assert_context(neutral, neutral_contexts[i - 1]);
    }
    neutral.dlevel = 8;
    assert_context(neutral, STEP10B_CTX_DISPENSARY);
    for (i = 1; i <= 12; ++i) {
        lost.dlevel = (xint16) i;
        assert_context(lost, STEP10B_CTX_LOST_CITIES);
    }
    lost.dlevel = 13;
    assert_context(lost, STEP10B_CTX_RLYEH);

    assert(step10c_silver_key_domain(&domain));
    assert(on_level(&domain.approach, &approach));
    assert(domain.neutral_dnum == neutral.dnum);
    assert(domain.lost_cities_dnum == lost.dnum);
    assert(domain.dispensary.dnum == neutral.dnum
           && domain.dispensary.dlevel == 8);
    assert(silver_key_destination_valid(&domain.dispensary, &domain));

    ordinary = approach;
    ordinary.dlevel += ordinary.dlevel == 199 ? -1 : 1;
    assert_context(ordinary, STEP10B_CTX_NONE);
    ordinary.dnum = lost.dnum;
    ordinary.dlevel = 14;
    assert_context(ordinary, STEP10B_CTX_NONE);

    neutral = level_of("gatetwn");
    assert(step10b_hero_spell_chance(step10c_level_context(&neutral),
                                     100, 2) == 80);
    neutral.dlevel = 5;
    assert(step10b_hero_spell_chance(step10c_level_context(&neutral),
                                     100, 2) == 0);
    neutral.dlevel = 6;
    assert(step10b_mon_spell_always_fumbles(step10c_level_context(&neutral)));
    neutral.dlevel = 7;
    assert(step10b_hero_spell_chance(step10c_level_context(&neutral),
                                     20, 3) == 90);
    assert(step10b_mon_spell_fumble_threshold(step10c_level_context(&neutral),
                                               2) == 1);
    assert(!step10b_tree_kick_has_loot(step10c_level_context(&neutral)));
    assert(step10b_mirror_pit_damage(step10c_level_context(&neutral), TRUE,
                                     7, 13) == 20);
    lost = level_of("lethe-b");
    assert(step10b_terrain_color(step10c_level_context(&lost), S_vwall,
                                 CLR_GRAY) == CLR_BLACK);
    lost.dlevel = 13;
    assert(step10b_terrain_color(step10c_level_context(&lost), S_vwall,
                                 CLR_GRAY) == CLR_BRIGHT_BLUE);

    for (i = 1; i <= 13; ++i) {
        lost.dlevel = (xint16) i;
        clear_level_structures();
        step10c_set_level_flags(&lost);
        assert(svl.level.flags.lethe);
    }
    clear_level_structures();
    step10c_set_level_flags(&neutral);
    assert(!svl.level.flags.lethe);

    load_map("neulev");
    assert(magic_portals(TRUE) >= 1);
    load_map("gatetwn");
    assert(magic_portals(TRUE) >= 1);
    load_map("out1");
    assert(magic_portals(TRUE) == 2);

    load_map("lethe-e");
    {
        struct monst *mon;
        int bridge_priests = 0;
        for (mon = fmon; mon; mon = mon->nmon)
            if (mon->data == &mons[PM_BLASPHEMOUS_LURKER]
                && mon->ispriest && has_epri(mon))
                ++bridge_priests;
        assert(bridge_priests == 1);
    }

    load_map("lethe-z");
    {
        struct obj *obj;
        int swords = 0;
        for (obj = fobj; obj; obj = obj->nobj)
            if (obj->otyp == LONG_SWORD && has_oname(obj)
                && !strcmp(ONAME(obj), "The Sword of the Deeps")) {
                ++swords;
                assert((obj->obranch_props & OBP_DEEP) != 0UL);
                assert(obj->spe == 12 && obj->cursed && !obj->blessed);
                assert(obj->oartifact == 0);
            }
        assert(swords == 1);
    }

    {
        int sample, shops = 0;
        for (sample = 0; sample < 200 && !shops; ++sample) {
            struct monst *mon;
#ifdef USE_ISAAC64
            init_isaac64(0x10cdUL + (unsigned long) sample, rn2);
#endif
            load_map("out2");
            for (mon = fmon; mon; mon = mon->nmon)
                if (mon->isshk && has_eshk(mon)) {
                    ++shops;
                    assert(mon->data == &mons[PM_PLUMACH_RILMANI]);
                }
        }
        assert(shops > 0);
    }
    if (getenv("NETHACK_STEP10C_E_MATRIX"))
        whole_identity_matrix();
    if (getenv("NETHACK_STEP10C_E_PERSISTENCE"))
        whole_persistence(getenv("NETHACK_STEP10C_E_SAVE")
                          ? getenv("NETHACK_STEP10C_E_SAVE")
                          : "step10c-e-save.bin");
    puts("PASS Step 10C-D native identity, content, auxiliaries, and B4 contexts");
    return 0;
}
