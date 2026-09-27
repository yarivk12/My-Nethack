/* Native Step 16A contracts; expectations are independent of the catalogue. */
extern int step13_hitmu(struct monst *, struct obj *);
extern int step13_mdamagem(struct monst *, struct monst *, struct obj *);
extern boolean step13_ohitmon(struct monst *, struct obj *, struct obj *, enum enhance_use);
extern int step16a_thitu(struct monst *, struct obj **, struct obj *, enum enhance_use);
static const uint64 offensive_bits[] = {
    OEP_VAMPIRIC_I,OEP_VAMPIRIC_II,OEP_VAMPIRIC_III,OEP_VAMPIRIC_IV,
    OEP_ACID_I,OEP_ACID_II,OEP_ACID_III,
    OEP_ANARCHIC_I,OEP_ANARCHIC_II,OEP_AXIOMATIC_I,OEP_AXIOMATIC_II,
    OEP_STONING_I,OEP_STONING_II,OEP_STONING_III,OEP_STONING_IV
};
static void step16a_catalog(void)
{
    const char *prefixes[]={"Bloodkissed","Leeching","Crimson","Dread-Vampire's",
        "Corrosive","Caustic","Flesh-Eating","Chaotic","Brutal","Righteous",
        "Immutable","Calcifying","Petrifying","Gorgon's","Medusa's"};
    const char *suffixes[]={"of the Bloodkiss","of the Leech","of the Rose",
        "of Exsanguination","of Corrosion","of Dissolution","of the Crucible",
        "of Discord","of Anarchy","of Order","of the Arbiter",
        "of Calcification","of Petrification","of the Gorgon","of Medusa"};
    const int tiers[]={1,2,3,4,1,2,3,1,2,1,2,1,2,3,4};
    struct obj *o=item(DAGGER), *arrow=item(ARROW), *tool=item(PICK_AXE);
    int i,j,n,pool[EP_COUNT],seen;
    for(i=0;i<SIZE(offensive_bits);++i) {
        const struct enhancement_entry *e=equipment_property(EP_VAMPIRIC_I+i);
        assert(e && e->bit==offensive_bits[i] && e->tier==tiers[i]);
        assert(!strcmp(e->prefix,prefixes[i])&&!strcmp(e->suffix,suffixes[i]));
        assert(enhancement_set(o,e->bit,OQ_STANDARD,FALSE));
        assert(enhancement_price(o,100)==100+(50L<<(tiers[i]-1)));
        enhancement_observe_hit(o,NULL,&gy.youmonst,ENHANCE_MELEE);
        assert(!enhancement_visible_word0(o,FALSE));
        enhancement_identify(o);assert(enhancement_visible_word0(o,FALSE)==e->bit);
        assert(!enhancement_set(tool,e->bit,OQ_STANDARD,FALSE));
        assert(!enhancement_property_allowed(arrow,e->bit));
        enhancement_clear(o);
        n=socket_candidates(o,tiers[i],-1,pool);seen=0;
        for(j=0;j<n;++j)seen+=pool[j]==EP_VAMPIRIC_I+i;
        assert(seen==(i>=4&&i<11));
    }
    for(i=0;i<4;++i)for(j=0;j<4;++j)if(i!=j) {
        assert(!enhancement_set(o,(OEP_VAMPIRIC_I<<i)|(OEP_VAMPIRIC_I<<j),0,FALSE));
        assert(!enhancement_set(o,(OEP_STONING_I<<i)|(OEP_STONING_I<<j),0,FALSE));
        assert(!enhancement_set(o,offensive_bits[7+i]|offensive_bits[7+j],0,FALSE));
    }
    for(i=0;i<4;++i)for(j=0;j<4;++j) {
        o->o_sockets[0].property=EP_ANARCHIC_I+i;
        assert(!enhancement_set(o,offensive_bits[7+j],0,FALSE));
        n=socket_candidates(o,j%2+1,-1,pool);
        for(seen=0;seen<n;++seen) assert(!(equipment_property(pool[seen])->bit&OEP_ALIGNMENT));
    }
    socket_init(o);
    o->quan=2;assert(!enhancement_set(o,OEP_STONING_I,0,FALSE));
    o->quan=1;assert(enhancement_set(o,OEP_STONING_I,0,FALSE));
    { struct obj *other=item(DAGGER);
      assert(!mergable(o,other));assert(enhancement_set(other,OEP_STONING_I,0,FALSE));
      assert(!mergable(o,other));obfree(other,NULL); }
    o->o_stoning_remaining=17;enhancement_change_type(o,LONG_SWORD);
    assert(!o->o_stoning_remaining && !o->o_enh_props);
    obfree(o,NULL);obfree(arrow,NULL);obfree(tool,NULL);
    puts("PASS Step 16A exact catalogue, pricing, socket pools, family conflicts, exclusions and stacking");
}
static void step16a_components(void)
{
    struct obj *o=item(DAGGER),*arrow=item(ARROW),*bow=item(BOW);
    struct monst m={0},a={0};
    struct permonst target=mons[PM_HUMAN];
    const int aligns[]={A_LAWFUL,A_NEUTRAL,A_CHAOTIC,A_NONE};
    const int matrix[4][4]={{1,1,0,0},{1,1,0,0},{0,1,1,0},{0,1,1,0}};
    const int percent[]={10,20,25,30};
    int i,j,k,seed,want,next,actual,hp=u.uhp,maxhp=u.uhpmax;
    m.data=&target;m.mhp=m.mhpmax=1000;a.data=&mons[PM_HUMAN];a.mhpmax=1000;
    HBlinded=1;
    for(i=0;i<3;++i)for(j=0;j<2;++j)for(k=0;k<2;++k)for(seed=1;seed<=30;++seed) {
        enhancement_clear(o);socket_init(o);
        if(j)o->o_sockets[0].property=EP_ACID_I+i;
        else assert(enhancement_set(o,OEP_ACID_I<<i,0,FALSE));
        m.mintrinsics=k?MR_ACID:0;
        init_isaac64(seed,rn2);want=k?0:d(i==0?1:i==1?3:5,i==2?6:4);next=rn2(100000);
        init_isaac64(seed,rn2);actual=enhancement_weapon_effects(o,NULL,&m,9,ENHANCE_THROWN);
        assert(actual==want&&rn2(100000)==next);
        assert(!o->oeroded && !o->oeroded2 && !o->o_enh_known && !o->o_sockets[0].known);
    }
    m.mintrinsics=0;
    for(i=0;i<4;++i)for(j=0;j<4;++j)for(k=0;k<2;++k) {
        enhancement_clear(o);socket_init(o);target.maligntyp=aligns[j];
        if(k)o->o_sockets[0].property=EP_ANARCHIC_I+i;
        else assert(enhancement_set(o,offensive_bits[7+i],0,FALSE));
        init_isaac64(1601,rn2);want=matrix[i][j]?d(i%2?5:2,4):0;next=rn2(100000);
        init_isaac64(1601,rn2);
        assert(enhancement_alignment_damage(o,NULL,&m,ENHANCE_MELEE)==want);
        assert(rn2(100000)==next);
        { aligntyp saved_alignment=u.ualign.type;
          u.ualign.type=aligns[j];init_isaac64(1601,rn2);
          assert(enhancement_alignment_damage(o,NULL,&gy.youmonst,ENHANCE_MELEE)==want);
          u.ualign.type=saved_alignment; }
        bow->o_enh_props=offensive_bits[7+i];
        init_isaac64(1601,rn2);
        assert(enhancement_alignment_damage(arrow,bow,&m,ENHANCE_AMMO)==want);
        assert(!enhancement_alignment_damage(arrow,bow,&m,ENHANCE_THROWN));
    }
    socket_init(o);enhancement_clear(bow);
    m.data=&mons[PM_HUMAN];m.ispriest=1;newepri(&m);EPRI(&m)->shralign=A_CHAOTIC;
    assert(enhancement_set(o,OEP_ANARCHIC_I,0,FALSE));
    assert(!enhancement_alignment_damage(o,NULL,&m,ENHANCE_MELEE));
    EPRI(&m)->shralign=A_NONE;
    assert(enhancement_set(o,OEP_AXIOMATIC_II,0,FALSE));
    assert(!enhancement_alignment_damage(o,NULL,&m,ENHANCE_MELEE));
    m.ispriest=0;dealloc_mextra(&m);
    assert(enhancement_set(o,OEP_AXIOMATIC_II,0,FALSE));
    m.data=&mons[PM_SHADE];
    assert(!enhancement_alignment_damage(o,NULL,&m,ENHANCE_MELEE));
    m.data=&target;
    for(i=0;i<4;++i)for(j=0;j<=101;++j) {
        assert(enhancement_set(o,OEP_VAMPIRIC_I<<i,0,FALSE));
        a.mhp=100;u.uhp=100;u.uhpmax=1000;
        want=(i<2?j:2*j)*percent[i]/100;if(j&&want<1)want=1;
        enhancement_vampiric(o,NULL,&a,&m,j,2*j,ENHANCE_MELEE);
        enhancement_vampiric(o,NULL,&gy.youmonst,&m,j,2*j,ENHANCE_THROWN);
        assert(a.mhp==100+want&&u.uhp==100+want);
        assert(!o->o_enh_known);
    }
    a.mhp=999;enhancement_vampiric(o,NULL,&a,&m,1000,1000,ENHANCE_MELEE);assert(a.mhp==1000);
    m.data=&mons[PM_IRON_GOLEM];a.mhp=100;
    enhancement_vampiric(o,NULL,&a,&m,1000,1000,ENHANCE_MELEE);assert(a.mhp==100);
    m.data=&mons[PM_HUMAN_ZOMBIE];
    enhancement_vampiric(o,NULL,&a,&m,1000,1000,ENHANCE_MELEE);assert(a.mhp==100);
    m.data=&mons[PM_HUMAN];m.mhp=5;
    assert(!enhancement_set(arrow,OEP_VAMPIRIC_I,0,FALSE));
    assert(enhancement_set(bow,OEP_VAMPIRIC_IV,0,FALSE));
    enhancement_vampiric(arrow,bow,&a,&m,20,40,ENHANCE_AMMO);assert(a.mhp==112);
    u.uhp=hp;u.uhpmax=maxhp;HBlinded=0;
    obfree(o,NULL);obfree(arrow,NULL);obfree(bow,NULL);
    puts("PASS Step 16A acid dice/RNG/resistance parity, alignment matrix, Vampiric percentages/minimum/cap/living/overkill/strongest source");
}
static void step16a_paths(void)
{
    struct monst *a=makemon(&mons[PM_HUMAN],33,10,NO_MINVENT);
    struct monst *target=makemon(&mons[PM_HUMAN],34,10,NO_MINVENT);
    int path,tier,seed,pass,damage[2],healing,want,cases=0;
    int hp=u.uhp,maxhp=u.uhpmax,ac=u.uac;
    const int percent[]={10,20,25,30};
    assert(a&&target);a->m_lev=100;HBlinded=1;u.uconduct.weaphit=10;
    for(path=0;path<9;++path)for(tier=0;tier<4;++tier)for(seed=1;seed<=12;++seed) {
        boolean shot=path==2||path==6||path==8;
        boolean hero_target=path==3||path==5||path==6;
        for(pass=0;pass<2;++pass) {
            struct obj *o=item(shot?ARROW:DAGGER),*bow=item(BOW);
            enum enhance_use use=shot?ENHANCE_AMMO:ENHANCE_THROWN;
            uint64 props=tier>=2?OEP_ACID_III:0;
            if(pass)props|=OEP_VAMPIRIC_I<<tier;
            assert(enhancement_set(shot?bow:o,props,OQ_FINE,FALSE));
            target->mhp=target->mhpmax=10000;a->mhp=100;a->mhpmax=1000;
            u.uhp=path<3?100:10000;u.uhpmax=10000;u.uac=10;
            init_isaac64(seed,rn2);
            if(path<3) { if(shot)uwep=bow;
                (void)hmon(target,o,path==0?HMON_MELEE:HMON_THROWN,10);uwep=NULL;
            } else if(path==3)(void)step13_hitmu(a,o);
            else if(path==4)(void)step13_mdamagem(a,target,o);
            else if(path==5||path==6)(void)step16a_thitu(a,&o,bow,use);
            else {gm.marcher=a;gm.mtarget=target;gb.bhitpos.x=target->mx;gb.bhitpos.y=target->my;
                (void)step13_ohitmon(target,o,bow,use);gm.marcher=gm.mtarget=NULL;o=NULL;}
            damage[pass]=10000-(hero_target?u.uhp:target->mhp);
            healing=(path<3?u.uhp:a->mhp)-100;
            want=pass?max(1,damage[pass]*percent[tier]/100):0;
            assert(healing==want);
            if(o)obfree(o,NULL);obfree(bow,NULL);
        }
        assert(damage[0]==damage[1]);++cases;
    }
    mongone(a);mongone(target);HBlinded=0;u.uhp=hp;u.uhpmax=maxhp;u.uac=ac;
    printf("PASS Step 16A Vampiric canonical damage and source ownership: %d native melee/throw/shot path cases\n",cases);
}
static void step16a_stoning(void)
{
    struct obj *o=item(DAGGER), *bow=item(BOW), *arrow=item(ARROW), *bag=item(SACK);
    struct monst *a=makemon(&mons[PM_HUMAN],35,10,NO_MINVENT),*target;
    const int turns[]={75,50,25,10};
    int i,j;long oldmoves=svm.moves;char text[BUFSZ];
    assert(a);HBlinded=1;
    for(i=0;i<4;++i) {
        assert(enhancement_set(o,OEP_STONING_I<<i,0,FALSE));
        target=makemon(&mons[PM_HUMAN],36,10,NO_MINVENT);assert(target);
        target->mintrinsics=MR_STONE;
        assert(!enhancement_stoning(o,NULL,a,target,ENHANCE_MELEE));assert(!o->o_stoning_remaining);
        target->mintrinsics=0;
        assert(enhancement_stoning(o,NULL,a,target,ENHANCE_THROWN));
        assert(o->o_stoning_remaining==turns[i]&&!o->o_enh_known);
        add_to_container(bag,o);addinv(bag);
        enhancement_tick();assert(o->o_stoning_remaining==turns[i]);
        for(j=1;j<=turns[i];++j) {++svm.moves;enhancement_tick();assert(o->o_stoning_remaining==turns[i]-j);}
        freeinv(bag);obj_extract_self(o);
    }
    target=makemon(&mons[PM_HUMAN],36,10,NO_MINVENT);assert(target);
    { struct obj *cure=item(CORPSE);cure->corpsenm=PM_LIZARD;add_to_minv(target,cure); }
    target->mspeed=MFAST;
    assert(!enhancement_stoning(o,NULL,a,target,ENHANCE_MELEE));
    assert(!target->minvent&&target->mspeed!=MFAST&&o->o_stoning_remaining==10);
    mongone(target);
    assert(enhancement_set(bow,OEP_STONING_III,0,FALSE));
    target=makemon(&mons[PM_FLESH_GOLEM],36,10,NO_MINVENT);assert(target);
    assert(!enhancement_stoning(arrow,bow,a,target,ENHANCE_AMMO));
    assert(target->data==&mons[PM_STONE_GOLEM]&&bow->o_stoning_remaining==25);
    assert(!arrow->o_stoning_remaining);mongone(target);
    o->o_stoning_remaining=17;o->o_stoning_turn=-1;
    add_to_migration(o);for(i=0;i<80;++i){++svm.moves;enhancement_tick();}
    assert(o->o_stoning_remaining==17);obj_extract_self(o);place_object(o,36,10);
    ++svm.moves;enhancement_tick();assert(o->o_stoning_remaining==16);obj_extract_self(o);
    enhancement_identify(o);enhancement_property_impact(o,EP_STONING_IV,0,text,sizeof text);
    assert(strstr(text,"16 turns remaining"));
    assert(enhancement_set(o,OEP_STONING_I,0,FALSE));
    HStone_resistance=FROMOUTSIDE;(void)enhancement_stoning(o,NULL,a,&gy.youmonst,ENHANCE_MELEE);
    assert(!o->o_stoning_remaining);HStone_resistance=0;
    (void)enhancement_stoning(o,NULL,a,&gy.youmonst,ENHANCE_MELEE);
    assert(Stoned==5&&o->o_stoning_remaining==75);
    make_stoned(0L,NULL,0,NULL);assert(o->o_stoning_remaining==75);
    obfree(o,NULL);obfree(bow,NULL);obfree(arrow,NULL);obfree(bag,NULL);mongone(a);
    svm.moves=oldmoves;HBlinded=0;
    puts("PASS Step 16A native petrification/resistance/transformation/countdown, launcher ownership, exact cooldown, nested active location and migration freeze");
}
static void step16a_flight(void)
{
    struct monst *a=makemon(&mons[PM_HUMAN],31,10,NO_MINVENT);
    struct obj *bow=item(BOW),*arrows=item(ARROW);
    int x=u.ux,y=u.uy,hp=u.uhp,maxhp=u.uhpmax,ac=u.uac;
    assert(a);u.ux=32;u.uy=10;u.uhp=u.uhpmax=2000;u.uac=10;
    assert(enhancement_set(bow,OEP_STONING_III|OEP_VAMPIRIC_IV,0,FALSE));
    bow->o_sockets[0].property=EP_ACID_III;
    arrows->quan=2;arrows->spe=50;
    add_to_minv(a,bow);add_to_minv(a,arrows);MON_WEP(a)=bow;bow->owornmask=W_WEP;
    a->misc_worn_check=W_WEP;a->mhp=100;a->mhpmax=1000;
    HBlinded=1;svc.context.mon_moving=TRUE;gm.marcher=a;
    init_isaac64(150013UL,rn2);m_throw(a,31,10,1,0,1,arrows);
    gm.marcher=NULL;svc.context.mon_moving=FALSE;
    assert(Stoned==5&&bow->o_stoning_remaining==25&&arrows->quan==1);
    assert(a->mhp==100+max(1,(2000-u.uhp)*30/100));
    make_stoned(0,NULL,0,NULL);
    assert(!bow->o_enh_known&&!bow->o_sockets[0].known);
    mongone(a);HBlinded=0;u.ux=x;u.uy=y;u.uhp=hp;u.uhpmax=maxhp;u.uac=ac;
    puts("PASS Step 16A real monster flight mutates launcher cooldown and heals once from ammunition plus socket damage");
}
static void step16a_pudding(void)
{
    struct monst *a=makemon(&mons[PM_HUMAN],37,10,NO_MINVENT),*clone,*next;
    struct obj *o=item(DAGGER);
    struct permonst *olddata=gy.youmonst.data;
    int oldnum=u.umonnum,oldmh=u.mh,oldmax=u.mhmax,ac=u.uac,tier,damage;
    const int percent[]={10,20,25,30};
    assert(a);u.umonnum=PM_BROWN_PUDDING;gy.youmonst.data=&mons[PM_BROWN_PUDDING];
    u.uac=10;HBlinded=1;
    for(tier=0;tier<4;++tier) {
        assert(enhancement_set(o,OEP_VAMPIRIC_I<<tier,0,FALSE));
        a->mhp=100;a->mhpmax=1000;u.mh=u.mhmax=200;
        init_isaac64(1616,rn2);(void)step13_hitmu(a,o);
        damage=200-u.mh;
        for(clone=fmon;clone;clone=next) {
            next=clone->nmon;
            if(clone->mcloned&&clone->data==&mons[PM_BROWN_PUDDING]) {
                damage-=clone->mhp;mongone(clone);
            }
        }
        assert(damage>0&&a->mhp==100+max(1,damage*percent[tier]/100));
    }
    gy.youmonst.data=olddata;u.umonnum=oldnum;u.mh=oldmh;u.mhmax=oldmax;u.uac=ac;
    HBlinded=0;obfree(o,NULL);mongone(a);
    puts("PASS Step 16A native pudding split reports direct physical damage, excludes HP redistribution, heals once at every tier");
}
static void step16a_mitigation(void)
{
    struct monst *a=makemon(&mons[PM_HUMAN],38,10,NO_MINVENT);
    struct obj *o=item(DAGGER),*bow=item(BOW),*arrow=item(ARROW);
    int path,tier,seed,half,loss[2],hp=u.uhp,maxhp=u.uhpmax,ac=u.uac;
    aligntyp alignment=u.ualign.type;
    long oldhalf=HHalf_physical_damage;
    assert(a);HBlinded=1;u.uac=10;u.ualign.type=A_NEUTRAL;u.uhpmax=2000;
    for(path=0;path<3;++path)for(tier=0;tier<4;++tier)for(seed=1;seed<=16;++seed) {
        assert(enhancement_set(path==2?bow:o,offensive_bits[7+tier],0,FALSE));
        for(half=0;half<2;++half) {
            HHalf_physical_damage=half?FROMOUTSIDE:0;u.uhp=2000;
            init_isaac64(seed,rn2);
            if(path==0)(void)step13_hitmu(a,o);
            else {struct obj *projectile=path==2?arrow:o;
                (void)step16a_thitu(a,&projectile,bow,path==2?ENHANCE_AMMO:ENHANCE_THROWN);}
            loss[half]=2000-u.uhp;
        }
        assert(loss[1]==(loss[0]+1)/2);
    }
    u.uhp=hp;u.uhpmax=maxhp;u.uac=ac;u.ualign.type=alignment;HHalf_physical_damage=oldhalf;
    HBlinded=0;obfree(o,NULL);obfree(bow,NULL);obfree(arrow,NULL);mongone(a);
    puts("PASS Step 16A alignment dice use combined native half-physical mitigation in melee, throw and launcher attacks");
}
static void step16a_engulfer(void)
{
    struct monst *target=makemon(&mons[PM_PURPLE_WORM],39,10,NO_MINVENT);
    struct obj *bow=item(BOW),*arrow=item(ARROW),*stack=item(ARROW);
    struct obj *amulet=item(AMULET_OF_LIFE_SAVING);
    assert(target);HBlinded=1;
    assert(enhancement_set(bow,OEP_STONING_III,0,FALSE));
    assert(mergable(arrow,stack));
    add_to_minv(target,stack);add_to_minv(target,amulet);
    amulet->owornmask=W_AMUL;target->misc_worn_check|=W_AMUL;
    target->mhp=target->mhpmax=1000;
    uwep=bow;u.uswallow=1;set_ustuck(target);gt.thrownobj=arrow;
    init_isaac64(1616,rn2);
    assert(hmon(target,arrow,HMON_THROWN,10));
    assert(!gt.thrownobj&&stack->quan==2);
    assert(!which_armor(target,W_AMUL)&&bow->o_stoning_remaining==25);
    assert(target->mhp>0&&target->mhp<target->mhpmax);
    u.uswallow=0;set_ustuck(NULL);uwep=NULL;HBlinded=0;
    obfree(bow,NULL);mongone(target);
    puts("PASS Step 16A life-saved engulfer merges projectile safely and retains pending hit damage");
}
static void step16a_tests(void)
{
    step16a_catalog();step16a_components();step16a_paths();step16a_stoning();step16a_flight();step16a_pudding();step16a_mitigation();
    step16a_engulfer();
}
