static void
step14_names_prices_tests(void)
{
    const struct {uint32 bits;const char *name;} cases[]={
        {OEP_FIRE_II,"Blazing long sword"},
        {OEP_PRIMORDIAL|OEP_FIRE_II,"Primordial long sword of the Inferno"},
        {OEP_FIRE_III|OEP_FIRE,"Cataclysmic long sword of Embers"},
        {OEP_FIRE_II|OEP_COLD,"Blazing long sword of Rime"},
        {OEP_FIRE_III|OEP_SHOCK_III,"Cataclysmic long sword of Heaven's Wrath"}
    };
    const struct {int type;uint32 excluded;} native[]={
        {SPEED_BOOTS,OEP_SPEED},{CLOAK_OF_MAGIC_RESISTANCE,OEP_MAGIC_RES},
        {SHIELD_OF_REFLECTION,OEP_REFLECTION},{BLUE_DRAGON_SCALE_MAIL,OEP_SPEED},
        {WHITE_DRAGON_SCALES,OEP_SLOW_DIGEST},{ALCHEMY_SMOCK,OEP_POISON_RES},
        {CHROMATIC_DRAGON_SCALES,OEP_MAGIC_RES|OEP_REFLECTION|OEP_POISON_RES|OEP_FIRE_RES|OEP_COLD_RES|OEP_SHOCK_RES}
    };
    /* Finalized specification, deliberately independent of production catalog
     * tiers, class flags, and the production percentage calculator. */
    const struct {uint32 bit;long surcharge;} prices[]={
        {OEP_FIRE,50},{OEP_COLD,50},{OEP_SHOCK,50},{OEP_TRUEFLIGHT,50},
        {OEP_FIRE_II,100},{OEP_COLD_II,100},{OEP_SHOCK_II,100},
        {OEP_FIRE_III,200},{OEP_COLD_III,200},{OEP_SHOCK_III,200},
        {OEP_PRIMORDIAL,400},
        {OEP_SEARCHING,50},{OEP_WARNING,50},{OEP_STEALTH,50},
        {OEP_FIRE_RES,100},{OEP_COLD_RES,100},{OEP_SHOCK_RES,100},
        {OEP_POISON_RES,100},{OEP_SPEED,200},{OEP_REGEN,200},
        {OEP_DISPLACED,200},{OEP_SLOW_DIGEST,200},
        {OEP_MAGIC_RES,400},{OEP_REFLECTION,400}
    };
    /* 34037 bounds any signed-short native cost plus +127 enchantment. */
    const long bases[]={0,1,99,123,1000,34037};
    struct obj *o=item(LONG_SWORD);
    int i,j,q,k,kind,count=0;
    uint32 bit;
    o->known=o->dknown=1;
    for(i=0;i<SIZE(cases);++i) {
        assert(enhancement_set(o,cases[i].bits,OQ_STANDARD,TRUE));
        assert(!strcmp(xname(o),cases[i].name));
    }
    enhancement_set(o,OEP_PRIMORDIAL|OEP_FIRE_II,OQ_EXCEPTIONAL,TRUE);
    assert(!strcmp(xname(o),"Exceptional Primordial long sword of the Inferno"));
    o->o_enh_known=OEP_FIRE_II;
    assert(!strcmp(xname(o),"Exceptional Blazing long sword"));
    o->o_enh_known=0;o->o_enh_flags=0;
    assert(!strcmp(xname(o),"long sword"));obfree(o,NULL);
    for(i=0;i<SIZE(native);++i) {
        o=item(native[i].type);
        for(bit=1;bit<=OEP_REFLECTION;bit<<=1)
            if(bit&native[i].excluded)assert(!enhancement_property_allowed(o,bit));
        obfree(o,NULL);
    }
    for(kind=0;kind<2;++kind) {
        int first=kind?11:0,last=kind?24:11;
        for(i=first-1;i<last;++i)for(j=max(i,first-1);j<last;++j) {
            uint32 bits=(i<first?0:prices[i].bit)|(j<first?0:prices[j].bit);
            long surcharge=(i<first?0:prices[i].surcharge)
                +(j<first||j==i?0:prices[j].surcharge);
            /* The i=-1 row supplies zero and every single property;
             * the other rows supply distinct pairs only. */
            if(i>=first && j==i)continue;
            for(q=0;q<3;++q) {
                o=item(kind?LEATHER_ARMOR:ARROW);
                assert(enhancement_set(o,bits,q,FALSE));
                for(k=0;k<SIZE(bases);++k) {
                    long want=max(1L,bases[k]*(100+surcharge)*(10+q)/1000);
                    assert(enhancement_price(o,bases[k])==want);++count;
                }
                enhancement_identify(o);
                for(k=0;k<SIZE(bases);++k) {
                    long want=max(1L,bases[k]*(100+surcharge)*(10+q)/1000);
                    assert(enhancement_price(o,bases[k])==want);++count;
                }
                obfree(o,NULL);
            }
        }
    }
    assert(count==5724);
    puts("PASS canonical tier/prefix/suffix and partial naming; native secondary-property filtering; 5724 independent price checks over all 159 legal masks, qualities, knowledge and base bounds");
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
    enhancement_observe_hit(o,NULL,m,ENHANCE_MELEE);assert(!o->o_enh_known);
    m->mintrinsics=0;
    enhancement_observe_hit(o,NULL,m,ENHANCE_MELEE);assert(!o->o_enh_known);
    armor=addinv(item(LOW_BOOTS));enhancement_set(armor,OEP_FIRE_RES,OQ_STANDARD,FALSE);setworn(armor,W_ARMF);
    assert(!armor->o_enh_known);monstseesu(M_SEEN_FIRE);
    assert(armor->o_enh_known==OEP_FIRE_RES);
    m_setseenres(m,M_SEEN_FIRE);setnotworn(armor);
    assert(!m_seenres(m,M_SEEN_FIRE));
    freeinv(armor);obfree(armor,NULL);obfree(o,NULL);mongone(m);
    puts("PASS visible elemental/Primordial tier ambiguity, resisted components hidden, native resistance observation and removal knowledge");
}

/* Exercise native callers, rather than only the shared observation helper. */
static void
step14_audit_lifecycle_observation_tests(void)
{
    struct obj *o, *other, saved;
    int failures = 0;
    long reflection = HReflecting;
    int vaul = u.mith_timers[MITH_VAUL];

    o = item(LONG_SWORD);
    assert(enhancement_set(o, OEP_FIRE_II | OEP_PRIMORDIAL,
                           OQ_EXCEPTIONAL, TRUE));
    o->o_enh_known = OEP_FIRE_II;
    saved = *o;
    place_object(o, 25, 10);
    o = poly_obj(o, LONG_SWORD);
    if (o->o_enh_props != saved.o_enh_props
        || o->o_enh_known != saved.o_enh_known
        || o->o_enh_quality != saved.o_enh_quality
        || o->o_enh_flags != saved.o_enh_flags) {
        fprintf(stderr, "FAIL same-type native poly_obj lost enhancement state\n");
        ++failures;
    }
    obj_extract_self(o); obfree(o, NULL);

    o = addinv(item(SMALL_SHIELD));
    assert(enhancement_set(o, OEP_REFLECTION, OQ_STANDARD, FALSE));
    setworn(o, W_ARMS);
    HReflecting = FROMOUTSIDE;
    assert(ureflects("%s reflects from your %s!", "The beam"));
    if (o->o_enh_known) {
        fprintf(stderr, "FAIL redundant intrinsic reflection identified shield\n");
        ++failures;
    }
    o->o_enh_known = 0;
    HReflecting = 0;
    other = addinv(item(AMULET_OF_REFLECTION));
    setworn(other, W_AMUL);
    assert(ureflects("%s reflects from your %s!", "The beam"));
    if (o->o_enh_known) {
        fprintf(stderr, "FAIL redundant native reflection identified shield\n");
        ++failures;
    }
    setnotworn(other); freeinv(other); obfree(other, NULL);
    o->o_enh_known = 0;
    assert(ureflects("%s reflects from your %s!", "The beam"));
    assert(o->o_enh_known == OEP_REFLECTION);
    setnotworn(o); freeinv(o); obfree(o, NULL);
    HReflecting = reflection;

    o = addinv(item(LOW_BOOTS));
    assert(enhancement_set(o, OEP_DISPLACED, OQ_STANDARD, FALSE));
    setworn(o, W_ARMF);
    u.mith_timers[MITH_VAUL] = 10;
    assert(Displaced);
    enhancement_observe_worn(DISPLACED);
    if (o->o_enh_known) {
        fprintf(stderr, "FAIL redundant Vaul displacement identified boots\n");
        ++failures;
    }
    o->o_enh_known = 0;
    u.mith_timers[MITH_VAUL] = 0;
    enhancement_observe_worn(DISPLACED);
    assert(o->o_enh_known == OEP_DISPLACED);
    setnotworn(o); freeinv(o); obfree(o, NULL);
    u.mith_timers[MITH_VAUL] = vaul;
    assert(!failures);
    puts("PASS audit same-type native polymorph preservation and reflection/displacement attribution (six cases)");
}

static void
step14_audit_polymorph_context_tests(void)
{
    struct obj *o, saved;
    int context, changed, seed, pass, next[2], count=0;
    for(context=ENH_CONTEXT_NONE;context<=ENH_CONTEXT_SHOP;++context)
        for(changed=0;changed<2;++changed)for(seed=1;seed<=16;++seed) {
            for(pass=0;pass<2;++pass) {
                enum enhancement_context scope=pass?context:ENH_CONTEXT_NONE;
                o=item(LONG_SWORD);
                assert(enhancement_set(o,OEP_PRIMORDIAL|OEP_COLD,
                                       OQ_EXCEPTIONAL,FALSE));
                o->o_enh_known=OEP_COLD;o->o_enh_flags=OEF_QUALITY_KNOWN;
                saved=*o;place_object(o,25,10);
                assert(enhancement_context_set(scope)==ENH_CONTEXT_NONE);
                init_isaac64(seed,rn2);
                o=poly_obj(o,changed?DAGGER:LONG_SWORD);
                next[pass]=rn2(1000000);
                assert(enhancement_context_set(ENH_CONTEXT_NONE)==scope);
                if(changed){ZERO(o);}else{SAME(o,&saved);}
                obj_extract_self(o);obfree(o,NULL);
            }
            assert(next[0]==next[1]);++count;
        }
    assert(count==128);
    o=addinv(item(LOW_BOOTS));
    assert(enhancement_set(o,OEP_FIRE_RES,OQ_FINE,FALSE));
    saved=*o;setworn(o,W_ARMF);
    o=poly_obj(o,LOW_BOOTS);
    SAME(o,&saved);assert(uarmf==o && (EFire_resistance&W_ARMF));
    setnotworn(o);assert(!(EFire_resistance&W_ARMF));
    freeinv(o);obfree(o,NULL);
    puts("PASS audit 128 same/different-type polymorph context/RNG replays and native worn replacement");
}

static void
step14_audit_monster_transform_tests(void)
{
    int kind,source;
    const int types[]={GLOWING_DRAGON_SCALE_MAIL,CHROMATIC_DRAGON_SCALE_MAIL};
    const int result[]={GLOWING_DRAGON_SCALES,CHROMATIC_DRAGON_SCALES};
    for(kind=0;kind<2;++kind)for(source=0;source<4;++source) {
        struct monst m={0};
        struct obj *o=item(types[kind]),*other=NULL;
        m.data=&mons[PM_HUMAN];m.mhp=m.mhpmax=100;
        m.permspeed=source==1?MFAST:source==3?MSLOW:0;
        assert(enhancement_set(o,OEP_SPEED,OQ_FINE,FALSE));
        add_to_minv(&m,o);o->owornmask=W_ARM;m.misc_worn_check=W_ARM;
        if(source==2) {
            other=item(LOW_BOOTS);
            assert(enhancement_set(other,OEP_SPEED,OQ_STANDARD,FALSE));
            add_to_minv(&m,other);other->owornmask=W_ARMF;
            m.misc_worn_check|=W_ARMF;
            update_mon_extrinsics(&m,other,TRUE,TRUE);
        }
        update_mon_extrinsics(&m,o,TRUE,TRUE);assert(m.mspeed==MFAST);
        cancel_item(o);
        assert(o->otyp==result[kind]);ZERO(o);
        if(m.mspeed!=(source==2?MFAST:m.permspeed))
            fprintf(stderr,"FAIL cancelled monster enhanced mail retained speed\n");
        assert(m.mspeed==(source==2?MFAST:m.permspeed));
        o->owornmask=0;update_mon_extrinsics(&m,o,FALSE,TRUE);
        obj_extract_self(o);obfree(o,NULL);
        if(other) {
            other->owornmask=0;update_mon_extrinsics(&m,other,FALSE,TRUE);
            assert(m.mspeed==m.permspeed);
            obj_extract_self(other);obfree(other,NULL);
        }
    }
    puts("PASS audit eight monster mail cancellations remove enhancement speed and retain redundant/native speed");
}
