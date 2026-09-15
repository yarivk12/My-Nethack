#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

struct flag flags;
static uint32_t rng;
static int protect, wet, nighttime, donor_changed;
static int lit, unlit, unequipped, equipped, wielded, broken, redraws;
int rn2(int n) { assert(n > 0); rng = rng * 1664525U + 1013904223U; return (rng >> 8) % n; }
int night(void) { return nighttime; }
boolean is_pool(coordxy x, coordxy y) { (void) x; (void) y; return wet; }
#undef Protection_from_shape_changers
#define Protection_from_shape_changers protect
#undef canseemon
#define canseemon(m) FALSE
#undef Hallucination
#define Hallucination FALSE
#undef pline
#define pline(...) ((void) 0)
#undef You_hear
#define You_hear(...) ((void) 0)
#define is_heladrin(p) ((p) == &mons[PM_COURE_ELADRIN] || (p) == &mons[PM_NOVIERE_ELADRIN] || (p) == &mons[PM_BRALANI_ELADRIN])
#define is_eeladrin(p) ((p) == &mons[PM_MOTE_OF_LIGHT] || (p) == &mons[PM_WATER_DOLPHIN] || (p) == &mons[PM_SINGING_SAND])
#define is_yochlol(p) FALSE
#define humanoid_torso(p) (is_heladrin(p) || (p) == &mons[PM_SELKIE])
/* Unselected donor-only branches remain unreachable in this comparison. */
#define PM_MAMMON PM_PIT_FIEND
#define PM_GREEN_PIT_FIEND PM_PIT_FIEND
#define PM_ANUBAN_JACKAL PM_JACKAL
#define PM_BALL_OF_LIGHT PM_YELLOW_LIGHT
#define PM_INCUBUS PM_AMOROUS_DEMON
#define PM_SUCCUBUS PM_AMOROUS_DEMON
#define INCUBUS_FACTION (-104)
#define SUCCUBUS_FACTION (-105)
void set_mon_data(struct monst *m, struct permonst *p) { m->data = p; }
int healmon(struct monst *m, int hp, int maxhp) { assert(!maxhp); m->mhp += hp; return hp; }
anything *monst_to_any(struct monst *m) { static anything a; a.a_monst = m; return &a; }
void new_light_source(coordxy x, coordxy y, int radius, int type, anything *a) {
    assert(x == 10 && y == 0 && radius == 1 && type == LS_MONSTER && a->a_monst); ++lit;
}
void del_light_source(int type, anything *a) { assert(type == LS_MONSTER && a->a_monst); ++unlit; }
boolean artifact_light(struct obj *o) { (void) o; return FALSE; }
void end_burn(struct obj *o, boolean timers) { (void) o; (void) timers; assert(0); }
void update_mon_extrinsics(struct monst *m, struct obj *o, boolean on, boolean silent) {
    assert(!on && silent && !o->owornmask); (void) m; ++unequipped;
}
void setmnotwielded(struct monst *m, struct obj *o) { assert(o == MON_WEP(m)); MON_NOWEP(m); }
void m_dowear(struct monst *m, boolean creation) { assert(creation); (void) m; ++equipped; }
int mon_wield_item(struct monst *m) { assert(m->weapon_check == NEED_HTH_WEAPON); ++wielded; return 1; }
void mon_break_armor(struct monst *m, boolean poly) { (void) m; assert(!poly); ++broken; }
void possibly_unwield(struct monst *m, boolean poly) { assert(!poly); (void) m; }
void newsym(coordxy x, coordxy y) { assert(x == 10 && y == 0); ++redraws; }
#include "step9c_forms.h"

int main(void) {
    const int pairs[][2] = { { PM_COURE_ELADRIN, PM_MOTE_OF_LIGHT },
        { PM_NOVIERE_ELADRIN, PM_WATER_DOLPHIN },
        { PM_BRALANI_ELADRIN, PM_SINGING_SAND }, { PM_SELKIE, PM_SEAL } };
    struct monst m = { 0 };
    struct obj armor = { 0 }, weapon = { 0 };
    int pair, side, hp, seed, full, wanted;
    unsigned cases = 0;
    uint32_t actual_rng;
    monst_globals_init();
    for (pair = 0; pair < 4; ++pair) for (side = 0; side < 2; ++side) {
        m.data = &mons[pairs[pair][side]]; m.mhpmax = 101;
        assert(mith_fey_counter(pairs[pair][side]) == pairs[pair][!side]);
        assert(!is_were(m.data));
        for (hp = 1; hp <= 101; ++hp)
            for (protect = 0; protect < 2; ++protect)
                for (wet = 0; wet < 2; ++wet)
                    for (nighttime = 0; nighttime < 2; ++nighttime)
                        for (full = 0; full < 2; ++full)
                            for (seed = 1; seed <= 32; ++seed) {
                                m.mhp = hp; flags.moonphase = full ? FULL_MOON : NEW_MOON;
                                rng = seed; wanted = mith_fey_change_ready(&m); actual_rng = rng;
                                rng = seed; donor_changed = 0; donor_check(&m);
                                assert(wanted == !!donor_changed && actual_rng == rng); ++cases;
                            }
    }
    printf("PASS %u pinned-donor transformation decisions and RNG traces\n", cases);
    for (pair = 0; pair < 4; ++pair) {
        memset(&m, 0, sizeof m); memset(&armor, 0, sizeof armor); memset(&weapon, 0, sizeof weapon);
        m.data = &mons[pairs[pair][0]]; m.mx = 10; m.my = 0;
        m.mhp = 40; m.mhpmax = 100; m.mspare1 = 0x15555;
        m.msleeping = 1; m.mfrozen = 5; m.mcanmove = 0;
        armor.owornmask = W_ARM; weapon.owornmask = W_WEP; armor.nobj = &weapon;
        m.minvent = &armor; m.mw = &weapon; m.misc_worn_check = W_ARM | W_WEP;
        unequipped = equipped = wielded = broken = lit = unlit = redraws = 0;
        mith_fey_shift(&m, pairs[pair][1]);
        assert(m.data == &mons[pairs[pair][1]] && m.mhp == 55 && m.mhpmax == 100);
        assert(!m.msleeping && !m.mfrozen && m.mcanmove && m.mspare1 == 0x15555);
        assert(lit == (pair == 0) && !unlit && redraws == 1);
        if (pair < 3) {
            assert(m.minvent == &armor && armor.nobj == &weapon && !m.misc_worn_check);
            assert(!armor.owornmask && !weapon.owornmask && !MON_WEP(&m));
            assert(unequipped == 2 && !broken);
        } else assert(broken == 1);
        mith_fey_shift(&m, pairs[pair][0]);
        assert(m.mhp == 66 && m.data == &mons[pairs[pair][0]]);
        assert(unlit == (pair == 0) && redraws == 2);
        if (pair < 3) assert(equipped == 1 && wielded == 1);
        else assert(broken == 2);
    }
    assert(mith_fey_counter(PM_WOOD_NYMPH) == NON_PM);
    puts("PASS form pairs, healing, wakeup, retained inventory, re-equipping, light updates and saved-bit preservation");
    return 0;
}
