#include "hack.h"
#include "region.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

struct you u;
struct instance_globals_y gy;
struct instance_globals_saved_l svl;
struct instance_globals_saved_m svm;
struct instance_globals_f gf;
struct instance_globals_m gm;
const struct const_globals cg = {0};
static uint32_t rng;
static int donor_x, donor_y, spawned, spawn_x, spawn_y, spawn_damage;
static int blind, breathless_hero, sick_res, constitution=12, hp, killed_type;
static int healed, stopped, lifesave;
static long sick;
static int fixed_roll=-1,desert,erased,wiped,buried,unearthed,redrawn;
static struct trap floortrap;
static boolean have_trap;
static NhRegion created;
static NhRect rectangles[6];
int rn2(int n) { assert(n>0); if(fixed_roll>=0)return fixed_roll%n; rng=rng*1664525U+1013904223U; return (rng>>8)%n; }
int rnd(int n) { return rn2(n)+1; }
int d(int n,int sides) { int sum=0; while(n--) sum+=rnd(sides); return sum; }
int isok(coordxy x,coordxy y) { return x>=1 && x<COLNO && y>=0 && y<ROWNO; }
NhRegion *create_region(NhRect *r,int n) {
    (void)r; assert(n==0); memset(&created,0,sizeof created);
    created.bounding_box.lx=COLNO; created.bounding_box.ly=ROWNO;
    created.rects=rectangles; return &created;
}
void add_rect_to_reg(NhRegion *r,NhRect *p) {
    assert(r->nrects<6); r->rects[r->nrects++]=*p;
    r->bounding_box.lx=min(r->bounding_box.lx,p->lx);
    r->bounding_box.ly=min(r->bounding_box.ly,p->ly);
    r->bounding_box.hx=max(r->bounding_box.hx,p->hx);
    r->bounding_box.hy=max(r->bounding_box.hy,p->hy);
}
void add_region(NhRegion *r) { assert(r==&created); }
NhRegion *create_dust_cloud(coordxy x,coordxy y,int radius,int damage) {
    assert(radius==damage && damage>=1 && damage<=6);
    ++spawned; spawn_x=x; spawn_y=y; spawn_damage=damage; return 0;
}
void healup(int n,int extra,boolean a,boolean b) {
    (void)extra;(void)a;(void)b; healed+=n;
}
void losehp(int n,const char *why,boolean format) { (void)why;(void)format; hp-=n; }
void nomul(int n) { assert(n==0||n==-3); ++stopped; }
void wipe_engr_at(coordxy x,coordxy y,xint16 n,boolean magical) {
    assert(x==10&&y==0&&n>=1&&n<=3&&!magical);++wiped;
}
void del_engr_at(coordxy x,coordxy y) { assert(x==10&&y==0);++erased; }
void bury_objs(int x,int y) { assert(x==10&&y==0);++buried; }
void unearth_objs(int x,int y) { assert(x==10&&y==0);++unearthed; }
void newsym(coordxy x,coordxy y) { assert(x==10&&y==0);++redrawn; }
void reset_utrap(boolean msg) { assert(msg);u.utrap=0; }
void seetrap(struct trap *t) { t->tseen=1; }
struct trap *t_at(coordxy x,coordxy y) {
    return have_trap&&x==floortrap.tx&&y==floortrap.ty ? &floortrap : 0;
}
boolean delfloortrap(struct trap *t) { assert(t==&floortrap);have_trap=FALSE;return TRUE; }
#undef In_mithardir_desert
#define In_mithardir_desert(lev) desert
#undef pline_The
#define pline_The(...) ((void)0)
void monkilled(struct monst *m,const char *why,int type) {
    (void)why;killed_type=type; if(lifesave) m->mhp=10;
}
#undef Blind
#undef Breathless
#undef Sick_resistance
#undef Sick
#undef ACURR
#undef canseemon
#undef cansee
#undef You
#undef pline
#undef cmap_to_glyph
#define Blind blind
#define Breathless breathless_hero
#define Sick_resistance sick_res
#define Sick sick
#define ACURR(a) constitution
#define canseemon(m) FALSE
#define cansee(x,y) FALSE
#define You(...) ((void)0)
#define pline(...) ((void)0)
#define cmap_to_glyph(n) (n)
#define make_blinded(n,talk) (blind=(int)(n))
#define make_sick(n,why,talk,type) (sick=(n))
#define setmangry(m) assert(0)
#define killed(m) assert(0)
#define INSIDE_DUST_CLOUD 4
#define EXPIRE_DUST_CLOUD 5
#include "step9c_dust.h"

static void reset(uint32_t seed) {
    rng=seed; hp=100; blind=0; sick=0; healed=0; stopped=0; killed_type=-1;
    spawned=0; spawn_x=spawn_y=spawn_damage=-1;
}
int main(void) {
    int seed,damage,species,i,x,y,blocked,checks=0;
    static const int types[]={PM_HUMAN,PM_IRON_GOLEM,PM_SENTINEL_OF_MITHARDIR,
        PM_GHOUL,PM_SHRIEKER,PM_ALABASTER_MUMMY,PM_DEEP_ONE,PM_XORN,
        PM_GARGOYLE,PM_FIRE_ELEMENTAL};
    NhRegion reg={0},*r;
    struct monst a,b;
    uint32_t next;
    int result,oldhp,oldblind,oldheal,oldstop,oldkill,ns,nx,ny,nd;
    long oldsick;
    monst_globals_init();
    for(x=1;x<COLNO;x+=13) for(y=0;y<ROWNO;y+=5)
        for(damage=1;damage<=6;++damage) for(seed=1;seed<=64;++seed) {
            reset(seed); r=actual_create_dust_cloud(x,y,damage,damage);
            assert(r && r->nrects==damage && r->arg.a_int==damage);
            assert(!heros_fault(r) && r->visible && r->glyph==S_dustcloud);
            assert(r->inside_f==INSIDE_DUST_CLOUD && r->expire_f==EXPIRE_DUST_CLOUD);
            assert(r->ttl>=3 && r->ttl<=9);
            assert(r->bounding_box.lx+r->bounding_box.hx==2*x);
            assert(r->bounding_box.ly+r->bounding_box.hy==2*y);
            for(i=0;i<damage;++i) {
                assert(r->rects[i].lx==x-i && r->rects[i].hx==x+i);
                assert(r->rects[i].ly==y-damage+1+i);
                assert(r->rects[i].hy==y+damage-1-i);
            }
            ++checks;
        }
    assert(!actual_create_dust_cloud(0,0,1,1));
    assert(!actual_create_dust_cloud(2,2,7,7));
    printf("PASS %d actual dust creation geometry/TTL/flags checks including map edges\n",checks);
    checks=0;
    for(blocked=0;blocked<2;++blocked) {
        for(x=0;x<COLNO;++x) for(y=0;y<ROWNO;++y) levl[x][y].typ=blocked?STONE:ROOM;
        for(donor_x=1;donor_x<COLNO;donor_x+=13)
            for(donor_y=0;donor_y<ROWNO;donor_y+=5)
                for(damage=1;damage<=6;++damage) for(seed=1;seed<=128;++seed) {
                    reg.bounding_box.lx=donor_x-damage+1;
                    reg.bounding_box.hx=donor_x+damage-1;
                    reg.bounding_box.ly=donor_y-damage+1;
                    reg.bounding_box.hy=donor_y+damage-1;
                    reg.arg.a_int=damage;
                    reset(seed); assert(expire_dust_cloud(&reg,0)); next=rng;
                    ns=spawned;nx=spawn_x;ny=spawn_y;nd=spawn_damage;
                    reset(seed); assert(donor_expire_dust_cloud(&reg,0));
                    assert(rng==next && spawned==ns && spawn_x==nx && spawn_y==ny && spawn_damage==nd);
                    ++checks;
                }
    }
    printf("PASS %d pinned dust drift/growth/shrink/RNG comparisons\n",checks);
    checks=0; clear_heros_fault(&reg);
    for(species=0;species<SIZE(types);++species) for(damage=1;damage<=6;++damage)
        for(seed=1;seed<=512;++seed) {
            reg.arg.a_int=damage; gy.youmonst.data=&mons[types[species]];
            breathless_hero=breathless(gy.youmonst.data); sick_res=seed%2;
            reset(seed); if(seed%3==0) sick=600;
            assert(!inside_dust_cloud(&reg,0)); next=rng;
            oldhp=hp;oldblind=blind;oldsick=sick;oldheal=healed;oldstop=stopped;
            reset(seed); if(seed%3==0) sick=600;
            assert(!donor_inside_dust_cloud(&reg,0));
            assert(rng==next && hp==oldhp && blind==oldblind && sick==oldsick && healed==oldheal && stopped==oldstop);
            memset(&a,0,sizeof a); a.data=&mons[types[species]];
            a.mhp=seed%3 ? 100 : 1; a.mhpmax=101; a.m_lev=20; a.mcansee=1;
            b=a;lifesave=seed%2;
            reset(seed);result=inside_dust_cloud(&reg,&a);next=rng;oldkill=killed_type;
            reset(seed);assert(donor_inside_dust_cloud(&reg,&b)==result);
            assert(rng==next && a.mhp==b.mhp && a.mcansee==b.mcansee && a.mblinded==b.mblinded && killed_type==oldkill);
            checks+=2;
        }
    printf("PASS %d pinned hero/monster dust damage, blindness, sickness, salt, immunity, sentinel healing and lifesaving comparisons\n",checks);
    memset(&svl.level,0,sizeof svl.level);
    levl[10][0].typ=SAND;floortrap.tx=10;floortrap.ty=0;floortrap.ttyp=PIT;
    u.ux=10;u.uy=0;u.utrap=3;u.utraptype=TT_PIT;have_trap=TRUE;
    fixed_roll=0;desert=0;spawned=stopped=0;
    mith_dust_storm();
    assert(!have_trap&&!u.utrap&&erased==1&&buried==1&&stopped==1&&wiped==1&&!spawned);
    assert(!strcmp(gm.multi_reason,"stuck in the sand"));
    fixed_roll=6000;mith_dust_storm();assert(unearthed==1&&wiped==2);
    fixed_roll=2000;mith_dust_storm();assert(buried==2&&wiped==3);
    fixed_roll=0;desert=1;floortrap.ttyp=MAGIC_PORTAL;floortrap.tx=12;
    gf.ftrap=&floortrap;floortrap.tseen=0;mith_dust_storm();
    assert(floortrap.tseen&&spawned==1);
    floortrap.tx=13;floortrap.tseen=0;mith_dust_storm();assert(!floortrap.tseen);
    puts("PASS actual dust terrain turn: engraving erosion, occupied pit fill/release/burial, unearth/bury rolls, desert cloud and portal radius");
    return 0;
}
