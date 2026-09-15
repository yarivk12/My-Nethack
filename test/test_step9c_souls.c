#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
static uint32_t rng;
static int redraws, deaths;
int rn2(int n) { assert(n > 0); rng = rng * 1664525U + 1013904223U; return (rng >> 8) % n; }
int rnd(int n) { return rn2(n) + 1; }
boolean is_home_elemental(struct permonst *p) { (void) p; return FALSE; }
void set_mon_data(struct monst *m, struct permonst *p) { m->data = p; }
void newsym(coordxy x, coordxy y) { assert(x > 0 && x < COLNO && y >= 0 && y < ROWNO); ++redraws; }
void mondied(struct monst *m) { m->mhp = 0; ++deaths; }
#undef canspotmon
#define canspotmon(m) FALSE
#undef pline
#define pline(...) ((void) 0)
#define pline_mon(...) ((void) 0)
#undef update_inventory
#define update_inventory() ((void) 0)
#undef pmname
#define pmname(p, g) ((p)->pmnames[NEUTRAL])
#include "step9c_souls.h"

static struct monst make_mon(int type, int hp, int level) {
    struct monst m = { 0 };
    m.data = &mons[type]; m.mhpmax = m.mhp = hp; m.m_lev = level;
    m.mx = 10; m.my = 0; m.cham = NON_PM;
    return m;
}

int main(void) {
    struct monst one, two, native, dead, a, b, victim;
    uint32_t final_rng;
    int kind, hp, level, seed;
    const int forms[] = { PM_DEEP_ONE, PM_DEEPER_ONE, PM_DEEPEST_ONE };
    const int gains[] = { 2, 4, 8 };
    monst_globals_init();
    assert(little_to_big(PM_DEEP_ONE) == PM_DEEPER_ONE);
    assert(little_to_big(PM_DEEPER_ONE) == PM_DEEPEST_ONE);
    assert(little_to_big(PM_DEEPEST_ONE) == PM_DEEPEST_ONE);
    for (kind = 0; kind < 3; ++kind) {
        one = make_mon(PM_DEEP_ONE, 30, 7);
        two = make_mon(PM_DEEPER_ONE, 90, 15);
        native = make_mon(PM_ORC, 30, 7);
        dead = make_mon(PM_DEEPEST_ONE, 0, 30);
        one.nmon = &two; two.nmon = &native; native.nmon = &dead;
        fmon = &one; rng = 123;
        mith_deep_soul(&mons[forms[kind]]);
        assert(one.mhpmax == 30 + gains[kind] && one.mhp == one.mhpmax);
        assert(two.mhpmax == 90 + gains[kind]);
        assert(native.mhpmax == 30 && !dead.mhp);
        assert(rng == 123); /* self-growth condition is unconditionally true */
    }
    puts("PASS donor 2/4/8 soul pulses, living-recipient filter and native exclusion");
    one = make_mon(PM_DEEP_ONE, 120, 14); fmon = &one;
    mith_deep_soul(&mons[PM_DEEP_ONE]);
    assert(one.data == &mons[PM_DEEPER_ONE] && one.m_lev == 15 && one.mhpmax == 122);
    one = make_mon(PM_DEEPER_ONE, 240, 29); fmon = &one;
    mith_deep_soul(&mons[PM_DEEPER_ONE]);
    assert(one.data == &mons[PM_DEEPEST_ONE] && one.m_lev == 30 && one.mhpmax == 244);
    assert(redraws == 2 && !deaths);
    puts("PASS both deep-one growth transformations and top-row redraws");
    for (hp = 299; hp <= 500; ++hp) {
        one = make_mon(PM_DEEPEST_ONE, hp, 45); fmon = &one;
        mith_deep_soul(&mons[PM_DEEPEST_ONE]);
        assert(one.mhpmax == hp + (hp < 300 ? 8 : hp >= 359 ? 1 : 2));
        assert(one.mhp <= one.mhpmax); /* preserve native current/max invariant */
        assert(one.m_lev == 45);
    }
    one = make_mon(PM_DEEPEST_ONE, LARGEST_INT, 45); fmon = &one;
    mith_deep_soul(&mons[PM_DEEPEST_ONE]);
    assert(one.mhpmax == LARGEST_INT - 1 && one.mhp == one.mhpmax);
    puts("PASS 300-HP taper, donor level ceiling, growth above 400 and overflow safety");
    for (kind = 0; kind < 4; ++kind)
        for (level = 1; level < 50; level += 3)
            for (hp = 1; hp <= 450; hp += 17)
                for (seed = 1; seed <= 8; ++seed) {
                    int type = kind == 0 ? PM_ORC : kind == 1 ? PM_GIANT_ANT
                               : kind == 2 ? PM_DEEP_ONE : PM_DEEPEST_ONE;
                    a = b = make_mon(type, hp, level);
                    victim = make_mon(PM_ORC, 10, seed);
                    rng = seed; grow_up(&a, seed & 1 ? &victim : 0); final_rng = rng;
                    rng = seed; baseline_grow_up(&b, seed & 1 ? &victim : 0);
                    assert(rng == final_rng && a.data == b.data && a.mhp == b.mhp
                           && a.mhpmax == b.mhpmax && a.m_lev == b.m_lev);
                }
    puts("PASS native kill and potion growth unchanged across 14688 cases");
    one = make_mon(PM_ALABASTER_ELF, 80, 9);
    grow_up(&one, (struct monst *) 0);
    assert(one.data == &mons[PM_ALABASTER_ELF_ELDER] && one.m_lev == 10);
    puts("PASS Alabaster elf grows into its pinned elder form");
    {
        int species,clone,shop,cases=0;boolean result;
        for(species=LOW_PM;species<NUMMONS;++species)
            for(clone=0;clone<2;++clone) for(shop=0;shop<2;++shop)
                for(seed=1;seed<=32;++seed) {
                    one=make_mon(species,50,10);one.mcloned=clone;one.isshk=shop;
                    rng=seed;result=corpse_tail(&one);final_rng=rng;
                    if(species==PM_ALABASTER_MUMMY) {
                        assert(result && rng==(uint32_t)seed);
                    } else {
                        rng=seed;assert(baseline_corpse_tail(&one)==result && rng==final_rng);
                    }
                    ++cases;
                }
        printf("PASS %d actual corpse-chance cases: guaranteed Alabaster mummy drops, every other species/RNG unchanged\n",cases);
    }
    return 0;
}
