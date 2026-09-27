/* Native menu/transaction tests, included after the retained forge fixtures. */
static int affix_test_index(struct obj *wanted)
{
    struct obj *o; int i;
    for (o=gi.invent,i=1;o && o!=wanted;o=o->nobj,++i) ;
    assert(o); return i;
}

static void step15d_forge_tests(void)
{
    struct obj *hammer,*target,*gem,*o,saved,hs;
    int tier,buc,seed,mode,pool[EP_COUNT],n,id,value,success,next,cases=0;
    const int gems[]={JET,BLACK_OPAL,RUBY,DIAMOND};
    unsigned tid,gid;
    forge_test_clear();
    levl[u.ux][u.uy].typ=FORGE;ABASE(A_STR)=18;
    for(tier=1;tier<=4;++tier)for(buc=-1;buc<=1;++buc)
        for(mode=0;mode<2;++mode)for(seed=1;seed<=20;++seed) {
            hammer=forge_test_item(WAR_HAMMER,1);
            hammer->blessed=buc>0;hammer->cursed=buc<0;hammer->bknown=0;
            target=forge_test_item(DAGGER,1);
            gem=forge_test_item(gems[tier-1],2);gem->bknown=0;
            gem->blessed=seed%2;gem->cursed=!(seed%2);
            if(mode) {target->o_sockets[0].property=EP_FIRE;target->o_sockets[0].known=0;}
            saved=*target;hs=*hammer;tid=target->o_id;gid=gem->o_id;
            assert(affix_chance(hammer,tier)==80-10*tier+10*buc);
            n=socket_candidates(target,tier,0,pool);
            init_isaac64(seed,rn2);
            success=rn2(100)<80-10*tier+10*buc;
            id=value=0;
            if(success) {
                const struct enhancement_entry *e;
                id=pool[rn2(n)];e=equipment_property(id);
                if(e->stat)value=d(e->dice,e->sides);
            }
            next=rn2(1000000);init_isaac64(seed,rn2);
            assert(affix_commit(hammer,tid,gid,0)==ECMD_TIME);
            assert(rn2(1000000)==next);
            assert(forge_find(gid)->quan==1&&!forge_find(gid)->bknown);
            forge_test_same_object(&hs,hammer);
            target=forge_find(tid);
            assert(target->quan==1 && target->o_sockets[0].property==id);
            assert(target->o_sockets[0].value==value && target->o_sockets[0].known==success);
            if (!success && !mode) forge_test_same_object(&saved,target);
            ++cases;forge_test_clear();
        }
    printf("PASS Step 15D %d tier/hammer/BUC/stack outcomes with exact success-selection-value RNG tails\n",cases);

    for(mode=0;mode<2;++mode) {
        hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(DAGGER,1);
        gem=forge_test_item(WORTHLESS_RED_GLASS,2);
        objects[gem->otyp].oc_name_known=0;
        if(mode)target->o_sockets[0].property=EP_STR_IV,target->o_sockets[0].value=6;
        saved=*target;tid=target->o_id;gid=gem->o_id;
        init_isaac64(715,rn2);
        next=rn2(1000000);init_isaac64(715,rn2);
        assert(affix_commit(hammer,tid,gid,0)==ECMD_TIME);
        assert(rn2(1000000)==next&&!objects[gem->otyp].oc_name_known);
        assert(gem->quan==1);
        assert(target->quan==1 && !socket_count(target));
        if (!mode) forge_test_same_object(&saved,target);
        objects[gem->otyp].oc_name_known=1;
        saved=*target;
        assert(affix_commit(hammer,tid,gid,0)==ECMD_OK);
        forge_test_same_object(&saved,target);assert(gem->quan==1);
        forge_test_clear();
    }

    hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(BOW,1);
    assert(enhancement_set(target,OEP_PRIMORDIAL|OEP_STR_IV,OQ_STANDARD,FALSE));
    target->o_sockets[0].property=EP_DEX_IV;target->o_sockets[0].value=2;
    gem=forge_test_item(DIAMOND,2);gid=gem->o_id;tid=target->o_id;
    saved=*target;
    assert(!socket_candidates(target,4,1,pool));
    assert(affix_commit(hammer,tid,gid,1)==ECMD_OK);
    forge_test_same_object(&saved,target);assert(gem->quan==2);
    objects[DIAMOND].oc_name_known=0;
    init_isaac64(715,rn2);next=rn2(1000000);init_isaac64(715,rn2);
    assert(affix_commit(hammer,tid,gid,1)==ECMD_TIME);
    assert(rn2(1000000)==next);forge_test_same_object(&saved,target);
    target->o_sockets[1].property=EP_FIRE;
    assert(affix_commit(hammer,tid,gid,1)==ECMD_TIME);
    assert(!target->o_sockets[1].property&&target->o_sockets[0].property==EP_DEX_IV);
    assert(!objects[DIAMOND].oc_name_known);
    forge_test_clear();
    puts("PASS Step 15D glass and exhausted-tier rejection/commitment, no RNG, hidden identity and replacement destruction");

    /* Socketing one item is in-place even when inventory is full. Stacks
     * must fail without payment, RNG, mutation or attempted splitting. */
    hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(DAGGER,1);
    gem=forge_test_item(DIAMOND,2);makeknown(DIAMOND);
    while(inv_cnt(FALSE)<invlet_basic) {o=forge_output(ROCK);o->nomerge=1;addinv(o);}
    assert(affix_room(target,gem,0));
    target->quan=3;saved=*target;tid=target->o_id;gid=gem->o_id;
    init_isaac64(715,rn2);next=rn2(1000000);init_isaac64(715,rn2);
    assert(affix_commit(hammer,tid,gid,0)==ECMD_OK && rn2(1000000)==next);
    forge_test_same_object(&saved,target);assert(gem->quan==2);
    forge_test_clear();
    puts("PASS Step 17 socket target stack rejection, full-pack in-place capacity and nonmutation");

    /* Highest stored value per identity, with no inheritance RNG or sockets. */
    {
        struct forge_state state={0};struct obj *other,*output;
        target=forge_test_item(LONG_SWORD,1);
        enhancement_set(target,OEP_STR_III|OEP_DEX_IV,OQ_FINE,FALSE);
        target->o_enh_values[2]=4;target->o_enh_values[7]=2;
        target->o_sockets[0].property=EP_FIRE;
        other=forge_test_item(LONG_SWORD,1);
        enhancement_set(other,OEP_STR_III|OEP_DEX_IV,OQ_STANDARD,FALSE);
        other->o_enh_values[2]=2;other->o_enh_values[7]=6;
        output=forge_output(KATANA);
        init_isaac64(715,rn2);next=rn2(1000000);init_isaac64(715,rn2);
        forge_gather(&state,target);forge_gather(&state,other);forge_inherit(output,&state);
        assert(rn2(1000000)==next&&output->o_enh_values[2]==4&&output->o_enh_values[7]==6);
        assert(!socket_count(output)&&output->o_socket_capacity==1&&!output->o_enh_known);
        assert(output->o_enh_props==(OEP_STR_III|OEP_DEX_IV));obfree(output,NULL);
        forge_test_clear();
    }
    puts("PASS Step 15D highest stored ordinary STR/DEX inheritance, no reroll, fresh empty output sockets");

    /* Each cancellable menu boundary is production forge_interact dispatch. */
    for(mode=0;mode<5;++mode) {
        hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(DAGGER,1);
        gem=forge_test_item(JET,2);target->o_sockets[0].property=EP_STR_I;
        target->o_sockets[0].value=2;saved=*target;hs=*hammer;
        forge_test_script();
        if(mode>0)forge_test_choose("Use the forge",2,-1);
        if(mode>1)forge_test_choose("Choose equipment",affix_test_index(target),-1);
        if(mode>2)forge_test_choose("Choose a gemstone",affix_test_index(gem),-1);
        if(mode>3)forge_test_choose("Choose a socket",1,-1);
        init_isaac64(715,rn2);next=rn2(1000000);init_isaac64(715,rn2);
        assert(forge_interact(hammer)==ECMD_OK&&rn2(1000000)==next);
        forge_test_same_object(&saved,target);forge_test_same_object(&hs,hammer);
        assert(gem->quan==2);
        assert(!strstr(forge_screen,"Strength I")&&!strstr(forge_screen,"+2"));
        forge_test_clear();
    }
    hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(DAGGER,1);
    gem=forge_test_item(JET,2);gem->dknown=1;hammer->bknown=0;
    forge_test_script();forge_test_choose("Use the forge",2,-1);
    forge_test_choose("Choose equipment",affix_test_index(target),-1);
    forge_test_choose("Choose a gemstone",affix_test_index(gem),-1);
    forge_test_choose("Confirm socketing",1,-1);
    assert(forge_interact(hammer)==ECMD_TIME);
    assert(strstr(forge_screen,"Success chance: unknown"));
    assert(!hammer->bknown&&gem->quan==1);
    forge_test_clear();
    puts("PASS Step 15D production menu navigation, five zero-turn cancellation boundaries, knowledge-safe replacement/chance and commit");

    hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(DAGGER,1);
    gem=forge_test_item(JET,2);makeknown(JET);
    assert(!affix_target(hammer,hammer)&&affix_target(target,hammer));
    target->unpaid=1;assert(!affix_target(target,hammer));target->unpaid=0;
    target->oartifact=ART_EXCALIBUR;assert(!affix_target(target,hammer));target->oartifact=0;
    target->where=OBJ_FLOOR;assert(!affix_target(target,hammer));
    target->where=OBJ_CONTAINED;assert(!affix_target(target,hammer));target->where=OBJ_INVENT;
    target->owornmask=W_WEP;assert(!affix_target(target,hammer));
    target->owornmask=W_SWAPWEP;u.twoweap=FALSE;assert(affix_target(target,hammer));
    u.twoweap=TRUE;assert(!affix_target(target,hammer));u.twoweap=FALSE;target->owornmask=0;
    gem->unpaid=1;assert(!affix_gem(gem,target,0));gem->unpaid=0;
    for(mode=0;mode<4;++mode) {
        char chance[80];
        hammer->bknown=(mode&1)!=0;gem->dknown=(mode&2)!=0;
        saved=*target;hs=*hammer;
        forge_test_script();forge_test_choose("Use the forge",2,-1);
        forge_test_choose("Choose equipment",affix_test_index(target),-1);
        forge_test_choose("Choose a gemstone",affix_test_index(gem),-1);
        init_isaac64(715,rn2);next=rn2(1000000);init_isaac64(715,rn2);
        assert(forge_interact(hammer)==ECMD_OK&&rn2(1000000)==next);
        Sprintf(chance,"Success chance: %s",mode==3?"70%":"unknown");
        assert(strstr(forge_screen,chance));
        forge_test_same_object(&saved,target);forge_test_same_object(&hs,hammer);
        assert(gem->quan==2);
    }
    forge_test_clear();
    hammer=forge_test_item(WAR_HAMMER,1);target=forge_test_item(BOW,1);
    gem=forge_test_item(DIAMOND,2);makeknown(DIAMOND);
    enhancement_set(target,OEP_PRIMORDIAL|OEP_STR_IV,OQ_STANDARD,FALSE);
    target->o_sockets[0].property=EP_DEX_IV;target->o_sockets[0].value=2;
    target->o_sockets[1].property=EP_FIRE;
    assert(affix_gem(gem,target,-1)&&affix_gem(gem,target,0));
    assert(!affix_gem(gem,target,1));
    memset(target->o_sockets,0,sizeof target->o_sockets);saved=*target;
    assert(affix_commit(hammer,target->o_id,gem->o_id,1)==ECMD_OK);
    forge_test_same_object(&saved,target);assert(gem->quan==2);
    forge_test_clear();
    puts("PASS Step 15D recipient/unpaid/ownership gates, four chance-disclosure states, full-target replacement filtering and first-empty ordering");
}

static void
step15d_continuation_tests(void)
{
    struct obj *hammer, *target, *gem, *box, *hidden;
    unsigned gem_id;
    int seed, failure_seed = 0, i;
    boolean found;

    ABASE(A_STR) = 18;
    for (seed = 1; seed < 1000 && !failure_seed; ++seed) {
        init_isaac64(seed, rn2);
        if (rn2(100) >= 30)
            failure_seed = seed;
    }
    assert(failure_seed);

    /* A failed attempt with another directly carried candidate asks exactly
     * once and keeps the same target workflow. */
    forge_test_clear();
    hammer = forge_test_item(WAR_HAMMER, 1); hammer->cursed = 1;
    target = forge_test_item(BOW, 1);
    gem = forge_test_item(DIAMOND, 2);
    forge_test_script(); forge_test_answer("Try again?", 'n');
    forge_test_choose("Use the forge", 2, -1);
    forge_test_choose("Choose equipment", affix_test_index(target), -1);
    forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
    forge_test_choose("Confirm socketing", 1, -1);
    init_isaac64(failure_seed, rn2);
    assert(forge_interact(hammer) == ECMD_TIME);
    assert(forge_socket_shortcut_seen);
    assert(forge_yn_count == 1 && !strcmp(forge_yn_prompts[0], "Try again?"));
    assert(gem->quan == 1 && !socket_count(target));
    forge_test_clear();

    /* A container-only remainder never enables a continuation prompt. */
    hammer = forge_test_item(WAR_HAMMER, 1); hammer->cursed = 1;
    target = forge_test_item(BOW, 1);
    gem = forge_test_item(DIAMOND, 1);
    gem_id = gem->o_id;
    box = forge_test_item(SACK, 1);
    hidden = forge_output(DIAMOND); add_to_container(box, hidden);
    forge_test_script();
    forge_test_choose("Use the forge", 2, -1);
    forge_test_choose("Choose equipment", affix_test_index(target), -1);
    forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
    forge_test_choose("Confirm socketing", 1, -1);
    init_isaac64(failure_seed, rn2);
    assert(forge_interact(hammer) == ECMD_TIME);
    assert(!forge_yn_count && !forge_find(gem_id)
           && hidden->where == OBJ_CONTAINED);
    forge_test_clear();

    /* Two successful fills stay on the same target, with no second hammer
     * activation or target selection, and the prompt is exact. */
    found = FALSE;
    for (seed = 1; seed < 1000 && !found; ++seed) {
        forge_test_clear();
        hammer = forge_test_item(WAR_HAMMER, 1); hammer->blessed = 1;
        target = forge_test_item(BOW, 1);
        gem = forge_test_item(JET, 2);
        gem_id = gem->o_id;
        forge_test_script(); forge_test_answer_default('y');
        forge_test_choose("Use the forge", 2, -1);
        forge_test_choose("Choose equipment", affix_test_index(target), -1);
        forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
        forge_test_choose("Confirm socketing", 1, -1);
        forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
        forge_test_choose("Confirm socketing", 1, -1);
        init_isaac64(seed, rn2);
        assert(forge_interact(hammer) == ECMD_TIME);
        if (forge_yn_count == 1
            && !strcmp(forge_yn_prompts[0], "Socket another gem?")
            && socket_count(target) == 2) {
            found = TRUE;
            assert(!forge_find(gem_id));
        }
    }
    assert(found);
    forge_test_clear();

    /* A successful full target with another valid gem enters replacement. */
    found = FALSE;
    for (seed = 1; seed < 1000 && !found; ++seed) {
        forge_test_clear();
        hammer = forge_test_item(WAR_HAMMER, 1); hammer->blessed = 1;
        target = forge_test_item(BOW, 1);
        target->o_sockets[0].property = EP_FIRE;
        target->o_sockets[1].property = EP_COLD;
        gem = forge_test_item(JET, 2);
        forge_test_script(); forge_test_answer_default('n');
        forge_test_choose("Use the forge", 2, -1);
        forge_test_choose("Choose equipment", affix_test_index(target), -1);
        forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
        forge_test_choose("Choose a socket", 1, -1);
        forge_test_choose("Confirm socketing", 1, -1);
        init_isaac64(seed, rn2);
        assert(forge_interact(hammer) == ECMD_TIME);
        if (forge_yn_count == 1
            && !strcmp(forge_yn_prompts[0], "Replace a gem?")) {
            found = TRUE;
            assert(socket_count(target) == 2 && gem->quan == 1);
        }
    }
    assert(found);
    forge_test_clear();

    /* A successful one-slot target with no remaining direct gem does not
     * prompt. This also protects the no-valid-replacement branch. */
    found = FALSE;
    for (seed = 1; seed < 1000 && !found; ++seed) {
        forge_test_clear();
        hammer = forge_test_item(WAR_HAMMER, 1); hammer->blessed = 1;
        target = forge_test_item(DAGGER, 1);
        gem = forge_test_item(JET, 1);
        forge_test_script();
        forge_test_choose("Use the forge", 2, -1);
        forge_test_choose("Choose equipment", affix_test_index(target), -1);
        forge_test_choose("Choose a gemstone", affix_test_index(gem), -1);
        forge_test_choose("Confirm socketing", 1, -1);
        init_isaac64(seed, rn2);
        assert(forge_interact(hammer) == ECMD_TIME);
        if (!forge_yn_count && socket_count(target) == 1)
            found = TRUE;
    }
    assert(found);
    forge_test_clear();
    puts("PASS Step 15D socket continuation prompts, direct-inventory gating, same-target looping and replacement entry");
}
