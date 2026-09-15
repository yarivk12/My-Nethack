#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

struct you u;
struct instance_globals_saved_m svm;
static uint32_t state, trace;
static unsigned calls, cells;
int rn2(int n) {
    assert(n > 0); ++calls;
    state = state * UINT32_C(1664525) + UINT32_C(1013904223);
    return (int) ((state >> 8) % (unsigned) n);
}
boolean In_mithardir(const d_level *l) { return l->dnum == 1; }
boolean In_mithardir_desert(const d_level *l) {
    return In_mithardir(l) && l->dlevel >= 2 && l->dlevel <= 4;
}
boolean In_mithardir_catacombs(const d_level *l) {
    return In_mithardir(l) && l->dlevel > 4;
}
struct permonst *mkclass(char cls, int flags) {
    assert(flags == G_NOHELL);
    /* Class expansion is native by policy; distinguish every requested class
       without importing unrelated donor species into the fixture. */
    return &mons[(unsigned char) cls];
}
static void mith_liquify(coordxy x, coordxy y, boolean edge) {
    assert(x >= 1 && x < COLNO && y >= 0 && y < ROWNO);
    ++cells;
    trace = trace * 16777619U ^ (unsigned) (x + COLNO * y + (edge ? 4096 : 0));
    /* Interleave reproducible terrain-side RNG to test the river caller's
       ordering under side effects, not only under a no-op callback. */
    if ((x + y) % 3 == 0) (void) rn2(13);
}
#include "step9c_generators.h"

int main(void) {
    uint32_t seed, endstate, expected;
    unsigned count, steps;
    int level;
    struct permonst *pm;
    monst_globals_init();
    for (seed = 1; seed <= 512; ++seed) {
        state = seed; trace = calls = cells = 0;
        donor_river(); expected = trace; endstate = state; count = cells; steps = calls;
        state = seed; trace = calls = cells = 0;
        mith_river();
        assert(trace == expected && state == endstate && cells == count && calls == steps);
    }
    puts("PASS 512 immutable-donor/production river paths, boundaries and RNG traces");
    u.uz.dnum = 1;
    for (level = 1; level <= 10; ++level) {
        u.uz.dlevel = level;
        for (seed = 1; seed <= 4096; ++seed) {
            state = seed; calls = 0; pm = donor_pool(); endstate = state; steps = calls;
            state = seed; calls = 0;
            assert(mith_rndmonst() == pm && state == endstate && calls == steps);
        }
    }
    u.uz.dnum = 0; calls = 0;
    assert(!mith_rndmonst() && !calls);
    puts("PASS 40960 immutable-donor/production pool selections; no outside-branch RNG");
    return 0;
}
