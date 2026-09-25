/* Native Step 13 fixtures; linked only by STEP13_TEST builds. */
#include "hack.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
extern void add_to_billobjs(struct obj *);
extern long step13_getprice(struct obj *, boolean);
static struct obj *fixture_item(const char *);
static int check_fixture_chain(struct obj *);
extern void init_isaac64(unsigned long, int (*)(int));
extern void step13_save_chain(NHFILE *, struct obj **);
extern struct obj *step13_restore_chain(NHFILE *);
#define SAME(a,b) assert((a)->o_enh_props == (b)->o_enh_props \
    && (a)->o_enh_known == (b)->o_enh_known \
    && (a)->o_enh_quality == (b)->o_enh_quality \
    && (a)->o_enh_flags == (b)->o_enh_flags \
    && (a)->o_stoning_remaining == (b)->o_stoning_remaining \
    && (a)->o_stoning_turn == (b)->o_stoning_turn \
    && (a)->o_socket_capacity == (b)->o_socket_capacity \
    && !memcmp((a)->o_sockets,(b)->o_sockets,sizeof (a)->o_sockets) \
    && !memcmp((a)->o_enh_values,(b)->o_enh_values,sizeof (a)->o_enh_values))
#define ZERO(a) assert(!(a)->o_enh_props && !(a)->o_enh_known \
    && !(a)->o_enh_quality && !(a)->o_enh_flags)
_Static_assert(sizeof(uint32) == 4 && sizeof(uint8) == 1, "fixed width");
_Static_assert(sizeof(struct obj) == 152, "Windows x64 object size (baseline 104)");
_Static_assert(OQ_STANDARD == 0 && OQ_FINE == 1 && OQ_EXCEPTIONAL == 2, "qualities");
_Static_assert(OEP_ALL == 0xfffffffffffff7fULL && OEF_QUALITY_KNOWN == 1U, "masks");

static void
test_message(const char *s)
{
    fprintf(stderr, "%s\n", s);
}

static struct obj *
item(int type)
{
    struct obj *o = mksobj(type, FALSE, FALSE);
    o->quan = 1; o->spe = 0; o->cursed = o->blessed = 0;
    return o;
}

#include "test_step15d.c"
#include "test_step16a.c"
#include "test_step16b.c"

static void
matrix(void)
{
    int i, q;
    uint32 bit;
    for (i = 1; i < NUM_OBJECTS; ++i) {
        struct obj *o = item(i);
        boolean eligible = !o->oartifact && (o->oclass == WEAPON_CLASS
            || o->oclass == ARMOR_CLASS);
        ZERO(o);
        assert(enhancement_eligible(o) == eligible);
        for (q = 0; q <= 2; ++q) {
            assert(enhancement_set(o, 0, q, FALSE) == eligible);
            for (bit = 1; bit <= OEP_REFLECTION; bit <<= 1) {
                int ci;
                boolean allowed = FALSE;
                for (ci=0;ci<24;++ci) if(enhancement_catalog[ci].bit==bit) {
                    int prop=enhancement_catalog[ci].native_property;
                    allowed=eligible && (o->oclass==ARMOR_CLASS
                      ? prop && !enhancement_native_property(o,prop)
                      : !prop && (bit!=OEP_TRUEFLIGHT || is_launcher(o) || is_ammo(o) || is_missile(o) || is_spear(o)));
                }
                assert(enhancement_property_allowed(o, bit) == allowed);
                assert(enhancement_set(o, bit, q, FALSE) == allowed);
            }
        }
        o->o_enh_props = o->o_enh_known = ~0ULL;
        o->o_enh_quality = 255; o->o_enh_flags = 255;
        o->obranch_props = OBP_ACID; o->spe = 3;
        program_state.in_sanity_check=TRUE;
        enhancement_normalize(o);
        program_state.in_sanity_check=FALSE;
        assert(o->spe == 3 && o->obranch_props == OBP_ACID);
        if (!eligible) { ZERO(o); }
        else {
            assert(!(o->o_enh_props & ~OEP_ALL) && o->o_enh_quality == 0);
            assert(o->o_enh_known == OEP_ALL && o->o_enh_flags == OEF_QUALITY_KNOWN);
        }
        obfree(o, NULL);
    }
    puts("PASS every object-table recipient, all properties/qualities, defaults and normalization");
}

static void
combat(void)
{
    struct obj *o = item(ARROW), *bow = item(BOW), saved;
    struct monst m = {0};
    int use, q, seed, want, next, value, bit, prop;
    m.data = &mons[PM_HUMAN]; m.mhp = m.mhpmax = 1000;
    HBlinded = 1; /* effect messages aren't part of RNG fixtures */
    for (q = 0; q <= 2; ++q) {
        assert(enhancement_set(o, OEP_TRUEFLIGHT, q, FALSE));
        assert(enhancement_set(bow, OEP_TRUEFLIGHT, 2-q, FALSE));
        assert(enhancement_hit_bonus(o,bow,&m,ENHANCE_AMMO) == 4-q);
        assert(enhancement_damage_bonus(o,bow,&m,ENHANCE_AMMO) == q);
        assert(enhancement_hit_bonus(o,bow,&m,ENHANCE_THROWN) == q+2);
        assert(enhancement_hit_bonus(o,bow,&m,ENHANCE_MELEE) == q);
        assert(enhancement_damage_bonus(o,bow,&m,ENHANCE_THROWN) == q);
    }
    for (bit = OEP_FIRE, prop = FIRE_RES; bit <= OEP_SHOCK; bit <<= 1) {
        prop = bit == OEP_FIRE ? FIRE_RES : bit == OEP_COLD ? COLD_RES : SHOCK_RES;
        enhancement_set(o, bit, OQ_STANDARD, FALSE);
        enhancement_set(bow, bit, OQ_STANDARD, FALSE);
        saved = *o;
        for (use = ENHANCE_MELEE; use <= ENHANCE_AMMO; ++use)
            for (seed = 1; seed <= 20; ++seed) {
                init_isaac64(seed, rn2); want = d(1,4); if(use==ENHANCE_AMMO) want += d(1,4); next = rn2(100000);
                init_isaac64(seed, rn2);
                value = enhancement_weapon_effects(o,bow,&m,10,use);
                assert(value == want && rn2(100000) == next); SAME(o,&saved);
                init_isaac64(seed,rn2);
                assert(enhancement_weapon_effects(o,bow,&gy.youmonst,10,use)==want);
                assert(rn2(100000)==next);
                m.mintrinsics = res_to_mr(prop); u.uprops[prop].intrinsic = FROMOUTSIDE;
                init_isaac64(seed,rn2); next=rn2(100000);init_isaac64(seed,rn2);
                assert(!enhancement_weapon_effects(o,bow,&m,10,use));
                assert(!enhancement_weapon_effects(o,bow,&gy.youmonst,10,use));
                assert(rn2(100000)==next);
                m.mintrinsics=0;u.uprops[prop].intrinsic=0;
            }
    }
    enhancement_set(o,OEP_FIRE|OEP_COLD,OQ_EXCEPTIONAL,FALSE);
    saved=*o;
    for(seed=1;seed<=100;++seed) {
        int base;
        init_isaac64(seed,rn2); want=rn2(100000);init_isaac64(seed,rn2);
        (void) enhancement_hit_bonus(o,bow,&m,ENHANCE_AMMO);
        (void) enhancement_damage_bonus(o,bow,&m,ENHANCE_AMMO);
        assert(rn2(100000)==want); SAME(o,&saved);
        o->o_enh_quality=0;init_isaac64(seed,rn2);base=dmgval(o,&m);next=rn2(100000);
        o->o_enh_quality=2;init_isaac64(seed,rn2);
        assert(dmgval(o,&m)==base+2 && rn2(100000)==next);SAME(o,&saved);
    }
    /* A monster's projectile shares gt.thrownobj: must not gain knowledge. */
    gt.thrownobj=o;
    (void)enhancement_weapon_effects(o,bow,&gy.youmonst,10,ENHANCE_AMMO);
    assert(!o->o_enh_known);gt.thrownobj=NULL;
    assert(enhancement_weapon_effects(o,bow,&m,0,ENHANCE_AMMO)>0);
    bow->oartifact=ART_EXCALIBUR; /* query guard independent of normalization */
    assert(enhancement_hit_bonus(o,bow,&m,ENHANCE_AMMO)==0);
    assert(enhancement_damage_bonus(o,bow,&m,ENHANCE_AMMO)==2);
    bow->oartifact=0;
    obfree(o,NULL);obfree(bow,NULL);HBlinded=0;
    puts("PASS pure queries, native dmgval RNG, independent component dice, resistance, shot source stacking and monster knowledge");
}

extern int step13_hitmu(struct monst *, struct obj *);
extern int step13_mdamagem(struct monst *, struct monst *, struct obj *);
extern int step13_thitu(struct obj **, struct obj *, enum enhance_use);
extern boolean step13_ohitmon(struct monst *, struct obj *, struct obj *, enum enhance_use);

#include "test_step14_combat.c"
#include "test_step14_generation.c"
#include "test_step14_names.c"

static void
combat_paths(void)
{
    int path, bit, seed, pass, losses[3];
    struct monst *a=makemon(&mons[PM_HUMAN],21,10,NO_MINVENT);
    struct monst *d=makemon(&mons[PM_HUMAN],22,10,NO_MINVENT);
    assert(a && d);a->m_lev=100;
    HBlinded=1;u.uconduct.weaphit=10;
    for(path=0;path<9;++path) for(bit=OEP_FIRE;bit<=OEP_SHOCK;bit<<=1)
        for(seed=1;seed<=10;++seed) {
            for(pass=0;pass<3;++pass) {
                boolean shot=path==2||path==6||path==8;
                struct obj *o=item(shot?ARROW:DAGGER),*bow=item(BOW);
                int prop=bit==OEP_FIRE?FIRE_RES:bit==OEP_COLD?COLD_RES:SHOCK_RES;
                enum enhance_use use=shot?ENHANCE_AMMO:ENHANCE_THROWN;
                enhancement_set(o,pass?bit:0,OQ_STANDARD,FALSE);
                enhancement_set(bow,pass?bit:0,OQ_STANDARD,FALSE);
                d->mhp=d->mhpmax=a->mhp=a->mhpmax=1000;
                u.uhp=u.uhpmax=1000;u.uac=10;
                d->mintrinsics=pass==2?res_to_mr(prop):0;
                u.uprops[prop].intrinsic=pass==2?FROMOUTSIDE:0;
                init_isaac64(seed,rn2);
                if(path<3) {
                    if(shot)uwep=bow;
                    (void)hmon(d,o,path==0?HMON_MELEE:HMON_THROWN,10);
                    uwep=NULL;
                } else if(path==3) (void)step13_hitmu(a,o);
                else if(path==4) (void)step13_mdamagem(a,d,o);
                else if(path==5||path==6) (void)step13_thitu(&o,bow,use);
                else {
                    gm.marcher=a;gm.mtarget=d;gb.bhitpos.x=d->mx;gb.bhitpos.y=d->my;
                    (void)step13_ohitmon(d,o,bow,use);
                    gm.marcher=gm.mtarget=NULL;o=NULL; /* drop_throw owns it */
                }
                losses[pass]=1000-((path==3||path==5||path==6)?u.uhp:d->mhp);
                if(o)obfree(o,NULL);obfree(bow,NULL);
                d->mintrinsics=0;u.uprops[prop].intrinsic=0;
            }
            assert(losses[0]>0 && losses[2]==losses[0]);
            assert(losses[1]-losses[0]>=1 && losses[1]-losses[0]<=((path==2||path==6||path==8)?8:4));
        }
    HBlinded=0;
    puts("PASS actual hero melee/throw/shot, monster melee vs hero/monster and thrown/shot vs hero/monster: 1d4 per source, resistance and source stacking");
}

static void
armor(void)
{
    const int types[]={LEATHER_ARMOR,SMALL_SHIELD,HELMET,LEATHER_GLOVES,LOW_BOOTS,CLOAK_OF_PROTECTION,HAWAIIAN_SHIRT};
    const long slots[]={W_ARM,W_ARMS,W_ARMH,W_ARMG,W_ARMF,W_ARMC,W_ARMU};
    int i, q, ac;
    struct monst m={0};m.data=&mons[PM_HUMAN];m.mhp=100;
    for(i=0;i<7;++i) for(q=0;q<=2;++q) {
        struct obj *o=item(types[i]), snapshot;
        o=addinv(o);setworn(o,slots[i]);find_ac();ac=u.uac;
        assert(enhancement_set(o,OEP_WARNING|OEP_SEARCHING,q,FALSE));snapshot=*o;
        assert(u.uac==ac-q);
        assert((EWarning&slots[i]) && (ESearching&slots[i]));
        setnotworn(o);
        assert(!(EWarning&slots[i]) && !(ESearching&slots[i]));
        freeinv(o);add_to_minv(&m,o);o->owornmask=slots[i];m.misc_worn_check=slots[i];
        ac=find_mac(&m);o->o_enh_quality=0;assert(find_mac(&m)==ac+q);o->o_enh_quality=q;
        update_mon_extrinsics(&m,o,TRUE,TRUE);SAME(o,&snapshot);
        assert(!m.mextrinsics && !EWarning && !ESearching && !EStealth);
        o->owornmask=0;update_mon_extrinsics(&m,o,FALSE,TRUE);SAME(o,&snapshot);
        obj_extract_self(o);obfree(o,NULL);m.misc_worn_check=0;
    }
    {
        struct obj *o=addinv(item(LOW_BOOTS));
        BStealth=W_SADDLE;
        enhancement_set(o,OEP_STEALTH,OQ_STANDARD,FALSE);
        setworn(o,W_ARMF);
        assert(EStealth && !Stealth && !(o->o_enh_known&OEP_STEALTH));
        setnotworn(o);BStealth=0;
        setworn(o,W_ARMF);assert(Stealth && (o->o_enh_known&OEP_STEALTH));
        setnotworn(o);freeinv(o);obfree(o,NULL);
    }
    puts("PASS seven armor slots, quality AC, native hero extrinsics and inert monster wear/remove");
}

static void
lifecycle_names(void)
{
    struct obj *o=item(ARROW),*b,*copy,saved;
    char base[BUFSZ],name[BUFSZ],tiny[4], custom[PL_PSIZ];
    int field;
    o->quan=20;o->known=o->dknown=o->bknown=o->rknown=1;
    makeknown(o->otyp);Strcpy(base,doname(o));
    enhancement_set(o,OEP_FIRE|OEP_PRIMORDIAL,OQ_EXCEPTIONAL,FALSE);
    assert(!strcmp(base,doname(o)));
    o->o_enh_known=OEP_FIRE;
    Strcpy(name,xname(o));assert(strstr(name,"Smoldering ")&&!strstr(name,"Chilled ")&&!strstr(name,"Exceptional "));
    o->o_enh_flags=OEF_QUALITY_KNOWN;
    assert(strstr(xname(o),"Exceptional Smoldering "));
    assert(!strstr(simpleonames(o),"Smoldering ")&&!strstr(ysimple_name(o),"Exceptional "));
    fully_identify_obj(o);assert((o->o_enh_known & o->o_enh_props)==o->o_enh_props);
    assert(!not_fully_identified(o));
    enhancement_prefix(o,FALSE,tiny,sizeof tiny);assert(tiny[3]=='\0');
    o->spe=9;o->blessed=1;o->oeroded=o->oeroded2=3;
    o->obranch_props=OBP_COATINGS|OBP_ANARCHIC|OBP_CONCORDANT|OBP_DEEP;
    o->obranch_material=SILVER;o->obranch_size=MZ_HUGE+1;
    memset(custom,'x',sizeof custom-1);custom[sizeof custom-1]='\0';
    o=oname(o,custom,ONAME_NO_FLAGS);
    assert(strlen(doname(o))<BUFSZ);assert(strlen(xname(o))<BUFSZ);
    saved=*o;b=splitobj(o,7);SAME(o,b);SAME(o,&saved);
    assert(o->o_id!=b->o_id&&o->quan==13&&b->quan==7&&b->spe==9&&b->obranch_props==saved.obranch_props);
    assert(mergable(o,b));
    for(field=0;field<4;++field) {
        saved=*b;
        if(field==0)b->o_enh_props^=OEP_FIRE;
        if(field==1)b->o_enh_known^=OEP_FIRE;
        if(field==2)b->o_enh_quality=OQ_FINE;
        if(field==3)b->o_enh_flags=0;
        assert(!mergable(o,b));*b=saved;
    }
    o->nobj=b->nobj;b->nobj=NULL;obfree(b,NULL);
    saved=*o;copy=newobj();*copy=*o;copy->oextra=NULL;copy->nobj=NULL;SAME(o,copy);dealloc_obj(copy);
    enhancement_change_type(o,POT_WATER);o->oclass=POTION_CLASS;enhancement_normalize(o);ZERO(o);
    assert(o->spe==9&&o->obranch_props==saved.obranch_props);obfree(o,NULL);
    o=item(LONG_SWORD);enhancement_set(o,OEP_FIRE,OQ_FINE,TRUE);
    place_object(o,25,10);o=poly_obj(o,DAGGER);ZERO(o);
    obj_extract_self(o);obfree(o,NULL);
    puts("PASS exact split/merge/copy, partial/full ID, minimal names, maximum name bounds, transformation");
}

static void
impact_descriptions(void)
{
    struct obj *o;
    char buf[BUFSZ];

    o = item(LONG_SWORD);
    enhancement_property_impact(o, EP_FIRE, 0, buf, sizeof buf);
    assert(!strcmp(buf, "+1d4 fire damage on a confirmed hit"));
    enhancement_property_impact(o, EP_PRIMORDIAL, 0, buf, sizeof buf);
    assert(!strcmp(buf, "+5d6 fire, +5d6 cold, and +5d6 shock damage on a confirmed hit"));
    o->o_enh_values[3] = 3;
    enhancement_property_impact(o, EP_STR_IV, o->o_enh_values[3], buf, sizeof buf);
    assert(!strcmp(buf, "+3 STR while wielded"));
    enhancement_quality_impact(o, buf, sizeof buf);
    assert(!strcmp(buf, "no additional quality bonus"));
    enhancement_set(o, OEP_STR_IV, OQ_EXCEPTIONAL, TRUE);
    enhancement_quality_impact(o, buf, sizeof buf);
    assert(!strcmp(buf, "+2 to hit and +2 physical damage in melee or when directly thrown"));
    obfree(o, NULL);

    o = item(BOW);
    enhancement_set(o, 0, OQ_EXCEPTIONAL, TRUE);
    enhancement_quality_impact(o, buf, sizeof buf);
    assert(!strcmp(buf, "+2 to hit when firing ammunition"));
    obfree(o, NULL);

    o = item(ARROW);
    enhancement_set(o, 0, OQ_FINE, TRUE);
    enhancement_quality_impact(o, buf, sizeof buf);
    assert(!strcmp(buf, "+1 physical damage when fired"));
    obfree(o, NULL);

    o = item(LEATHER_ARMOR);
    enhancement_set(o, 0, OQ_FINE, TRUE);
    enhancement_quality_impact(o, buf, sizeof buf);
    assert(!strcmp(buf, "+1 AC while worn"));
    enhancement_property_impact(o, EP_WARNING, 0, buf, sizeof buf);
    assert(!strcmp(buf, "grants Warning while worn"));
    enhancement_property_impact(o, EP_FIRE_RES, 0, buf, sizeof buf);
    assert(!strcmp(buf, "grants fire resistance while worn"));
    enhancement_property_impact(o, EP_PROT_I, 3, buf, sizeof buf);
    assert(!strcmp(buf, "improves AC by 3 while worn"));
    enhancement_property_impact(o, EP_HP_I, 17, buf, sizeof buf);
    assert(!strcmp(buf, "+17 maximum HP while worn"));
    enhancement_property_impact(o, EP_MANA_I, 24, buf, sizeof buf);
    assert(!strcmp(buf, "+24 maximum Pw while worn"));
    enhancement_property_impact(o, EP_TELEPORT_CONTROL, 0, buf, sizeof buf);
    assert(!strcmp(buf, "grants Teleport Control while worn"));
    obfree(o, NULL);

    o = item(LONG_SWORD);
    enhancement_set(o, OEP_FIRE | OEP_STR_IV, OQ_EXCEPTIONAL, TRUE);
    o->o_enh_values[3] = 3;
    assert(strstr(xname(o), "Exceptional Herculean long sword of Embers"));
    assert(!strstr(xname(o), "exceptional") && !strstr(xname(o), "herculean"));
    obfree(o, NULL);
    puts("PASS canonical enhancement impacts, recipient-specific quality and Phase 1 capitalization");
}

static void
artifacts_prices(void)
{
    struct obj *o=item(LONG_SWORD),*other;
    int q,bits;
    for(q=0;q<=2;++q) for(bits=0;bits<256;++bits) {
        long pct=100; int ci;
        struct obj *a=item(ARROW);
        if(enhancement_set(a,bits,q,FALSE)) {
            for(ci=0;ci<24;++ci)if(bits&enhancement_catalog[ci].bit)pct+=50L<<(enhancement_catalog[ci].tier-1);
            pct=pct*(100+10*q)/100;
            assert(enhancement_price(a,123)==123*pct/100);
            assert(enhancement_price(a,1)>=1);
            fully_identify_obj(a);assert(enhancement_price(a,123)==123*pct/100);
        }
        obfree(a,NULL);
    }
    o->spe=3;
    {
        long base=step13_getprice(o,FALSE);
        enhancement_set(o,OEP_FIRE|OEP_PRIMORDIAL,OQ_EXCEPTIONAL,FALSE);
        assert(step13_getprice(o,FALSE)==base*660/100);
        assert(step13_getprice(o,TRUE)==base*660/100);
        enhancement_identify(o);assert(step13_getprice(o,FALSE)==base*660/100);
    }
    enhancement_set(o,OEP_FIRE,OQ_FINE,FALSE);o=oname(o,"ordinary",ONAME_NO_FLAGS);
    assert(o->o_enh_props==OEP_FIRE);
    o=oname(o,"Excalibur",ONAME_VIA_DIP);assert(o->oartifact==ART_EXCALIBUR);ZERO(o);
    assert(enhancement_price(o,123)==123);
    other=item(LONG_SWORD);enhancement_set(other,OEP_COLD,OQ_FINE,FALSE);
    other=oname(other,"Excalibur",ONAME_VIA_NAMING);assert(!other->oartifact&&other->o_enh_props==OEP_COLD);
    artifact_exists(o,"Excalibur",FALSE,0);ZERO(o);obfree(o,NULL);obfree(other,NULL);
    o=item(LONG_SWORD);enhancement_set(o,OEP_SHOCK,OQ_EXCEPTIONAL,FALSE);
    o=mk_artifact(o,A_NONE,99,FALSE);assert(o->oartifact);ZERO(o);obfree(o,NULL);
    puts("PASS exact price formula, knowledge independence, ordinary/failed naming, Excalibur dip and mk_artifact exclusion");
}

static void
billing(void)
{
    struct monst a={0},b={0},*old=fmon;
    struct obj *o=item(ARROW),*part;
    long original,enhanced;
    a.data=b.data=&mons[PM_SHOPKEEPER];a.mhp=b.mhp=100;
    a.mpeaceful=b.mpeaceful=1;a.isshk=b.isshk=1;
    neweshk(&a);neweshk(&b);a.nmon=&b;fmon=&a;
    b.mx=20;b.my=10;
    svr.rooms[0].rtype=SHOPBASE;svr.rooms[0].resident=&b;
    levl[20][10].roomno=ROOMOFFSET;
    ESHK(&b)->shoplevel=u.uz;ESHK(&b)->shoproom=ROOMOFFSET;
    u.ushops[0]=ROOMOFFSET;u.ushops[1]=0;
    ESHK(&a)->bill_p=ESHK(&a)->bill;ESHK(&b)->bill_p=ESHK(&b)->bill;
    ESHK(&a)->billct=ESHK(&b)->billct=1;
    ESHK(&a)->bill[0].bo_id=o->o_id+999;
    ESHK(&b)->bill[0].bo_id=o->o_id;ESHK(&b)->bill[0].bquan=10;
    o->quan=10;o->unpaid=1;
    enhancement_rebill(o);original=ESHK(&b)->bill[0].price;
    enhancement_set(o,OEP_FIRE|OEP_TRUEFLIGHT,OQ_EXCEPTIONAL,FALSE);
    enhanced=ESHK(&b)->bill[0].price;assert(enhanced>original);
    enhancement_set(o,0,OQ_STANDARD,FALSE);assert(ESHK(&b)->bill[0].price==original);
    enhancement_set(o,OEP_FIRE|OEP_TRUEFLIGHT,OQ_EXCEPTIONAL,FALSE);
    /* Keep native bill split/price behavior; nextoid can select a safe ID. */
    part=splitobj(o,3);assert(part->unpaid);SAME(o,part);assert(same_price(o,part));
    assert(mergable(o,part));enhancement_set(part,OEP_COLD,OQ_FINE,FALSE);assert(!mergable(o,part));
    {
        struct obj snapshot=*part;
        long debt=ESHK(&b)->debit, price=ESHK(&b)->bill[1].price*part->quan;
        assert(stolen_value(part,20,10,TRUE,TRUE)==price);
        assert(ESHK(&b)->debit==debt+price && !part->unpaid);
        SAME(part,&snapshot);
        snapshot=*o;subfrombill(o,&b);assert(!o->unpaid);SAME(o,&snapshot);
        assert(ESHK(&b)->billct==0);
    }
    ESHK(&a)->billct=ESHK(&b)->billct=0;
    o->unpaid=part->unpaid=0;o->nobj=part->nobj;part->nobj=NULL;obfree(part,NULL);obfree(o,NULL);
    u.ushops[0]=0;svr.rooms[0].resident=NULL;svr.rooms[0].rtype=OROOM;
    fmon=old;dealloc_mextra(&a);dealloc_mextra(&b);
    puts("PASS multi-shop unpaid mutation up/down, billed split/merge, theft debt and return-to-shop");
}

static void
file_mode(NHFILE *f,int mode,int fd)
{
    init_nhfile(f);f->mode=mode;f->ftype=NHF_LEVELFILE;
    f->structlevel=TRUE;f->fieldlevel=FALSE;f->addinfo=FALSE;f->fnidx=historical;f->fd=fd;
    assert(fd>=0);
}

static void
chains(void)
{
    struct obj *chain=NULL,*o,*restored,*walk,*bag;
    int type,props,known,q,flags,count=0;
    NHFILE *f;
    for(type=0;type<2;++type)
    for(props=0;props<256;++props) for(known=0;known<256;++known)
        for(q=0;q<3;++q)for(flags=0;flags<2;++flags) {
            o=item(type?LEATHER_ARMOR:ARROW);
            if(!enhancement_property_allowed(o,props)){obfree(o,NULL);continue;}
            enhancement_set(o,props,q,FALSE);o->o_enh_known=known;o->o_enh_flags=flags;
            o->nobj=chain;chain=o;++count;
        }
    bag=item(SACK);bag->cobj=item(SACK);bag->cobj->cobj=item(DAGGER);
    enhancement_set(bag->cobj->cobj,OEP_FIRE,OQ_EXCEPTIONAL,TRUE);
    bag->nobj=chain;chain=bag;
    f=get_freeing_nhfile();file_mode(f,WRITING,open("step13-chain.tmp",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,_S_IREAD|_S_IWRITE));
    step13_save_chain(f,&chain);close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step13-chain.tmp",O_RDONLY|O_BINARY));
    restored=step13_restore_chain(f);close_nhfile(f);
    SAME(restored->cobj->cobj,bag->cobj->cobj);
    for(o=chain,walk=restored;o&&walk;o=o->nobj,walk=walk->nobj){SAME(o,walk);assert(o->o_id==walk->o_id);}
    assert(!o&&!walk);
    printf("PASS native saveobjchn/restobjchn %d active/knowledge/quality/flags combinations and nested containers\n",count);
}

#include "test_step14_persistence.c"
#include "test_step15d_persistence.c"
#include "test_step16a_persistence.c"

static void
version_gate(void)
{
    NHFILE *f;
    struct version_info v;
    unsigned char header[2];
    f=get_freeing_nhfile();file_mode(f,WRITING,open("step13-version.tmp",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,_S_IREAD|_S_IWRITE));
    store_version(f);close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step13-version.tmp",O_RDONLY|O_BINARY));
    assert(uptodate(f,NULL,UTD_QUIETLY)==0);close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step13-version.tmp",O_RDONLY|O_BINARY));
    assert(read(f->fd,header,2)==2);assert(header[0]=='h');
    lseek(f->fd,2+header[1],SEEK_SET);Sfi_version_info(f,&v,"version_info");
    assert((v.incarnation&255)==12);assert(check_version(&v,NULL,FALSE,0));
    v.incarnation=(v.incarnation&~255UL)|11;
    assert(!check_version(&v,NULL,FALSE,0));close_nhfile(f);
    puts("PASS native critical sizes/epoch-12 header and controlled epoch-11 check_version rejection");
}

static int
level_fixture_count(void)
{
    struct monst *m;
    int n=check_fixture_chain(fobj)+check_fixture_chain(svl.level.buriedobjlist);
    for(m=fmon;m;m=m->nmon)n+=check_fixture_chain(m->minvent);
    return n;
}

static void
free_level(void)
{
    NHFILE *f=get_freeing_nhfile();
    if(iflags.purge_monsters)dmonsfree();
    savelev(f,-1);close_nhfile(f);
}

static void
level_bones(void)
{
    NHFILE *f;
    struct obj *o;
    struct monst *m;
    char *id,why[BUFSZ],count;
    unsigned long seed;
    free_level();u.uz.dnum=medusa_level.dnum;u.uz.dlevel=10;
    gi.in_mklev=TRUE;mklev();gi.in_mklev=FALSE;
    o=item(DAGGER);assert(enhancement_set(o,OEP_STONING_III|OEP_VAMPIRIC_IV,0,FALSE));
    o->o_stoning_remaining=17;o->o_stoning_turn=svm.moves;
    o=oname(o,"step16a-bones",ONAME_NO_FLAGS);place_object(o,20,10);
    o=item(PLATE_MAIL);assert(enhancement_set(o,OEP_LIGHTNESS_III|OEP_DR_IV,0,FALSE));
    o=oname(o,"step16b-bones",ONAME_NO_FLAGS);place_object(o,20,10);
    o=fixture_item("step13-floor");place_object(o,20,10);
    o=fixture_item("step13-hero-bones");place_object(o,20,10);
    o=fixture_item("step13-buried");o->ox=20;o->oy=10;add_to_buried(o);
    m=makemon(&mons[PM_HUMAN],0,0,NO_MINVENT);assert(m);
    add_to_minv(m,fixture_item("step13-monster"));assert(level_fixture_count()==4);
    f=get_freeing_nhfile();file_mode(f,WRITING|FREEING,open("step13-level.tmp",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,_S_IREAD|_S_IWRITE));
    savelev(f,ledger_no(&u.uz));close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step13-level.tmp",O_RDONLY|O_BINARY));
    getlev(f,0,ledger_no(&u.uz));close_nhfile(f);assert(level_fixture_count()==4);
    for(o=fobj;o;o=o->nobj)if(has_oname(o)&&!strcmp(ONAME(o),"step16a-bones"))break;
    assert(o&&o->o_stoning_remaining==17);
    /* This floor item is frozen while the hero is on a different level. */
    svm.moves+=1000;
    f=create_bonesfile(&u.uz,&id,why);assert(f);count=(char)(strlen(id)+1);
    f->mode=WRITING;store_version(f);
    Sfo_char(f,svn.nhuuid,"ancestor-nhuuid",sizeof svn.nhuuid);
    Sfo_char(f,&count,"bones_count",1);Sfo_char(f,id,"bonesid",count);
    savefruitchn(f);savelev(f,ledger_no(&u.uz));close_nhfile(f);commit_bonesfile(&u.uz);
    free_level();wizard=FALSE;flags.bones=TRUE;
    for(seed=1;seed<1000;++seed){init_isaac64(seed,rn2);if(!rn2(3))break;}
    assert(seed<1000);init_isaac64(seed,rn2);mklev();
    assert(level_fixture_count()==4);
    for(o=fobj;o;o=o->nobj)if(has_oname(o)&&!strcmp(ONAME(o),"step16a-bones"))break;
    assert(o&&o->o_stoning_remaining==17&&!o->o_enh_known);
    enhancement_tick();assert(o->o_stoning_remaining==16);
    for(o=fobj;o;o=o->nobj)if(has_oname(o)&&!strcmp(ONAME(o),"step16b-bones"))break;
    assert(o && o->o_enh_props==(OEP_LIGHTNESS_III|OEP_DR_IV)
           && !o->o_enh_known && o->owt==(unsigned)weight(o));
    puts("PASS Step 16B real level/bones codec preserves upper bits and effective weight");
    puts("PASS Step 16A real level/bones codec, frozen off-level cooldown and resumed active tick");
    puts("PASS actual savelev/getlev and accepted mklev/getbones with enhanced floor, hero remains, buried and monster equipment");
}

int
step13_test_main(void)
{
    int i,x,y;
    windowprocs.win_raw_print = test_message;
    windowprocs.win_raw_print_bold = test_message;
    sysopt.crashreporturl = NULL;
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    setvbuf(stdout,NULL,_IONBF,0);
    has_strong_rngseed=FALSE;init_isaac64(130001UL,rn2);init_isaac64(130002UL,rn2_on_display_rng);
    init_objects();flags.pantheon=-1;
    flags.initrole=flags.initrace=flags.initgend=flags.initalign=ROLE_NONE;
    Strcpy(svp.plname,"step13-probe");svp.pl_character[0]='\0';
    role_init();init_dungeons();init_artifacts();
    svc.context.current_fruit=fruitadd("slime mold",NULL);
    u.ulevel=10;u.uhp=u.uhpmax=1000;u.ux=20;u.uy=10;svm.moves=1;
    l_nhcore_init();vision_init();flags.bones=FALSE;wizard=TRUE;
    gy.youmonst.data=&mons[PM_HUMAN];u.umonnum=u.umonster=PM_HUMAN;
    for(i=0;i<A_MAX;++i)ABASE(i)=AMAX(i)=18;
    for(x=1;x<COLNO-1;++x)for(y=1;y<ROWNO-1;++y)levl[x][y].typ=ROOM;
    /* Native projectile animation must not flush an uninitialized test port. */
    flush_screen(-1);
    printf("SIZE|obj=%zu|before=104|props=%zu|known=%zu|quality=%zu|flags=%zu|epoch=%d\n",sizeof(struct obj),sizeof(((struct obj*)0)->o_enh_props),sizeof(((struct obj*)0)->o_enh_known),sizeof(((struct obj*)0)->o_enh_quality),sizeof(((struct obj*)0)->o_enh_flags),EDITLEVEL);
    if(getenv("STEP13_COMBAT_ONLY")) {
        step16b_basic(); step16b_damage(); step16b_reflection(); step16b_casting(); step16a_tests(); combat();combat_paths();armor();step14_combat_tests();
        step14_hero_use_observation_tests();step14_observation_tests();
        puts("PASS Step 13 focused combat/knowledge fixtures");return 0;
    }
    step16b_damage(); step16b_reflection(); step16b_casting(); step16b_basic(); step16a_tests(); step15d_state_tests();
    step15d_catalog_tests();
    step15d_effect_tests();
    step15d_combat_tests();
    matrix();combat();combat_paths();armor();step14_combat_tests();
    step14_hero_use_observation_tests();
    step14_names_prices_tests();step14_observation_tests();
    step14_audit_lifecycle_observation_tests();
    step14_audit_polymorph_context_tests();
    step14_audit_monster_transform_tests();
    step14_generation_tests();step14_corpus();step14_creation_tests();
    lifecycle_names();impact_descriptions();artifacts_prices();billing();chains();step14_persistence_tests();step15d_persistence_tests();step16a_persistence_tests();step16b_persistence();version_gate();level_bones();
    puts("PASS Step 13 native runtime fixtures");return 0;
}

/* Actual newgame/dosave/dorecover fixture; no production acquisition command. */
static struct obj *
fixture_item(const char *name)
{
    struct obj *o=item(DAGGER);
    enhancement_set(o,OEP_FIRE|OEP_PRIMORDIAL,OQ_EXCEPTIONAL,FALSE);
    o->o_enh_known=OEP_FIRE;o->o_enh_flags=OEF_QUALITY_KNOWN;
    o->o_sockets[0].property=EP_STR_III;o->o_sockets[0].value=4;
    return oname(o,name,ONAME_NO_FLAGS);
}

static int
check_fixture_chain(struct obj *chain)
{
    int found=0;
    struct obj *o;
    for(o=chain;o;o=o->nobj) {
        if(has_oname(o)&&!strncmp(ONAME(o),"step13-",7)) {
            assert(o->o_socket_capacity==1&&o->o_sockets[0].property==EP_STR_III
                   &&o->o_sockets[0].value==4&&!o->o_sockets[0].known);
            assert(o->o_enh_props==(OEP_FIRE|OEP_PRIMORDIAL));
            assert(o->o_enh_known==OEP_FIRE&&o->o_enh_quality==OQ_EXCEPTIONAL);
            assert(o->o_enh_flags==OEF_QUALITY_KNOWN);++found;
        }
        if(o->cobj) {
            struct obj *child;
            for(child=o->cobj;child;child=child->nobj)
                assert(child->where==OBJ_CONTAINED&&child->ocontainer==o);
        }
        found+=check_fixture_chain(o->cobj);
    }
    return found;
}

static int
fixture_named(struct obj *chain, const char *name, int where)
{
    int count=0;
    struct obj *o;
    for(o=chain;o;o=o->nobj) {
        if(has_oname(o)&&!strcmp(ONAME(o),name)) {
            assert(o->where==where);++count;
        }
        count+=fixture_named(o->cobj,name,where);
    }
    return count;
}

#include "test_step15d_game.c"

void
step13_game_fixture(boolean resuming)
{
    struct obj *o,*bag,*inner;
    struct monst *m;
    FILE *log;
    int n,local_monster=0,migrating_monster=0;
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    if (getenv("STEP15D_GAME_SEED") || getenv("STEP15D_GAME_CHECK")) {
        if (!resuming || getenv("STEP15D_GAME_CHECK")) step15d_game_fixture(resuming);
        return;
    }
    /* Step 15C inspects unmodified production saves with this diagnostic
     * executable. This is read-only verification, not item acquisition. */
    if (getenv("STEP15C_GAME_CHECK")) {
        int swords = 0, athames = 0;
        assert(resuming);
        for (o = gi.invent; o; o = o->nobj) {
            if (o->otyp != TSURUGI && o->otyp != ATHAME) continue;
            assert(o->quan == 1 && o->dknown && objects[o->otyp].oc_name_known);
            assert(!o->known && !o->bknown && !o->rknown
                   && !o->o_enh_known && !o->o_enh_flags);
            assert(!o->blessed && !o->obranch_material
                   && !o->greased && !o->opoisoned && !has_oname(o));
            if (o->otyp == TSURUGI) {
                assert(o->spe == 4 && !o->o_enh_quality && !o->o_enh_props);
                assert(!o->cursed && !o->oeroded && !o->oeroded2 && !o->oerodeproof);
                ++swords;
            } else {
                assert(o->spe == -2 && o->cursed && o->oeroded == 1
                       && o->oeroded2 == 1 && o->oerodeproof
                       && o->o_enh_quality == OQ_EXCEPTIONAL
                       && o->o_enh_props == (OEP_FIRE | OEP_PRIMORDIAL));
                ++athames;
            }
        }
        assert(swords == 1 && athames == 1);
        log = fopen("step15c-game-results.txt", "a"); assert(log);
        fprintf(log, "PASS production forged tsurugi +4 and cursed -2 eroded/proof Exceptional Fire I/Primordial athame; actual state and hidden knowledge preserved\n");
        fclose(log);
        return;
    }
    if(!resuming) {
        struct obj *defense_bag=item(SACK);
        int property;
        for(property=0;property<12;++property) {
            o=item(PLATE_MAIL);
            assert(enhancement_set(o,1ULL<<(48+property),0,FALSE));
            add_to_container(defense_bag,o);
        }
        defense_bag->owt=weight(defense_bag); /* native container insertion callers refresh caches */
        addinv(oname(defense_bag,"step16b-checkpoint",ONAME_NO_FLAGS));
        o=item(DAGGER);assert(enhancement_set(o,OEP_STONING_III|OEP_VAMPIRIC_IV,0,FALSE));
        o->o_stoning_remaining=17;o->o_stoning_turn=svm.moves;
        o->o_sockets[0].property=EP_ACID_III;
        addinv(oname(o,"step16a-checkpoint",ONAME_NO_FLAGS));
        o = fixture_item("step13-inventory");
        if (getenv("STEP15C_GAME_SEED")) {
            o->spe = -5; o->cursed = 1;
            o->oeroded = 1; o->oeroded2 = 2; o->oerodeproof = 1;
        }
        addinv(o);
        bag=item(SACK);inner=item(SACK);add_to_container(inner,fixture_item("step13-nested"));
        add_to_container(bag,inner);addinv(bag);
        o=fixture_item("step13-floor");place_object(o,u.ux,u.uy);
        o=fixture_item("step13-buried");o->ox=u.ux;o->oy=u.uy;add_to_buried(o);
        o=fixture_item("step13-migrating");add_to_migration(o);
        o=fixture_item("step13-bill");add_to_billobjs(o);
        m=makemon(&mons[PM_HUMAN],u.ux,u.uy,MM_ADJACENTOK|NO_MINVENT);
        assert(m);m->mpeaceful=TRUE;m->msleeping=TRUE;
        add_to_minv(m,fixture_item("step13-monster"));
        m=makemon(&mons[PM_HUMAN],u.ux,u.uy,MM_ADJACENTOK|NO_MINVENT);
        assert(m);m->mpeaceful=TRUE;m->msleeping=TRUE;
        add_to_minv(m,fixture_item("step13-migrating-monster"));
        {
            d_level dest=u.uz;
            ++dest.dlevel;
            migrate_to_level(m,ledger_no(&dest),MIGR_RANDOM,NULL);
        }
    }
    n=check_fixture_chain(gi.invent)+check_fixture_chain(fobj)
       +check_fixture_chain(svl.level.buriedobjlist)+check_fixture_chain(gm.migrating_objs)
       +check_fixture_chain(gb.billobjs);
    for(m=fmon;m;m=m->nmon) {
        n+=check_fixture_chain(m->minvent);
        local_monster+=fixture_named(m->minvent,"step13-monster",OBJ_MINVENT);
        for(o=m->minvent;o;o=o->nobj)assert(o->ocarry==m);
    }
    for(m=gm.migrating_mons;m;m=m->nmon) {
        n+=check_fixture_chain(m->minvent);
        migrating_monster+=fixture_named(m->minvent,"step13-migrating-monster",OBJ_MINVENT);
        for(o=m->minvent;o;o=o->nobj)assert(o->ocarry==m);
    }
    assert(n==8&&local_monster==1&&migrating_monster==1);
    assert(fixture_named(gi.invent,"step13-inventory",OBJ_INVENT)==1);
    assert(fixture_named(gi.invent,"step13-nested",OBJ_CONTAINED)==1);
    assert(fixture_named(fobj,"step13-floor",OBJ_FLOOR)==1);
    assert(fixture_named(svl.level.buriedobjlist,"step13-buried",OBJ_BURIED)==1);
    assert(fixture_named(gm.migrating_objs,"step13-migrating",OBJ_MIGRATING)==1);
    assert(fixture_named(gb.billobjs,"step13-bill",OBJ_ONBILL)==1);
    for(o=gi.invent;o;o=o->nobj)if(has_oname(o)&&!strcmp(ONAME(o),"step16a-checkpoint"))break;
    assert(o&&o->o_stoning_remaining==17&&o->o_enh_props==(OEP_STONING_III|OEP_VAMPIRIC_IV)
           &&o->o_sockets[0].property==EP_ACID_III&&!o->o_enh_known);
    for(o=gi.invent;o;o=o->nobj)if(has_oname(o)&&!strcmp(ONAME(o),"step16b-checkpoint"))break;
    assert(o && o->owt==(unsigned)weight(o));
    {
        uint64 restored=0;
        for(inner=o->cobj;inner;inner=inner->nobj) {
            restored |= inner->o_enh_props;
            assert(!inner->o_enh_known && inner->owt==(unsigned)weight(inner));
        }
        assert(restored==OEP_DEFENSIVE);
    }
    log=fopen("step13-game-results.txt","a");assert(log);
    fprintf(log,"PASS %s all eight ownership paths, migrating monster, container/monster backlinks, offensive upper bits and Stoning cooldown\n",resuming?"restored":"created");
    fclose(log);
}
