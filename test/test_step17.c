/* Step 17 contracts, included in the existing native diagnostic harness. */
extern void step17_forge_tests(void);
static void
step17_codec(void)
{
    struct obj *chain=0,*o;
    int open,history,second;
    for(open=0;open<2;++open)for(history=0;history<=10;history+=10)
        for(second=0;second<3;++second) {
            o=item(DAGGER);
            assert(enhancement_slot_set(o,0,EP_STR_IV,FALSE,6));
            if(open)assert(enhancement_slot_set(o,0,0,TRUE,-1));
            o->o_affixes[0].history=(uint8)history;
            if(second) {
                assert(enhancement_slot_set(o,1,EP_FIRE,TRUE,-1));
                if(second==2)assert(enhancement_slot_set(o,1,0,TRUE,-1));
                o->o_affixes[1].history=(uint8)history;
            }
            o->o_sockets[0].property=EP_COLD;o->o_sockets[0].known=1;
            assert(enhancement_slots_valid(o));
            assert(!enhancement_mask_allowed(o,enhancement_mask_property(EP_COLD)));
            o->nobj=chain;chain=o;
        }
    o=item(GENERIC_ESSENCE);o->quan=57;o->owt=weight(o);o->nobj=chain;chain=o;
    step14_roundtrip_batch(&chain);
    o=item(PICK_AXE);
    assert(enhancement_slot_set(o,0,EP_PURIFICATION_IV,TRUE,-1));
    assert(o->o_purification_remaining==100);o->o_purification_remaining=17;
    assert(enhancement_slot_set(o,0,0,TRUE,-1));
    assert(!o->o_purification_remaining&&!o->o_purification_sampled);
    assert(enhancement_slot_set(o,0,EP_PURIFICATION_IV,TRUE,-1));
    assert(o->o_purification_remaining==100&&!o->o_purification_sampled);
    obfree(o,NULL);
    puts("PASS Step 17 native object codec cross-product, socket collision exclusion and fresh Purification");
}

static void
step17_essence_rng(void)
{
    int request,seed,roll,want,next,proc,count=0,contents=0,quantities[3]={0};
    struct obj *o,*p,*expected;
    char wish[40];
    for(request=1;request<=12;++request)for(seed=1;seed<=200;++seed) {
        init_isaac64(seed,rn2);
        want=request;
        if(request>=10)want=1;
        else if(request>=6) {roll=rn2(100);want=roll < (10-request)*20 ? request : 1;}
        next=rn2(1000000);init_isaac64(seed,rn2);
        assert(essence_wish_quantity(request)==want && rn2(1000000)==next);
    }
    for(request=1;request<=5;++request) {
        Sprintf(wish,"%d blessed Essence",request);
        o=readobjnam(wish,(struct obj *)0);
        assert(o && o->otyp==GENERIC_ESSENCE && o->quan==request);
        assert(!o->blessed&&!o->cursed&&o->known&&o->bknown);
        obfree(o,NULL);
    }
    for(seed=1;seed<=20000;++seed) {
        init_isaac64(seed,rn2);proc=rn2(100)==0;
        init_isaac64(seed,rn2);
        o=ordinary_loot_at(RANDOM_CLASS,21,10,FALSE);
        assert((o->otyp==GENERIC_ESSENCE)==proc);
        if(proc) {
            ++count;assert(o->quan==1||o->quan==2);++quantities[o->quan];
            next=rn2(1000000);
            init_isaac64(seed,rn2);(void)rn2(100);expected=essence_random();
            assert(o->quan==expected->quan && rn2(1000000)==next);obfree(expected,NULL);
        }
        for(p=o->cobj;p;p=p->nobj) if(p->otyp==GENERIC_ESSENCE) {
            ++contents;assert(p->quan==1||p->quan==2);
        }
        obj_extract_self(o);obfree(o,NULL);
        assert(!essence_loot_context());
        init_isaac64(seed,rn2);
        o=ordinary_loot_at(WEAPON_CLASS,21,10,FALSE);
        assert(o->oclass==WEAPON_CLASS && o->otyp!=GENERIC_ESSENCE);
        obj_extract_self(o);obfree(o,NULL);
        o=mksobj(CHEST,TRUE,FALSE);
        for(p=o->cobj;p;p=p->nobj) assert(p->otyp!=GENERIC_ESSENCE);
        obfree(o,NULL);
    }
    assert(count>140 && count<260 && quantities[1]>60 && quantities[2]>60 && contents>0);
    puts("PASS Step 17 exact wish table replay, native wishes, 1 percent replacement and constrained-loot exclusion");
}
static void
step17_tests(void)
{
    struct obj *a = item(DAGGER), *b = item(DAGGER), *ammo = item(ARROW);
    assert(enhancement_set(a, OEP_FIRE, OQ_STANDARD, TRUE));
    assert(enhancement_set(b, OEP_FIRE, OQ_STANDARD, TRUE));
    assert(!mergable(a, b));
    assert(!enhancement_set(ammo, OEP_FIRE, OQ_STANDARD, TRUE));
    {
        int i, ids[] = { EP_FIRE, EP_FIRE_II, EP_FIRE_III, EP_PRIMORDIAL };
        int pool[EP_COUNT], n;
        for (i = 0; i < 4; ++i) {
            enhancement_clear(a);
            assert(enhancement_slot_set(a, 0, ids[i], TRUE, -1));
            assert(a->o_affixes[0].tier == i + 1 && !a->o_affixes[0].history);
            assert(enhancement_slot_set(a, 0, 0, TRUE, -1));
            enhancement_slot_history(a, 0);
            assert(a->o_affixes[0].tier == i + 1 && a->o_affixes[0].history == 1);
            assert(!a->o_affixes[0].property && !a->o_enh_props);
            assert(enhancement_slot_count(a) == 1);
            assert(!enhancement_forge_candidates(a, 1, i + 1, FALSE, pool));
            n = enhancement_forge_candidates(a, 0, i + 1, FALSE, pool);
            assert(n > 0);
            assert(enhancement_slot_set(a, 0, ids[i], TRUE, -1));
            assert(a->o_affixes[0].history == 1 && enhancement_slots_valid(a));
        }
        enhancement_clear(a);
        assert(enhancement_slot_set(a, 0, EP_STONING_I, TRUE, -1));
        a->o_stoning_remaining = 21; a->o_stoning_turn = 3;
        assert(enhancement_slot_set(a, 1, EP_STR_III, TRUE, 3));
        assert(enhancement_slot_set(a, 0, 0, TRUE, -1));
        assert(!a->o_stoning_remaining && !a->o_stoning_turn);
        assert(a->o_affixes[1].property == EP_STR_III && a->o_enh_values[2] == 3);
        assert(!enhancement_slot_set(a, 0, EP_FIRE_II, TRUE, -1));
        for (i = 0; i < 50; ++i) enhancement_slot_history(a, 0);
        assert(a->o_affixes[0].history == 10);
        assert(enhancement_slot_set(a, 0, EP_STONING_I, TRUE, -1));
        assert(!a->o_stoning_remaining && a->o_affixes[0].history == 10);
        n = enhancement_forge_candidates(a, 0, 1, TRUE, pool);
        assert(n > 0);
        for (i = 0; i < n; ++i) assert(pool[i] != EP_STONING_I);
        assert(enhancement_slot_set(a, 1, 0, TRUE, -1));
        assert(!a->o_enh_values[2] && enhancement_slots_valid(a));
        a->o_affixes[1].history = 11;
        assert(!enhancement_slots_valid(a)); a->o_affixes[1].history = 0;
        a->quan = 2;
        assert(!enhancement_slots_valid(a));
        enhancement_finalize_stack(a);
        assert(enhancement_slots_valid(a) && !enhancement_slot_count(a));
    }
    obfree(a, NULL); obfree(b, NULL); obfree(ammo, NULL);
    puts("PASS Step 17 slot tier, order, Open capacity, saturation, candidates and runtime cleanup");
    puts("PASS Step 17 global affix merge and ammunition invariants");
    a = item(GENERIC_ESSENCE); b = item(GENERIC_ESSENCE);
    a->quan = 7; a->owt = weight(a);
    assert(a->owt == 7 && a->known && a->dknown && a->bknown);
    bless(a); assert(!a->blessed && !a->cursed);
    curse(a); assert(!a->blessed && !a->cursed);
    unbless(a); uncurse(a);
    unknow_object(a);
    assert(a->known && a->dknown && a->bknown && mergable(a, b));
    a->blessed=1;b->cursed=1;
    assert(mergable(a,b) && !a->blessed && !a->cursed && !b->blessed && !b->cursed);
    assert(!enhancement_eligible(a) && !socket_capacity(a));
    assert(objects[GENERIC_ESSENCE].oc_prob == 0);
    {
        struct monst merchant={0}, *shkp=&merchant;
        neweshk(&merchant);ESHK(&merchant)->shoptype=SHOPBASE;
        assert(!saleable(&merchant,a));
        assert(!billable(&shkp,a,ROOMOFFSET,TRUE) && !a->unpaid);
        dealloc_mextra(&merchant);
    }
    obfree(a, NULL); obfree(b, NULL);
    puts("PASS Step 17 Essence neutrality, identification, weight and stacking");
    step17_codec();
    step17_essence_rng();
    step17_forge_tests();
    level_bones();
}
