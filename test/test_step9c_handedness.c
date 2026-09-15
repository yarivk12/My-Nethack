#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include "step9c_handedness.h"

int main(void)
{
    struct obj obj = { 0 };
    struct permonst wielder = { 0 };
    int i, size, msize, total = 0;
    monst_globals_init(); objects_globals_init();
    obj.oclass = WEAPON_CLASS;
    for (i = 0; i < SIZE(tested_types); ++i) {
        obj.otyp = tested_types[i];
        for (size = MZ_TINY; size <= MZ_GIGANTIC; ++size)
            for (msize = MZ_TINY; msize <= MZ_GIGANTIC; ++msize) {
                obj.obranch_size = size + 1; wielder.msize = msize;
                if (mith_bimanual(&obj, &wielder) != donor_bimanual(&obj, &wielder)) {
                    fprintf(stderr, "handedness mismatch type=%d (%s) size=%d wielder=%d\n",
                            obj.otyp, OBJ_NAME(objects[obj.otyp]), size, msize);
                    return 1;
                }
                ++total;
            }
    }
    for (i = 0; i < NUM_OBJECTS; ++i) {
        obj.otyp = i; obj.obranch_size = 0;
        assert(mith_bimanual(&obj, &wielder) == !!objects[i].oc_bimanual);
    }
    printf("PASS %d pinned donor sized handedness comparisons; every unsized native object unchanged\n", total);
    return 0;
}
