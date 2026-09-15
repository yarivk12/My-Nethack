#include "hack.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

struct you u;
struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
struct instance_globals_l gl;
struct display_hints disp;
static int healed, ac_updates, blocked_x;
void healup(int hp,int extra,boolean a,boolean b) {
    (void)extra;(void)a;(void)b;healed+=hp;
}
void find_ac(void) {++ac_updates;}
boolean get_mon_location(struct monst *m,coordxy *x,coordxy *y,int flags) {
    (void)flags;*x=m->mx;*y=m->my;return !DEADMONSTER(m);
}
boolean get_obj_location(struct obj *o,coordxy *x,coordxy *y,int flags) {
    (void)flags;*x=o->ox;*y=o->oy;return TRUE;
}
boolean clear_path(int x,int y,int a,int b) {
    (void)x;(void)y;(void)b;return a!=blocked_x;
}
#include "step9c_aspect.h"

int main(void) {
    struct monst m[3]={{0}};
    light_source source={0};
    seenV cells[ROWNO][COLNO],*rows[ROWNO];
    int i,x,y,energy,naen,chance,actual,cases=0;
    monst_globals_init();
    for(i=0;i<3;++i) {m[i].data=&mons[PM_ASPECT_OF_THE_SILENCE];m[i].mhp=100;}
    m[0].nmon=&m[1];m[1].nmon=&m[2];m[2].mhp=0;
    fmon=m;u.uenmax=200;
    for(energy=0;energy<=200;++energy) for(naen=0;naen<2;++naen) {
        memset(u.mith_syllables,0,sizeof u.mith_syllables);
        memset(u.mith_timers,0,sizeof u.mith_timers);
        u.uen=energy;u.mith_timers[MITH_NAEN]=naen;
        mith_syllable_turn();
        assert(u.uen==min(200,max(0,energy-6)+(naen?10:0)));
        assert(!u.mith_timers[MITH_NAEN]);
    }
    fmon=NULL;u.uen=100;mith_syllable_turn();assert(u.uen==100);
    puts("PASS exact three-energy drain per live Aspect, floor zero, dead exclusion and Naen order");
    m[0].nmon=NULL;fmon=m;u.ux=40;u.uy=10;
    for(x=1;x<COLNO;++x) for(y=0;y<ROWNO;++y)
        for(naen=0;naen<2;++naen) for(chance=-50;chance<=250;chance+=5) {
            m[0].mx=x;m[0].my=y;u.mith_timers[MITH_NAEN]=naen;
            assert(local_chance(chance)==donor_chance(chance,1));++cases;
        }
    u.mith_timers[MITH_NAEN]=0;m[0].mhp=0;
    assert(local_chance(90)==90);m[0].mhp=100;
    printf("PASS %d pinned distance/spell-chance comparisons, Naen override and dead exclusion\n",cases);
    source.type=LS_MONSTER;source.id.a_monst=m;source.range=3;gl.light_base=&source;
    u.ux=20;u.uy=10;
    for(y=0;y<ROWNO;++y)rows[y]=cells[y];
    for(i=0;i<3;++i) {
        /* Reuse one source while its owner moves, including map edges. */
        m[0].mx=i==0?1:i==1?40:79;m[0].my=i==0?0:i==1?10:20;
        for(blocked_x=0;blocked_x<3;++blocked_x) {
            memset(cells,0,sizeof cells);do_light_sources(rows);
            assert(source.x==m[0].mx&&source.y==m[0].my);
            for(y=0;y<ROWNO;++y)for(x=1;x<COLNO;++x) {
                int dx=abs(x-m[0].mx),dy=abs(y-m[0].my);
                boolean los=(dx==0&&dy==0)||x!=blocked_x;
                actual=dy<=3&&dx<=circle_ptr(3)[dy]&&los?MITH_DARK1:0;
                if(actual&&dy<=2&&dx<=circle_ptr(2)[dy])actual|=MITH_DARK2;
                assert(cells[y][x]==actual);
            }
        }
    }
    m[0].mhp=0;memset(cells,0,sizeof cells);do_light_sources(rows);
    assert(!(source.flags&LSF_SHOW));
    m[0].mhp=100;m[0].mx=u.ux;m[0].my=u.uy;
    memset(cells,0,sizeof cells);cells[u.uy][u.ux]=COULD_SEE;
    cells[u.uy][u.ux+3]=COULD_SEE;do_light_sources(rows);
    assert(cells[u.uy][u.ux]==(COULD_SEE|MITH_DARK1|MITH_DARK2));
    assert(cells[u.uy][u.ux+3]==(COULD_SEE|MITH_DARK1));
    assert(!cells[u.uy][u.ux+1]);
    m[0].mhp=100;m[0].data=&mons[PM_MOTE_OF_LIGHT];blocked_x=0;
    m[0].mx=40;memset(cells,0,sizeof cells);do_light_sources(rows);
    assert(cells[m[0].my][m[0].mx]==TEMP_LIT);
    puts("PASS moving Aspect radius3/radius2 darkness, map edges, blocked paths, dead source and native light");
    return 0;
}
