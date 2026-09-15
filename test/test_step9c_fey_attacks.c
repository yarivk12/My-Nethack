#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct instance_globals_y gy;
struct instance_globals_m gm;
static int native_calls, physical_calls, rust_calls, negated, resistance;
static int roll, rolls, slept, duration, rust_done;
static struct obj *expected_weapon;
int rn2(int n) { assert(n>0); ++rolls; return roll%n; }
int rnd(int n) { return rn2(n)+1; }
static boolean mhitm_mgc_atk_negated(struct monst *a, struct monst *b, boolean verbose) {
    (void)b; (void)verbose; return a->mcan || negated;
}
void fall_asleep(int n, boolean msg) { (void)msg; ++slept; duration=-n; }
int sleep_monst(struct monst *m, int n, int how) {
    (void)m; assert(how==-1); duration=n; return TRUE;
}
void slept_monst(struct monst *m) { (void)m; ++slept; }
void mhitm_ad_phys(struct monst *a, struct attack *atk, struct monst *b, struct mhitm_data *h) {
    (void)a; (void)atk; (void)b; ++physical_calls;
    assert(h->weapon == expected_weapon); h->damage += 7;
}
void mhitm_ad_rust(struct monst *a, struct attack *atk, struct monst *b, struct mhitm_data *h) {
    (void)atk; (void)b; ++rust_calls;
    if (a->mcan) return;
    h->damage=0; h->done=rust_done;
}
#undef resists_sleep
#define resists_sleep(m) resistance
#undef Sleep_resistance
#define Sleep_resistance resistance
#undef Blind
#define Blind FALSE
#undef canseemon
#define canseemon(m) FALSE
#define pline_mon(...) ((void)0)
#undef You
#define You(...) ((void)0)
#include "step9c_fey_attacks.h"

int main(void)
{
    struct monst a={0}, b={0};
    struct obj weapon={0};
    struct attack atk={AT_WEAP,AD_SLEE,1,4};
    struct mhitm_data hit={0};
    int level, value, hero, cases=0;
    monst_globals_init();
    a.data=&mons[PM_COURE_ELADRIN]; b.data=&mons[PM_ORC];
    a.mhp=b.mhp=40; a.mcanmove=b.mcanmove=1;
    hit.damage=4; hit.weapon=expected_weapon=&weapon;
    mhitm_adtyping(&a,&atk,&b,&hit);
    assert(hit.damage==11 && physical_calls==1 && !rust_calls && !slept && !native_calls);
    a.data=&mons[PM_NOVIERE_ELADRIN]; atk.adtyp=AD_RUST; hit.damage=4;
    mhitm_adtyping(&a,&atk,&b,&hit);
    assert(hit.damage==11 && physical_calls==2 && rust_calls==1);
    a.mcan=1; hit.damage=4;
    mhitm_adtyping(&a,&atk,&b,&hit);
    assert(hit.damage==11 && physical_calls==3); a.mcan=0;
    rust_done=1; hit.done=FALSE; hit.damage=4;
    mhitm_adtyping(&a,&atk,&b,&hit);
    assert(hit.done && !hit.damage && physical_calls==3); rust_done=0;
    hit.done=FALSE; b.data=&mons[PM_IRON_GOLEM]; hit.damage=4;
    mhitm_adtyping(&a,&atk,&b,&hit);
    assert(hit.done && !hit.damage && physical_calls==3);
    a.data=&mons[PM_RUST_MONSTER]; b.data=&mons[PM_ORC];
    mhitm_adtyping(&a,&atk,&b,&hit); assert(native_calls==1);
    puts("PASS fey weapon damage, rust before physical, cancellation, lethal/restore exits and native exclusion");
    a.data=&mons[PM_COURE_ELADRIN]; atk.adtyp=AD_SLEE;
    for(hero=0;hero<2;++hero) for(level=1;level<=128;++level) {
        a.m_lev=level;
        for(value=0;value<max(1,7-level);++value) {
            roll=value; rolls=slept=duration=0;
            mith_coure_sleep(&a,hero?&gy.youmonst:&b);
            assert(slept==(value==0));
            if(slept) assert(duration>=1 && duration<=10);
            ++cases;
        }
    }
    resistance=1; rolls=slept=0; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); resistance=0;
    negated=1; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); negated=0;
    a.mcan=1; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); a.mcan=0;
    b.msleeping=1; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); b.msleeping=0;
    b.mcanmove=0; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); b.mcanmove=1;
    b.mhp=0; mith_coure_sleep(&a,&b); assert(!rolls&&!slept); b.mhp=40;
    gm.multi=-1; mith_coure_sleep(&a,&gy.youmonst); assert(!rolls&&!slept);
    printf("PASS %d Coure chance/duration cases, level-7+ clamp and immunity/cancellation/helplessness/death guards\n", cases);
    return 0;
}
