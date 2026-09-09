#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

struct you u;
struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
static uint32_t rng;
static int syllable, poly;
int rn2(int n) { assert(n > 0); rng = rng * 1664525U + 1013904223U; return (rng >> 8) % n; }
int rnd(int n) { return rn2(n) + 1; }
int d(int n, int sides) { int total=0; while (n-- > 0) total += rnd(sides); return total; }
#undef mith_mon_syllable
int mith_mon_syllable(struct monst *m) { (void)m; return syllable; }
int dist2(coordxy x, coordxy y, coordxy a, coordxy b) { return (x-a)*(x-a)+(y-b)*(y-b); }
void healup(int n, int extra, boolean a, boolean b) {
    (void)extra; (void)a; (void)b;
    if (poly) u.mh = min(u.mhmax, u.mh+n);
    else u.uhp = min(u.uhpmax, u.uhp+n);
}
#undef Upolyd
#define Upolyd poly
#undef canseemon
#define canseemon(m) FALSE
#define pline_mon(...) ((void)0)
#undef pline
#define pline(...) ((void)0)
#undef You
#define You(...) ((void)0)
#undef You_feel
#define You_feel(...) ((void)0)
#define MCASTU_ENUM
enum spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM
#include "step9c_spells.h"

int main(void)
{
    struct monst caster = {0}, local[6], expected[6];
    struct attack attack = { AT_MAGC, AD_SPEL, 0, 4 };
    uint32_t saved_rng;
    int i, seed, level, far, tame, hp, cases=0, n;
    monst_globals_init();
    caster.data = &mons[PM_ALABASTER_ELF_ELDER];
    for (seed=1; seed<=4096; ++seed) {
        rng=seed; n=mith_elder_spell(); saved_rng=rng;
        rng=seed; assert(n==donor_elder() && rng==saved_rng);
    }
    for (level=1; level<=128; ++level)
        for (syllable=-1; syllable<6; ++syllable)
            for (seed=1; seed<=16; ++seed) {
                caster.m_lev=level; attack.damn=seed%4; attack.damd=seed%7;
                rng=seed; n=mith_spell_damage(&caster,&attack); saved_rng=rng;
                rng=seed; assert(n==donor_damage(&caster,&attack) && rng==saved_rng);
                ++cases;
            }
    printf("PASS 4096 elder selections and %d pinned spell-dice comparisons including Krau/cap\n", cases);
    cases=0;
    for (far=0; far<2; ++far) for (tame=0; tame<2; ++tame)
        for (poly=0; poly<2; ++poly) for (seed=1; seed<=128; ++seed) {
            memset(local,0,sizeof local);
            for (i=0;i<6;++i) {
                local[i].data=caster.data; local[i].m_lev=seed;
                local[i].mx=10+i; local[i].my=10;
                local[i].mhp=10; local[i].mhpmax=500;
                local[i].mux=seed%2 ? 14 : 10; local[i].muy=10;
                local[i].mtame=tame;
            }
            local[2].mhp=0; local[3].mpeaceful=1;
            local[4].mhp=local[4].mhpmax; local[5].mx=30;
            memcpy(expected,local,sizeof local);
            for(i=0;i<5;++i) {local[i].nmon=&local[i+1];expected[i].nmon=&expected[i+1];}
            u.ux=13; u.uy=10; u.uhp=u.mh=10; u.uhpmax=u.mhmax=500;
            fmon=local; rng=seed;
            mith_mass_cure(&local[0],far,local[0].mux,local[0].muy);
            saved_rng=rng; hp=poly?u.mh:u.uhp;
            fmon=expected; rng=seed; u.uhp=u.mh=10;
            if(far) donor_far(&expected[0]); else donor_close(&expected[0]);
            assert(saved_rng==rng && hp==(poly?u.mh:u.uhp));
            for(i=0;i<6;++i) assert(local[i].mhp==expected[i].mhp);
            ++cases;
        }
    printf("PASS %d donor mass-heal comparisons: radius, allegiance, dead/full targets, self exclusion and pet/hero healing\n", cases);
    return 0;
}
