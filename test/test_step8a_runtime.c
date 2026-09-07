/* Inherit the Step 7 fixture and retain all its regression assertions. */
#define main step7_main
#include "test_step7_runtime.c"
#undef main

static int class_calls;
struct instance_globals_i gi;
struct instance_globals_y gy;
struct instance_globals_saved_l svl;
struct obj *uarmf;
static int spawns, adults, wet, rust, splits, blocked, clouds;
static NhRegion cloud;
int find_mac(struct monst *m) { return m->data->ac; }
NhRegion *create_gas_cloud(coordxy x, coordxy y, int radius, int damage) {
    (void)x;(void)y;assert(radius>=1 && radius<=2 && damage>=1 && damage<=8);
    clouds++;return &cloud;
}
void You(const char *s, ...) { (void)s; }
void Your(const char *s, ...) { (void)s; }
void Norep(const char *s, ...) { (void)s; }
int rnd(int n) { return rn2(n)+1; }
int dist2(coordxy x, coordxy y, coordxy a, coordxy b) {
    return (x-a)*(x-a)+(y-b)*(y-b);
}
boolean on_level(d_level *a, d_level *b) {
    return a->dnum==b->dnum && a->dlevel==b->dlevel;
}
void water_damage_chain(struct obj *o, boolean f) { (void)o;(void)f;wet++; }
int water_damage(struct obj *o, const char *s, boolean f) {
    (void)o;(void)s;(void)f;wet++;return 0;
}
void losehp(int n, const char *s, schar f) { (void)s;(void)f;rust+=n; }
struct monst *split_mon(struct monst *m, struct monst *a) {
    (void)a;splits++;return m;
}
boolean enexto(coord *c, coordxy x, coordxy y, struct permonst *m) {
    assert(m==&mons[PM_SWAMP_FERN_SPORE]);c->x=x+1;c->y=y;return !blocked;
}
struct monst *makemon(struct permonst *m, coordxy x, coordxy y, mmflags_nht f) {
    (void)x;(void)y;assert((m==&mons[PM_SWAMP_FERN_SPORE] || is_swamp_fern(m)) && f==NO_MM_FLAGS);
    adults+=(m==&mons[PM_SWAMP_FERN]);
    spawns++;return &gy.youmonst;
}
struct permonst *mkclass(char c, int f) {
    assert(c==S_ORC && f==0); class_calls++; return &mons[PM_HILL_ORC];
}
s_level *Is_special(d_level *lev) {
    s_level *p; for(p=svs.sp_levchn;p;p=p->next)
        if(p->dlevel.dnum==lev->dnum && p->dlevel.dlevel==lev->dlevel) return p;
    return NULL;
}
#include "step8a_functions.h"

/* Pinned donor util/makedefs.c:mstrength, restricted to these six definitions
 * (none has drain/stoning/poison/were damage or the grid-bug special case).
 * NetHack 5 stores the generated difficulty directly in the monster table. */
static int donor_strength(struct permonst *m) {
    int i,n=0,ranged=0;
    n+=(!!(m->geno&G_SGROUP))+2*(!!(m->geno&G_LGROUP));
    n+=(m->ac<4)+(m->ac<0)+(m->mmove>=18);
    for(i=0;i<NATTK;i++) {
        struct attack *a=&m->mattk[i];
        if(a->aatyp>=AT_WEAP || a->aatyp==AT_BREA || a->aatyp==AT_SPIT || a->aatyp==AT_GAZE) ranged=1;
        n+=(a->aatyp>0)+(a->aatyp==AT_MAGC);
        n+=(a->aatyp==AT_WEAP && (m->mflags2&M2_STRONG)!=0);
        n+=(a->adtyp!=AD_PHYS)+(a->damn*a->damd>23);
    }
    n+=ranged;
    return m->mlevel+(n==0?-1:n>=6?n/2:n/3+1);
}

static void definitions(void) {
    int ids[]={PM_DEEP_ORC,PM_DURINS_BANE,PM_WATCHER_IN_THE_WATER,
               PM_SWAMP_FERN,PM_SWAMP_FERN_SPROUT,PM_SWAMP_FERN_SPORE};
    int i;
    struct obj safe={0}, rock={0};
    for(i=0;i<6;i++) {
        struct permonst *m=&mons[ids[i]];
        assert(m->mlet && m->mattk[0].aatyp);
        assert(m->difficulty==donor_strength(m));
        if(ids[i]!=PM_SWAMP_FERN) assert(uncommon(ids[i]));
    }
    assert(mons[PM_DURINS_BANE].geno & G_UNIQ);
    assert(mons[PM_WATCHER_IN_THE_WATER].geno & G_UNIQ);
    assert(mons[PM_DURINS_BANE].mattk[0].damn==8);
    assert(mons[PM_DURINS_BANE].mattk[1].damn==4);
    assert(mons[PM_WATCHER_IN_THE_WATER].mattk[2].adtyp==AD_WRAP);
    assert(mons[PM_WATCHER_IN_THE_WATER].mresists==(MR_POISON|MR_SLEEP));
    assert(mons[PM_DEEP_ORC].mattk[1].aatyp==AT_WEAP);
    assert(mons[PM_DEEP_ORC].mresists==0);
    assert(mons[PM_SWAMP_FERN].mflags3 & M3_STATIONARY);
    assert(mons[PM_SWAMP_FERN_SPROUT].mattk[0].adtyp==AD_SPOR);
    assert(mons[PM_SWAMP_FERN_SPORE].mattk[1].aatyp==AT_BOOM);
    assert(mons[PM_SWAMP_FERN_SPORE].geno & G_NOCORPSE);
    safe.otyp=IRON_SAFE;
    assert(Is_box((&safe)) && Is_container((&safe)));
    assert(objects[IRON_SAFE].oc_material==IRON);
    assert(objects[IRON_SAFE].oc_weight==900 && objects[IRON_SAFE].oc_prob==10);
    rock.otyp=UNREFINED_MITHRIL;
    assert(objects[rock.otyp].oc_prob==0 && objects[rock.otyp].oc_cost==10000);
    assert(objects[rock.otyp].oc_weight==1 && objects[rock.otyp].oc_material==MINERAL);
    assert(IS_TREE(DEADTREE) && IS_OBSTRUCTED(DEADTREE));
    assert(!ACCESSIBLE(DEADTREE) && !ZAP_POS(DEADTREE));
    assert(ACCESSIBLE(BOG) && IS_SOFT(BOG) && !IS_POOL(BOG));
    assert(ICED_BOG < 32 && TT_SWAMP < 8);
    puts("PASS Moria production databases, generation restrictions, terrain predicates, safe container and mithril");
}

static void overrides(void) {
    s_level sp={0}; int n,v,i,hit,deep,expected;
    struct permonst *m;
    int pct[]={0,50,60,70,90,10,40};
    sp.dlevel.dnum=3; sp.dlevel.dlevel=6;
    svs.sp_levchn=&sp; u.uz=sp.dlevel;
    memset(&svm.mvitals,0,sizeof svm.mvitals);
    for(n=1;n<=6;n++) for(v=1;v<=(n==6?2:1);v++) {
        sprintf(sp.proto,"moria%d-%d",n,v);
        assert(moria_level(&u.uz)==n && moria_sky(&u.uz)==(n>=5));
        hit=deep=class_calls=0; rng_state=738492U;
        for(i=0;i<100000;i++) {
            m=moria_rndmonst(); hit+=(m!=NULL); deep+=(m==&mons[PM_DEEP_ORC]);
        }
        expected=n==6 && v==2 ? 60 : pct[n];
        assert(abs(hit-expected*1000)<1500);
        assert(abs(deep*10-hit*(n==6?8:7))<15000);
        assert(class_calls==hit-deep);
        svm.mvitals[PM_DEEP_ORC].mvflags=G_GENOD;
        for(i=0;i<1000;i++) assert(moria_rndmonst()!=&mons[PM_DEEP_ORC]);
        svm.mvitals[PM_DEEP_ORC].mvflags=0;
    }
    u.uz.dnum=0; assert(!moria_level(&u.uz) && !moria_rndmonst());
    svs.sp_levchn=NULL;
    puts("PASS 700000 branch-generation samples: donor override/weight probabilities, sky, identity, genocide fallback");
}

static void persistence_guard(void) {
    struct obj box={0}, buried={0}, required={0};
    int ids[]={AMULET_OF_YENDOR,BELL_OF_OPENING,CANDELABRUM_OF_INVOCATION,SPE_BOOK_OF_THE_DEAD},i;
    box.otyp=IRON_SAFE; buried.otyp=UNREFINED_MITHRIL;
    box.nobj=&buried; assert(!moria_unique_chain(&box));
    box.cobj=&required;
    for(i=0;i<4;i++) { required.otyp=ids[i]; assert(moria_unique_chain(&box)); }
    required.otyp=SAPPHIRE; assert(!moria_unique_chain(&box));
    puts("PASS nonpersistent-level guard protects nested invocation items; ordinary treasures do not freeze regeneration");
}
static void biology(void) {
    struct monst fern={0}; int i, count;
    memset(&u,0,sizeof u); u.ux=u.uy=10; fern.mx=fern.my=12;
    for(i=0;i<2;i++) {
        fern.data=&mons[i ? PM_SWAMP_FERN_SPROUT : PM_SWAMP_FERN];
        rng_state=859402;spawns=0;
        for(count=0;count<10000;count++) (void)fern_release(&fern);
        assert(abs(spawns-(i?2500:5000))<150);
    }
    fern.mcan=1;for(i=0;i<100;i++) assert(!fern_release(&fern));
    fern.mcan=0;fern.mx=40;for(i=0;i<100;i++) assert(!fern_release(&fern));
    fern.mx=12;blocked=1;for(i=0;i<100;i++) assert(!fern_release(&fern));
    gy.youmonst.data=&mons[PM_HUMAN];u.umonnum=PM_HUMAN;
    u.uz.dnum=4;wet=0;
    assert(swamp_effects() && u.utraptype==TT_SWAMP && u.utrap>=1 && u.utrap<=3);
    for(i=0;i<1000;i++) swamp_effects();
    assert(wet>150 && wet<250);
    u.utraptype=u.utrap=0;HWwalking=1;wet=0;
    for(i=0;i<100;i++) swamp_effects();
    assert(!u.utrap && !wet);
    HWwalking=0;HSwimming=1;
    for(i=0;i<100;i++) swamp_effects();
    assert(!u.utrap && wet);
    HSwimming=0;u.umonnum=PM_GREMLIN;
    for(i=0;i<100;i++) swamp_effects();assert(splits>40 && splits<90);
    u.umonnum=PM_IRON_GOLEM;u.mhmax=1000;
    for(i=0;i<10;i++) swamp_effects();assert(rust>=10 && rust<=60 && u.mhmax==1000-rust);
    puts("PASS actual fern release probabilities/cancellation/range/blocking and bog trapping/wetting/water-walking/swimming/gremlin/rust behavior");
    fern.data=&mons[PM_SWAMP_FERN_SPORE];fern.m_lev=30;
    assert(experience(&fern,1)==0 && experience(&fern,255)==0);
    assert(is_swamp_fern(&mons[PM_SWAMP_FERN]) && is_swamp_fern(&mons[PM_SWAMP_FERN_SPROUT]));
    assert(!is_swamp_fern(&mons[PM_VIOLET_FUNGUS]) && !is_swamp_fern(fern.data));
    spawns=clouds=0;levl[fern.mx][fern.my].typ=ROOM;
    for(i=0;i<100;i++) { swamp_spore_dies(&fern);assert(cloud.ttl>=2 && cloud.ttl<=4); }
    assert(clouds==100 && spawns==0);
    levl[fern.mx][fern.my].typ=BOG;spawns=adults=0;
    for(i=0;i<10000;i++) swamp_spore_dies(&fern);
    assert(spawns>3100 && spawns<3550 && adults>450 && adults<650);
    puts("PASS zero spore XP; 10100 real death-helper calls: poison clouds, bog-only regrowth, 1/3 plant and 1/6 adult probabilities");
}
int main(void) {
    step7_main(); definitions(); overrides(); persistence_guard(); biology(); return 0;
}
