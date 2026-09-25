/* Included by the linked Step 13 harness; assertions use contract values. */
static void step15d_state_tests(void)
{
    struct obj *o = item(DAGGER), *copy;
    int i;
    assert(o->o_socket_capacity == 1 && socket_count(o) == 0);
    assert(socket_gem_tier(BLACK_OPAL) == 2);
    assert(socket_gem_tier(DIAMOND) == 4);
    assert(socket_gem_tier(ROCK) == 0);
    assert(enhancement_set(o, OEP_STR_III | OEP_DEX_IV, OQ_FINE, FALSE));
    assert(o->o_enh_values[2] >= 2 && o->o_enh_values[2] <= 4);
    assert(o->o_enh_values[7] >= 2 && o->o_enh_values[7] <= 6);
    o->o_sockets[0].property = EP_FIRE;
    o->o_sockets[0].known = 1;
    o->quan = 3;
    copy = splitobj(o, 1L);
    assert(copy->o_socket_capacity == 1 && socket_count(copy) == 1);
    assert(!memcmp(o->o_enh_values, copy->o_enh_values, 8));
    assert(mergable(o, copy));
    copy->o_sockets[0].known = 0;
    assert(!mergable(o, copy));
    o->nobj = copy->nobj = NULL; obfree(copy, NULL);
    enhancement_change_type(o, TWO_HANDED_SWORD);
    assert(o->o_socket_capacity == 2 && socket_count(o) == 0);
    for (i = 0; i < 8; ++i) assert(!o->o_enh_values[i]);
    obfree(o, NULL);
    o=addinv(item(HELM_OF_BRILLIANCE));
    o->o_sockets[0].property=EP_FIRE_RES;
    setworn(o,W_ARMH);assert(EFire_resistance & W_ARMH);
    o->oartifact=ART_MITRE_OF_HOLINESS;enhancement_strip_for_artifact(o);
    assert(!(EFire_resistance & W_ARMH)&&!o->o_socket_capacity);
    setnotworn(o);useupall(o);
    puts("PASS Step 15D initial state, stored values, split, merge and type change");
}

static void step15d_combat_tests(void)
{
    const int ids[]={EP_FIRE,EP_COLD,EP_SHOCK,EP_FIRE_II,EP_COLD_II,
        EP_SHOCK_II,EP_FIRE_III,EP_COLD_III,EP_SHOCK_III,EP_PRIMORDIAL};
    const int res[]={0,FIRE_RES,COLD_RES,SHOCK_RES};
    struct obj *o=item(DAGGER), *arrow=item(ARROW), *bow=item(BOW);
    struct monst mon={0};
    int i,r,seed,hero,want,next,component,prop,value,cases=0;
    mon.data=&mons[PM_HUMAN];mon.mhp=mon.mhpmax=1000;HBlinded=1;
    for(i=0;i<SIZE(ids);++i)for(r=0;r<4;++r)
        for(hero=0;hero<2;++hero)for(seed=1;seed<=16;++seed) {
            o->o_sockets[0].property=ids[i];
            mon.mintrinsics=res[r]?res_to_mr(res[r]):0;
            if(res[r])u.uprops[res[r]].intrinsic=FROMOUTSIDE;
            init_isaac64(seed,rn2);want=0;
            for(component=0;component<(i==9?3:1);++component) {
                value=i==9?component:i%3;
                prop=value==0?FIRE_RES:value==1?COLD_RES:SHOCK_RES;
                if(prop!=res[r])want+=d(i<3?1:i<6?3:5,i<6?4:6);
            }
            next=rn2(100000);init_isaac64(seed,rn2);
            assert(enhancement_weapon_effects(o,NULL,hero?&gy.youmonst:&mon,
                                             10,ENHANCE_THROWN)==want);
            assert(rn2(100000)==next&&!o->o_sockets[0].known);
            if(res[r])u.uprops[res[r]].intrinsic=0;
            ++cases;
        }
    mon.mintrinsics=0;
    assert(enhancement_set(arrow,OEP_FIRE,OQ_STANDARD,FALSE));
    bow->o_sockets[0].property=EP_COLD_III;
    bow->o_sockets[1].property=EP_TRUEFLIGHT;
    init_isaac64(121,rn2);want=d(1,4)+d(5,6);next=rn2(100000);
    init_isaac64(121,rn2);
    assert(enhancement_weapon_effects(arrow,bow,&mon,10,ENHANCE_AMMO)==want);
    assert(rn2(100000)==next);
    assert(enhancement_hit_bonus(arrow,bow,&mon,ENHANCE_AMMO)==2);
    assert(enhancement_hit_bonus(arrow,bow,&mon,ENHANCE_THROWN)==0);
    obfree(o,NULL);obfree(arrow,NULL);obfree(bow,NULL);HBlinded=0;
    printf("PASS Step 15D %d socket elemental/resistance/RNG outcomes and mixed launcher/projectile layers\n",cases);
}

static void step15d_catalog_tests(void)
{
    const int gems[4][10] = {
        {JET,OPAL,CHRYSOBERYL,GARNET,AMETHYST,JASPER,FLUORITE,JADE,OBSIDIAN,AGATE},
        {BLACK_OPAL,EMERALD,TURQUOISE,CITRINE,AQUAMARINE,AMBER,TOPAZ},
        {RUBY,JACINTH,SAPPHIRE}, {DILITHIUM_CRYSTAL,DIAMOND}
    };
    const int sizes[4]={10,7,3,2};
    const int types[]={DAGGER,BOW,TWO_HANDED_SWORD,HELMET,LEATHER_ARMOR,
        SMALL_SHIELD,LEATHER_GLOVES,LOW_BOOTS,CLOAK_OF_PROTECTION,HAWAIIAN_SHIRT,
        RIN_PROTECTION,AMULET_OF_LIFE_SAVING,ARROW,DART,ROCK,PICK_AXE};
    const int caps[]={1,2,2,2,2,1,1,1,1,1,1,1,0,0,0,0};
    const int counts[3][4]={{8,8,6,3},{5,5,4,4},{7,7,7,8}};
    int i,j,k,pool[EP_COUNT],n;
    struct obj *o;
    for(i=0;i<4;++i)for(j=0;j<sizes[i];++j)
        assert(socket_gem_tier(gems[i][j])==i+1);
    for(i=LUCKSTONE;i<=ROCK;++i) assert(!socket_gem_tier(i));
    for(i=0;i<SIZE(types);++i) {
        o=item(types[i]);assert(o->o_socket_capacity==caps[i]);
        assert(!socket_count(o));obfree(o,NULL);
    }
    for(k=0;k<3;++k) {
        o=item(k==0?DAGGER:k==1?LEATHER_ARMOR:RIN_CONFLICT);
        for(i=1;i<=4;++i) assert(socket_candidates(o,i,-1,pool)==counts[k][i-1]);
        obfree(o,NULL);
    }
    o=item(BOW);assert(socket_candidates(o,1,-1,pool)==9);
    assert(enhancement_set(o,OEP_FIRE,OQ_STANDARD,FALSE));
    assert(socket_candidates(o,1,-1,pool)==8);
    o->o_sockets[0].property=EP_COLD;
    assert(socket_candidates(o,1,-1,pool)==7);
    assert(socket_candidates(o,1,0,pool)==8);
    n=socket_candidates(o,3,-1,pool);assert(n==6);
    for(i=0;i<n;++i) assert(pool[i]!=EP_TRUEFLIGHT);
    o->o_sockets[1].property=EP_COLD_III;
    socket_normalize(o);assert(socket_count(o)==2);
    o->o_sockets[1].property=EP_COLD;
    socket_normalize(o);assert(socket_count(o)==1);
    obfree(o,NULL);
    o=item(RIN_REGENERATION);assert(socket_candidates(o,3,-1,pool)==6);obfree(o,NULL);
    o=item(AMULET_OF_REFLECTION);assert(socket_candidates(o,4,-1,pool)==7);obfree(o,NULL);
    o=item(AMULET_OF_MAGICAL_BREATHING);assert(socket_candidates(o,2,-1,pool)==6);obfree(o,NULL);
    puts("PASS Step 15D 22 gem mappings, capacities, exact category/tier pools, cross-layer and native exclusions");
}

static void step15d_effect_tests(void)
{
    struct obj *weapon=item(LONG_SWORD), *armor=item(LEATHER_ARMOR), *ring=item(RIN_CONFLICT);
    int oldstr=ABASE(A_STR), olddex=ABASE(A_DEX), ac, i;
    char label[100];
    weapon=addinv(weapon);armor=addinv(armor);ring=addinv(ring);
    weapon->o_sockets[0].property=EP_STR_III;weapon->o_sockets[0].value=4;
    ABASE(A_STR)=16;setuwep(weapon);assert(ACURR(A_STR)==STR19(20));
    assert(!weapon->o_sockets[0].known);
    setuwep(NULL);assert(ACURR(A_STR)==16);
    assert(enhancement_set(weapon,OEP_DEX_IV,OQ_STANDARD,FALSE));
    weapon->o_enh_values[7]=6;ABASE(A_DEX)=10;
    setuswapwep(weapon);assert(ACURR(A_DEX)==10);
    set_twoweap(TRUE);assert(ACURR(A_DEX)==16);
    set_twoweap(FALSE);assert(ACURR(A_DEX)==10);setuswapwep(NULL);
    u.uhp=u.uhpmax=50;u.uen=u.uenmax=20;
    armor->o_sockets[0].property=EP_HP_III;armor->o_sockets[0].value=20;
    armor->o_sockets[1].property=EP_PROT_III;armor->o_sockets[1].value=3;
    ring->o_sockets[0].property=EP_MANA_IV;ring->o_sockets[0].value=54;
    setworn(armor,W_ARM);assert(u.uhp==50&&u.uhpmax==70);
    setworn(ring,W_RINGL);assert(u.uen==20&&u.uenmax==74);
    for(i=0;i<10;++i)equipment_refresh();assert(u.uhpmax==70&&u.uenmax==74);
    {
        int level=u.ulevel, maxpw=u.uenmax, gain;
        u.ulevel=MAXULEV;
        for(i=1;i<100;++i) {
            u.uenmax=190;u.equipment_pw=0;init_isaac64(i,rn2);gain=newpw();
            u.uenmax=244;u.equipment_pw=54;init_isaac64(i,rn2);assert(newpw()==gain);
        }
        u.ulevel=level;u.uenmax=maxpw;
    }
    find_ac();ac=u.uac;armor->o_sockets[1].value=0;find_ac();assert(u.uac==ac+3);
    armor->o_sockets[1].value=3;
    socket_label(armor,0,label,sizeof label);assert(!strcmp(label,"unknown"));
    enhancement_identify(armor);socket_label(armor,0,label,sizeof label);
    assert(!strcmp(label,"Max HP III +20"));
    assert(strstr(doname(armor),"[2/2]"));
    assert(enhancement_price_adjustment(ring)==500);
    u.umonnum=PM_FLESH_GOLEM;gy.youmonst.data=&mons[PM_FLESH_GOLEM];
    u.mh=10;u.mhmax=40;u.equipment_mh=0;equipment_refresh();
    assert(u.mh==10&&u.mhmax==60);
    for(i=0;i<10;++i)ugolemeffects(AD_ELEC,6);
    assert(u.mhmax==60&&u.equipment_mh==20);
    u.uhp=68;u.uen=70;setnotworn(armor);setnotworn(ring);
    assert(u.mhmax==40);u.umonnum=u.umonster;gy.youmonst.data=&mons[PM_HUMAN];
    u.mh=u.mhmax=u.equipment_mh=0;
    assert(u.uhpmax==50&&u.uhp==50&&u.uenmax==20&&u.uen==20);
    ring->o_sockets[0].property=EP_TELEPATHY;ring->o_sockets[0].value=0;
    setworn(ring,W_RINGL);assert(ETelepat&&u.unblind_telepat_range>0);setnotworn(ring);
    setuwep(ring);recalc_telepat_range();assert(!ETelepat&&u.unblind_telepat_range<0);
    setuwep(NULL);
    ring->o_sockets[0].property=EP_SPEED;
    setworn(ring,W_RINGL);assert(Very_fast);setnotworn(ring);
    ABASE(A_STR)=oldstr;ABASE(A_DEX)=olddex;
    {
        struct monst mon={0};
        mon.data=&mons[PM_HUMAN];mon.mhp=mon.mhpmax=50;
        freeinv(armor);add_to_minv(&mon,armor);armor->owornmask=W_ARM;
        equipment_mon_refresh(&mon);assert(mon.mhp==50&&mon.mhpmax==70);
        equipment_mon_refresh(&mon);assert(mon.mhpmax==70);
        armor->owornmask=0;equipment_mon_refresh(&mon);assert(mon.mhpmax==50);
        obj_extract_self(armor);obfree(armor,NULL);
    }
    useupall(weapon);useupall(ring);
    {
        struct monst *mon=makemon(&mons[PM_GREMLIN],25,10,NO_MINVENT|MM_NOMSG),*clone;
        assert(mon);mon->mhp=mon->mhpmax=50;
        armor=item(HELMET);armor->o_sockets[0].property=EP_HP_III;
        armor->o_sockets[0].value=20;add_to_minv(mon,armor);armor->owornmask=W_ARMH;
        equipment_mon_refresh(mon);assert(mon->mhpmax==70);
        clone=split_mon(mon,NULL);assert(clone);
        assert(mon->mhpmax==45&&mon->equipment_hp==20);
        assert(clone->mhpmax==25&&!clone->equipment_hp&&!clone->minvent);
        mongone(clone);mongone(mon);
    }
    puts("PASS Step 15D effective STR encoding, actual dual wield, HP/Pw no healing/clamping, AC, telepathy, speed, naming and price");
}
