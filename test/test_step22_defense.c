/* Native defensive event, timing and source invariants. */
extern int step22_test_committed_spell(struct monst *, int, int);
extern void remove_region(NhRegion *);
#define MCASTU_INIT
static const struct { int level, flags; } step22_spell_metadata[] = {
#include "mcastu.h"
};
#undef MCASTU_INIT
#include "test_step22_defense_edges.c"
static void
step22_defense_tests(void)
{
    struct obj *a, *b, *item;
    struct monst *enemy, *m, *next;
    struct ex_sources snapshot;
    struct ex_melee_context event;
    struct you saved_hero = u;
    int seed, expected, energy, next_draw, actual, p, count, hp, hpmax;
    int oldwizard = wizard, oldac = u.uac, before, counters = 0;
    int oldenergy = u.uen, oldenergymax = u.uenmax;
    int oldepisode = u.ex_teleport_disarmed;
    boolean oldstatus = iflags.status_updates;
    long now = svm.moves, saves = u.umortality, oldlevel = u.ulevel;
    unsigned id;
    struct prop props[LAST_PROP+1];
    memcpy(props,u.uprops,sizeof props);
    memset(u.uprops,0,sizeof props);
    inspect_window_setup();
    windowprocs.win_status_init=step21_status_init;
    windowprocs.win_status_enablefield=step21_status_field;
    windowprocs.win_status_update=step21_status_update;
    windowprocs.win_mark_synch=step21_no_animation_delay;
    windowprocs.win_wait_synch=step21_no_animation_delay;
    windowprocs.win_curs=step21_cursor;
    windowprocs.win_clear_nhwindow=step21_clear;
    windowprocs.win_print_glyph=step21_glyph;
    iflags.status_updates=FALSE;
    if (!gb.blinit) status_initialize(FALSE);
    a = inspect_item(DAGGER); a->oartifact = ART_STING;
    b = addinv(mksobj(BROADSWORD,FALSE,FALSE));
    b->known = b->dknown = TRUE; b->oartifact = ART_ORCRIST;
    ex_test_grant(a,EX_ABSORB,EX_DIRECT,EX_HERO,0);
    ex_test_grant(b,EX_ABSORB,EX_WIELDED,EX_HERO,0);
    setuwep(b);
    enemy = makemon(&mons[PM_GOBLIN],u.ux+1,u.uy,MM_ADJACENTOK);
    assert(enemy); enemy->mpeaceful = 0; enemy->mtame = 0;
    enemy->msleeping = 0; enemy->mux = u.ux; enemy->muy = u.uy;
    u.uenmax = 10000;
    for (p = 0; p < 3; ++p) for (seed = 1; seed <= 300; ++seed) {
        init_isaac64(220000UL+seed,rn2);
        expected = rn2(100)<20;
        if (!expected) expected = rn2(100)<20;
        energy = expected ? (p == 0 ? d(6,6) : max(1,p == 1 ? 0 : 12)*rnd(6)) : 0;
        next_draw = rn2(1000000);
        init_isaac64(220000UL+seed,rn2); u.uen = 0;
        actual = ex_absorb(enemy,p != 0,p == 2 ? 12 : 0);
        assert(actual == expected && u.uen == energy && rn2(1000000) == next_draw);
    }
    enemy->mpeaceful = 1; u.uen = 0;
    assert(!ex_absorb(enemy,1,12) && u.uen == 0); enemy->mpeaceful = 0;
    /* A successful committed cast cancellation must prevent all native
     * status, inventory, hazard and caster-side components. */
    for(seed=1;;++seed){init_isaac64(223000UL+seed,rn2);if(rn2(100)<20)break;}
    {
        int j, spells = 0;
        for(j=0;j<SIZE(step22_spell_metadata);++j){
            int hp_before=u.uhp, conf_before=HConfusion;
            int regions_before=svn.n_regions;
            if (step22_spell_metadata[j].flags & MCF_INDIRECT) continue;
            enemy->mcan=0;u.uen=0;
            init_isaac64(223000UL+seed,rn2);
            assert(step22_test_committed_spell(enemy,j,50));
            assert(u.uhp==hp_before && HConfusion==conf_before && u.uen>0
                   && !enemy->mcan && svn.n_regions==regions_before);
            ++spells;
        }
        assert(spells > 0);
        printf("PASS Step22 D4 all %d directed native named spells cancel at shared commitment before HP/status/inventory/cloud/caster effects\n",spells);
        u.uen=0;enemy->mhp=1;enemy->mhpmax=20;
        assert(!step22_test_committed_spell(enemy,MCAST_CURE_SELF,50));
        assert(enemy->mhp>1 && u.uen==0);
    }
    {
        struct attack coldspell = {AT_MAGC, AD_COLD, 0, 6};
        long oldcold = HCold_resistance;
        int oldml = enemy->m_lev, misses = 0, successes = 0, mode;
        HCold_resistance = INTRINSIC; enemy->m_lev = 50;
        for (mode=0;mode<2;++mode) for(seed=1;seed<=300;++seed) {
            int fumbled, hp_before=u.uhp;
            init_isaac64(227000UL+seed,rn2);
            fumbled=rn2(500)<20;
            expected=0;
            if (!fumbled) {
                expected=rn2(100)<20;
                if (!expected) expected=rn2(100)<20;
            }
            energy=expected?d(6,6):0;
            enemy->seen_resistance=0;enemy->mspec_used=0;enemy->mcan=0;u.uen=0;
            init_isaac64(227000UL+seed,rn2);
            actual=castmu(enemy,&coldspell,TRUE,mode==0);
            assert(u.uen==energy && u.uhp==hp_before);
            if (expected) {assert(actual==M_ATTK_HIT);++successes;}
            if (fumbled) {assert(actual==M_ATTK_MISS);++misses;}
        }
        assert(successes>100 && misses>5);
        HCold_resistance=oldcold;enemy->m_lev=oldml;enemy->seen_resistance=0;
        puts("PASS Step22 D4 600 actual unlisted cold spell casts, committed displaced misses, precommit fumbles, independent source RNG and exact six-die restoration (227001..227300)");
    }
    {
        NhRegion *cloud;
        int n, absorbed=0, regions_before;
        for(n=1;n<=100;++n) {
            regions_before=svn.n_regions;u.uen=0;
            ex_area_spell_begin(enemy,12);
            init_isaac64(228000UL+n,rn2);
            cloud=create_gas_cloud(u.ux,u.uy,1,1);
            ex_area_spell_end();
            if (!cloud) {
                assert(u.uen>=12 && u.uen<=72 && svn.n_regions==regions_before);
                ++absorbed;
            } else {
                assert(u.uen==0 && svn.n_regions==regions_before+1);
                remove_region(cloud);
            }
        }
        assert(absorbed>20 && absorbed<60);
        u.uen=0;ex_area_spell_begin(enemy,12);
        cloud=create_gas_cloud(u.ux+3,u.uy,1,1);
        ex_area_spell_end();assert(cloud && u.uen==0);remove_region(cloud);
        cloud=create_gas_cloud(u.ux,u.uy,1,1);
        assert(cloud && u.uen==0);remove_region(cloud);
        puts("PASS Step22 D4 native initial cloud footprint interception, whole-region cancellation and off-hero/existing-hazard exclusions (228001..228100)");
    }
    ex_snapshot(&snapshot,&gy.youmonst);
    assert(snapshot.count == 2 && snapshot.ids[0] < snapshot.ids[1]);
    setuwep(NULL);
    assert(!ex_source(&snapshot,1,EX_ABSORB,&gy.youmonst));
    ex_test_clear(a); ex_test_clear(b);
    puts("PASS Step22 D4 900 seeded independent two-source listed-zero/listed12/unlisted6d6 energy; caster-only exclusions and source removal");

    /* Each miss can produce one genuine native counterattack with the
     * configured granting weapon, without granting the enemy another turn. */
    setuwep(a); ex_test_grant(a,EX_COUNTER,EX_WIELDED,EX_HERO,0);
    hp=u.uhp; hpmax=u.uhpmax; u.uhp=u.uhpmax=10000; u.uac=-100;
    enemy->mhp=enemy->mhpmax=100000;
    for (seed=1;seed<=150;++seed) {
        before=enemy->mhp; enemy->mspec_used=0;
        init_isaac64(221000UL+seed,rn2);
        (void)mattacku(enemy);
        if (enemy->mhp<before) ++counters;
        assert(u.umortality == saves && !ex_secondary());
    }
    assert(counters>5);
    ex_test_clear(a); setuwep(NULL); u.uac=oldac;
    puts("PASS Step22 D1 native hostile adjacent miss attempts generate independent native weapon counterattacks without recursion");
    /* Native weapon-tools are legal wielded counterattack weapons too. */
    item = addinv(mksobj(PICK_AXE,FALSE,FALSE));
    item->oartifact = ART_STING;
    setuwep(item);
    ex_test_grant(item,EX_COUNTER,EX_WIELDED,EX_HERO,0);
    counters=0;u.uac=-100;
    for(seed=1;seed<=150;++seed) {
        before=enemy->mhp;enemy->mspec_used=0;
        init_isaac64(224000UL+seed,rn2);
        (void)mattacku(enemy);
        if(enemy->mhp<before)++counters;
        assert(u.umortality==saves && !ex_secondary());
    }
    assert(counters>5);
    ex_test_clear(item);setuwep(NULL);freeinv(item);obfree(item,NULL);
    u.uac=oldac;
    puts("PASS Step22 D1 native wielded weapon-tool counterattacks without recursion");

    /* Native NO_MINVENT guardians retain their exact deadline after source
     * removal; expiry drops acquired loot and never awards hero XP. */
    ex_test_grant(a,EX_GUARDIANS,EX_DIRECT,EX_HERO,0);
    u.ulevel=10;u.uhpmax=100;u.uhp=26;
    ex_melee_begin(&event,enemy);u.uhp=25;ex_melee_end(&event);
    assert(a->exotic.guardian_until == svm.moves+30);
    count=0;id=0;
    for(m=fmon;m;m=m->nmon)if(m->ex_guardian_source==a->o_id) {
        ++count;id=m->m_id;assert(m->mtame && !m->minvent && m->ex_guardian_until==svm.moves+30);
    }
    assert(count>=1&&count<=3);
    item=mksobj(DAGGER,FALSE,FALSE);
    for(m=fmon;m&&m->m_id!=id;m=m->nmon);
    assert(m);(void)mpickobj(m,item);
    ex_test_clear(a);svm.moves+=30;before=u.uexp;
    ex_world_turn();assert(u.uexp==before);
    for(m=fmon;m;m=m->nmon)assert(!m->ex_guardian_source);
    puts("PASS Step22 D3 native placement NO_MINVENT, stable group deadlines after source removal, acquired loot expiry and no expiry XP");

    /* D2 uses native completion even if the completed destination is the
     * current square. Native controlled target cancellation remains free. */
    {
    boolean oldnoteleport=svl.level.flags.noteleport;
    long oldstasis=svl.level.flags.stasis_until;
    boolean oldamulet=u.uhave.amulet;
    d_level olddungeon=u.uz;
    coordxy oldx=u.ux,oldy=u.uy;
    svl.level.flags.noteleport=FALSE;
    svl.level.flags.stasis_until=0;
    u.uhave.amulet=FALSE;
    u.uz.dnum=medusa_level.dnum;u.uz.dlevel=40;
    wizard=FALSE;HTeleport_control=INTRINSIC;
    cmdq_clear(CQ_CANNED);
    svl.level.flags.noteleport=TRUE;
    assert(!ex_reactive_teleport() && u.ux==oldx && u.uy==oldy);
    svl.level.flags.noteleport=FALSE;
    svl.level.flags.stasis_until=svm.moves+5;
    assert(!ex_reactive_teleport() && u.ux==oldx && u.uy==oldy);
    svl.level.flags.stasis_until=0;
    /* getpos accepts a canned direction as a coordinate delta. A canned
     * key is rejected before its interactive '.' handling is entered. */
    cmdq_add_dir(CQ_CANNED,0,0,0);
    assert(ex_reactive_teleport() && u.ux==oldx && u.uy==oldy);
    cmdq_add_key(CQ_CANNED,'\033');
    assert(!ex_reactive_teleport());
    ex_test_grant(a,EX_TELEPORT,EX_DIRECT,EX_HERO,0);
    u.ex_teleport_disarmed=FALSE;u.uhp=26;
    ex_melee_begin(&event,enemy);u.uhp=25;
    cmdq_add_dir(CQ_CANNED,0,0,0);ex_melee_end(&event);
    assert(u.ex_teleport_disarmed);
    ex_test_clear(a);ex_test_grant(b,EX_TELEPORT,EX_DIRECT,EX_HERO,0);
    ex_melee_begin(&event,enemy);ex_melee_end(&event);
    assert(u.ex_teleport_disarmed);
    u.uhp=26;ex_melee_begin(&event,enemy);ex_melee_end(&event);
    assert(!u.ex_teleport_disarmed);
    cmdq_clear(CQ_CANNED);
    svl.level.flags.noteleport=oldnoteleport;
    svl.level.flags.stasis_until=oldstasis;
    u.uhave.amulet=oldamulet;u.uz=olddungeon;
    }
    ex_test_clear(b);
    wizard=oldwizard;u.ulevel=oldlevel;u.uhp=hp;u.uhpmax=hpmax;svm.moves=now;
    u.uen=oldenergy;u.uenmax=oldenergymax;
    memcpy(u.uprops,props,sizeof props);u.ex_teleport_disarmed=oldepisode;
    for(m=fmon;m;m=next){next=m->nmon;if(m==enemy||m->ex_guardian_source) mongone(m);}
    setnotworn(b);freeinv(b);obfree(b,NULL);
    setnotworn(a);freeinv(a);obfree(a,NULL);
    /* Real counterattacks also advance native weapon practice/conduct;
     * guardians update the saved registry and same-square teleports update
     * position history. Keep every hero field local to this fixture. */
    u = saved_hero;
    iflags.status_updates=oldstatus;
    step15b_window_setup();
    puts("PASS Step22 D2 native same-square completion, controlled cancellation, saved global episode surviving source swaps, and strict above25 rearm");
    step22_defense_edge_tests();
}
