/* Included by the native enhancement harness. */
static void step16b_basic(void)
{
    const int tiers[] = {3,2,4,1,2,3,1,2,1,2,3,4};
    struct obj *armor=item(PLATE_MAIL), *ring=item(RIN_ADORNMENT), *bag=item(SACK);
    int i, base=weight(armor), oldweight=objects[PLATE_MAIL].oc_weight;
    for(i=0;i<12;++i) {
        const struct enhancement_entry *e=equipment_property(EP_WARDING+i);
        assert(e && e->tier==tiers[i] && e->bit==(1ULL<<(48+i)));
        assert(enhancement_set(armor,e->bit,0,FALSE));
        assert(!enhancement_visible_word0(armor,FALSE));
    }
    assert(!enhancement_set(armor,OEP_DR_I|OEP_DR_II,0,FALSE));
    assert(enhancement_set(armor,OEP_DR_I|OEP_WARDING,0,FALSE));
    for(i=0;i<3;++i) {
        assert(enhancement_set(armor,OEP_LIGHTNESS_I<<i,0,FALSE));
        assert(weight(armor)==base*(70-i*30)/100);
        assert(armor->owt==weight(armor));
        enhancement_normalize(armor);
        assert(armor->owt==base*(70-i*30)/100);
    }
    objects[PLATE_MAIL].oc_weight=99;
    assert(!enhancement_property_allowed(armor,OEP_LIGHTNESS_I));
    objects[PLATE_MAIL].oc_weight=100;
    assert(enhancement_property_allowed(armor,OEP_LIGHTNESS_I));
    objects[PLATE_MAIL].oc_weight=oldweight;
    add_to_container(bag,armor);
    assert(weight(bag)==objects[SACK].oc_weight+weight(armor));
    assert(enhancement_set(armor,OEP_LIGHTNESS_I,0,FALSE));
    assert(bag->owt==weight(bag));
    obj_extract_self(armor);
    addinv(armor);addinv(ring);
    setworn(armor,W_ARM);setworn(ring,W_RINGL);
    assert(enhancement_set(armor,OEP_WARDING,0,FALSE));
    curse(armor);curse(ring);
    assert(!armor->cursed&&!ring->cursed&&!armor->o_enh_known);
    setworn(NULL,W_RINGL);curse(ring);assert(ring->cursed);
    setworn(ring,W_RINGL);curse(ring);assert(ring->cursed);
    assert(enhancement_set(armor,OEP_CASTING_II,0,FALSE));
    assert(armor->o_enh_known&OEP_CASTING_II);
    assert(enhancement_casting_penalty(armor,5)==2);
    assert(enhancement_casting_penalty(armor,3)==1);
    assert(enhancement_casting_penalty(armor,1)==0);
    assert(enhancement_set(armor,OEP_CASTING_IV,0,FALSE));
    assert(enhancement_casting_penalty(armor,5)==0);
    setworn(NULL,W_ARM);assert(enhancement_casting_penalty(armor,5)==5);
    assert(enhancement_set(armor,OEP_WARDING,0,FALSE));
    curse(armor);assert(armor->cursed);
    setworn(NULL,W_RINGL);freeinv(ring);freeinv(armor);
    obfree(ring,NULL);obfree(armor,NULL);obfree(bag,NULL);
    armor=item(SMALL_SHIELD);assert(armor_spell_penalty(armor));obfree(armor,NULL);
    armor=item(ROBE);assert(!armor_spell_penalty(armor));obfree(armor,NULL);
    puts("PASS Step 16B catalogue, exclusivity, intrinsic weight/cache/container, warding and casting");
}

static void step16b_damage(void)
{
    struct monst m={0};
    struct obj *a=item(PLATE_MAIL), *b=item(HELMET), *c=item(LOW_BOOTS);
    int i;
    m.data=&mons[PM_HUMAN];m.mhp=m.mhpmax=100;
    m.minvent=a;a->nobj=b;b->nobj=c;
    a->owornmask=W_ARM;b->owornmask=W_ARMH;c->owornmask=W_ARMF;
    for(i=0;i<4;++i) {
        assert(enhancement_set(a,OEP_DR_I<<i,0,FALSE));
        assert(enhancement_reduce(&m,100,100)==90-i*10);
        assert(enhancement_reduce(&m,100,0)==(i<2?100:90-i*10));
    }
    assert(enhancement_set(a,OEP_DR_II,0,FALSE));
    assert(enhancement_set(b,OEP_DR_III,0,FALSE));
    assert(enhancement_reduce(&m,100,100)==56);
    assert(enhancement_reduce(&m,100,0)==70);
    assert(enhancement_reduce(&m,5,5)==3);
    assert(enhancement_set(a,OEP_DR_IV,0,FALSE));
    assert(enhancement_set(b,OEP_DR_IV,0,FALSE));
    assert(enhancement_set(c,OEP_DR_IV,0,FALSE));
    assert(enhancement_reduce(&m,100,100)==30);
    assert(enhancement_reduce(&m,1,1)==1);
    assert(enhancement_reduce(&m,2,2)==1);
    assert(enhancement_reduce(&m,5,5)==2);
    assert(enhancement_reduce(&m,10,0)==3);
    assert(enhancement_reduce(&m,0,0)==0);
    assert(enhancement_reduce(&m,100,ENH_FATAL)==100);
    assert(enhancement_damage_component(10,20,10)==5);
    assert(enhancement_damage_component(10,20,0)==0);
    {
        struct obj *blade=item(LONG_SWORD);
        boolean fatal=FALSE;int damage=10,nonphysical=0;
        blade->oartifact=ART_VORPAL_BLADE;
        (void)artifact_hit_fatal(&gy.youmonst,&m,blade,&damage,1,&fatal,&nonphysical);
        assert(fatal && damage>m.mhp);
        assert(enhancement_mon_damage(&m,NULL,damage,ENH_FATAL)<1);
        obfree(blade,NULL);
    }
    assert(!a->o_enh_known&&!b->o_enh_known&&!c->o_enh_known);
    a->nobj=b->nobj=NULL;obfree(a,NULL);obfree(b,NULL);obfree(c,NULL);
    puts("PASS Step 16B exact multiplicative DR, physical filter, cap and single rounding");
}

static void step16b_reflection(void)
{
    struct monst *m=makemon(&mons[PM_HUMAN],u.ux+1,u.uy,NO_MINVENT);
    struct obj *a=item(PLATE_MAIL),*b=item(HELMET),*c=item(LOW_BOOTS),*ma=item(PLATE_MAIL);
    int hp=u.uhp,maxhp=u.uhpmax,oldac=u.uac, before, seed;
    long xp=u.uexp, realxp=u.urexp;
    assert(m);HBlinded=1;u.uhpmax=10000;u.uhp=10000;
    assert(enhancement_set(a,OEP_THORNS_I,0,FALSE));
    assert(enhancement_set(b,OEP_THORNS_II,0,FALSE));
    assert(enhancement_set(c,OEP_THORNS_II,0,FALSE));
    addinv(a);addinv(b);addinv(c);setworn(a,W_ARM);setworn(b,W_ARMH);setworn(c,W_ARMF);
    m->mhp=m->mhpmax=10000;
    mdamageu_damage(m,20,0);assert(u.uhp==9980&&m->mhp==9974);
    before=m->mhp;losehp(20,"unattributed magic",KILLED_BY);assert(m->mhp==before);
    mdamageu_damage(m,0,0);assert(m->mhp==before);
    remove_monster(m->mx,m->my);place_monster(m,u.ux+3,u.uy);
    mdamageu_damage(m,20,0);assert(m->mhp==before);
    remove_monster(m->mx,m->my);place_monster(m,u.ux+1,u.uy);
    setworn(NULL,W_ARMH);setworn(NULL,W_ARMF);
    assert(enhancement_set(a,OEP_THORNS_II|OEP_DR_III,0,FALSE));
    assert(enhancement_set(ma,OEP_THORNS_II|OEP_DR_II,0,FALSE));
    add_to_minv(m,ma);ma->owornmask=W_ARM;m->misc_worn_check=W_ARM;
    {
        struct obj *helmet=item(HELMET);
        assert(enhancement_set(helmet,OEP_THORNS_I,0,FALSE));
        add_to_minv(m,helmet);helmet->owornmask=W_ARMH;
        m->misc_worn_check|=W_ARMH;
    }
    before=u.uhp;m->mhp=10000;
    mdamageu_damage(m,60,0);
    assert(u.uhp==before-42&&m->mhp==9983); /* 21 physical -> 17, no recursion */
    before=u.uhp;m->mhp=10000;
    enhancement_mon_damage(m,&gy.youmonst,100,100);
    assert(m->mhp==9920&&u.uhp==before-28); /* 80 loss -> 40 -> 28 */
    /* Native projectile path, fixed-seed baseline and defensive pass. */
    for(seed=1;seed<=12;++seed) {
        struct obj *arrow=item(ARROW);int loss;
        u.uac=10;m->mhp=10000;before=u.uhp;
        init_isaac64(seed,rn2);
        (void)step16a_thitu(m,&arrow,NULL,ENHANCE_THROWN);
        loss=before-u.uhp;
        assert(10000-m->mhp==enhancement_reduce(m,loss/2,-1));
        if(arrow)obfree(arrow,NULL);
    }
    assert(!a->o_enh_known&&!ma->o_enh_known);
    /* Native player-kill credit, rather than a parallel death/XP path. */
    m->mhp=1;before=u.uhp;
    mdamageu_damage(m,10,0);
    assert(DEADMONSTER(m) && u.uexp>xp && u.uhp==before-7);
    u.uexp=xp;u.urexp=realxp;
    setworn(NULL,W_ARM);freeinv(a);freeinv(b);freeinv(c);
    obfree(a,NULL);obfree(b,NULL);obfree(c,NULL);
    if(!DEADMONSTER(m))mongone(m);u.uhp=hp;u.uhpmax=maxhp;u.uac=oldac;HBlinded=0;
    puts("PASS Step 16B adjacent magic/projectile reflection, 130 percent hero stacking, DR ordering and recursion suppression");
}

static void step14_roundtrip_batch(struct obj **);
static void step16b_persistence(void)
{
    struct obj *chain=NULL,*o;
    int i,k;
    for(i=0;i<12;++i)for(k=0;k<2;++k) {
        o=item(PLATE_MAIL);
        assert(enhancement_set(o,1ULL<<(48+i),OQ_EXCEPTIONAL,k));
        o->nobj=chain;chain=o;
    }
    step14_roundtrip_batch(&chain);
    puts("PASS Step 16B native codec, all twelve upper-bit identities and knowledge states");
}

extern int step16b_spell_success(int);
static void step16b_casting(void)
{
    struct obj *suit=item(PLATE_MAIL),*shield=item(LARGE_SHIELD);
    struct spell saved=svs.spl_book[0];
    int i,naked;
    assert(enhancement_set(suit,OEP_CASTING_IV,0,FALSE));
    assert(enhancement_set(shield,OEP_CASTING_IV,0,FALSE));
    addinv(suit);addinv(shield);
    svs.spl_book[0].sp_id=SPE_MAGIC_MISSILE;
    for(i=1;i<=7;++i) {
        svs.spl_book[0].sp_lev=i;
        naked=step16b_spell_success(0);
        setworn(suit,W_ARM);setworn(shield,W_ARMS);
        assert(step16b_spell_success(0)==naked);
        assert((suit->o_enh_known&OEP_CASTING_IV)&&(shield->o_enh_known&OEP_CASTING_IV));
        setworn(NULL,W_ARM);setworn(NULL,W_ARMS);
    }
    svs.spl_book[0]=saved;
    freeinv(suit);freeinv(shield);obfree(suit,NULL);obfree(shield,NULL);
    puts("PASS Step 16B actual spell success: independent suit/shield reductions, heavy-shield component and unrelated factors");
}
