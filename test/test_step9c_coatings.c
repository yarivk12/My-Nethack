#include "hack.h"
#include <assert.h>
#include <stdio.h>
struct you u;
struct instance_globals_y gy;
struct instance_globals_m gm;
static int roll,rolls,asleep,paralyzed,blinded;
int rn2(int n) { assert(n>0);++rolls;return roll%n; }
int rnd(int n) { return rn2(n)+1; }
int sgn(int n) { return n < 0 ? -1 : n > 0; }
schar acurr(int attr) { return u.acurr.a[attr]; }
boolean defended(struct monst *m,int type) { (void)m;(void)type;return FALSE; }
boolean Resists_Elem(struct monst *m,int prop) {
    assert(prop>=FIRE_RES&&prop<=STONE_RES);
    return m==&gy.youmonst ? !!(u.uprops[prop].intrinsic||u.uprops[prop].extrinsic)
                          : !!(mon_resistancebits(m)&(1<<(prop-1)));
}
#undef pline
#define pline(...) ((void)0)
#undef You
#define You(...) ((void)0)
#undef canseemon
#define canseemon(m) FALSE
void fall_asleep(int n,boolean talk) { (void)talk;asleep=-n; }
int sleep_monst(struct monst *m,int n,int how) { (void)how;m->msleeping=1;asleep=n;return TRUE; }
void slept_monst(struct monst *m) { assert(m->msleeping); }
void make_blinded(long n,boolean talk) { (void)talk;blinded=(int)n; }
int resist(struct monst *m,char how,int dam,int tell) { (void)m;(void)how;(void)dam;(void)tell;return FALSE; }
void nomul(int n) { paralyzed=-n; }
void paralyze_monst(struct monst *m,int n) { m->mcanmove=0;m->mfrozen=n;paralyzed=n; }
#include "step9c_coatings.h"
int main(void) {
    struct monst m={0};struct obj o={0},gear={0};
    unsigned long bit;int i,expected,saved,cases=0;
    monst_globals_init();objects_globals_init();
    u.umonnum=u.umonster=PM_HUMAN;gy.youmonst.data=m.data=&mons[PM_HUMAN];
    u.acurr.a[A_CON]=18;u.ualign.type=A_NEUTRAL;
    o.otyp=DAGGER;o.oclass=WEAPON_CLASS;
    for(roll=0;roll<80;++roll) {
        rolls=0;assert(!mith_weapon_effects(0,&m,10));
        o.obranch_props=0;assert(!mith_weapon_effects(&o,&m,10));assert(!rolls);
        for(bit=OBP_ACID;bit<=OBP_FILTH;bit<<=1) {
            o.obranch_props=bit;rolls=0;
            expected=10+mith_weapon_effects(&o,&m,10);saved=rolls;
            o.obranch_props=bit;rolls=0;
            assert(missile_mon(&o,&m,10)==expected&&rolls==saved);++cases;
            o.obranch_props=bit;rolls=0;
            expected=10+mith_weapon_effects(&o,&gy.youmonst,10);saved=rolls;
            o.obranch_props=bit;rolls=0;
            assert(missile_hero(&o,10)==expected&&rolls==saved);++cases;
        }
    }
    roll=1;o.obranch_props=OBP_ACID;m.mintrinsics=MR_ACID;
    assert(!mith_weapon_effects(&o,&m,10));m.mintrinsics=0;
    assert(mith_weapon_effects(&o,&m,10)==2);
    roll=0;o.obranch_props=OBP_SLEEP;asleep=0;
    assert(!mith_weapon_effects(&o,&m,10)&&asleep&&!(o.obranch_props&OBP_SLEEP));
    m.mintrinsics=MR_SLEEP;o.obranch_props=OBP_SLEEP;asleep=0;
    assert(!mith_weapon_effects(&o,&m,10)&&!asleep);m.mintrinsics=0;
    o.obranch_props=OBP_PARALYZE;m.mcanmove=1;paralyzed=0;
    assert(mith_weapon_effects(&o,&m,10)==6&&paralyzed&&!m.mcanmove);
    gear.otyp=RIN_FREE_ACTION;gear.owornmask=W_RINGL;m.minvent=&gear;
    o.obranch_props=OBP_PARALYZE;m.mcanmove=1;paralyzed=0;
    assert(mith_weapon_effects(&o,&m,10)==1&&!paralyzed&&m.mcanmove);m.minvent=0;
    o.obranch_props=OBP_BLIND;m.mcansee=1;m.mblinded=0;
    assert(mith_weapon_effects(&o,&m,10)==3&&!m.mcansee&&m.mblinded==64);
    o.obranch_props=OBP_FILTH;m.data=&mons[PM_GHOUL];
    assert(!mith_weapon_effects(&o,&m,10));m.data=&mons[PM_HUMAN];
    o.obranch_props=OBP_FILTH;assert(mith_weapon_effects(&o,&m,10)==9999);
    o.obranch_props=OBP_ANARCHIC;assert(mith_weapon_effects(&o,&m,10)==10);
    o.obranch_props=OBP_CONCORDANT;u.ualign.type=A_NEUTRAL;
    assert(!mith_weapon_effects(&o,&gy.youmonst,10));
    u.ualign.type=A_LAWFUL;assert(mith_weapon_effects(&o,&gy.youmonst,10)==10);
    m.data=&mons[PM_ORC];assert(mith_weapon_effects(&o,&m,10)==10);
    m.data=&mons[PM_HUMAN];
    for(i=0;i<2;++i) {
        o.otyp=i?POT_ACID:ROCK;o.oclass=i?POTION_CLASS:GEM_CLASS;
        o.obranch_props=OBP_ACID;rolls=0;
        assert(missile_hero(&o,10)==10&&missile_mon(&o,&m,10)==10&&!rolls);
    }
    printf("PASS %d actual hero/monster projectile dispatch cases; acid, sleep, paralysis, blindness, filth, depletion, native no-property RNG\n",cases);
    return 0;
}
