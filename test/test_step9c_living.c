#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
struct you u;
struct instance_globals_m gm;
struct instance_globals_b gb;
struct instance_globals_n gn;
struct instance_globals_y gy;
struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
struct obj *uarm;
static struct obj armor;
static struct monst targets[8];
static uint32_t rng;
static int hits,passives,hunger,blocked,paralyze,breakarmor,needs,sides,used;
int rn2(int n) { assert(n>0);rng=rng*1664525U+1013904223U;return(rng>>8)%n; }
int rnd(int n) { return rn2(n)+1; }
int isok(coordxy x,coordxy y) { return x>0 && x<COLNO && y>=0 && y<ROWNO; }
static struct monst *target_at(int x,int y) {
    int i;for(i=0;i<8;++i) if(targets[i].mx==x && targets[i].my==y) return &targets[i];return 0;
}
#undef m_at
#define m_at(x,y) target_at(x,y)
#undef Your
#define Your(...) ((void)0)
boolean clear_path(int x,int y,int a,int b) { (void)x;(void)y;(void)a;(void)b;return !blocked; }
static int find_roll_to_hit(struct monst *m,schar type,struct obj *o,int *count,int *penalty) {
    assert(m && type==AT_TENT && !o); ++*count;*penalty=0;return needs;
}
static int damageum(struct monst *m,struct attack *a,int extra) {
    int index=(int)(m-targets);
    assert(a->aatyp==AT_TENT && a->adtyp==AD_PHYS && a->damn==3 && a->damd==sides && !extra);
    assert(u.uswallow || !(used&(1<<index)));used|=1<<index;++hits;return 1;
}
static int passive(struct monst *m,struct obj *o,boolean hit,boolean alive,uchar type,boolean weapon) {
    (void)hit;assert(m && !o && alive && type==AT_TENT && !weapon);++passives;
    if(paralyze) gm.multi=-5;
    if(breakarmor) uarm=0;
    return 0;
}
void morehungry(int n) { assert(n==1);hunger+=n; }
#include "step9c_living.h"
static void reset(int seed) {
    static const int dx[8]={0,1,1,1,0,-1,-1,-1},dy[8]={-1,-1,0,1,1,1,0,-1};
    int i;memset(&u,0,sizeof u);memset(targets,0,sizeof targets);
    u.ux=u.uy=10;uarm=&armor;armor.otyp=LIVING_ARMOR;armor.spe=0;
    gy.youmonst.data=&mons[PM_HUMAN];
    for(i=0;i<8;++i) {
        targets[i].data=&mons[PM_HUMAN];targets[i].mhp=1000;
        targets[i].mx=10+dx[i];targets[i].my=10+dy[i];
    }
    gm.multi=0;gb.bhitpos.x=2;gb.bhitpos.y=3;gn.notonhead=TRUE;
    hits=passives=hunger=blocked=paralyze=breakarmor=used=0;needs=100;sides=3;rng=seed;
}
int main(void) {
    int seed,mode,i,cases=0,spe;uint32_t saved_rng;
    monst_globals_init();
    for(seed=1;seed<=1024;++seed) for(spe=-10;spe<=20;++spe) {
        reset(seed);armor.spe=spe;sides=max(1,3+spe);mith_living_armor_turn();
        assert(hits<=5 && hits==passives && hunger==hits);
        assert(gb.bhitpos.x==2 && gb.bhitpos.y==3 && gn.notonhead);
        ++cases;
    }
    for(mode=0;mode<11;++mode) {
        reset(1);saved_rng=rng;
        switch(mode) {
        case 0:uarm=0;break;
        case 1:armor.otyp=PLATE_MAIL;break;
        case 2:gm.multi=-5;break;
        case 3:u.usleep=1;break;
        case 4:blocked=1;break;
        default:for(i=0;i<8;++i) {
            if(mode==5) targets[i].mpeaceful=1;
            if(mode==6) targets[i].mtame=1;
            if(mode==7) targets[i].mhp=0;
            if(mode==8) targets[i].data=&mons[PM_COCKATRICE];
            if(mode==9) targets[i].data=&mons[PM_MEDUSA];
            if(mode==10) targets[i].data=&mons[PM_CHICKATRICE];
        }}
        mith_living_armor_turn();assert(!hits && !passives && !hunger);
        if(mode<4) assert(rng==saved_rng);
    }
    for(seed=1;seed<=1024;++seed) {
        reset(seed);needs=-100;mith_living_armor_turn();assert(!hits && hunger==passives && passives<=5);
        reset(seed);paralyze=1;mith_living_armor_turn();assert(hits==1 && hunger==1);
        reset(seed);breakarmor=1;mith_living_armor_turn();assert(hits==1 && hunger==1 && !uarm);
        reset(seed);u.uswallow=1;u.ustuck=&targets[0];needs=-100;
        mith_living_armor_turn();assert(hits==passives && hits<=5);
        reset(seed);gy.youmonst.data=&mons[PM_GRID_BUG];mith_living_armor_turn();
        assert(!(used&0xaa) && hits<=4);
    }
    printf("PASS %d actual living-armor enchantment/attack-limit cases plus helplessness, path, petrification, hunger, passive interruption, engulfment and grid-bug guards\n",cases);
    return 0;
}
