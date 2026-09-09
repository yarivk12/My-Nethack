#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static struct obj items[16], expected[16];
static int count;
static uint32_t rng;
static int obj_size(struct obj *o) { return o->obranch_size ? o->obranch_size - 1 : MZ_HUMAN; }
int rn2(int n) { assert(n > 0); rng = rng * 1664525U + 1013904223U; return (rng >> 8) % n; }
int rnd(int n) { return rn2(n) + 1; }
int d(int n, int size) { int total = 0; while (n-- > 0) total += rnd(size); return total; }
struct obj *mksobj(int type, boolean init, boolean art) {
    struct obj *o = &items[count++];
    assert(count <= SIZE(items) && init && !art);
    memset(o, 0, sizeof *o); o->otyp = (short) type;
    o->oclass = objects[type].oc_class; o->quan = 1;
    o->spe = rn2(7) - 3; /* object initialization consumes RNG in both versions */
    return o;
}
int mpickobj(struct monst *m, struct obj *o) { (void) m; (void) o; return 0; }
int weight(struct obj *o) { return obj_size(o) * 10 + obj_material(o); }
static void donor_mongets(struct monst *m, int type) {
    struct obj *o = mksobj(type, TRUE, FALSE);
    if (o->oclass == ARMOR_CLASS || o->oclass == WEAPON_CLASS)
        o->obranch_size = m->data->msize + 1;
    o->owt = weight(o); (void) mpickobj(m, o);
}
static void donor_initthrow(struct monst *m, int type, int quantity) {
    struct obj *o = mksobj(type, TRUE, FALSE);
    o->quan = rn1(quantity, 3); o->obranch_size = m->data->msize + 1;
    o->owt = weight(o); (void) mpickobj(m, o);
}
#include "step9c_fey_equipment.h"

int main(void) {
    const int types[] = { PM_ALABASTER_ELF, PM_ALABASTER_ELF_ELDER,
        PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN,
        PM_SELKIE, PM_OCEANID };
    struct monst m = { 0 };
    int seed, type, i, expected_count;
    uint32_t expected_rng;
    monst_globals_init(); objects_globals_init();
    for (type = 0; type < SIZE(types); ++type)
        for (seed = 1; seed <= 4096; ++seed) {
            m.data = &mons[types[type]];
            count = 0; rng = seed; assert(mith_fey_equipment(&m));
            expected_count = count; expected_rng = rng;
            memcpy(expected, items, sizeof expected);
            count = 0; rng = seed; assert(donor_equipment(&m));
            assert(count == expected_count && rng == expected_rng);
            for (i = 0; i < count; ++i) {
                struct obj *a = &expected[i], *b = &items[i];
                assert(a->otyp == b->otyp && a->quan == b->quan && a->spe == b->spe
                       && obj_size(a) == obj_size(b) && obj_material(a) == obj_material(b)
                       && a->owt == b->owt);
            }
        }
    m.data = &mons[PM_WOODLAND_ELF]; count = 0;
    assert(!mith_fey_equipment(&m) && !count);
    puts("PASS 28672 pinned-donor fey loadouts, RNG traces, quantities, sizes and native exclusion");
    return 0;
}
