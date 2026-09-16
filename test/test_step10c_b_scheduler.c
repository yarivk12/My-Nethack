/* Exactly 1,000 deterministic seeds through extracted production scheduler
 * bodies.  The fixture supplies state/RNG only; it does not copy the algorithm. */
#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct you u;
struct instance_globals_saved_b svb;
struct instance_globals_saved_d svd;
struct instance_globals_saved_s svs;
struct instance_globals_saved_n svn;
static unsigned rng_state;
static branch temple, tomb, moria, sheol, dragon, mithardir, neutral, lost;
static s_level medusa, castle, mith_approach, neutral_approach;
static s_level alt_a, alt_c, alt_d, alt_nkai;

void panic(const char *fmt, ...) { fprintf(stderr, "%s\n", fmt); abort(); }
void impossible(const char *fmt, ...) { fprintf(stderr, "%s\n", fmt); abort(); }
long *alloc(unsigned n) { return (long *) calloc(1, n); }
int rn2(int n) {
    rng_state = rng_state * 1664525U + 1013904223U;
    return (int) ((rng_state >> 1) % (unsigned) n);
}
int dname_to_dnum(const char *s) {
    return !strcmp(s, "The Dungeons of Doom") ? 0
        : !strcmp(s, "The Temple of Moloch") ? 1
        : !strcmp(s, "The Lost Tomb") ? 2
        : !strcmp(s, "The Ruins of Moria") ? 3
        : !strcmp(s, "Sheol") ? 4
        : !strcmp(s, "The Dragon Caves") ? 5
        : !strcmp(s, "Mithardir") ? 6
        : !strcmp(s, "Neutral Quest") ? 7
        : !strcmp(s, "The Lost Cities") ? 8 : -1;
}
s_level *find_level(const char *s) {
    s_level *p;
    for (p = svs.sp_levchn; p; p = p->next)
        if (!strcmp(p->proto, s)) return p;
    return NULL;
}
static void add_level(s_level *p) { p->next = svs.sp_levchn; svs.sp_levchn = p; }
void insert_branch(branch *p, boolean extract) {
    if (!extract) { p->next = svb.branches; svb.branches = p; }
}

#include "step10c_b_scheduler_functions.h"

static void
init_special(s_level *lev, const char *name, int dnum, int dlevel,
             s_level *next)
{
    memset(lev, 0, sizeof *lev);
    strcpy(lev->proto, name);
    lev->dlevel.dnum = (xint16) dnum;
    lev->dlevel.dlevel = (xint16) dlevel;
    lev->next = next;
}

static void
fresh_state(unsigned seed)
{
    memset(&svd, 0, sizeof svd);
    memset(&svs, 0, sizeof svs);
    memset(&svb, 0, sizeof svb);
    memset(&temple, 0, sizeof temple);
    memset(&tomb, 0, sizeof tomb);
    memset(&moria, 0, sizeof moria);
    memset(&sheol, 0, sizeof sheol);
    memset(&dragon, 0, sizeof dragon);
    memset(&mithardir, 0, sizeof mithardir);
    memset(&neutral, 0, sizeof neutral);
    memset(&lost, 0, sizeof lost);
    rng_state = seed;
    svn.n_dgns = 9;
    svd.dungeons[0].num_dunlevs = 200;
    svd.dungeons[0].depth_start = 1;
    svd.dungeons[1].entry_lev = svd.dungeons[2].entry_lev = 1;
    svd.dungeons[3].entry_lev = 6;
    svd.dungeons[4].entry_lev = svd.dungeons[5].entry_lev
        = svd.dungeons[6].entry_lev = svd.dungeons[7].entry_lev = 1;
    svd.dungeons[7].num_dunlevs = 8;
    svd.dungeons[8].entry_lev = 2;
    svd.dungeons[8].num_dunlevs = 13;

    temple.end1.dlevel = 30; temple.end2.dnum = 1; temple.end2.dlevel = 1;
    tomb.end1.dlevel = 31; tomb.end2.dnum = 2; tomb.end2.dlevel = 1;
    moria.end1.dlevel = 45; moria.end1_up = TRUE;
    moria.end2.dnum = 3; moria.end2.dlevel = 6;
    sheol.end1.dlevel = 108; sheol.end2.dnum = 4; sheol.end2.dlevel = 1;
    dragon.end1.dlevel = 109; dragon.end2.dnum = 5; dragon.end2.dlevel = 1;
    mithardir.end1.dlevel = 110; mithardir.end2.dnum = 6;
    mithardir.end2.dlevel = 1; mithardir.type = BR_PORTAL;
    neutral.end1.dlevel = 111; neutral.end2.dnum = 7;
    neutral.end2.dlevel = 1; neutral.type = BR_PORTAL;
    lost.end1.dnum = 7; lost.end1.dlevel = 7;
    lost.end2.dnum = 8; lost.end2.dlevel = 2;
    temple.next = &tomb; tomb.next = &moria; moria.next = &sheol;
    sheol.next = &dragon; dragon.next = &mithardir;
    mithardir.next = &neutral; neutral.next = &lost;
    svb.branches = &temple;

    init_special(&mith_approach, "chalv2", 0, 110, NULL);
    init_special(&neutral_approach, "neulev", 0, 111, &mith_approach);
    init_special(&alt_nkai, "nkai-a-1", 8, 9, &neutral_approach);
    init_special(&alt_d, "leth-d-1", 8, 4, &alt_nkai);
    init_special(&alt_c, "leth-c-1", 8, 3, &alt_d);
    init_special(&alt_a, "leth-a-1", 8, 1, &alt_c);
    init_special(&castle, "castle", 0, 200, &alt_a);
    init_special(&medusa, "medusa", 0, 196, &castle);
    svs.sp_levchn = &medusa;
}

static branch *
dispensary_branch(void)
{
    branch *br;
    for (br = svb.branches; br; br = br->next)
        if (br->end1.dnum == 7 && br->end2.dnum == 7
            && br->end2.dlevel == STEP10C_DISPENSARY_LEVEL)
            return br;
    return NULL;
}

static unsigned long long
state_hash(void)
{
    unsigned long long h = 1469598103934665603ULL;
    branch *br;
    s_level *lev;
    const unsigned char *p;
    int i;
#define MIX(v) do { h ^= (unsigned long long) (v); h *= 1099511628211ULL; } while (0)
    for (br = svb.branches; br; br = br->next) {
        MIX(br->type); MIX(br->end1.dnum); MIX(br->end1.dlevel);
        MIX(br->end2.dnum); MIX(br->end2.dlevel); MIX(br->end1_up);
    }
    for (lev = svs.sp_levchn; lev; lev = lev->next) {
        MIX(lev->dlevel.dnum); MIX(lev->dlevel.dlevel);
        for (p = (const unsigned char *) lev->proto; *p; ++p) MIX(*p);
    }
    for (i = 0; i < svn.n_dgns; ++i) MIX(svd.dungeons[i].depth_start);
#undef MIX
    return h;
}

static void
cleanup_dynamic(void)
{
    s_level *lev = svs.sp_levchn, *next;
    branch *disp = dispensary_branch();
    while (lev) {
        next = lev->next;
        if (lev != &medusa && lev != &castle && lev != &mith_approach
            && lev != &neutral_approach && lev != &alt_a && lev != &alt_c
            && lev != &alt_d && lev != &alt_nkai)
            free(lev);
        lev = next;
    }
    free(disp);
}

static void
representative_topology(void)
{
    static const int parents[] = { 30, 100, 199 };
    static const int dispensary_parents[] = { 2, 4, 6 };
    int i;

    for (i = 0; i < SIZE(parents); ++i) {
        d_level neutral_first = { 7, 1 }, neutral_last = { 7, 7 };
        d_level lost_first = { 8, 1 }, lost_entry = { 8, 2 };
        d_level lost_last = { 8, 13 };
        d_level dispensary = { 7, STEP10C_DISPENSARY_LEVEL };
        branch *disp;
        int p = parents[i], n = dispensary_parents[i];

        fresh_state(1U);
        neutral.end1.dlevel = (xint16) p;
        step6b_rebase_level(&neutral_approach, p);
        step6b_rebase_branch(&neutral);
        step6b_rebase_branch(&lost);
        disp = step10c_internal_branch(7, n);

        assert(svn.n_dgns == 9 && svn.n_dgns <= MAXDUNGEON);
        assert(svd.dungeons[0].num_dunlevs == 200);
        assert(medusa.dlevel.dlevel == 196);
        assert(castle.dlevel.dnum == 0 && castle.dlevel.dlevel == 200);
        assert(neutral.end1.dnum == 0 && neutral.end1.dlevel == p);
        assert(neutral.end2.dnum == 7 && neutral.end2.dlevel == 1);
        assert(neutral_approach.dlevel.dnum == 0
               && neutral_approach.dlevel.dlevel == p);
        assert(lost.end1.dnum == 7 && lost.end1.dlevel == 7);
        assert(lost.end2.dnum == 8 && lost.end2.dlevel == 2);
        assert(disp && disp->type == BR_STAIR);
        assert(disp->end1.dnum == 7 && disp->end1.dlevel == n);
        assert(disp->end2.dnum == 7
               && disp->end2.dlevel == STEP10C_DISPENSARY_LEVEL);
        assert(depth(&neutral_first) == p);
        assert(depth(&neutral_last) == p + 6);
        assert(depth(&lost_first) == p + 6);
        assert(depth(&lost_entry) == p + 7);
        assert(depth(&lost_last) == p + 18);
        assert(depth(&dispensary) == p + n);
        assert(depth(&castle.dlevel) == 200);
        if (p == 199) {
            d_level below = { 0, 200 };
            assert(depth(&below) == 200);
            assert(depth(&lost_last) == 217);
        }
        cleanup_dynamic();
    }
    puts("PASS Step 10C-B representative P=30/middle/P=199 topology and depths");
}

static void
assert_scheduled_state(void)
{
    const int parents[] = {
        sheol.end1.dlevel, dragon.end1.dlevel, mithardir.end1.dlevel,
        temple.end1.dlevel, moria.end1.dlevel, tomb.end1.dlevel,
        neutral.end1.dlevel
    };
    int i, j, bigrooms = 0, giant = 0, zoos = 0, dragonrooms = 0;
    int neutral_branches = 0, dispensaries = 0;
    branch *br;
    s_level *lev;

    assert(svn.n_dgns <= MAXDUNGEON);
    assert(svd.dungeons[0].num_dunlevs == 200);
    assert(svd.dungeons[7].num_dunlevs == 8);
    assert(svd.dungeons[8].num_dunlevs == 13);
    assert(castle.dlevel.dnum == 0 && castle.dlevel.dlevel == 200);
    assert(medusa.dlevel.dnum == 0 && medusa.dlevel.dlevel == 196);
    for (i = 0; i < SIZE(parents); ++i) {
        assert(parents[i] >= STEP6B_MIN_LEVEL
               && parents[i] <= STEP6B_MAX_LEVEL);
        for (j = i + 1; j < SIZE(parents); ++j)
            assert(parents[i] != parents[j]);
    }
    assert(temple.end1.dlevel <= medusa.dlevel.dlevel);
    assert(!step6b_depth_used(neutral.end1.dlevel));

    for (br = svb.branches; br; br = br->next) {
        if (br->end1.dnum == 0 && br->end2.dnum == 7)
            ++neutral_branches;
        if (br->end1.dnum == 7 && br->end2.dnum == 7
            && br->end2.dlevel == STEP10C_DISPENSARY_LEVEL)
            ++dispensaries;
    }
    assert(neutral_branches == 1 && dispensaries == 1);
    assert((!strcmp(alt_a.proto, "leth-a-1")
            || !strcmp(alt_a.proto, "leth-a-2")));
    assert((!strcmp(alt_c.proto, "leth-c-1")
            || !strcmp(alt_c.proto, "leth-c-2")));
    assert((!strcmp(alt_d.proto, "leth-d-1")
            || !strcmp(alt_d.proto, "leth-d-2")));
    assert((!strcmp(alt_nkai.proto, "nkai-a-1")
            || !strcmp(alt_nkai.proto, "nkai-a-2")));

    for (lev = svs.sp_levchn; lev; lev = lev->next) {
        if (!strcmp(lev->proto, "bigrm"))
            ++bigrooms;
        else if (!strcmp(lev->proto, "x6b-giant"))
            ++giant;
        else if (!strcmp(lev->proto, "x6b-realzoo"))
            ++zoos;
        else if (!strcmp(lev->proto, "x6b-dragon"))
            ++dragonrooms;
    }
    assert(bigrooms >= 3 && bigrooms <= 5);
    /* Step 11 removes only room reservations, not topology reservations. */
    assert(giant == 0 && zoos == 0 && dragonrooms == 0);
}

int
main(void)
{
    int seed, repeat;
    unsigned long long expected = 0;
    representative_topology();
    for (seed = 1; seed <= 1000; ++seed) {
        for (repeat = 0; repeat < 2; ++repeat) {
            branch *disp;
            d_level dlev = { 7, STEP10C_DISPENSARY_LEVEL };
            fresh_state((unsigned) seed);
            step6b_schedule();
            assert_scheduled_state();
            disp = dispensary_branch();
            assert(disp && disp->end1.dlevel >= 2 && disp->end1.dlevel <= 6);
            assert(neutral.end1.dlevel >= 30 && neutral.end1.dlevel <= 199);
            assert(neutral_approach.dlevel.dlevel == neutral.end1.dlevel);
            assert(svd.dungeons[7].depth_start == neutral.end1.dlevel);
            assert(svd.dungeons[8].depth_start == neutral.end1.dlevel + 6);
            assert(depth(&dlev) == neutral.end1.dlevel + disp->end1.dlevel);
            assert(neutral.end1.dlevel != sheol.end1.dlevel
                   && neutral.end1.dlevel != dragon.end1.dlevel
                   && neutral.end1.dlevel != mithardir.end1.dlevel);
            if (!repeat) {
                expected = state_hash();
                printf("SEED|%d|P=%d|A=%d|C=%d|D=%d|N=%d|DISP=%d\n",
                       seed, neutral.end1.dlevel,
                       !strcmp(alt_a.proto, "leth-a-2"),
                       !strcmp(alt_c.proto, "leth-c-2"),
                       !strcmp(alt_d.proto, "leth-d-2"),
                       !strcmp(alt_nkai.proto, "nkai-a-2"),
                       disp->end1.dlevel);
            } else {
                assert(state_hash() == expected);
            }
            cleanup_dynamic();
        }
    }
    puts("PASS Step 10C-B exactly 1000 deterministic seeds with idempotence");
    return 0;
}
