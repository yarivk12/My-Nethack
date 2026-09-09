#include "hack.h"
#include <assert.h>
#include <stdio.h>

static struct obj items[4];
static int count, armor_choice, weapon_choice, dice_total;
int rn2(int n) { assert(n == 3 || n == 6); return n == 3 ? armor_choice : weapon_choice; }
int d(int n, int size) { assert(n == 4 && size == 2); return dice_total; }
struct obj *mksobj(int type, boolean init, boolean art) {
    struct obj *o = &items[count++];
    assert(count <= SIZE(items) && init && !art);
    memset(o, 0, sizeof *o); o->otyp = (short) type;
    o->oclass = objects[type].oc_class; o->quan = 1;
    return o;
}
int mpickobj(struct monst *m, struct obj *o) { (void) m; (void) o; return 0; }
struct obj *mongets(struct monst *m, int type) { (void) m; return mksobj(type, TRUE, FALSE); }
int weight(struct obj *o) { assert(o->obranch_size == MZ_HUGE + 1); return 123; }
#include "step9c_equipment.h"

int main(void) {
    static const int armor[] = { JACKET, LEATHER_ARMOR, CHAIN_MAIL };
    static const int weapons[] = { TWO_HANDED_SWORD, SCIMITAR, TRIDENT, SHORT_SWORD, DAGGER, SPEAR };
    static const int huge[][2] = { { TWO_HANDED_SWORD, 0 }, { SCIMITAR, SCIMITAR },
                                 { SCIMITAR, 0 }, { TRIDENT, KNIFE }, { KNIFE, 0 }, { 0, 0 } };
    struct monst m = { 0 };
    int type, i;
    monst_globals_init(); objects_globals_init();
    for (type = PM_DEEP_ONE; type <= PM_DEEPER_ONE; ++type)
        for (armor_choice = 0; armor_choice < 3; ++armor_choice)
            for (weapon_choice = 0; weapon_choice < 6; ++weapon_choice)
                for (dice_total = 4; dice_total <= 8; ++dice_total) {
                    m.data = &mons[type]; count = 0;
                    assert(mith_deep_equipment(&m));
                    assert(count == 1 + dice_total / 3);
                    assert(items[0].otyp == armor[armor_choice]);
                    assert(items[0].oeroded == (armor_choice == 2 ? 2 : 0));
                    for (i = 1; i < count; ++i)
                        assert(items[i].otyp == weapons[weapon_choice] && items[i].oeroded == 3);
                }
    puts("PASS all 180 deep/deeper-one armor and rusted-weapon combinations");
    for (weapon_choice = 0; weapon_choice < 6; ++weapon_choice) {
        m.data = &mons[PM_DEEPEST_ONE]; count = 0;
        assert(mith_deep_equipment(&m));
        assert(count == !!huge[weapon_choice][0] + !!huge[weapon_choice][1]);
        for (i = 0; i < count; ++i)
            assert(items[i].otyp == huge[weapon_choice][i] && items[i].spe == 3
                   && items[i].oerodeproof && items[i].obranch_size == MZ_HUGE + 1
                   && items[i].owt == 123);
    }
    m.data = &mons[PM_GIANT]; count = 0;
    assert(!mith_deep_equipment(&m) && !count);
    puts("PASS all six deepest-one weapon choices, dual-weapon pairs and native exclusion");
    return 0;
}
