#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct obj *uwep;
static boolean safe = TRUE;
boolean can_touch_safely(struct monst *m, struct obj *o) { (void) m; (void) o; return safe; }
int touch_artifact(struct obj *o, struct monst *m) { (void) o; (void) m; return TRUE; }
boolean mon_hates_silver(struct monst *m) { (void) m; return FALSE; }
#undef resists_ston
#define resists_ston(m) TRUE
#include "step9c_weapon_selection.h"

int main(void) {
    const int imported[] = { CRYSTAL_SWORD, MOON_AXE, HIGH_ELVEN_WARSWORD, RAPIER, ELVEN_SICKLE };
    struct monst mon = { 0 };
    struct obj weapon = { 0 }, other = { 0 };
    int i, spike = 0;
    monst_globals_init(); objects_globals_init();
    mon.data = &mons[PM_ALABASTER_ELF]; mon.minvent = &weapon;
    /* Native elf isn't strong; a giant can use the two-handed moon axe. */
    for (i = 0; i < SIZE(imported); ++i) {
        mon.data = &mons[PM_GIANT];
        weapon.otyp = imported[i]; weapon.oclass = WEAPON_CLASS;
        assert(select_hwep(&mon) == &weapon);
        safe = FALSE; assert(!select_hwep(&mon)); safe = TRUE;
    }
    mon.data = &mons[PM_ALABASTER_ELF]; weapon.otyp = ELVEN_SICKLE;
    weapon.obranch_size = MZ_HUGE + 1;
    assert(!select_hwep(&mon)); /* huge donor sickle takes two hands */
    weapon.obranch_size = MZ_HUMAN + 1;
    assert(select_hwep(&mon) == &weapon);
    mon.data = &mons[PM_COURE_ELADRIN]; weapon.otyp = RAPIER;
    weapon.obranch_size = MZ_TINY + 1;
    assert(select_hwep(&mon) == &weapon);
    weapon.otyp = MOON_AXE; assert(!select_hwep(&mon)); /* donor weak-monster restriction */
    mon.data = &mons[PM_GIANT]; assert(select_hwep(&mon));
    mon.misc_worn_check = W_ARMS; assert(!select_hwep(&mon));
    mon.misc_worn_check = 0;
    weapon.otyp = CRYSTAL_SWORD; weapon.nobj = &other;
    other.otyp = DAGGER; other.oclass = WEAPON_CLASS; other.oartifact = 1;
    assert(select_hwep(&mon) == &other); /* native artifact preference */
    for (i = 0; i < SIZE(rwep); ++i) if (rwep[i] == SPIKE) ++spike;
    assert(spike == 1);
    /* Imported species' distinct offhand/multi-arm slots. Native repeated
       AT_WEAP attacks retain their original mainhand behavior. */
    {
        int pm, slot, count = 0;
        for (pm = LOW_PM; pm < NUMMONS; ++pm)
            for (slot = 0; slot < NATTK; ++slot) {
                boolean expected =
                    (pm == PM_ALABASTER_ELF && slot == 2)
                    || (pm == PM_BRALANI_ELADRIN && (slot == 1 || slot == 3))
                    || (pm == PM_LURKING_ONE && slot >= 1 && slot <= 3)
                    || (pm == PM_MOTHER_HYDRA && slot == 5)
                    || ((pm == PM_COURE_ELADRIN || pm == PM_DEEP_ONE
                         || pm == PM_DEEPER_ONE || pm == PM_DEEPEST_ONE
                         || pm == PM_CUPRILACH_RILMANI
                         || (pm >= PM_SMALL_GOAT_SPAWN
                             && pm <= PM_GIANT_GOAT_SPAWN)
                         || pm == PM_HMNYW_PHARAOH || pm == PM_DEMINYMPH
                         || pm == PM_FATHER_DAGON
                         || pm == PM_STAR_SPAWN)
                        && slot == 1);
                assert(mith_offhand_attack(&mons[pm], slot) == expected);
                if (expected) {
                    assert(mons[pm].mattk[slot].aatyp == AT_WEAP);
                    ++count;
                }
            }
        assert(count == 19);
    }
    memset(&mon, 0, sizeof mon);
    memset(&weapon, 0, sizeof weapon);
    memset(&other, 0, sizeof other);
    mon.data = &mons[PM_DEEPER_ONE]; mon.m_lev = 15;
    mon.minvent = &weapon; mon.mw = &weapon;
    weapon.otyp = other.otyp = SCIMITAR;
    weapon.oclass = other.oclass = WEAPON_CLASS;
    weapon.owt = other.owt = 40; weapon.nobj = &other;
    assert(mith_select_offhand(&mon) == &other); /* same type, different object */
    mon.misc_worn_check = W_ARMS; assert(!mith_select_offhand(&mon));
    mon.misc_worn_check = 0;
    other.cursed = 1; assert(!mith_select_offhand(&mon)); other.cursed = 0;
    other.oartifact = 1; assert(!mith_select_offhand(&mon)); other.oartifact = 0;
    other.owornmask = W_WEP; assert(!mith_select_offhand(&mon)); other.owornmask = 0;
    other.owt = 46; assert(!mith_select_offhand(&mon)); other.owt = 45;
    assert(mith_select_offhand(&mon) == &other);
    safe = FALSE; assert(!mith_select_offhand(&mon)); safe = TRUE;
    other.otyp = TWO_HANDED_SWORD; assert(!mith_select_offhand(&mon));
    other.otyp = SCIMITAR; other.obranch_size = MZ_HUGE + 1;
    assert(!mith_select_offhand(&mon));
    mon.data = &mons[PM_DEEPEST_ONE];
    assert(mith_select_offhand(&mon) == &other);
    weapon.nobj = 0; assert(!mith_select_offhand(&mon)); /* stolen offhand */
    puts("PASS offhand attack slots, distinct objects, size/weight/touch/shield/curse/artifact restrictions");
    puts("PASS imported melee selection, touch safety, shields, artifacts and ranged spikes");
    return 0;
}
