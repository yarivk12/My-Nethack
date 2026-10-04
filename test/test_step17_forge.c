#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
/* Script actual production menus and record all visible text. */
static char fa_screen[40000], fa_prompt[BUFSZ];
static int fa_choices[20], fa_nchoices, fa_next, fa_ids[EP_COUNT + 10], fa_nids;
static struct obj *fa_inspect;
static winid fa_window(int kind) { (void) kind; return 1; }
static void fa_destroy(winid win) { (void) win; }
static void fa_display(winid win, boolean block) { (void) win; (void) block; }
static void fa_text(winid win, int attr, const char *text) {
    (void) win; (void) attr;
    assert(strlen(fa_screen) + strlen(text) + 2 < sizeof fa_screen);
    strcat(fa_screen, text); strcat(fa_screen, "\n");
}
static void fa_start(winid win, unsigned long behavior) {
    (void) win; (void) behavior; fa_nids = 0;
}
static void fa_end(winid win, const char *prompt) { (void) win; Strcpy(fa_prompt, prompt); }
static void fa_add(winid win, const glyph_info *glyph, const ANY_P *id,
                   char letter, char group, int attr, int color,
                   const char *text, unsigned int flags) {
    (void) glyph; (void) letter; (void) group; (void) color; (void) flags;
    assert(fa_nids < SIZE(fa_ids)); fa_ids[fa_nids++] = id->a_int;
    fa_text(win, attr, text);
}
static int fa_select(winid win, int how, MENU_ITEM_P **picked) {
    int choice, i;
    (void) win; assert(how == PICK_ONE); *picked = 0;
    if (fa_inspect && strstr(fa_prompt, "inspect")) {
        *picked = (menu_item *) alloc(sizeof **picked);
        (*picked)->item.a_obj = fa_inspect; (*picked)->count = -1;
        return 1;
    }
    if (fa_next == fa_nchoices || !(choice = fa_choices[fa_next++])) return -1;
    for (i = 0; i < fa_nids && fa_ids[i] != choice; ++i) ;
    assert(i < fa_nids);
    *picked = (menu_item *) alloc(sizeof **picked);
    (*picked)->item.a_int = choice; (*picked)->count = -1; return 1;
}
static void fa_script(void) { fa_screen[0] = 0; fa_next = fa_nchoices = 0; fa_inspect = 0; }
static void fa_choice(int choice) { assert(fa_nchoices < SIZE(fa_choices)); fa_choices[fa_nchoices++] = choice; }
static int fa_index(struct obj *obj) {
    struct obj *p; int n = 1;
    for (p = gi.invent; p && p != obj; p = p->nobj) ++n;
    assert(p); return n;
}
static void fa_known(struct obj *obj) {
    obj->known = obj->dknown = obj->bknown = 1;
    objects[obj->otyp].oc_name_known = 1; enhancement_identify(obj);
}
static long fa_quantity(int typ) {
    struct obj *p; long n = 0;
    for (p = gi.invent; p; p = p->nobj) if (p->otyp == typ) n += p->quan;
    return n;
}

static void
fa_transactions(struct obj *hammer, struct obj *obj)
{
    struct forge_affix_plan plan;
    struct obj before;
    int seed, first, second, next, n, id, pool[EP_COUNT], seen[EP_COUNT] = {0};
    long gold, essence;
    /* Exact deterministic sampling replay verifies uniform indices, distinct
     * choices and no magnitude roll when keeping or escaping the paid menu. */
    for (seed = 1; seed <= 160; ++seed) {
        enhancement_clear(obj);
        assert(enhancement_slot_set(obj, 0, EP_STR_I, TRUE, 1)); fa_known(obj);
        obj->o_affixes[0].history = 10;
        assert(forge_affix_plan(hammer, obj, FA_REROLL, 0, 1, 0, &plan));
        n = enhancement_forge_candidates(obj, 0, 1, TRUE, pool);
        assert(n == plan.count && n > 2);
        for (id = 0; id < n; ++id) assert(pool[id] != EP_STR_I);
        init_isaac64(seed, rn2);
        first = rn2(n); second = rn2(n-1); if (second >= first) ++second;
        next = rn2(1000000); assert(first != second); ++seen[first];
        gold = fa_quantity(GOLD_PIECE); essence = fa_quantity(GENERIC_ESSENCE);
        fa_script(); fa_choice(seed % 2 ? 3 : 0);
        init_isaac64(seed, rn2);
        assert(forge_affix_commit(hammer, &plan) == ECMD_TIME);
        assert(rn2(1000000) == next);
        assert(strstr(fa_screen, equipment_property(pool[first])->prefix));
        assert(strstr(fa_screen, equipment_property(pool[second])->prefix));
        assert(obj->o_affixes[0].property == EP_STR_I && obj->o_enh_values[0] == 1);
        assert(obj->o_affixes[0].history == 10);
        assert(fa_quantity(GOLD_PIECE) == gold - plan.gold);
        assert(fa_quantity(GENERIC_ESSENCE) == essence - plan.essence);
    }
    for (id = 0; id < n; ++id) assert(seen[id] > 0);
    /* Selecting a variable property rolls only after the paid choice. */
    enhancement_clear(obj); assert(enhancement_slot_set(obj,0,EP_FIRE,TRUE,-1)); fa_known(obj);
    assert(forge_affix_plan(hammer,obj,FA_REROLL,0,1,0,&plan));
    for (seed = 1; seed < 10000; ++seed) {
        init_isaac64(seed,rn2); first=rn2(plan.count);
        if (plan.pool[first] == EP_STR_I) break;
    }
    assert(seed < 10000);
    init_isaac64(seed,rn2); (void)rn2(plan.count); (void)rn2(plan.count-1);
    id=d(1,2); next=rn2(1000000);
    fa_script(); fa_choice(1); init_isaac64(seed,rn2);
    assert(forge_affix_commit(hammer,&plan)==ECMD_TIME);
    assert(obj->o_enh_values[0]==id && rn2(1000000)==next);
    /* Every precommit menu exit preserves bytes, currency and RNG. */
    for (id = 0; id < 4; ++id) {
        enhancement_clear(obj); fa_known(obj); before=*obj;
        gold=fa_quantity(GOLD_PIECE); essence=fa_quantity(GENERIC_ESSENCE);
        fa_script();
        if(id>0)fa_choice(fa_index(obj));
        if(id>1)fa_choice(1);
        if(id>2)fa_choice(1);
        fa_choice(0);
        init_isaac64(1717,rn2); next=rn2(1000000); init_isaac64(1717,rn2);
        assert(forge_affix_attempt(hammer,FA_ADD)==ECMD_OK);
        assert(!memcmp(&before,obj,sizeof before) && rn2(1000000)==next);
        assert(fa_quantity(GOLD_PIECE)==gold && fa_quantity(GENERIC_ESSENCE)==essence);
    }
    /* Container exclusions produce one-candidate Add and zero-candidate
     * Reroll; a retained tool family also produces one-candidate Reroll. */
    {
        struct obj *tool=addinv(mksobj(SACK,FALSE,FALSE));
        fa_known(tool);
        assert(enhancement_slot_set(tool,0,EP_PURIFICATION_IV,TRUE,-1));
        assert(forge_affix_plan(hammer,tool,FA_ADD,1,4,0,&plan));
        assert(plan.count==1 && plan.pool[0]==EP_CURSE_IV);
        init_isaac64(1720,rn2);next=rn2(1000000);init_isaac64(1720,rn2);
        assert(forge_affix_commit(hammer,&plan)==ECMD_TIME && rn2(1000000)==next);
        before=*tool;gold=fa_quantity(GOLD_PIECE);essence=fa_quantity(GENERIC_ESSENCE);
        assert(!forge_affix_plan(hammer,tool,FA_REROLL,1,4,0,&plan));
        assert(!memcmp(&before,tool,sizeof before));
        assert(fa_quantity(GOLD_PIECE)==gold && fa_quantity(GENERIC_ESSENCE)==essence);
        useupall(tool);tool=addinv(mksobj(TIN_OPENER,FALSE,FALSE));fa_known(tool);
        assert(enhancement_slot_set(tool,0,EP_PURIFICATION_IV,TRUE,-1));
        assert(forge_affix_plan(hammer,tool,FA_ADD,1,4,0,&plan));
        assert(plan.count==2); /* Erosion IV and Curse IV, no generation weights. */
        assert(enhancement_slot_set(tool,1,EP_CURSE_IV,TRUE,-1));
        assert(forge_affix_plan(hammer,tool,FA_REROLL,1,4,0,&plan));
        assert(plan.count==1 && plan.pool[0]==EP_EROSION_IV);
        fa_script(); fa_choice(1);
        init_isaac64(1721,rn2);next=rn2(1000000);init_isaac64(1721,rn2);
        assert(forge_affix_commit(hammer,&plan)==ECMD_TIME);
        assert(rn2(1000000)==next && tool->o_affixes[1].property==EP_EROSION_IV);
        useupall(tool);
    }
    puts("PASS Step 17 paid Reroll Keep/ESC/selection, uniform distinct replay and all precommit exits");
}

static void
fa_entry_and_replay(struct obj *hammer, struct obj *obj)
{
    struct forge_affix_plan plan;
    struct obj before, *expected;
    const int values[] = { EP_STR_I, EP_STR_II, EP_STR_III, EP_STR_IV };
    int op, seed, pick, next, id, t;
    long gold, essence;
    long turn = svm.moves;
    unsigned oid = obj->o_id;
    fa_script(); fa_choice(0);
    assert(forge_interact(hammer) == ECMD_OK && svm.moves == turn);
    assert(!strcmp(fa_prompt, "Use the forge"));
    assert(!strcmp(fa_screen,
                   "Craft Equipment\nSocket Gems\nManage Affixes\n"
                   "Salvage Equipment\nLeave Forge\n"));
    assert(fa_nids == 5);
    for (op = 0; op < 5; ++op) assert(fa_ids[op] == op + 1);
    /* The ordinary activation entry returns the command dispatcher exactly
     * one ECMD_TIME, with no private turn mutation or second menu commit. */
    hammer->oartifact = ART_MJOLLNIR;
    for (op = FA_ADD; op <= FA_IMPRINT; ++op) {
        enhancement_clear(obj); socket_init(obj); fa_known(obj);
        if (op != FA_ADD) assert(enhancement_slot_set(obj,0,EP_FIRE,TRUE,-1));
        if (op == FA_IMPRINT) {
            assert(enhancement_slot_set(obj,0,0,TRUE,-1));
            svc.context.affix_essence[EP_FIRE] = 1;
        }
        before = *obj; gold = fa_quantity(GOLD_PIECE); essence = fa_quantity(GENERIC_ESSENCE);
        fa_script(); fa_choice(3); fa_choice(op+1); fa_choice(fa_index(obj)); fa_choice(1);
        if (op == FA_ADD) fa_choice(1);
        if (op == FA_IMPRINT) fa_choice(EP_FIRE);
        fa_choice(2); fa_choice(6); fa_choice(5);
        init_isaac64(1733,rn2); next=rn2(1000000); init_isaac64(1733,rn2);
        assert(forge_interact(hammer) == ECMD_OK && svm.moves == turn);
        assert(!memcmp(&before,obj,sizeof before) && rn2(1000000)==next);
        assert(fa_quantity(GOLD_PIECE)==gold && fa_quantity(GENERIC_ESSENCE)==essence);
        assert(svc.context.affix_essence[EP_FIRE] == (op==FA_IMPRINT ? 1 : 0));
        fa_script(); fa_choice(3); fa_choice(op+1); fa_choice(fa_index(obj)); fa_choice(1);
        if (op == FA_ADD) fa_choice(1);
        if (op == FA_IMPRINT) fa_choice(EP_FIRE);
        fa_choice(1); if (op == FA_REROLL) fa_choice(0);
        assert(forge_interact(hammer)==ECMD_TIME && svm.moves==turn);
        assert(fa_next==fa_nchoices);
        memset(svc.context.affix_essence,0,sizeof svc.context.affix_essence);
    }
    hammer->oartifact = 0;
    expected=mksobj(DAGGER,FALSE,FALSE);
    for(seed=1;seed<=160;++seed) {
        enhancement_clear(obj);fa_known(obj);
        assert(forge_affix_plan(hammer,obj,FA_ADD,0,1,0,&plan));
        enhancement_clear(expected);
        init_isaac64(seed,rn2);pick=rn2(plan.count);id=plan.pool[pick];
        assert(enhancement_slot_set(expected,0,id,TRUE,-1));next=rn2(1000000);
        init_isaac64(seed,rn2);assert(forge_affix_commit(hammer,&plan)==ECMD_TIME);
        assert(obj->o_affixes[0].property==id && !obj->o_affixes[0].history);
        assert(!memcmp(obj->o_enh_values,expected->o_enh_values,sizeof obj->o_enh_values));
        assert(rn2(1000000)==next);
    }
    for(t=1;t<=4;++t) {
        enhancement_clear(obj);fa_known(obj);
        assert(enhancement_slot_set(obj,0,values[t-1],TRUE,-1));
        obj->o_affixes[0].history=10;
        assert(forge_affix_plan(hammer,obj,FA_EXTRACT,0,t,0,&plan));
        init_isaac64(1739,rn2);next=rn2(1000000);init_isaac64(1739,rn2);
        assert(forge_affix_commit(hammer,&plan)==ECMD_TIME && rn2(1000000)==next);
        assert(!obj->o_affixes[0].property && obj->o_affixes[0].history==10);
        assert(!obj->o_enh_values[t-1] && svc.context.affix_essence[values[t-1]]==1);
        assert(!forge_affix_plan(hammer,obj,FA_IMPRINT,1,t,values[t-1],&plan));
        assert(forge_affix_plan(hammer,obj,FA_IMPRINT,0,t,values[t-1],&plan));
        enhancement_clear(expected);init_isaac64(1741,rn2);
        assert(enhancement_slot_set(expected,0,values[t-1],TRUE,-1));next=rn2(1000000);
        init_isaac64(1741,rn2);assert(forge_affix_commit(hammer,&plan)==ECMD_TIME);
        assert(rn2(1000000)==next && obj->o_enh_values[t-1]==expected->o_enh_values[t-1]);
        assert(obj->o_affixes[0].history==10 && !svc.context.affix_essence[values[t-1]]);
        assert(enhancement_slot_set(obj,0,0,TRUE,-1));
        assert(forge_affix_plan(hammer,obj,FA_ADD,0,t,0,&plan));
        assert(forge_affix_commit(hammer,&plan)==ECMD_TIME && obj->o_affixes[0].history==10);
    }
    obfree(expected,NULL);
    before=*obj;fa_script();fa_choice(4);fa_choice(fa_index(obj));fa_choice(2);fa_choice(5);
    assert(forge_interact(hammer)==ECMD_OK && !memcmp(&before,obj,sizeof before));
    fa_script();fa_choice(4);fa_choice(fa_index(obj));fa_choice(1);
    assert(forge_interact(hammer)==ECMD_TIME && !forge_find(oid) && svm.moves==turn);
    puts("PASS Step 17 activation dispatch for all five operations, artifact hammer, Add sampling and fresh exact-tier Imprint replay");
}

static void
fa_knowledge(struct obj *hammer, struct obj *obj)
{
    char screens[3][40000];
    int variant, op, next;
    struct obj before;
    struct forge_affix_plan plan;
    for(op=0;op<4;++op) {
        for(variant=0;variant<3;++variant) {
            enhancement_clear(obj); socket_init(obj);
            if(variant) assert(enhancement_slot_set(obj,0,variant==1?EP_FIRE:EP_STR_IV,FALSE,-1));
            if(variant==2) obj->o_sockets[0].property=EP_COLD;
            before=*obj; fa_script();fa_choice(fa_index(obj));
            init_isaac64(91,rn2);next=rn2(1000000);init_isaac64(91,rn2);
            assert(forge_affix_attempt(hammer,op)==ECMD_OK);
            assert(!memcmp(&before,obj,sizeof before) && rn2(1000000)==next);
            assert(!forge_affix_plan(hammer,obj,op,0,1,0,&plan));
            Strcpy(screens[variant],fa_screen);
        }
        assert(!strcmp(screens[0],screens[1])&&!strcmp(screens[1],screens[2]));
    }
    for(variant=0;variant<3;++variant) {
        enhancement_clear(obj);socket_init(obj);
        if(variant)assert(enhancement_slot_set(obj,0,EP_STR_IV,FALSE,-1));
        if(variant==2)obj->o_sockets[0].property=EP_COLD;
        before=*obj;
        fa_script();fa_choice(fa_index(obj));fa_choice(2);
        assert(forge_salvage(hammer)==ECMD_OK);
        assert(!memcmp(&before,obj,sizeof before));
        assert(strstr(fa_screen,"Some properties relevant to Salvage are unidentified."));
        assert(!strstr(fa_screen,"Socketed gems will")&&!strstr(fa_screen,"2d10 gold"));
        Strcpy(screens[variant],fa_screen);
    }
    assert(!strcmp(screens[0],screens[1])&&!strcmp(screens[1],screens[2]));
    for(variant=0;variant<3;++variant) {
        enhancement_clear(obj);socket_init(obj);
        if(variant)assert(enhancement_slot_set(obj,0,EP_STR_IV,FALSE,-1));
        if(variant==2)obj->o_sockets[0].property=EP_COLD;
        fa_script();fa_inspect=obj;assert(doinspect()==ECMD_OK);
        Strcpy(screens[variant],fa_screen);
    }
    assert(!strcmp(screens[0],screens[1])&&!strcmp(screens[1],screens[2]));
    assert(enhancement_slot_set(obj,0,0,TRUE,-1));obj->o_affixes[0].history=10;
    fa_script();fa_inspect=obj;assert(doinspect()==ECMD_OK);
    assert(strstr(fa_screen,"Open Affix Slot, Tier IV; Rerolls: 10 (maximum)"));
    assert(!strstr(fa_screen,"Unused capacity:"));
    fa_inspect=0;
    puts("PASS Step 17 paired hidden-state menus, Salvage previews, unknown sockets and inspect");
}

static void
fa_salvage_and_inherit(struct obj *hammer)
{
    const int ids[]={EP_FIRE,EP_FIRE_II,EP_FIRE_III,EP_PRIMORDIAL};
    struct obj *o,*bag,*inside,*out,*donor;
    struct forge_state state;
    long before, gold, expected;
    int q,t,open,next,i;
    unsigned oid;
    for(q=0;q<3;++q)for(t=1;t<=4;++t)for(open=0;open<2;++open) {
        o=addinv(mksobj(DAGGER,FALSE,FALSE));fa_known(o);
        assert(enhancement_slot_set(o,0,ids[t-1],TRUE,-1));o->o_enh_quality=(uint8)q;
        if(open)assert(enhancement_slot_set(o,0,0,TRUE,-1));
        o->o_affixes[0].history=10;
        expected=q+(t==4?5:t);oid=o->o_id;
        before=fa_quantity(GENERIC_ESSENCE);gold=fa_quantity(GOLD_PIECE);
        assert(forge_salvage_yield(o)==expected);
        assert(forge_salvage_commit(hammer,oid)==ECMD_TIME && !forge_find(oid));
        assert(fa_quantity(GENERIC_ESSENCE)==before+expected && fa_quantity(GOLD_PIECE)==gold);
    }
    for(q=1;q<=2;++q) {
        o=mksobj(DAGGER,FALSE,FALSE);o->quan=10;o->o_enh_quality=(uint8)q;
        o=addinv(o);fa_known(o);oid=o->o_id;
        before=fa_quantity(GENERIC_ESSENCE);gold=fa_quantity(GOLD_PIECE);
        assert(forge_salvage_commit(hammer,oid)==ECMD_TIME && !forge_find(oid));
        assert(fa_quantity(GENERIC_ESSENCE)==before+10*q && fa_quantity(GOLD_PIECE)==gold);
    }
    o=addinv(mksobj(DAGGER,FALSE,FALSE));fa_known(o);
    assert(enhancement_slot_set(o,0,EP_STR_IV,TRUE,-1));
    assert(enhancement_slot_set(o,1,EP_DEX_IV,TRUE,-1));
    assert(enhancement_slot_set(o,1,0,TRUE,-1));o->o_enh_quality=OQ_EXCEPTIONAL;
    o->o_sockets[0].property=EP_FIRE;o->o_sockets[0].known=1;
    before=fa_quantity(GENERIC_ESSENCE);oid=o->o_id;
    assert(forge_salvage_yield(o)==12);
    assert(forge_salvage_commit(hammer,oid)==ECMD_TIME && !forge_find(oid));
    assert(fa_quantity(GENERIC_ESSENCE)==before+12);
    o=mksobj(DAGGER,FALSE,FALSE);o->quan=10;o=addinv(o);fa_known(o);oid=o->o_id;
    gold=fa_quantity(GOLD_PIECE);before=fa_quantity(GENERIC_ESSENCE);
    init_isaac64(901,rn2);expected=0;for(i=0;i<10;++i)expected+=d(2,10);
    /* Constructor ID draws follow the gold rolls, so compare replay reward. */
    init_isaac64(901,rn2);assert(forge_salvage_commit(hammer,oid)==ECMD_TIME);
    assert(!forge_find(oid) && fa_quantity(GOLD_PIECE)==gold+expected);
    assert(fa_quantity(GENERIC_ESSENCE)==before);
    bag=addinv(mksobj(SACK,FALSE,FALSE));fa_known(bag);bag->cknown=0;
    assert(!forge_affix_target(bag,hammer,TRUE));
    inside=mksobj(GENERIC_ESSENCE,FALSE,FALSE);inside->quan=500;
    before=forge_essence_count();add_to_container(bag,inside);
    assert(!forge_affix_target(bag,hammer,TRUE)&&forge_essence_count()==before);
    bag->cknown=1;assert(!forge_affix_target(bag,hammer,TRUE));
    obj_extract_self(inside);obfree(inside,NULL);
    assert(forge_affix_target(bag,hammer,TRUE));useupall(bag);
    o=addinv(mksobj(ARROW,FALSE,FALSE));fa_known(o);
    assert(!forge_affix_target(o,hammer,TRUE));useupall(o);
    o=addinv(mksobj(GENERIC_ESSENCE,FALSE,FALSE));
    assert(!forge_affix_target(o,hammer,TRUE));
    o=addinv(mksobj(DAGGER,FALSE,FALSE));fa_known(o);
    o->known=o->dknown=0;
    assert(forge_affix_target(o,hammer,TRUE) && !forge_affix_target(o,hammer,FALSE));
    fa_known(o);
    o->oartifact=ART_EXCALIBUR;assert(!forge_affix_target(o,hammer,TRUE));o->oartifact=0;
    o->owornmask=W_WEP;assert(!forge_affix_target(o,hammer,TRUE));o->owornmask=0;
    o->in_use=1;assert(!forge_affix_target(o,hammer,TRUE));o->in_use=0;
    o->unpaid=1;assert(!forge_affix_target(o,hammer,TRUE));o->unpaid=0;
    assert(enhancement_slot_set(o,0,EP_FIRE,TRUE,-1));o->quan=2;
    assert(!forge_affix_target(o,hammer,TRUE));o->quan=1;useupall(o);
    /* Rank retained properties exactly as historical recipes did, then assign
     * in donor/slot order, omitting Open and resetting histories. */
    donor=mksobj(LONG_SWORD,FALSE,FALSE);out=forge_output(KATANA);
    assert(enhancement_slot_set(donor,0,EP_DEX_IV,FALSE,6));
    assert(enhancement_slot_set(donor,1,EP_STR_IV,FALSE,5));
    donor->o_affixes[0].history=10;donor->o_affixes[1].history=7;
    memset(&state,0,sizeof state);forge_gather(&state,donor);
    init_isaac64(909,rn2);next=rn2(1000000);init_isaac64(909,rn2);
    forge_inherit(out,&state);assert(rn2(1000000)==next);
    assert(out->o_affixes[0].property==EP_DEX_IV && out->o_affixes[1].property==EP_STR_IV);
    assert(!out->o_affixes[0].history&&!out->o_affixes[1].history);
    assert(out->o_enh_values[7]==6 && out->o_enh_values[3]==5);
    obfree(out,NULL);assert(enhancement_slot_set(donor,0,0,TRUE,-1));
    memset(&state,0,sizeof state);forge_gather(&state,donor);
    out=forge_output(KATANA);forge_inherit(out,&state);
    assert(out->o_affixes[0].property==EP_STR_IV && !out->o_affixes[1].tier);
    obfree(out,NULL);obfree(donor,NULL);
    memset(svc.context.affix_essence,0,sizeof svc.context.affix_essence);
    svc.context.affix_essence[EP_SHOCK]=2;svc.context.affix_essence[EP_FIRE]=3;
    svc.context.affix_essence[EP_PRIMORDIAL]=4;
    svc.context.affix_essence[EP_TRUEFLIGHT]=1;
    fa_script();forge_ledger();
    assert(strstr(fa_screen,"Smoldering I Essence x3")<strstr(fa_screen,"Sparking I Essence x2"));
    assert(strstr(fa_screen,"Primordial IV Essence x4") && !strstr(fa_screen,"Essence x0"));
    assert(strstr(fa_screen,"throwing the item or firing its ammunition"));
    memset(svc.context.affix_essence,0,sizeof svc.context.affix_essence);
    puts("PASS Step 17 all Salvage contributions, stack gold replay, container/payment scope, exclusions, donor order and ledger display");
}
/* Direct production transactions plus the menu and deterministic RNG cases. */
void
step17_forge_tests(void)
{
    struct obj *hammer, *obj, *gold, *essence;
    struct forge_affix_plan p;
    long g, e;
    int t, h;
    hammer = addinv(mksobj(WAR_HAMMER, FALSE, FALSE));
    obj = addinv(mksobj(DAGGER, FALSE, FALSE));
    gold = mksobj(GOLD_PIECE, FALSE, FALSE); gold->quan = 2000000;
    gold->known = gold->dknown = gold->bknown = 1; (void) addinv(gold);
    essence = mksobj(GENERIC_ESSENCE, FALSE, FALSE); essence->quan = 10000;
    (void) addinv(essence);
    obj->known = obj->dknown = obj->bknown = 1;
    objects[DAGGER].oc_name_known = objects[GOLD_PIECE].oc_name_known = 1;
    enhancement_identify(obj);
    levl[u.ux][u.uy].typ = FORGE;
    windowprocs.win_create_nhwindow=fa_window; windowprocs.win_destroy_nhwindow=fa_destroy;
    windowprocs.win_display_nhwindow=fa_display;windowprocs.win_putstr=fa_text;
    windowprocs.win_start_menu=fa_start;windowprocs.win_end_menu=fa_end;
    windowprocs.win_add_menu=fa_add;windowprocs.win_select_menu=fa_select;
    for (t = 1; t <= 4; ++t) for (h = 0; h <= 10; ++h) {
        static const int bg[] = {0,100,200,400,800}, be[] = {0,2,4,7,10};
        forge_affix_cost(FA_REROLL,t,TRUE,h,&g,&e);
        assert(g == bg[t]*(2+h)/2 && e == (be[t]*(4+h)+3)/4);
        forge_affix_cost(FA_EXTRACT,t,TRUE,h,&g,&e);
        assert(g == bg[t] && e == be[t]);
        forge_affix_cost(FA_IMPRINT,t,TRUE,h,&g,&e);
        assert(g == bg[t] && e == be[t]);
        forge_affix_cost(FA_ADD,t,TRUE,h,&g,&e);
        assert(g == bg[t]*(2+h)/2 && e == (((t==1?1:t==2?3:t==3?5:8)*(4+h))+3)/4);
    }
    assert(forge_affix_plan(hammer,obj,FA_ADD,0,3,0,&p));
    assert(p.gold == 1000 && p.essence == 6);
    assert(forge_affix_commit(hammer,&p) == ECMD_TIME);
    assert(obj->o_affixes[0].tier == 3 && !obj->o_affixes[0].history);
    t = obj->o_affixes[0].property;
    assert(forge_affix_plan(hammer,obj,FA_EXTRACT,0,3,0,&p));
    assert(forge_affix_commit(hammer,&p) == ECMD_TIME);
    assert(!obj->o_affixes[0].property && obj->o_affixes[0].history == 1);
    assert(svc.context.affix_essence[t] == 1);
    assert(forge_affix_plan(hammer,obj,FA_IMPRINT,0,3,t,&p));
    assert(forge_affix_commit(hammer,&p) == ECMD_TIME);
    assert(obj->o_affixes[0].property == t && obj->o_affixes[0].history == 1);
    assert(!svc.context.affix_essence[t]);
    assert(forge_salvage_yield(obj) == 3);
    fa_transactions(hammer,obj);
    fa_knowledge(hammer,obj);
    fa_entry_and_replay(hammer,obj);
    fa_salvage_and_inherit(hammer);
    while (gi.invent) useupall(gi.invent);
    levl[u.ux][u.uy].typ = ROOM;
    puts("PASS Step 17 exact cost tables, committed Add/Extract/Imprint/Salvage and ledger");
}
