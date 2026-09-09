/* Actual production function bodies, with rendering-only fixture adapters. */
#define STEP9A_MAIN step9a_main
#include "test_step9a_runtime.c"
struct instance_globals_saved_c svc;
struct instance_globals_i gi;
struct instance_globals_saved_q svq;
struct sinfo program_state;
struct sysopt_s sysopt;
int rnd(int n) { return rn2(n)+1; }
struct obj *uwep, *uswapwep;
static int corpse_checks, block_updates;
void see_monsters(void) {}
boolean make_hallucinated(long t,boolean b,long m) { (void)t;(void)b;(void)m;return FALSE; }
void wielding_corpse(struct obj *o,struct obj *armor,boolean purpose) {
    (void)o;(void)armor;(void)purpose;corpse_checks++;
}
void mon_adjust_speed(struct monst *m,int n,struct obj *o) {
    (void)m;(void)n;(void)o;assert(0 && "unexpected speed effect");
}
void dismount_steed(int why) { (void)why;assert(0 && "unexpected dismount"); }
static int w_blocks(struct obj *o,long mask) { (void)o;(void)mask;return 0; }
void recalc_block_point(coordxy x,coordxy y) { (void)x;(void)y;block_updates++; }
/* Allocation/map adapters only; revival state changes execute montraits(). */
static struct monst restored, temporary;
struct monst *get_mtraits(struct obj *o,boolean copy) {
    assert(copy && has_omonst(o));restored=*OMONST(o);return &restored;
}
struct monst *makemon(struct permonst *pm,coordxy x,coordxy y,mmflags_nht flags) {
    assert(flags & MM_NOCOUNTBIRTH);memset(&temporary,0,sizeof temporary);
    temporary.data=pm;temporary.mx=x;temporary.my=y;
    temporary.m_lev=pm->mlevel;temporary.mhpmax=50;
    return &temporary;
}
void dealloc_monst(struct monst *m) { (void)m;assert(0 && "unexpected revival failure"); }
int monhp_per_lvl(struct monst *m) { (void)m;return 8; }
void neweshk(struct monst *m) { (void)m;assert(0 && "dragon is not a shopkeeper"); }
void replmon(struct monst *old,struct monst *next) {
    assert(old==&temporary && next==&restored);
}
void restore_cham(struct monst *m) { assert(m==&restored); }
static struct obj corpse_result, scale_result;
static struct monst *corpse_traits;
static int scale_drops;
struct obj *mksobj_at(int typ,coordxy x,coordxy y,boolean init,boolean art) {
    (void)init;(void)art;memset(&scale_result,0,sizeof scale_result);
    scale_result.otyp=typ;scale_result.ox=x;scale_result.oy=y;
    assert(Is_dragon_scales(&scale_result));scale_drops++;return &scale_result;
}
struct obj *mkcorpstat(int typ,struct monst *mon,struct permonst *pm,
                       coordxy x,coordxy y,unsigned flags) {
    assert(typ==CORPSE && (flags&CORPSTAT_INIT));
    memset(&corpse_result,0,sizeof corpse_result);
    corpse_result.otyp=typ;corpse_result.corpsenm=monsndx(pm);
    corpse_result.ox=x;corpse_result.oy=y;corpse_traits=mon;
    return &corpse_result;
}
void pline_mon(struct monst *m,const char *s,...) { (void)m;(void)s; }
char *s_suffix(const char *s) { return (char *)s; }
int undead_to_corpse(int i) { (void)i;assert(0);return 0; }
void free_mgivenname(struct monst *m) { (void)m; }
int weight(struct obj *o) { (void)o;return 1; }
int rnl(int n) { return rn2(n); }
struct obj *mkgold(long n,coordxy x,coordxy y) {
    (void)n;(void)x;(void)y;assert(0);return 0;
}
struct obj *obj_nexto(struct obj *o) { (void)o;return 0; }
struct obj *obj_meld(struct obj **a,struct obj **b) { (void)b;return *a; }
void pudding_merge_message(struct obj *a,struct obj *b) { (void)a;(void)b; }
struct obj *bury_an_obj(struct obj *o,boolean *gone) { *gone=FALSE;return o; }
void bypass_obj(struct obj *o) { (void)o; }
struct obj *oname(struct obj *o,const char *s,unsigned f) { (void)s;(void)f;return o; }
int sensemon(struct monst *m) { (void)m;return 1; }
void clear_dknown(struct obj *o) { o->dknown=0; }
void stackobj(struct obj *o) { (void)o; }
#define altprop(o) (((o)->otyp==ALCHEMY_SMOCK) \
    ? POISON_RES+ACID_RES-objects[(o)->otyp].oc_oprop : 0)
#include "step9b_functions.h"

static void armor_contract(void) {
    int types[]={GLOWING_DRAGON_SCALES,GLOWING_DRAGON_SCALE_MAIL,
                 CHROMATIC_DRAGON_SCALES,CHROMATIC_DRAGON_SCALE_MAIL};
    int p,i;struct monst m={0};struct obj a={0},smock={0};
    const int powers[]={FIRE_RES,COLD_RES,SLEEP_RES,DISINT_RES,SHOCK_RES,
                        POISON_RES,ACID_RES,STONE_RES,REFLECTING,ANTIMAGIC};
    for(i=0;i<4;i++) {
        a.otyp=types[i];a.owornmask=W_ARM;
        assert(Is_dragon_armor(&a));
        assert(armor_to_dragon(a.otyp)==(i<2?PM_GLOWING_DRAGON:PM_CAVE_CHROMATIC_DRAGON));
        assert((Is_dragon_scales(&a)?Dragon_scales_to_pm(&a):Dragon_mail_to_pm(&a))
               ==&mons[armor_to_dragon(a.otyp)]);
        if(i<2)continue;
        memset(u.uprops,0,sizeof u.uprops);
        EFire_resistance=W_RINGL;HAntimagic=FROMOUTSIDE;
        dragon_armor_handling(&a,TRUE,TRUE);
        for(p=0;p<SIZE(powers);p++)assert(u.uprops[powers[p]].extrinsic&W_ARM);
        dragon_armor_handling(&a,FALSE,TRUE);
        for(p=0;p<SIZE(powers);p++)assert(!(u.uprops[powers[p]].extrinsic&W_ARM));
        assert(EFire_resistance==W_RINGL && HAntimagic==FROMOUTSIDE);
        assert(corpse_checks>=2);
        m.data=&mons[PM_HUMAN];m.minvent=&a;a.nobj=&smock;
        smock.otyp=ALCHEMY_SMOCK;smock.owornmask=W_ARMC;
        m.mextrinsics=0;update_mon_extrinsics(&m,&a,TRUE,TRUE);
        assert((m.mextrinsics&255)==255);
        update_mon_extrinsics(&m,&a,FALSE,TRUE);
        assert(m.mextrinsics==(MR_POISON|MR_ACID));
    }
    puts("PASS actual hero/monster chromatic armor: all powers, removal/other-source preservation, corpse safety and body mappings");
}
static void dragon_contract(void) {
    struct monst m={0};int i,seen[AD_SPC2+1]={0};
    assert(PM_CAVE_CHROMATIC_DRAGON!=PM_CHROMATIC_DRAGON);
    assert((mons[PM_CHROMATIC_DRAGON].geno&G_UNIQ)!=0);
    assert(!(mons[PM_CAVE_CHROMATIC_DRAGON].geno&(G_UNIQ|G_GENO|G_FREQ)));
    assert(mons[PM_CAVE_CHROMATIC_DRAGON].geno&G_NOGEN);
    assert(mons[PM_CAVE_CHROMATIC_DRAGON].mflags2&M2_NOPOLY);
    for(i=0;i<2;i++) {
        m.data=&mons[i?PM_GLOWING_DRAGON:PM_CAVE_CHROMATIC_DRAGON];
        m.mspare1=0;assert(cave_dragon_scale_chance(&m)==(i?3:6));
        set_mon_frozen_feet(&m,17);set_dragon_revivals(&m,1);
        assert(cave_dragon_scale_chance(&m)==20 && mon_frozen_feet(&m)==17);
        set_dragon_revivals(&m,2);set_mon_frozen_feet(&m,0);
        assert(!cave_dragon_scale_chance(&m) && !mon_frozen_feet(&m));
    }
    for(i=0;i<4096;i++) {
        int type=cave_breath_type(&mons[PM_CAVE_CHROMATIC_DRAGON],AD_RBRE);
        assert(type>=AD_MAGM && type<=AD_LAVA);seen[type]++;
        assert(cave_breath_type(&mons[PM_CHROMATIC_DRAGON],AD_RBRE)!=AD_LAVA);
    }
    for(i=AD_MAGM;i<=AD_LAVA;i++)assert(seen[i]>0);
    assert(emits_light(&mons[PM_GLOWING_DRAGON])==1);
    assert(emits_light(&mons[PM_BABY_GLOWING_DRAGON])==1);
    puts("PASS imported identity/restrictions, finite scale chances and independent state, nine breaths/native pool isolation, light");
}
static void lava_contract(void) {
    struct rm *r=&levl[10][10];
    memset(r,0,sizeof *r);r->typ=VWALL;lava_jet_obstacle(10,10);
    assert(r->typ==LAVAPOOL && r->lit);
    r->typ=VWALL;r->wall_info=W_NONDIGGABLE;lava_jet_obstacle(10,10);
    assert(r->typ==VWALL);
    r->typ=CRYSTALICEWALL;lava_jet_obstacle(10,10);assert(r->typ==ICE);
    r->typ=ICEWALL;lava_jet_obstacle(10,10);assert(r->typ==ICE);
    r->typ=BOG;lava_jet_obstacle(10,10);assert(r->typ==BOG);
    r->typ=MOAT;lava_jet_obstacle(10,10);assert(r->typ==MOAT);
    assert(block_updates==3);
    puts("PASS actual lava obstacle handler: stone to lava, nondiggable restriction, both ice walls, open water/bog unchanged");
}
static void revival_contract(void) {
    struct obj corpse={0};struct oextra extra={0};struct monst traits={0},*m;
    coord cc={10,10};int species,cycle;
    corpse.otyp=CORPSE;corpse.oextra=&extra;extra.omonst=&traits;
    for(species=0;species<2;species++) {
        memset(&traits,0,sizeof traits);
        traits.mnum=species?PM_CAVE_CHROMATIC_DRAGON:PM_GLOWING_DRAGON;
        traits.data=&mons[traits.mnum];traits.mhpmax=140;
        traits.m_lev=traits.data->mlevel;traits.m_id=123;
        for(cycle=1;cycle<=4;cycle++) {
            set_mon_frozen_feet(&traits,17);
            m=montraits(&corpse,&cc,FALSE);
            assert(m && m->mhp==140 && m->m_id==123 && m->mrevived);
            assert(dragon_revivals(m)==min(cycle,2));
            assert(!mon_frozen_feet(m));
            assert(cave_dragon_scale_chance(m)==(cycle==1?20:0));
            traits=*m; /* next corpse stores the revived monster's traits */
        }
    }
    puts("PASS actual montraits revival: identity/HP restored, freeze released, saved count saturates and later scales exhausted");
}
static void corpse_contract(void) {
    const int ids[]={PM_DEEP_ORC,PM_DURINS_BANE,PM_WATCHER_IN_THE_WATER,
        PM_SWAMP_FERN,PM_SWAMP_FERN_SPROUT,PM_SWAMP_FERN_SPORE,
        PM_ARCTIC_FERN,PM_ARCTIC_FERN_SPROUT,PM_ARCTIC_FERN_SPORE,
        PM_EVIL_EYE,PM_CHILLBUG,PM_DARK_ANGEL,PM_WEEPING_ANGEL,
        PM_WEEPING_ARCHANGEL,PM_WHITE_NAGA,PM_WHITE_NAGA_HATCHLING,
        PM_BLUE_SLIME,PM_ICE_GOLEM,PM_CRYSTAL_ICE_GOLEM,PM_EXECUTIONER,
        PM_PUNISHER,PM_BABY_GLOWING_DRAGON};
    struct monst m={0};struct obj *o;int i,species,cycle;
    m.m_id=1234;m.mx=m.my=10;svq.quest_status.leader_m_id=0;
    for(i=0;i<SIZE(ids);i++) {
        m.data=&mons[ids[i]];
        svm.mvitals[ids[i]].mvflags=m.data->geno&G_NOCORPSE;
        o=make_corpse(&m,0);
        if(m.data->geno&G_NOCORPSE)assert(!o);
        else assert(o && o->otyp==CORPSE && o->corpsenm==ids[i]);
    }
    for(species=0;species<2;species++) {
        m.data=&mons[species?PM_CAVE_CHROMATIC_DRAGON:PM_GLOWING_DRAGON];
        for(cycle=0;cycle<3;cycle++) {
            scale_drops=0;set_dragon_revivals(&m,cycle);
            for(i=0;i<4096;i++) {
                o=make_corpse(&m,0);
                assert(o && corpse_traits==&m);
                assert(o->corpsenm==monsndx(m.data));
            }
            if(cycle==2)assert(scale_drops==0);
            else if(cycle==1)assert(scale_drops>120 && scale_drops<300);
            else if(species)assert(scale_drops>550 && scale_drops<820);
            else assert(scale_drops>1200 && scale_drops<1520);
        }
    }
    puts("PASS actual corpse dispatch for all Moria/Sheol additions, NOCORPSE exclusions, cave dragon trait retention and finite scale drops over 24576 deaths");
}
int main(void) {
    step9a_main();armor_contract();dragon_contract();lava_contract();revival_contract();corpse_contract();return 0;
}
