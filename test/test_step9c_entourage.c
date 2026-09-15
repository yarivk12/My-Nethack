#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
static uint32_t rng;
static int entries, journal[128][3];
static struct monst companion;
int rn2(int n) { assert(n>0);rng=rng*1664525U+1013904223U;return (rng>>8)%n; }
static void record(int action,int type,int flags) {
    assert(entries<128);journal[entries][0]=action;journal[entries][1]=type;
    journal[entries++][2]=flags;
}
struct monst *makemon(struct permonst *ptr,coordxy x,coordxy y,mmflags_nht flags) {
    assert(x==12 && y==3 && flags==MM_ADJACENTOK);
    record(0,monsndx(ptr),flags);
    if(!rn2(13)) return 0; /* creation failure must not dereference a null */
    companion.data=ptr; return &companion;
}
static void fixture_group(struct monst *mon,coordxy x,coordxy y,int n,mmflags_nht flags) {
    assert(mon && x==12 && y==3);
    record(n,monsndx(mon->data),flags); (void)rn2(11);
}
#define m_initsgrp(m,x,y,f) fixture_group(m,x,y,3,f)
#define m_initlgrp(m,x,y,f) fixture_group(m,x,y,10,f)
#include "step9c_entourage.h"
int main(void) {
    struct monst mon={0}; uint32_t after; int saved[128][3];
    int types[]={PM_DEEPEST_ONE,PM_DEEPER_ONE,PM_ALABASTER_ELF_ELDER,PM_ORC};
    mmflags_nht flags[]={0,MM_EDOG,MM_NOGRP,MM_ANGRY};
    int i,f,any,seed,count,total=0;
    monst_globals_init();mon.mx=12;mon.my=3;
    for(i=0;i<SIZE(types);++i) for(f=0;f<SIZE(flags);++f)
        for(any=0;any<2;++any) for(seed=1;seed<=4096;++seed) {
            mon.data=&mons[types[i]];
            rng=seed;entries=0;memset(journal,0,sizeof journal);
            mith_entourage(&mon,(boolean)any,flags[f]);
            count=entries;after=rng;memcpy(saved,journal,sizeof saved);
            rng=seed;entries=0;memset(journal,0,sizeof journal);
            donor_entourage(&mon,(boolean)any,flags[f]);
            assert(count==entries && after==rng && !memcmp(saved,journal,sizeof saved));
            if(!any || f==1 || f==2 || i==3) assert(!entries && rng==(uint32_t)seed);
            ++total;
        }
    printf("PASS %d pinned entourage order/RNG/failed-creation comparisons; native, explicit, pet and no-group exclusions\n",total);
    return 0;
}
