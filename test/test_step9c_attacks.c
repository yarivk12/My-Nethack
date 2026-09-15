/* These are production handlers, not a second implementation of their rules.
   Adapters replace UI, RNG, and final death dispatch only. */
#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct you u;
struct instance_globals_y gy;
struct instance_globals_saved_m svm;
struct instance_globals_saved_k svk;
static int rolls[32], roll_count, roll_at;
static int heal_amount, kills, drains, poison_calls;
static boolean negated, lifesave;
static struct obj *worn_armor;
static boolean item_resist, disint_resist;
static int armor_destroyed, armor_adjustments, hero_deaths, last_death_type;
int rn2(int n) {
    assert(n > 0 && roll_at < roll_count);
    assert(rolls[roll_at] >= 0 && rolls[roll_at] < n);
    return rolls[roll_at++];
}
int rnd(int n) { return rn2(n) + 1; }
int d(int n, int x) { int sum = 0; while (n--) sum += rnd(x); return sum; }
void pline(const char *s, ...) { (void) s; }
void You(const char *s, ...) { (void) s; }
void You_feel(const char *s, ...) { (void) s; }
char *s_suffix(const char *s) { return (char *) s; }
#undef Monnam
#define Monnam(m) "fixture"
#undef canspotmon
#define canspotmon(m) TRUE
void hitmsg(struct monst *m, struct attack *a) { (void) m; (void) a; }
void healup(int n, int maxhp, boolean sick, boolean blind) {
    assert(!maxhp && !sick && !blind); heal_amount += n;
}
boolean mhitm_mgc_atk_negated(struct monst *a, struct monst *b, boolean c) {
    (void) a; (void) b; (void) c; return negated;
}
boolean defended(struct monst *m, int ad) { (void) m; (void) ad; return FALSE; }
int Mgender(struct monst *m) { (void) m; return NEUTRAL; }
const char *pmname(struct permonst *p, int gender) {
    (void) gender; return p->pmnames[NEUTRAL];
}
boolean Resists_Elem(struct monst *m, int prop) {
    if (prop == DISINT_RES)
        return disint_resist;
    assert(prop == POISON_RES); return !!(m->data->mresists & MR_POISON);
}
static void fixture_kill(struct monst *m) { ++kills; m->mhp = lifesave ? 20 : 0; }
#undef killed
#define killed(m) fixture_kill(m)
void monkilled(struct monst *m, const char *s, int ad) {
    (void) s; last_death_type = ad; fixture_kill(m);
}
void xkilled(struct monst *m, int flags) {
    assert(flags & XKILL_NOCORPSE); fixture_kill(m);
}
struct obj *some_armor(struct monst *m) { (void) m; return worn_armor; }
boolean obj_resists(struct obj *o, int a, int b) {
    (void) o; assert((a == 0 && b == 80) || (a == 10 && b == 90));
    return item_resist;
}
boolean is_quest_artifact(struct obj *o) { (void) o; return FALSE; }
void costly_alteration(struct obj *o, int cost) { (void) o; (void) cost; }
void adj_abon(struct obj *o, schar delta) {
    (void) o; assert(delta == -1); ++armor_adjustments;
}
void find_ac(void) { }
void update_inventory(void) { }
int disintegrate_arm(struct obj *o) {
    assert(o == worn_armor); worn_armor = 0; ++armor_destroyed; return 1;
}
void m_useup(struct monst *m, struct obj *o) {
    (void) m; (void) disintegrate_arm(o);
}
void done(int how) {
    assert(how == DIED && u.ugrave_arise == -3); ++hero_deaths;
}
struct permonst *grow_up(struct monst *m, struct monst *victim) {
    (void) victim; return m->data;
}
void losexp(const char *s) { (void) s; ++drains; }
void poisoned(const char *s, int a, const char *k, int n, boolean b) {
    (void) s; (void) k; (void) b; assert(a == A_STR && n == 30); ++poison_calls;
}
#include "step9c_attacks.h"

static void reset_rolls(int a, int b, int c, int e, int f) {
    rolls[0] = a; rolls[1] = b; rolls[2] = c; rolls[3] = e; rolls[4] = f;
    roll_at = 0; roll_count = 5;
}

int main(void) {
    struct monst attacker = { 0 }, target = { 0 };
    struct mhitm_data hit = { 0 };
    struct attack bite = { AT_BITE, AD_DRLI, 1, 8 };
    struct attack desc = { AT_TUCH, AD_DESC, 3, 4 };
    int i;
    monst_globals_init(); objects_globals_init();
    u.umonnum = PM_HUMAN; u.ulycn = NON_PM;
    gy.youmonst.data = &mons[PM_HUMAN];
    gy.youmonst.cham = NON_PM;
    target.cham = attacker.cham = NON_PM;
    attacker.data = &mons[PM_LIVING_MIRAGE];
    attacker.mhp = 2; attacker.mhpmax = 50;
    target.data = &mons[PM_WATER_ELEMENTAL]; target.mhp = 7;
    hit.damage = 9;
    mith_desiccate(&attacker, &desc, &target, &hit);
    assert(hit.damage == 18 && attacker.mhp == 9);
    target.data = &mons[PM_HUMAN]; target.mhp = 30; hit.damage = 6;
    mith_desiccate(&attacker, &desc, &target, &hit);
    assert(hit.damage == 6 && attacker.mhp == 15);
    target.data = &mons[PM_SENTINEL_OF_MITHARDIR]; hit.damage = 12;
    mith_desiccate(&attacker, &desc, &target, &hit);
    assert(hit.damage == 0 && attacker.mhp == 15);
    target.data = &mons[PM_HUMAN]; hit.damage = 6;
    mith_desiccate(&gy.youmonst, &desc, &target, &hit);
    assert(heal_amount == 6);
    puts("PASS desiccation: watery double damage, capped life steal, dry immunity, hero healing");

    for (i = 0; i < 2; ++i) {
        attacker.data = &mons[i ? PM_FIRST_WRAITHWORM : PM_WRAITHWORM];
        assert(resists_drli(&attacker));
        target.mhp = target.mhpmax = 30; target.m_lev = 5;
        hit.damage = 4; hit.done = FALSE;
        reset_rolls(0, 2, 3, 1, 0); /* drain 3+4, no poison */
        mith_drain_attack(&attacker, &bite, &target, &hit);
        assert(hit.damage == 7 && target.mhpmax == 23 && target.m_lev == 4);
        assert(target.mhp == 30 && !hit.done && roll_at == 4);
        target.m_lev = 0; reset_rolls(0, 0, 0, 1, 0);
        mith_drain_attack(&attacker, &bite, &target, &hit);
        assert(hit.done && (hit.hitflags & M_ATTK_DEF_DIED));
        lifesave = TRUE; target.m_lev = 0; target.mhp = 30;
        hit.done = FALSE; reset_rolls(0, 0, 0, 1, 0);
        mith_drain_attack(&attacker, &bite, &target, &hit);
        assert(hit.done && !(hit.hitflags & M_ATTK_DEF_DIED) && target.mhp == 20);
        lifesave = FALSE;
    }
    target.data = &mons[PM_HUMAN]; assert(!resists_drli(&target));
    negated = TRUE; hit.damage = 4; hit.done = FALSE; roll_at = roll_count = 0;
    mith_drain_attack(&attacker, &bite, &target, &hit);
    assert(!roll_at && hit.damage == 4 && !hit.done);
    negated = FALSE; reset_rolls(1, 0, 0, 0, 0);
    target.mhp = 30; target.m_lev = 5;
    mith_drain_attack(&attacker, &bite, &target, &hit);
    assert(hit.done && target.mhp == 0); /* poison's independent lethal roll */
    reset_rolls(0, 0, 0, 0, 0);
    mith_drain_attack(&attacker, &bite, &gy.youmonst, &hit);
    assert(drains == 1 && poison_calls == 1);
    bite.adtyp = AD_VAMP; negated = TRUE; attacker.mcan = FALSE;
    reset_rolls(0, 1, 0, 0, 0);
    mith_drain_attack(&attacker, &bite, &gy.youmonst, &hit);
    assert(drains == 2);
    puts("PASS both worms: drain, poison, cancellation, lethal and lifesaving paths; vampiric MC bypass");
    {
        struct obj armor = { 0 };
        struct attack touch = { AT_TUCH, AD_DISN, 1, 4 };
        attacker.data = &mons[PM_ASPECT_OF_THE_SILENCE];
        armor.otyp = LEATHER_ARMOR; armor.oclass = ARMOR_CLASS;
        armor.spe = 1; worn_armor = &armor;
        hit.damage = 2; hit.done = FALSE;
        mith_disintegrate(&attacker, &touch, &gy.youmonst, &hit);
        assert(armor.spe == -1 && armor_adjustments == 2 && hit.damage == 0);
        assert(!hit.done && !hero_deaths);
        item_resist = TRUE; hit.damage = 4;
        mith_disintegrate(&attacker, &touch, &gy.youmonst, &hit);
        assert(armor.spe == -1 && !hero_deaths);
        item_resist = FALSE; armor.otyp = BLACK_DRAGON_SCALES; hit.damage = 4;
        mith_disintegrate(&attacker, &touch, &gy.youmonst, &hit);
        assert(armor.spe == -1 && !hero_deaths);
        armor.otyp = LEATHER_ARMOR; armor.spe = -objects[armor.otyp].a_ac;
        hit.damage = 2;
        mith_disintegrate(&attacker, &touch, &gy.youmonst, &hit);
        assert(!worn_armor && armor_destroyed == 1 && hero_deaths == 1 && hit.done);
        disint_resist = TRUE; target.mhp = 30; hit.damage = 4; hit.done = FALSE;
        mith_disintegrate(&attacker, &touch, &target, &hit);
        assert(!hit.done && target.mhp == 30 && hit.damage == 0);
        disint_resist = FALSE; lifesave = TRUE; hit.damage = 4;
        mith_disintegrate(&attacker, &touch, &target, &hit);
        assert(hit.done && target.mhp == 20 && !(hit.hitflags & M_ATTK_DEF_DIED));
        assert(last_death_type == -AD_RBRE);
        lifesave = FALSE; hit.damage = 4;
        mith_disintegrate(&attacker, &touch, &target, &hit);
        assert(hit.done && (hit.hitflags & M_ATTK_DEF_DIED));
        target.mhp = 10; target.mhpmax = 30;
        mith_cold_heal(&target, 9);
        assert(target.mhp == 14 && target.mhpmax == 30);
        target.mhp = 29; mith_cold_heal(&target, 9);
        assert(target.mhp == 33 && target.mhpmax == 33);
        puts("PASS Aspect: armor erosion, resistance, disintegration, life saving and cold healing");
    }
    return 0;
}
