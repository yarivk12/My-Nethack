#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
struct you u;
struct instance_globals_saved_l svl;
struct instance_globals_saved_m svm;
struct instance_globals_v gv;
static uint32_t rng;
static struct monst target;
static struct trap pit;
static int saved, killed_count, buried, thrown, throwx, throwy, cancelled;
static int trap_exists, blocked_points, redraws, expelled;
int rn2(int n) { assert(n>0);rng=rng*1664525U+1013904223U;return (rng>>8)%n; }
int rnd(int n) { return rn2(n)+1; }
int d(int n,int s) { int sum=0;while(n--) sum+=rnd(s);return sum; }
int isok(coordxy x,coordxy y) { return x>0 && x<COLNO && y>=0 && y<ROWNO; }
boolean dmgtype(const struct permonst *p,int typ) {
    int i;for(i=0;i<NATTK;++i) if(p->mattk[i].adtyp==typ) return TRUE;return FALSE;
}
#undef m_at
#define m_at(x,y) (((x)==target.mx && (y)==target.my)?&target:0)
#undef couldsee
#define couldsee(x,y) ((x)>=10 && (x)<=13 && (y)==10)
#undef pline
#define pline(...) ((void)0)
void newsym(coordxy x,coordxy y) { (void)x;(void)y; }
void killed(struct monst *m) { ++killed_count;m->mhp=saved?10:0; }
void mhurtle(struct monst *m,int x,int y,int range) {
    (void)m;assert(range==33);++thrown;throwx=x;throwy=y;
}
void bury_objs(coordxy x,coordxy y) { (void)x;(void)y;++buried; }
void recalc_block_point(coordxy x,coordxy y) { (void)x;(void)y;++blocked_points; }
int doredraw(void) { ++redraws;return 0; }
int getdir(const char *s) { (void)s;return !cancelled; }
struct trap *t_at(coordxy x,coordxy y) { (void)x;(void)y;return trap_exists?&pit:0; }
struct trap *maketrap(coordxy x,coordxy y,int type) {
    pit.tx=x;pit.ty=y;pit.ttyp=type;trap_exists=1;return &pit;
}
void expels(struct monst *m,struct permonst *p,boolean msg) {
    (void)m;(void)p;(void)msg;u.uswallow=0;++expelled;
}
#include "step9c_word_combat.h"
static void reset(int species,int seed) {
    memset(&svl,0,sizeof svl);memset(&target,0,sizeof target);memset(&pit,0,sizeof pit);
    target.data=&mons[species];target.mx=11;target.my=10;
    target.mhp=target.mhpmax=10000;target.movement=24;
    u.ux=10;u.uy=10;u.dx=1;u.dy=0;u.uswallow=0;u.ustuck=0;
    rng=seed;saved=killed_count=buried=thrown=cancelled=trap_exists=0;
    blocked_points=redraws=expelled=0;
    levl[10][10].typ=ROOM;levl[11][10].typ=ROOM;
    levl[12][10].typ=SOIL;levl[13][10].typ=TREE;levl[13][10].looted=TREE_LOOTED;
}
int main(void) {
    int species,level,seed,hp,peace,cases=0,dead,life;
    uint32_t next;
    static const int types[]={PM_HUMAN,PM_WRAITH,PM_VAMPIRE,PM_HORNED_DEVIL,
        PM_UMBER_HULK,PM_DISENCHANTER,PM_ASPECT_OF_THE_SILENCE,PM_SHADOW};
    monst_globals_init();
    for(species=0;species<SIZE(types);++species) for(level=1;level<=30;++level)
        for(peace=0;peace<2;++peace) for(seed=1;seed<=64;++seed) {
            reset(types[species],seed);u.ulevel=level;target.mpeaceful=peace;
            mith_blessed_light(11,10);hp=target.mhp;next=rng;
            assert(levl[11][10].lit);
            reset(types[species],seed);target.mpeaceful=peace;
            donor_light(11,10);assert(target.mhp==hp && rng==next);++cases;
        }
    printf("PASS %d pinned First Word damage/multiplier/RNG comparisons\n",cases);
    cases=0;u.ulevel=30;
    for(seed=1;seed<=1024;++seed) for(life=0;life<2;++life) {
        reset(PM_HUMAN,seed);saved=life;
        levl[11][10].typ=POOL;levl[12][10].typ=PUDDLE;
        assert(mith_word_effect(1));
        assert(levl[11][10].typ==ROOM && levl[12][10].typ==ROOM);
        assert(pit.tx==11 && pit.ty==10 && pit.ttyp==PIT && pit.tseen);
        assert(killed_count+thrown==1);
        if(thrown) assert(throwx==0 && (throwy==1 || throwy==-1) && target.mhp<10000);
        else assert(target.mhp==(life?10:0));
        ++cases;
    }
    reset(PM_HUMAN,1);cancelled=1;assert(!mith_word_effect(1));assert(!thrown && !killed_count);
    reset(PM_HUMAN,1);u.dx=u.dy=0;assert(!mith_word_effect(1));
    reset(PM_HUMAN,1);target.mx=10;u.uswallow=1;u.ustuck=&target;
    assert(mith_word_effect(1));assert(expelled==1 && killed_count==1 && !u.uswallow);
    printf("PASS %d Dividing Word water/pits/bisection/hurtle/lifesaving cases and cancellation/engulfment\n",cases);
    cases=0;
    for(species=0;species<3;++species) for(dead=0;dead<2;++dead)
        for(life=0;life<2;++life) for(peace=0;peace<2;++peace) for(seed=1;seed<=128;++seed) {
            reset(species==0?PM_STONE_GOLEM:species==1?PM_HUMAN:PM_ASPECT_OF_THE_SILENCE,seed);
            saved=life;target.mpeaceful=peace;target.mhp=dead?1:10000;
            assert(mith_word_effect(2));
            assert(levl[10][10].typ==GRASS && levl[12][10].typ==GRASS);
            assert(!(levl[13][10].looted&TREE_LOOTED));
            if(species!=1 && !peace) {
                if(dead) {
                    assert(killed_count==1);
                    assert(levl[11][10].typ==(life?GRASS:TREE));
                    assert(buried==!life);
                } else assert(target.mhp<10000 && target.movement==12);
            } else assert(!killed_count && target.mhp==(dead?1:10000));
            assert(redraws==1 && blocked_points>=1);++cases;
        }
    printf("PASS %d Nurturing Word target/grass/tree/fruit/movement/lifesaving cases\n",cases);
    return 0;
}
