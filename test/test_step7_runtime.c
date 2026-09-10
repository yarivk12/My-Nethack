/* Production scheduler/light function bodies extracted by run_step7.py.
 * Fixture adapters supply RNG, topology lists and light/timer bookkeeping;
 * no production test hooks. Playable save/traversal is tested separately. */
#include "hack.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct you u;
struct instance_globals_saved_b svb;
struct instance_globals_saved_d svd;
struct instance_globals_saved_s svs;
struct instance_globals_saved_m svm;
struct instance_globals_saved_n svn;
static unsigned rng_state;
static int light_count, timer_count, light_radius;
static branch temple, tomb, moria, sheol, dragon_caves, mithardir;
static s_level mithardir_approach;

void panic(const char *fmt, ...) { fprintf(stderr, "%s\n", fmt); abort(); }
void impossible(const char *fmt, ...) { fprintf(stderr, "%s\n", fmt); abort(); }
long *alloc(unsigned n) { return (long *) calloc(1, n); }
int rn2(int n) { rng_state = rng_state * 1664525U + 1013904223U;
    return (int) ((rng_state >> 1) % (unsigned) n); }
int dname_to_dnum(const char *s) {
    return !strcmp(s,"The Dungeons of Doom") ? 0
        : !strcmp(s,"The Temple of Moloch") ? 1
        : !strcmp(s,"The Lost Tomb") ? 2
        : !strcmp(s,"The Ruins of Moria") ? 3
        : !strcmp(s,"Sheol") ? 4
        : !strcmp(s,"The Dragon Caves") ? 5
        : !strcmp(s,"Mithardir") ? 6 : -1;
}
s_level *find_level(const char *s) {
    s_level *p; for (p=svs.sp_levchn; p; p=p->next)
        if (!strcmp(p->proto,s)) return p;
    return NULL;
}
static void add_level(s_level *p) { p->next=svs.sp_levchn; svs.sp_levchn=p; }
void insert_branch(branch *p, boolean extract) { (void)p; (void)extract; }
boolean In_hell(d_level *lev) { (void)lev; return FALSE; }
int inhishop(struct monst *m) { (void)m; return TRUE; }
static long get_cost(struct obj *o, struct monst *m) {
    (void)m; return objects[o->otyp].oc_cost;
}
boolean artifact_light(struct obj *o) { (void)o; return FALSE; }
int arti_light_radius(struct obj *o) { (void)o; return 2; }
char *xname(struct obj *o) { (void)o; return "fixture"; }
void update_inventory(void) {}
anything *obj_to_any(struct obj *o) { static anything a; a.a_obj=o; return &a; }
boolean get_obj_location(struct obj *o, coordxy *x, coordxy *y, int f) {
    (void)o; (void)f; *x=10; *y=10; return TRUE;
}
void new_light_source(coordxy x, coordxy y, int r, int t, anything *a) {
    (void)x; (void)y; (void)t; (void)a; light_count++; light_radius=r;
}
void del_light_source(int t, anything *a) {
    (void)t; (void)a; assert(light_count > 0); light_count--;
}
boolean start_timer(long turns, short kind, short fn, anything *a) {
    (void)turns; (void)kind; (void)fn; a->a_obj->timed++; timer_count++; return TRUE;
}
long stop_timer(short fn, anything *a) {
    (void)fn; assert(timer_count > 0); timer_count--;
    a->a_obj->timed--; a->a_obj->lamplit=0; light_count--; return 1;
}

#include "step7_functions.h"

static void topology(void) {
    int sample, level, seen[201]={0}, moria_seen[201]={0},
        step9_seen[201]={0}, variations=0;
    for (sample=1; sample<=2000; sample++) {
        int used[201]={0}, big=0, giant=0, zoo=0, dragon=0;
        s_level *p, *next;
        rng_state=(unsigned)sample;
        memset(&svd,0,sizeof svd); memset(&temple,0,sizeof temple);
        memset(&tomb,0,sizeof tomb);
        memset(&mithardir_approach,0,sizeof mithardir_approach);
        svd.dungeons[0].num_dunlevs=200; svd.dungeons[0].depth_start=1;
        svd.dungeons[1].entry_lev=svd.dungeons[2].entry_lev=1;
        svd.dungeons[3].entry_lev=6;
        svd.dungeons[4].entry_lev=svd.dungeons[5].entry_lev=
            svd.dungeons[6].entry_lev=1;
        temple.end1.dlevel=30; temple.end2.dnum=1; temple.end2.dlevel=1;
        tomb.end1.dlevel=31; tomb.end2.dnum=2; tomb.end2.dlevel=1;
        memset(&moria, 0, sizeof moria);
        moria.end1.dlevel=45; moria.end1_up=TRUE;
        moria.end2.dnum=3; moria.end2.dlevel=6;
        memset(&sheol, 0, sizeof sheol);
        sheol.end1.dlevel=108; sheol.end2.dnum=4; sheol.end2.dlevel=1;
        memset(&dragon_caves, 0, sizeof dragon_caves);
        dragon_caves.end1.dlevel=109;
        dragon_caves.end2.dnum=5; dragon_caves.end2.dlevel=1;
        memset(&mithardir, 0, sizeof mithardir);
        mithardir.end1.dlevel=110; mithardir.end2.dnum=6;
        mithardir.end2.dlevel=1; mithardir.type=BR_PORTAL;
        strcpy(mithardir_approach.proto,"chalv2");
        mithardir_approach.dlevel.dnum=0;
        mithardir_approach.dlevel.dlevel=110;
        svs.sp_levchn=&mithardir_approach;
        temple.next=&tomb; tomb.next=&moria; moria.next=&sheol;
        sheol.next=&dragon_caves; dragon_caves.next=&mithardir;
        svb.branches=&temple;
        step6b_add_level("medusa",196,'M',0);
        step6b_add_level("castle",200,'C',0);
        if (sample%2) step6b_add_level("bigrm",12,'B',14);
        step6b_schedule();
        assert(sheol.end1.dlevel>=30 && sheol.end1.dlevel<=199);
        assert(dragon_caves.end1.dlevel>=30
               && dragon_caves.end1.dlevel<=199);
        assert(mithardir.end1.dlevel>=30 && mithardir.end1.dlevel<=199);
        assert(sheol.end1.dlevel != dragon_caves.end1.dlevel
               && sheol.end1.dlevel != mithardir.end1.dlevel
               && dragon_caves.end1.dlevel != mithardir.end1.dlevel);
        assert(svd.dungeons[4].depth_start==sheol.end1.dlevel+1);
        assert(svd.dungeons[5].depth_start==dragon_caves.end1.dlevel+1);
        assert(svd.dungeons[6].depth_start==mithardir.end1.dlevel);
        assert(mithardir_approach.dlevel.dlevel==mithardir.end1.dlevel);
        assert(!used[sheol.end1.dlevel]++);
        assert(!used[dragon_caves.end1.dlevel]++);
        assert(!used[mithardir.end1.dlevel]++);
        step9_seen[sheol.end1.dlevel]++;
        step9_seen[dragon_caves.end1.dlevel]++;
        step9_seen[mithardir.end1.dlevel]++;
        assert(temple.end1.dlevel>=30 && temple.end1.dlevel<=199);
        assert(tomb.end1.dlevel>=30 && tomb.end1.dlevel<=199);
        assert(tomb.end1.dlevel!=temple.end1.dlevel);
        assert(svd.dungeons[2].depth_start==tomb.end1.dlevel+1);
        assert(svd.dungeons[1].depth_start==temple.end1.dlevel+1);
        assert(!used[tomb.end1.dlevel]++);
        assert(!used[temple.end1.dlevel]++);
        assert(moria.end1.dlevel>=30 && moria.end1.dlevel<=199
               && moria.end1_up && moria.end2.dlevel==6);
        assert(!used[moria.end1.dlevel]++);
        assert(svd.dungeons[3].depth_start == moria.end1.dlevel - 6);
        moria_seen[moria.end1.dlevel]++;
        seen[tomb.end1.dlevel]++;
        for (p=svs.sp_levchn;p;p=p->next) {
            level=p->dlevel.dlevel;
            if (!strcmp(p->proto,"chalv2"))
                continue; /* the Mithardir approach shares its parent */
            assert(!used[level]++);
            if (!strcmp(p->proto,"bigrm")) { big++; assert(p->rndlevs==14); }
            if (!strcmp(p->proto,"x6b-giant")) giant++;
            if (!strcmp(p->proto,"x6b-realzoo")) zoo++;
            if (!strcmp(p->proto,"x6b-dragon")) dragon++;
            if (!strncmp(p->proto,"x6b-",4)) assert(level>=30 && level<=199);
        }
        assert(big>=3 && big<=5 && giant==1 && (zoo==2 || zoo==3) && dragon==1);
        for (p=svs.sp_levchn;p;p=next) {
            next=p->next;
            if (p != &mithardir_approach)
                free(p);
        }
        svs.sp_levchn=NULL;
    }
    for (level=30;level<=199;level++) variations+=(moria_seen[level]>0);
    assert(variations>100);
    for (level=108; level<=111; level++)
        assert(step9_seen[level]>0);
    assert(seen[111]>0 && moria_seen[111]>0);
    puts("PASS Step9A-C randomized parents use the existing scheduler; Step9D is absent");
    printf("PASS: 2000 production scheduler samples; %d distinct Moria depths; Step 6/9 counts/collisions/depth semantics\n",variations);
}

static void database(void) {
    struct permonst *m=&mons[PM_SHADOW];
    struct objclass *o=&objects[MAGIC_CANDLE];
    int i;
    assert(m->mlevel==10 && m->mmove==9 && m->ac==-2 && m->mr==0);
    assert(m->geno==(G_NOCORPSE|G_NOGEN));
    assert(m->difficulty==10 && m->mcolor==DRAGON_SILVER);
    assert(m->mattk[0].aatyp==AT_TUCH && m->mattk[0].adtyp==AD_DRST);
    assert(m->mattk[1].aatyp==AT_TUCH && m->mattk[1].adtyp==AD_COLD);
    for(i=0;i<2;i++) assert(m->mattk[i].damn==4 && m->mattk[i].damd==4);
    assert(m->mresists==(MR_COLD|MR_DISINT|MR_SLEEP|MR_POISON|MR_STONE|MR_ACID|MR_ELEC));
    assert(is_undead(m) && noncorporeal(m) && passes_walls(m) && unsolid(m));
    assert(shadelike(m) && hates_light(m));
    for(i=0;i<NUMMONS;i++) { /* exercise production random exclusion */
        if (i==PM_SHADOW) assert(uncommon(i));
    }
    assert(o->oc_class==TOOL_CLASS && o->oc_magic && o->oc_merge);
    assert(o->oc_prob==5 && o->oc_weight==2 && o->oc_cost==500 && o->oc_material==WAX);
    assert(objects[MAGIC_LAMP].oc_prob==15 && objects[MAGIC_LAMP].oc_cost==50);
    puts("PASS: actual monster/object tables, Shadow properties and random exclusion, Magic Candle tool definition, magic lamp retained");
}

static void lights(void) {
    struct obj o={0}; int i;
    o.otyp=MAGIC_CANDLE; o.quan=1; o.where=OBJ_INVENT;
    assert(Is_candle((&o)) && ignitable(&o) && !age_is_relative(&o));
    for(i=0;i<100;i++) {
        begin_burn(&o,FALSE);
        assert(o.lamplit && o.age==300 && !o.timed && !timer_count);
        assert(light_count==1 && light_radius==3);
        svm.moves+=10000; /* no burn timer exists to consume fuel */
        assert(o.age==300);
        end_burn(&o,TRUE);
        assert(!o.lamplit && !light_count && !timer_count);
    }
    o.otyp=MAGIC_LAMP; o.age=0;
    begin_burn(&o,FALSE); assert(o.lamplit && !timer_count && o.age==0);
    end_burn(&o,TRUE); assert(!light_count);
    o.otyp=CANDELABRUM_OF_INVOCATION; o.spe=7; o.age=600;
    begin_burn(&o,FALSE); assert(o.lamplit && timer_count==1 && o.timed);
    end_burn(&o,TRUE); assert(!light_count && !timer_count);
    puts("PASS: production begin/end burn, permanent radius-3 light, extinguish/relight, timer cleanup, finite Candelabrum and unchanged magic lamp");
}
int main(void) {
    struct monst shopkeeper = {0};
    struct obj candle = {0};
    monst_globals_init(); objects_globals_init();
    topology(); database(); lights();
    candle.otyp=MAGIC_CANDLE;
    assert(cost_per_charge(&shopkeeper,&candle,FALSE)==20);
    assert(cost_per_charge(NULL,&candle,FALSE)==0);
    candle.otyp=MAGIC_LAMP;
    assert(cost_per_charge(&shopkeeper,&candle,FALSE)==10);
    assert(cost_per_charge(&shopkeeper,&candle,TRUE)==66);
    puts("PASS: production Magic Candle usage charge; unchanged magic lamp light/djinni charges");
    return 0;
}
