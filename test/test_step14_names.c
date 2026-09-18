static void
step14_names_prices_tests(void)
{
    const struct {uint32 bits;const char *name;} cases[]={
        {OEP_FIRE_II,"blazing long sword"},
        {OEP_PRIMORDIAL|OEP_FIRE_II,"primordial long sword of the Inferno"},
        {OEP_FIRE_III|OEP_FIRE,"cataclysmic long sword of Embers"},
        {OEP_FIRE_II|OEP_COLD,"blazing long sword of Rime"},
        {OEP_FIRE_III|OEP_SHOCK_III,"cataclysmic long sword of Heaven's Wrath"}
    };
    const struct {int type;uint32 excluded;} native[]={
        {SPEED_BOOTS,OEP_SPEED},{CLOAK_OF_MAGIC_RESISTANCE,OEP_MAGIC_RES},
        {SHIELD_OF_REFLECTION,OEP_REFLECTION},{BLUE_DRAGON_SCALE_MAIL,OEP_SPEED},
        {WHITE_DRAGON_SCALES,OEP_SLOW_DIGEST},{ALCHEMY_SMOCK,OEP_POISON_RES},
        {CHROMATIC_DRAGON_SCALES,OEP_MAGIC_RES|OEP_REFLECTION|OEP_POISON_RES|OEP_FIRE_RES|OEP_COLD_RES|OEP_SHOCK_RES}
    };
    struct obj *o=item(LONG_SWORD);
    int i,j,q;
    uint32 bit;
    o->known=o->dknown=1;
    for(i=0;i<SIZE(cases);++i) {
        assert(enhancement_set(o,cases[i].bits,OQ_STANDARD,TRUE));
        assert(!strcmp(xname(o),cases[i].name));
    }
    enhancement_set(o,OEP_PRIMORDIAL|OEP_FIRE_II,OQ_EXCEPTIONAL,TRUE);
    assert(!strcmp(xname(o),"exceptional primordial long sword of the Inferno"));
    o->o_enh_known=OEP_FIRE_II;
    assert(!strcmp(xname(o),"exceptional blazing long sword"));
    o->o_enh_known=0;o->o_enh_flags=0;
    assert(!strcmp(xname(o),"long sword"));obfree(o,NULL);
    for(i=0;i<SIZE(native);++i) {
        o=item(native[i].type);
        for(bit=1;bit<=OEP_REFLECTION;bit<<=1)
            if(bit&native[i].excluded)assert(!enhancement_property_allowed(o,bit));
        obfree(o,NULL);
    }
    for(i=0;i<24;++i)for(j=i;j<24;++j)for(q=0;q<3;++q) {
        const struct enhancement_entry *a=&enhancement_catalog[i],*b=&enhancement_catalog[j];
        long percent=100+(50L<<(a->tier-1));
        if(!!a->native_property!=!!b->native_property)continue;
        if(i!=j)percent+=50L<<(b->tier-1);
        percent=percent*(100+10*q)/100;
        o=item(a->native_property?LEATHER_ARMOR:ARROW);
        assert(enhancement_set(o,a->bit|b->bit,q,FALSE));
        assert(enhancement_price(o,1000)==10*percent);
        enhancement_identify(o);assert(enhancement_price(o,1000)==10*percent);
        obfree(o,NULL);
    }
    puts("PASS canonical tier/prefix/suffix and partial naming; native secondary-property filtering; every legal pair and quality price");
}

static void
step14_observation_tests(void)
{
    struct monst *m=makemon(&mons[PM_HUMAN],31,10,NO_MINVENT);
    struct obj *o=item(DAGGER),*armor;
    assert(m);m->minvis=0;HBlinded=0;gv.viz_array[10][31]=IN_SIGHT|COULD_SEE;
    assert(canseemon(m));
    enhancement_set(o,OEP_FIRE_II|OEP_PRIMORDIAL,OQ_STANDARD,FALSE);
    m->mintrinsics=MR_FIRE|MR_COLD|MR_ELEC;
    enhancement_observe_hit(o,NULL,m,ENHANCE_MELEE);assert(!o->o_enh_known);
    m->mintrinsics=MR_FIRE;
    enhancement_observe_hit(o,NULL,m,ENHANCE_MELEE);assert(o->o_enh_known==OEP_PRIMORDIAL);
    m->mintrinsics=0;
    enhancement_observe_hit(o,NULL,m,ENHANCE_MELEE);assert(o->o_enh_known==(OEP_PRIMORDIAL|OEP_FIRE_II));
    armor=addinv(item(LOW_BOOTS));enhancement_set(armor,OEP_FIRE_RES,OQ_STANDARD,FALSE);setworn(armor,W_ARMF);
    assert(!armor->o_enh_known);monstseesu(M_SEEN_FIRE);
    assert(armor->o_enh_known==OEP_FIRE_RES);
    m_setseenres(m,M_SEEN_FIRE);setnotworn(armor);
    assert(!m_seenres(m,M_SEEN_FIRE));
    freeinv(armor);obfree(armor,NULL);obfree(o,NULL);mongone(m);
    puts("PASS visible elemental/Primordial observation, resisted components hidden, native resistance observation and removal knowledge");
}
