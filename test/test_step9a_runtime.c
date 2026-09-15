/* Production functions with deterministic RNG and UI/terrain fixture adapters. */
#define main step7_main
#include "test_step7_runtime.c"
#undef main
struct instance_globals_saved_l svl;
struct instance_globals_y gy;
struct display_hints disp;
static int ice_effects,hp_damage,item_damage;
#undef canseemon
#define canseemon(mon) TRUE
#undef cansee
#define cansee(x,y) TRUE
#undef Monnam
#define Monnam(mon) "Fixture"
#undef mon_nam
#define mon_nam(mon) "fixture"
void pline(const char *s,...) { (void)s; }
void pline_The(const char *s,...) { (void)s; }
void You(const char *s,...) { (void)s; }
int d(int n,int sides) { int total=0;while(n--)total+=rn2(sides)+1;return total; }
void seetrap(struct trap *t) { t->tseen=1; }
void shieldeff(coordxy x,coordxy y) { (void)x;(void)y; }
const char *surface(coordxy x,coordxy y) { (void)x;(void)y;return "floor"; }
void losehp(int n,const char *s,schar how) { (void)s;(void)how;hp_damage+=n; }
int destroy_items(struct monst *m,int type,int original) {
    (void)m;assert(type==AD_COLD && original>=2 && original<=16);
    item_damage++;return 0;
}
void monkilled(struct monst *m,const char *s,int type) { (void)s;(void)type;m->mhp=0; }
static boolean thitm(int lev,struct monst *m,struct obj *o,int dmg,boolean nc) {
    assert(!lev && !o && !nc);m->mhp-=dmg;return m->mhp<=0;
}
#undef resists_cold
#define resists_cold(mon) ((mon)->data->mresists & MR_COLD)
char *makeplural(const char *s) { return (char *)s; }
const char *pmname(struct permonst *m,int gender) { return m->pmnames[gender]; }
boolean is_lava(coordxy x,coordxy y) { return levl[x][y].typ==LAVAPOOL; }
boolean is_pool(coordxy x,coordxy y) {
    return levl[x][y].typ==POOL || levl[x][y].typ==MOAT;
}
void obj_ice_effects(coordxy x,coordxy y,boolean f) {
    assert(levl[x][y].typ==ICE && f);ice_effects++;
}
void newsym(coordxy x,coordxy y) { (void)x;(void)y; }
#include "step9a_functions.h"

static void freeze_contract(void) {
    struct monst m={0};int damage,i,lo=100,hi=0;
    gy.youmonst.data=&mons[PM_HUMAN];u.ux=10;u.uy=10;
    levl[10][10].typ=ROOM;
    for(i=0;i<1000;i++) {
        Frozen_feet=0;disp.botl=FALSE;damage=8;
        sheol_freeze(&gy.youmonst,&damage);
        assert(Frozen_feet>=2 && Frozen_feet<=17 && damage==8 && disp.botl);
        lo=min(lo,(int)Frozen_feet);hi=max(hi,(int)Frozen_feet);
    }
    assert(lo==2 && hi==17);
    HCold_resistance=FROMOUTSIDE;
    Frozen_feet=0;damage=8;sheol_freeze(&gy.youmonst,&damage);
    assert(Frozen_feet && damage==8);HCold_resistance=0;
    Frozen_feet=17;sheol_freeze(&gy.youmonst,&damage);assert(Frozen_feet==17);
    Frozen_feet=0;HLevitation=FROMOUTSIDE;damage=8;
    sheol_freeze(&gy.youmonst,&damage);assert(!Frozen_feet && !damage);
    HLevitation=0;
    gy.youmonst.data=&mons[PM_FIRE_ELEMENTAL];damage=8;
    sheol_freeze(&gy.youmonst,&damage);assert(!Frozen_feet && !damage);
    gy.youmonst.data=&mons[PM_HUMAN];levl[10][10].typ=POOL;damage=8;
    sheol_freeze(&gy.youmonst,&damage);
    assert(Frozen_feet && ice_effects==1 && levl[10][10].icedpool==ICED_POOL);
    Frozen_feet=0;levl[10][10].typ=MOAT;damage=8;
    sheol_freeze(&gy.youmonst,&damage);assert(!Frozen_feet && !damage);
    levl[10][10].typ=LAVAPOOL;damage=8;
    sheol_freeze(&gy.youmonst,&damage);assert(!Frozen_feet && damage==8);
    m.data=&mons[PM_HUMAN];m.mx=m.my=10;damage=8;
    sheol_freeze(&m,&damage);assert(!mon_frozen_feet(&m) && !damage);
    levl[10][10].typ=ROOM;damage=8;sheol_freeze(&m,&damage);
    assert(mon_frozen_feet(&m)>=2 && mon_frozen_feet(&m)<=17 && damage==8);
    set_dragon_revivals(&m,2);
    damage=8;sheol_freeze(&m,&damage);
    assert(dragon_revivals(&m)==2 && mon_frozen_feet(&m)>=2);
    set_mon_frozen_feet(&m,0);
    assert(dragon_revivals(&m)==2 && !mon_frozen_feet(&m));
    assert(!m.mfrozen && !m.mtrapped && !u.utrap);
    puts("PASS actual freeze: 2..17 movement timer, renewal, cold resistance, flight/flame/water/lava, pool ice, independent trap/paralysis state");
}

static void database_contract(void) {
    struct obj pick={0};int i;
    int ids[]={PM_ARCTIC_FERN_SPORE,PM_EVIL_EYE,PM_CHILLBUG,PM_DARK_ANGEL,
        PM_WEEPING_ANGEL,PM_WEEPING_ARCHANGEL,PM_ARCTIC_FERN_SPROUT,
        PM_ARCTIC_FERN,PM_WHITE_NAGA_HATCHLING,PM_WHITE_NAGA,PM_BLUE_SLIME,
        PM_ICE_GOLEM,PM_CRYSTAL_ICE_GOLEM,PM_EXECUTIONER,PM_PUNISHER};
    u.uz.dnum=0;
    for(i=0;i<SIZE(ids);i++)
        assert(uncommon(ids[i]) || !(mons[ids[i]].geno & G_FREQ));
    svn.n_dgns=5;strcpy(svd.dungeons[4].dname,"Sheol");u.uz.dnum=4;
    assert(!uncommon(PM_CHILLBUG) && !uncommon(PM_BLUE_SLIME));
    assert(!uncommon(PM_WHITE_DRAGON) && !uncommon(PM_RED_DRAGON));
    assert(uncommon(PM_EXECUTIONER));
    assert(golemhp(PM_ICE_GOLEM)==130 && golemhp(PM_CRYSTAL_ICE_GOLEM)==160);
    assert(mbirth_limit(PM_WEEPING_ARCHANGEL)==7);
    assert(mbirth_limit(PM_NAZGUL)==9 && mbirth_limit(PM_ERINYS)==3);
    assert(mons[PM_EXECUTIONER].geno==(G_UNIQ|G_NOGEN));
    assert(mons[PM_EXECUTIONER].difficulty==35);
    assert(mons[PM_PUNISHER].mattk[0].adtyp==AD_PUNI);
    assert((mons[PM_PUNISHER].mflags3&(M3_NOREGEN|M3_STATIONARY))==(M3_NOREGEN|M3_STATIONARY));
    pick.otyp=CRYSTAL_PICK;pick.oclass=objects[CRYSTAL_PICK].oc_class;
    assert(is_pick((&pick)));
    assert(objects[CRYSTAL_PICK].oc_prob==0 && objects[CRYSTAL_PICK].oc_weight==80);
    assert(objects[CRYSTAL_PICK].oc_cost==500 && objects[CRYSTAL_PICK].oc_wsdam==12 && objects[CRYSTAL_PICK].oc_wldam==10);
    assert(objects[FREEZING_ICE].oc_prob==0 && objects[FREEZING_ICE].oc_wsdam==6);
    puts("PASS actual Sheol databases: generation boundaries, unique boss, ice-golem HP, archangel limit, Punisher flags, crystal pick and ice projectile");
}

static void chillbug_contract(void) {
    struct monst a={0},b={0};int i;
    a.data=b.data=&mons[PM_CHILLBUG];a.mx=10;a.my=b.my=10;b.mx=11;
    a.mhpmax=b.mhpmax=100;a.mcanmove=b.mcanmove=1;
    a.nmon=&b;fmon=&a;
    a.mhp=10;b.mhp=90;
    for(i=0;i<100;i++)sheol_share_hp(&a,&b);
    assert(a.mhp==50 && b.mhp==50);
    a.mhp=40;b.mhp=90;a.mspec_used=b.mspec_used=14;
    sheol_chillbug_turn(&a);assert(a.mflee && b.mflee);
    a.mhp=b.mhp=100;a.mspec_used=0;
    sheol_chillbug_turn(&a);assert(!a.mflee && !b.mflee);
    assert(a.mspec_used==14 && b.mspec_used==14);
    fmon=NULL;
    puts("PASS actual chillbug HP redistribution, collective retreat and reattack");
}
static void cold_trap_contract(void) {
    struct trap trap={0};struct monst m={0};int i;
    trap.ttyp=ICE_TRAP;trap.tx=trap.ty=10;
    gy.youmonst.data=&mons[PM_HUMAN];HCold_resistance=0;
    hp_damage=item_damage=0;
    for(i=0;i<100;i++)assert(trapeffect_ice_trap(&gy.youmonst,&trap)==Trap_Effect_Finished);
    assert(trap.tseen && hp_damage>=400 && hp_damage<=1600 && item_damage==100);
    HCold_resistance=FROMOUTSIDE;hp_damage=item_damage=0;
    assert(trapeffect_ice_trap(&gy.youmonst,&trap)==Trap_Effect_Finished);
    assert(!hp_damage && item_damage==1);HCold_resistance=0;
    m.data=&mons[PM_HUMAN];m.mhp=100;item_damage=0;
    assert(trapeffect_ice_trap(&m,&trap)==Trap_Effect_Finished);
    assert(m.mhp>=92 && m.mhp<=98);
    m.data=&mons[PM_ICE_GOLEM];m.mhp=100;item_damage=0;
    assert(trapeffect_ice_trap(&m,&trap)==Trap_Effect_Finished);
    assert(m.mhp==100 && item_damage==0);
    m.data=&mons[PM_HUMAN];m.mhp=1;
    assert(trapeffect_ice_trap(&m,&trap)==Trap_Killed_Mon);
    puts("PASS actual cold-trap dispatch: 4d4 hero/2d4 monster, resistance, potion damage and lethal return");
}
#ifndef STEP9A_MAIN
#define STEP9A_MAIN main
#endif
int STEP9A_MAIN(void) {
    step7_main();database_contract();freeze_contract();chillbug_contract();cold_trap_contract();
    return 0;
}
