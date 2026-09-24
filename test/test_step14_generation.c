/* Included in the established linked native diagnostic executable. */
extern void step14_fill_empty_maze(void);
static void free_level(void);
static const int step14_tables[5][9] = {
    {6,65,30,50,15,70,15,10,5}, {10,50,40,60,20,55,20,15,10},
    {16,40,40,70,30,40,25,20,15}, {22,30,40,80,40,25,30,25,20},
    {28,20,40,90,50,10,35,30,25}
};
static const uint64 step14_weapon_elements=OEP_ATTRIBUTES|OEP_FIRE|OEP_COLD|OEP_SHOCK
    |OEP_FIRE_II|OEP_COLD_II|OEP_SHOCK_II
    |OEP_FIRE_III|OEP_COLD_III|OEP_SHOCK_III|OEP_PRIMORDIAL;

static uint64
step14_reference(int band, uint64 allowed, int *quality, int *attempts,
                 int *rolled_tiers)
{
    /* Contract pools and caller-supplied eligibility, deliberately independent
     * of enhancement_catalog and all production filtering helpers. */
    const uint64 pool[4][9] = {
        {OEP_FIRE,OEP_COLD,OEP_SHOCK,OEP_TRUEFLIGHT,
         OEP_SEARCHING,OEP_WARNING,OEP_STEALTH,OEP_STR_I,OEP_DEX_I},
        {OEP_FIRE_II,OEP_COLD_II,OEP_SHOCK_II,
         OEP_FIRE_RES,OEP_COLD_RES,OEP_SHOCK_RES,OEP_POISON_RES,OEP_STR_II,OEP_DEX_II},
        {OEP_FIRE_III,OEP_COLD_III,OEP_SHOCK_III,
         OEP_SPEED,OEP_REGEN,OEP_DISPLACED,OEP_SLOW_DIGEST,OEP_STR_III,OEP_DEX_III},
        {OEP_PRIMORDIAL,OEP_MAGIC_RES,OEP_REFLECTION,OEP_STR_IV,OEP_DEX_IV,0,0,0,0}
    };
    const int *table=step14_tables[band];
    int presence, roll, n, slot, tier, i, count;
    uint64 props=0, valid[9];
    *quality=0;*attempts=0;
    if(rn2(100)>=table[0])return 0;
    do {
        ++*attempts;
        roll=rn2(100);
        *quality=roll<table[1]?0:roll<table[1]+table[2]?1:2;
        presence=rn2(100)<table[3];
    } while(!*quality&&!presence);
    n=presence?(rn2(100)<table[4]?2:1):0;
    for(slot=0;slot<n;++slot) {
        roll=rn2(100);
        for(tier=0;tier<3 && roll>=table[5+tier];++tier)roll-=table[5+tier];
        rolled_tiers[tier]++;
        for(;;--tier) {
            assert(tier>=0);count=0;
            for(i=0;i<9;++i)
                if((allowed&pool[tier][i])&&!(props&pool[tier][i]))
                    valid[count++]=pool[tier][i];
            if(count)break;
        }
        props|=valid[rn2(count)];
    }
    /* Acquisition draws happen once after both ordinary identities are chosen. */
    for (i=0;i<8;++i) if (props & (1ULL << (25+i))) {
        int acquisition_tier=i%4;
        (void)d(acquisition_tier<2?1:2, acquisition_tier%2?3:2);
    }
    return props;
}

static void
step14_generation_tests(void)
{
    const int boundaries[]={-100,1,29,30,59,60,99,100,149,150,199,200,999};
    const int bands[]={0,0,0,1,1,2,2,3,3,4,4,4,4};
    int i,b,seed,q,attempts,next,tiers[4]={0},rerolls=0;
    struct obj *o=item(LONG_SWORD);
    for(i=0;i<SIZE(boundaries);++i) {
        const struct enhancement_band *actual=enhancement_depth_band(boundaries[i]);
        const int *expect=step14_tables[bands[i]];
        assert(actual->gate==expect[0]&&actual->standard==expect[1]&&actual->fine==expect[2]);
        assert(actual->presence==expect[3]&&actual->two==expect[4]);
        for(b=0;b<4;++b)assert(actual->tier[b]==expect[5+b]);
    }
    for(b=0;b<5;++b)for(seed=1;seed<=10000;++seed) {
        uint64 props;
        init_isaac64(seed,rn2);props=step14_reference(b,step14_weapon_elements,&q,&attempts,tiers);next=rn2(1000000);
        rerolls+=attempts>1;
        enhancement_clear(o);o->spe=-3;o->cursed=1;o->oeroded=2;o->oerodeproof=1;
        init_isaac64(seed,rn2);enhancement_generate(o,b?b==1?30:b==2?60:b==3?100:150:1);
        assert(o->o_enh_quality==q&&o->o_enh_props==props&&rn2(1000000)==next);
        assert(o->spe==-3&&o->cursed&&o->oeroded==2&&o->oerodeproof);
        assert(!o->o_enh_known&&!o->o_enh_flags);
    }
    assert(rerolls>100);
    {
        /* Native and secondary powers: full T4 exhaustion on chromatic armor,
         * one-member T4 pools, and the ranged-only Trueflight pool. */
        const int types[]={ARROW,LEATHER_ARMOR,CLOAK_OF_MAGIC_RESISTANCE,
                           BLUE_DRAGON_SCALE_MAIL,WHITE_DRAGON_SCALE_MAIL,
                           CHROMATIC_DRAGON_SCALE_MAIL};
        const uint64 armor=OEP_SEARCHING|OEP_WARNING|OEP_STEALTH
            |OEP_FIRE_RES|OEP_COLD_RES|OEP_SHOCK_RES|OEP_POISON_RES
            |OEP_SPEED|OEP_REGEN|OEP_DISPLACED|OEP_SLOW_DIGEST
            |OEP_MAGIC_RES|OEP_REFLECTION;
        uint64 allowed[]={step14_weapon_elements|OEP_TRUEFLIGHT,armor,
                         armor&~OEP_MAGIC_RES,armor&~(OEP_SHOCK_RES|OEP_SPEED),
                         armor&~(OEP_COLD_RES|OEP_SLOW_DIGEST),
                         armor&~(OEP_FIRE_RES|OEP_COLD_RES|OEP_SHOCK_RES
                                 |OEP_POISON_RES|OEP_MAGIC_RES|OEP_REFLECTION)};
        int kind;
        for(kind=0;kind<SIZE(types);++kind) {
            struct obj *recipient=item(types[kind]);
            for(b=0;b<5;++b)for(seed=1;seed<=2000;++seed) {
                uint64 props;
                init_isaac64(seed,rn2);
                props=step14_reference(b,allowed[kind],&q,&attempts,tiers);
                next=rn2(1000000);
                enhancement_clear(recipient);
                init_isaac64(seed,rn2);
                enhancement_generate(recipient,b==0?1:b==1?30:b==2?60:b==3?100:150);
                assert(recipient->o_enh_quality==q&&recipient->o_enh_props==props);
                assert(rn2(1000000)==next);
            }
            obfree(recipient,NULL);
        }
    }
    {
        d_level old=u.uz;
        int start=svd.dungeons[1].depth_start;
        enum enhancement_context previous;
        struct obj copy;
        u.uz.dnum=1;u.uz.dlevel=2;svd.dungeons[1].depth_start=149;
        assert(depth(&u.uz)==150);
        for(seed=1;seed<=100;++seed) {
            enhancement_clear(o);init_isaac64(seed,rn2);enhancement_generate(o,150);copy=*o;
            enhancement_clear(o);init_isaac64(seed,rn2);
            previous=enhancement_context_set(ENH_CONTEXT_FLOOR);
            enhancement_created(o);(void)enhancement_context_set(previous);SAME(o,&copy);
        }
        svd.dungeons[1].depth_start=start;u.uz=old;
    }
    obfree(o,NULL);
    puts("PASS 110000 fixed-seed exact generation/RNG traces: independent rerolls, ranged/armor/native-secondary filtering, exhausted tiers, downward fallback, duplicates, untouched native fields");
}

static void
step14_corpus(void)
{
    const int depths[]={1,30,60,100,150};
    const long samples=200000;
    int b,i,q,has,count;
    long n,gate,quality[3],properties[3],tier[4],members[32];
    struct obj *o=item(LONG_SWORD);
    for(b=0;b<5;++b) {
        gate=0;memset(quality,0,sizeof quality);memset(properties,0,sizeof properties);
        memset(tier,0,sizeof tier);memset(members,0,sizeof members);
        init_isaac64(140000+b,rn2);
        for(n=0;n<samples;++n) {
            o->o_enh_props=o->o_enh_known=0;o->o_enh_quality=o->o_enh_flags=0;
            enhancement_generate(o,depths[b]);
            q=o->o_enh_quality;has=o->o_enh_props!=0;
            if(!q&&!has)continue;
            ++gate;++quality[q];count=0;
            for(i=0;i<32;++i)if(o->o_enh_props&enhancement_catalog[i].bit) {
                ++count;++tier[enhancement_catalog[i].tier-1];++members[i];
            }
            assert(count<=2);++properties[count];
        }
        assert(labs(gate-samples*step14_tables[b][0]/100)<1500);
        {
            /* Conditioning on acceptance removes Standard/no-property mass.
             * Integer ratios avoid floating point and do not consult engine
             * tables or generated outcomes to construct the expectations. */
            const int *t=step14_tables[b];
            long accepted=10000L-t[1]*(100-t[3]);
            long expected_quality[3]={t[1]*t[3],t[2]*100L,
                                      (100-t[1]-t[2])*100L};
            long expected_properties[3]={(100-t[1])*(100-t[3]),
                                         t[3]*(100-t[4]),t[3]*t[4]};
            for(i=0;i<3;++i) {
                assert(labs(quality[i]-gate*expected_quality[i]/accepted)<1200);
                assert(labs(properties[i]-gate*expected_properties[i]/accepted)<1200);
            }
        }
        printf("CORPUS|depth=%d|samples=%ld|gate=%ld|quality=%ld,%ld,%ld|properties=%ld,%ld,%ld|selected_tiers=%ld,%ld,%ld,%ld\n",
            depths[b],samples,gate,quality[0],quality[1],quality[2],properties[0],properties[1],properties[2],tier[0],tier[1],tier[2],tier[3]);
        for(i=0;i<11;++i)if(i!=3)assert(members[i]>100);
    }
    obfree(o,NULL);
    puts("PASS million-object fixed-seed five-band natural-generation corpus");
}

static int
step14_enhanced_chain(struct obj *o)
{
    int count=0;
    for(;o;o=o->nobj) {
        count+=(o->o_enh_quality||o->o_enh_props);
        count+=step14_enhanced_chain(o->cobj);
    }
    return count;
}

static void
step14_creation_tests(void)
{
    int i,seen=0,boxes=0,monsters=0;
    d_level saved=u.uz;
    boolean saved_mklev=gi.in_mklev;
    u.uz.dnum=medusa_level.dnum;u.uz.dlevel=1;
    init_isaac64(141414,rn2);
    gi.in_mklev=TRUE;
    {
        int prior,seed,next;
        enum enhancement_context entry=enhancement_context_set(ENH_CONTEXT_NONE);
        struct obj *probe=item(LONG_SWORD);
        for(seed=1;seed<=100;++seed) {
            init_isaac64(seed,rn2);next=rn2(1000000);
            init_isaac64(seed,rn2);
            enhancement_created(probe);ZERO(probe);
            (void)enhancement_eligible(probe);
            (void)enhancement_property_allowed(probe,OEP_FIRE|OEP_PRIMORDIAL);
            (void)enhancement_native_property(probe,FIRE_RES);
            assert(rn2(1000000)==next);
        }
        obfree(probe,NULL);
        for(prior=ENH_CONTEXT_NONE;prior<=ENH_CONTEXT_SHOP;++prior) {
            struct obj *o;
            struct monst *m;
            (void)enhancement_context_set((enum enhancement_context)prior);
            o=enhancement_mkobj(WEAPON_CLASS,FALSE);obfree(o,NULL);
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            o=enhancement_mkobj_at(ARMOR_CLASS,30,10,FALSE);
            obj_extract_self(o);obfree(o,NULL);
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            o=enhancement_mksobj_at(CHEST,30,10,TRUE,FALSE);
            obj_extract_self(o);obfree(o,NULL);
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            m=makemon(&mons[PM_SOLDIER],31,10,MM_NOGRP|MM_NOCOUNTBIRTH);
            assert(m&&!step14_enhanced_chain(m->minvent));
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            /* Failure before inventory scope must also preserve the caller. */
            assert(!enhancement_makemon(&mons[PM_SOLDIER],31,10,
                                        MM_NOGRP|MM_NOCOUNTBIRTH));
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            mongone(m);
            m=enhancement_makemon(&mons[PM_SOLDIER],31,10,
                                 MM_NOGRP|MM_NOCOUNTBIRTH);
            assert(m);
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            mongone(m);
            m=enhancement_makemon(&mons[PM_SOLDIER],31,10,
                                 MM_NOGRP|MM_NOCOUNTBIRTH|NO_MINVENT);
            assert(m&&!m->minvent);
            assert(enhancement_context_set((enum enhancement_context)prior)==prior);
            mongone(m);
        }
        (void)enhancement_context_set(entry);
        puts("PASS 100 NONE/filter RNG tails and 28 nested context restoration cases (four prior contexts, failure and NO_MINVENT)");
    }
    {
        int seed,natural_swords=0;
        enum enhancement_context entry=enhancement_context_set(ENH_CONTEXT_SHOP);
        for(seed=1;seed<=100;++seed) {
            struct monst *m;
            init_isaac64(seed,rn2);
            m=makemon(&mons[PM_CROESUS],31,10,MM_NOGRP|MM_NOCOUNTBIRTH);
            assert(m&&!step14_enhanced_chain(m->minvent));
            assert(enhancement_context_set(ENH_CONTEXT_SHOP)==ENH_CONTEXT_SHOP);
            mongone(m);
        }
        (void)enhancement_context_set(ENH_CONTEXT_NONE);
        for(seed=1;seed<=100;++seed) {
            struct monst *m;
            struct obj *o,*first;
            init_isaac64(seed,rn2);
            m=enhancement_makemon(&mons[PM_CROESUS],31,10,MM_NOGRP|MM_NOCOUNTBIRTH);
            assert(m&&m->minvent);first=m->minvent;
            for(o=m->minvent;o;o=o->nobj)if(o->o_id<first->o_id)first=o;
            /* The extra mitem sword precedes m_initweap/m_initinv. */
            assert(first->otyp==TWO_HANDED_SWORD);
            natural_swords+=(first->o_enh_props||first->o_enh_quality);
            assert(enhancement_context_set(ENH_CONTEXT_NONE)==ENH_CONTEXT_NONE);
            mongone(m);
        }
        assert(natural_swords>0);
        (void)enhancement_context_set(entry);
        puts("PASS 200 Croesus extra-equipment provenance cases: summoned in nested scope stays plain, natural bonus sword opts in");
    }
    init_isaac64(141414,rn2);
    for(i=0;i<1000;++i) {
        struct obj *o=mksobj(ARROW,TRUE,FALSE),*b;
        struct monst *m;
        ZERO(o);obfree(o,NULL); /* generic paths incl wishes/script/debug */
        o=mkobj(WEAPON_CLASS,FALSE);ZERO(o);obfree(o,NULL);
        o=enhancement_mkobj_at(WEAPON_CLASS,30,10,FALSE);
        seen+=(o->o_enh_quality||o->o_enh_props);obj_extract_self(o);obfree(o,NULL);
        o=enhancement_mksobj_at(CHEST,30,10,TRUE,FALSE);
        /* Native boxiprobs currently has no weapons/armor. Supply chests
         * use random class selection instead, through enhancement_mkobj. */
        b=enhancement_mkobj(WEAPON_CLASS,FALSE);add_to_container(o,b);
        boxes+=step14_enhanced_chain(o->cobj);obj_extract_self(o);obfree(o,NULL);
        m=makemon(&mons[PM_SOLDIER],31,10,MM_NOGRP|MM_NOCOUNTBIRTH);
        assert(m&&!step14_enhanced_chain(m->minvent));mongone(m);
        m=enhancement_makemon(&mons[PM_SOLDIER],31,10,MM_NOGRP|MM_NOCOUNTBIRTH);
        assert(m);monsters+=step14_enhanced_chain(m->minvent);mongone(m);
        o=mksobj(ARROW,TRUE,FALSE);ZERO(o);obfree(o,NULL); /* restored scope */
        /* Splitting preserves the result. Native next_ident may draw RNG;
         * that draw is unrelated to enhancement acquisition. */
        o=enhancement_mksobj_at(ARROW,30,10,TRUE,FALSE);o->quan=20;
        { struct obj snapshot=*o;
          b=splitobj(o,5);SAME(o,b);SAME(o,&snapshot);
        }
        obj_extract_self(b);obfree(b,NULL);obj_extract_self(o);obfree(o,NULL);
    }
    assert(seen>10&&boxes>0&&monsters>10);
    gi.in_mklev=FALSE;
    for(i=0;i<100;++i) {
        struct monst *m=enhancement_makemon(&mons[PM_SOLDIER],31,10,MM_NOGRP|MM_NOCOUNTBIRTH|MM_NOMSG);
        assert(m);monsters+=step14_enhanced_chain(m->minvent);mongone(m);
    }
    gi.in_mklev=saved_mklev;u.uz=saved;
    printf("PASS explicit creation contexts: floor=%d container=%d monster=%d; default constructors and unflagged monsters ordinary; scope restored\n",seen,boxes,monsters);
    {
        int run,floor_enhanced=0,monster_enhanced=0;
        int oldx=gx.x_maze_max,oldy=gy.y_maze_max;
        boolean old_random=svl.level.flags.rndmongen;
        free_level();
        assert(!fobj&&!fmon);
        gi.in_mklev=TRUE;u.uz.dnum=medusa_level.dnum;u.uz.dlevel=30;
        svl.level.flags.rndmongen=TRUE;
        gx.x_maze_max=COLNO-1;gy.y_maze_max=ROWNO-1;
        init_isaac64(140319,rn2);
        for(run=0;run<128;++run) {
            struct monst *m;
            step14_fill_empty_maze();
            floor_enhanced+=step14_enhanced_chain(fobj);
            for(m=fmon;m;m=m->nmon)
                monster_enhanced+=step14_enhanced_chain(m->minvent);
            for(m=fmon;m;m=m->nmon)if(!DEADMONSTER(m))mongone(m);
            if(iflags.purge_monsters)dmonsfree();
            while(fobj) {
                struct obj *o=fobj;obj_extract_self(o);obfree(o,NULL);
            }
            while(gf.ftrap)deltrap(gf.ftrap);
        }
        assert(floor_enhanced>0&&monster_enhanced>0);
        gx.x_maze_max=oldx;gy.y_maze_max=oldy;
        svl.level.flags.rndmongen=old_random;
        gi.in_mklev=saved_mklev;u.uz=saved;
        printf("PASS 128 native special-level exterior maze fills: floor=%d monster=%d\n",
               floor_enhanced,monster_enhanced);
    }
}
