/* Included in the established linked native diagnostic executable. */
static const int step14_tables[5][9] = {
    {6,65,30,50,15,70,15,10,5}, {10,50,40,60,20,55,20,15,10},
    {16,40,40,70,30,40,25,20,15}, {22,30,40,80,40,25,30,25,20},
    {28,20,40,90,50,10,35,30,25}
};

static uint32
step14_reference(int band, int *quality, int *attempts, int *rolled_tiers)
{
    /* Independent reference for a long sword: Trueflight is unavailable. */
    const uint32 pool[4][3] = {
        {OEP_FIRE,OEP_COLD,OEP_SHOCK},
        {OEP_FIRE_II,OEP_COLD_II,OEP_SHOCK_II},
        {OEP_FIRE_III,OEP_COLD_III,OEP_SHOCK_III},
        {OEP_PRIMORDIAL,0,0}
    };
    const int *table=step14_tables[band];
    int presence, roll, n, slot, tier, i, count;
    uint32 props=0, valid[3];
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
            for(i=0;i<3;++i)if(pool[tier][i]&&!(props&pool[tier][i]))valid[count++]=pool[tier][i];
            if(count)break;
        }
        props|=valid[rn2(count)];
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
        uint32 props;
        init_isaac64(seed,rn2);props=step14_reference(b,&q,&attempts,tiers);next=rn2(1000000);
        rerolls+=attempts>1;
        enhancement_clear(o);o->spe=-3;o->cursed=1;o->oeroded=2;o->oerodeproof=1;
        init_isaac64(seed,rn2);enhancement_generate(o,b?b==1?30:b==2?60:b==3?100:150:1);
        assert(o->o_enh_quality==q&&o->o_enh_props==props&&rn2(1000000)==next);
        assert(o->spe==-3&&o->cursed&&o->oeroded==2&&o->oerodeproof);
        assert(!o->o_enh_known&&!o->o_enh_flags);
    }
    assert(rerolls>100);
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
    puts("PASS 50000 fixed-seed exact generation/RNG traces: independent rerolls, uniform tier pool, downward fallback, duplicates, untouched native fields");
}

static void
step14_corpus(void)
{
    const int depths[]={1,30,60,100,150};
    const long samples=200000;
    int b,i,q,has,count;
    long n,gate,quality[3],properties[3],tier[4],members[24];
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
            for(i=0;i<24;++i)if(o->o_enh_props&enhancement_catalog[i].bit) {
                ++count;++tier[enhancement_catalog[i].tier-1];++members[i];
            }
            assert(count<=2);++properties[count];
        }
        assert(labs(gate-samples*step14_tables[b][0]/100)<1500);
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
        /* A stack receives one result; splitting/movement never draws RNG. */
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
}
