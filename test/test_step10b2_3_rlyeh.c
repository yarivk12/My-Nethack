#include "hack.h"
#include <assert.h>
#include <stdio.h>

#undef rn1
int rn1(int, int);

struct instance_globals_saved_m svm;
static struct monst made_mon;
static int chance_value, gate_value, values[4], value_index;
static int made_count, made_ids[32], mkclass_count, last_class;

int
d(int n, int x)
{
    if (n == 1 && x == 100)
        return chance_value;
    return values[value_index++];
}

int
rn2(int x)
{
    assert(x == 20);
    return gate_value;
}

int
rnd(int x)
{
    (void) x;
    return values[value_index++];
}

int
rn1(int x, int y)
{
    (void) x;
    (void) y;
    return values[value_index++];
}

struct permonst *
mkclass(char mclass, int flags)
{
    assert(flags == (G_NOHELL | G_HELL));
    ++mkclass_count;
    last_class = mclass;
    return mclass == S_BLOB ? &mons[PM_ACID_BLOB] : &mons[PM_UMBER_HULK];
}

struct monst *
makemon(struct permonst *ptr, coordxy x, coordxy y, mmflags_nht flags)
{
    assert(x == 7 && y == 9 && flags == NO_MM_FLAGS);
    made_ids[made_count++] = monsndx(ptr);
    return &made_mon;
}

static void
setup(int chance, int gate, int v0, int v1)
{
    memset(&svm, 0, sizeof svm);
    chance_value = chance;
    gate_value = gate;
    values[0] = v0;
    values[1] = v1;
    value_index = made_count = mkclass_count = last_class = 0;
}

int
main(void)
{
    monst_globals_init();
    setup(1, 1, 6, 0);
    assert(step10b_rlyeh_create(7, 9) == 0 && made_count == 0);

    setup(1, 0, 6, 0);
    assert(step10b_rlyeh_create(7, 9) == 7 && made_count == 7);
    assert(made_ids[0] == PM_HUNTING_HORROR);
    setup(5, 0, 8, 0);
    assert(step10b_rlyeh_create(7, 9) == 9 && made_ids[0] == PM_BYAKHEE);
    setup(7, 0, 0, 0);
    assert(step10b_rlyeh_create(7, 9) == 1 && made_ids[0] == PM_SHOGGOTH);
    setup(9, 0, 4, 2);
    assert(step10b_rlyeh_create(7, 9) == 9);
    assert(made_ids[0] == PM_DEEPEST_ONE && made_ids[1] == PM_DEEPER_ONE
           && made_ids[6] == PM_DEEP_ONE);
    setup(29, 0, 3, 0);
    assert(step10b_rlyeh_create(7, 9) == 4
           && made_ids[0] == PM_MASTER_MIND_FLAYER);
    setup(49, 0, 3, 0);
    assert(step10b_rlyeh_create(7, 9) == 4
           && made_ids[0] == PM_MIND_FLAYER);
    setup(69, 0, 6, 0);
    assert(step10b_rlyeh_create(7, 9) == 7
           && made_ids[0] == PM_DEEPER_ONE);
    setup(70, 0, 6, 0);
    assert(step10b_rlyeh_create(7, 9) == 7
           && made_ids[0] == PM_DEEP_ONE);

    setup(5, 0, 2, 0);
    svm.mvitals[PM_BYAKHEE].mvflags = G_GENOD;
    assert(step10b_rlyeh_create(7, 9) == 3 && mkclass_count == 3
           && last_class == S_UMBER && made_ids[0] == PM_UMBER_HULK);
    setup(5, 0, 2, 0);
    svm.mvitals[PM_BYAKHEE].mvflags = G_EXTINCT;
    assert(step10b_rlyeh_create(7, 9) == 3 && mkclass_count == 0
           && made_ids[0] == PM_BYAKHEE);
    setup(7, 0, 0, 0);
    svm.mvitals[PM_SHOGGOTH].mvflags = G_GENOD;
    assert(step10b_rlyeh_create(7, 9) == 1 && mkclass_count == 1
           && last_class == S_BLOB && made_ids[0] == PM_ACID_BLOB);

    puts("PASS Step 10B2-3 R'lyeh exact gate, inclusive groups and fallbacks");
    return 0;
}
