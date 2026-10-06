/* Included by apply.c only in the native STEP15_TEST executable: tests the
 * private transaction engine without exposing a production test API. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>

struct forge_test_choice { const char *prompt; int id; long count; };
struct forge_test_answer { const char *prompt; char answer; };
static struct forge_test_choice forge_choices[160];
static struct forge_test_answer forge_answers[40];
static int forge_choice_count, forge_choice_next, forge_menu_ids[100], forge_menu_n;
static int forge_answer_count, forge_answer_next;
static char forge_answer_default;
static char forge_yn_prompts[40][BUFSZ];
static int forge_yn_count;
static boolean forge_socket_shortcut_seen;
static char forge_menu_letters[100];
static int forge_last_recipe_ids[100], forge_last_recipe_n;
static char forge_last_recipe_letters[100];
static char forge_prompt[BUFSZ], forge_screen[200000];
static winid forge_test_window(int type) { (void) type; return 1; }
static void forge_test_destroy(winid win) { (void) win; }
static void forge_test_display(winid win, boolean block) { (void) win; (void) block; }
static void forge_test_putstr(winid win, int attr, const char *s) {
    (void) win; (void) attr;
    assert(strlen(forge_screen) + strlen(s) + 2 < sizeof forge_screen);
    strcat(forge_screen, s); strcat(forge_screen, "\n");
}
static void forge_test_start(winid win, unsigned long behavior) {
    (void) win; (void) behavior; forge_menu_n = 0;
}
static void forge_test_end(winid win, const char *s) {
    int i;
    (void) win; Strcpy(forge_prompt, s);
    if (!strcmp(s, "Weapons") || !strcmp(s, "Armor")
        || !strcmp(s, "Tools") || !strcmp(s, "Other")) {
        forge_last_recipe_n = forge_menu_n;
        for (i = 0; i < forge_menu_n; ++i) {
            forge_last_recipe_ids[i] = forge_menu_ids[i];
            forge_last_recipe_letters[i] = forge_menu_letters[i];
        }
    }
}
static void forge_test_add(winid win, const glyph_info *glyph, const ANY_P *id,
                           char letter, char group, int attr, int color,
                           const char *s, unsigned int flags) {
    (void) glyph; (void) letter; (void) group; (void) color; (void) flags;
    assert(forge_menu_n < SIZE(forge_menu_ids));
    forge_menu_ids[forge_menu_n++] = id->a_int;
    forge_menu_letters[forge_menu_n - 1] = letter;
    forge_test_putstr(win, attr, s);
    if (!strcmp(s, "Socket Gems")) {
        assert(letter == 's');
        forge_socket_shortcut_seen = TRUE;
    }
    if (!strcmp(s, "Socket gemstone")) {
        assert(letter == 'y');
        assert(strcmp(s, "Affix gemstone"));
    }
    if (!strcmp(s, "Tools") || !strcmp(s, "Other")) assert(!id->a_int);
}
static int forge_test_select(winid win, int how, MENU_ITEM_P **picked) {
    struct forge_test_choice *c;
    int i;
    (void) win; assert(how == PICK_ONE);
    *picked = NULL;
    if (forge_choice_next == forge_choice_count) return -1;
    c = &forge_choices[forge_choice_next++];
    assert(strstr(forge_prompt, c->prompt));
    if (!c->id) return -1;
    for (i = 0; i < forge_menu_n && forge_menu_ids[i] != c->id; ++i) ;
    assert(i < forge_menu_n);
    *picked = (menu_item *) alloc(sizeof **picked);
    (*picked)->item.a_int = c->id;
    (*picked)->count = c->count;
    return 1;
}
static char forge_test_yn_function(const char *query, const char *responses,
                                  char def) {
    struct forge_test_answer *a;
    assert(responses && !strcmp(responses, "yn"));
    assert(forge_yn_count < SIZE(forge_yn_prompts));
    Strcpy(forge_yn_prompts[forge_yn_count++], query);
    if (forge_answer_next < forge_answer_count) {
        a = &forge_answers[forge_answer_next++];
        assert(!strcmp(query, a->prompt));
        return a->answer ? a->answer : def;
    }
    return forge_answer_default ? forge_answer_default : def;
}
void step15b_window_setup(void) {
    windowprocs.win_create_nhwindow = forge_test_window;
    windowprocs.win_destroy_nhwindow = forge_test_destroy;
    windowprocs.win_display_nhwindow = forge_test_display;
    windowprocs.win_putstr = forge_test_putstr;
    windowprocs.win_start_menu = forge_test_start;
    windowprocs.win_end_menu = forge_test_end;
    windowprocs.win_add_menu = forge_test_add;
    windowprocs.win_select_menu = forge_test_select;
    windowprocs.win_yn_function = forge_test_yn_function;
}
static void forge_test_script(void) {
    forge_choice_count = forge_choice_next = 0;
    forge_answer_count = forge_answer_next = 0;
    forge_answer_default = '\0'; forge_yn_count = 0;
    forge_socket_shortcut_seen = FALSE;
    forge_last_recipe_n = 0;
    forge_screen[0] = '\0';
}
static void forge_test_choose(const char *prompt, int id, long count) {
    struct forge_test_choice *c;
    assert(forge_choice_count < SIZE(forge_choices));
    c = &forge_choices[forge_choice_count++];
    c->prompt = prompt; c->id = id; c->count = count;
}
static void forge_test_answer(const char *prompt, char answer) {
    struct forge_test_answer *a;
    assert(forge_answer_count < SIZE(forge_answers));
    a = &forge_answers[forge_answer_count++];
    a->prompt = prompt; a->answer = answer;
}
static void forge_test_answer_default(char answer) {
    forge_answer_default = answer;
}

static struct obj *forge_test_item(int, long);
static void forge_test_clear(void);

static boolean
forge_test_has_primary_erosion(struct obj *obj)
{
    return erosion_matters(obj);
}

static boolean
forge_test_output_has_primary_erosion(struct obj *obj)
{
    return forge_test_has_primary_erosion(obj)
        && (is_flammable(obj) || is_rustprone(obj) || is_crackable(obj));
}

/* Compare gameplay state explicitly, without depending on struct padding.
 * Pointer topology is included: free interactions may not reorder stacks. */
static void
forge_test_same_object(const struct obj *before, const struct obj *after)
{
#define SAME(field) assert(before->field == after->field)
    SAME(o_id); SAME(nobj); SAME(nexthere); SAME(otyp); SAME(oclass);
    SAME(where); SAME(invlet); SAME(quan); SAME(owt); SAME(owornmask);
    SAME(known); SAME(dknown); SAME(bknown); SAME(rknown); SAME(cknown);
    SAME(lknown); SAME(cursed); SAME(blessed); SAME(spe); SAME(unpaid);
    SAME(in_use); SAME(oeroded); SAME(oeroded2); SAME(oerodeproof);
    SAME(greased); SAME(opoisoned); SAME(oartifact); SAME(oextra);
    SAME(o_socket_capacity);
    assert(!memcmp(before->o_sockets,after->o_sockets,sizeof before->o_sockets));
    assert(!memcmp(before->o_enh_values,after->o_enh_values,sizeof before->o_enh_values));
    SAME(o_enh_props); SAME(o_enh_known); SAME(o_enh_quality); SAME(o_enh_flags);
    SAME(obranch_props); SAME(obranch_material); SAME(obranch_size);
    SAME(tknown); SAME(cobj); SAME(age); SAME(timed); SAME(recharged);
    SAME(bypass); SAME(no_charge); SAME(lamplit); SAME(globby); SAME(oeaten);
    SAME(how_lost); SAME(nomerge); SAME(ox); SAME(oy); SAME(pickup_prev);
    SAME(ghostly); SAME(olocked); SAME(obroken);
#undef SAME
}

/* A small fixture-side ledger: six stacks, independently declared eligibility,
 * three recipe shapes, and every subset of visible/relinquishable candidates.
 * Expected counts and survivors never call production eligibility/allocation
 * helpers. POTION/FOOD/TOOL recipes exercise the generic engine as well. */
static void
forge_test_ledger(void)
{
    int shape, mask, i, slot, cases = 0, successes = 0;
    const int inputs[3][2] = {
        { LONG_SWORD, LONG_SWORD }, { LONG_SWORD, DAGGER },
        { POT_HEALING, FOOD_RATION }
    };
    struct forge_recipe r;
    struct forge_allocation a[6];
    struct obj *obj, snapshots[6];
    unsigned ids[6];
    long qty[6], available[2], left[2], taken[6];
    boolean usable[6], expected;

    for (shape = 0; shape < 3; ++shape)
        for (mask = 0; mask < 64; ++mask) {
            forge_test_clear();
            forge_test_script();
            memset(a, 0, sizeof a);
            r.output = shape == 2 ? TIN_OPENER : KATANA;
            r.need[0].otyp = inputs[shape][0]; r.need[0].quantity = 3;
            r.need[1].otyp = inputs[shape][1]; r.need[1].quantity = 2;
            available[0] = available[1] = 0;
            for (i = 0; i < 6; ++i) {
                qty[i] = i % 3 + 1;
                obj = forge_output(inputs[shape][i % 2]);
                makeknown(obj->otyp);
                obj->nomerge = 1;
                obj->quan = qty[i]; obj->owt = weight(obj);
                obj = addinv(obj);
                ids[i] = a[i].oid = obj->o_id;
                usable[i] = (mask & (1 << i)) != 0;
                if (!usable[i]) {
                    if (i % 2) obj->dknown = 0;
                    else obj->in_use = 1;
                }
                for (slot = 0; slot < 2; ++slot)
                    if (usable[i] && inputs[shape][i % 2] == inputs[shape][slot])
                        available[slot] += qty[i];
                taken[i] = 0;
            }
            for (i = 0; i < 6; ++i) snapshots[i] = *forge_find(ids[i]);
            expected = shape == 0 ? available[0] >= 5
                                   : available[0] >= 3 && available[1] >= 2;
            assert(forge_available(&r, NULL) == expected);
            assert(forge_quantity(inputs[shape][0], NULL, NULL) == available[0]);
            for (i = 0; i < 6; ++i)
                forge_test_same_object(&snapshots[i], forge_find(ids[i]));
            if (expected) {
                left[0] = 3; left[1] = 2;
                for (slot = 0; slot < 2; ++slot)
                    for (i = 0; i < 6 && left[slot]; ++i)
                        if (usable[i] && inputs[shape][i % 2] == inputs[shape][slot]) {
                            long contribution = qty[i] - taken[i];
                            if (contribution > left[slot]) contribution = left[slot];
                            if (!contribution) continue;
                            forge_test_choose("Choose", i + 1, contribution);
                            taken[i] += contribution;
                            left[slot] -= contribution;
                        }
                assert(!left[0] && !left[1]);
                assert(forge_allocate(&r, NULL, a, 6));
                for (i = 0; i < 6; ++i) {
                    assert(a[i].quantity[0] + a[i].quantity[1] == taken[i]);
                    forge_test_same_object(&snapshots[i], forge_find(ids[i]));
                }
                assert(forge_commit(&r, a, 6) == ECMD_TIME);
                for (i = 0; i < 6; ++i) {
                    obj = forge_find(ids[i]);
                    assert(taken[i] == qty[i] ? !obj : obj && obj->quan == qty[i] - taken[i]);
                }
                for (i = 0, obj = gi.invent; obj; obj = obj->nobj)
                    if (obj->otyp == r.output) i += (int) obj->quan;
                assert(i == 1);
                ++successes;
            }
            ++cases;
        }
    forge_test_clear();
    printf("PASS Step 15B independent ledger cases=%d successful=%d\n", cases, successes);
}

static void
forge_test_many_stacks_and_counts(void)
{
    struct forge_recipe r = { KATANA, { { DAGGER, 20 }, { DAGGER, 20 } } };
    struct forge_allocation a[40] = { 0 };
    struct obj *obj, snapshots[40], hammer_snapshot;
    struct obj *hammer;
    int i, tail;
    enum enhancement_context old = enhancement_context_set(ENH_CONTEXT_SHOP);
    forge_test_clear();
    hammer = forge_test_item(WAR_HAMMER, 1);
    for (i = 0; i < 40; ++i) {
        obj = forge_output(DAGGER); obj->nomerge = 1;
        makeknown(DAGGER); obj = addinv(obj);
        a[i].oid = obj->o_id;
    }
    hammer_snapshot = *hammer;
    for (i = 0; i < 40; ++i) snapshots[i] = *forge_find(a[i].oid);
    forge_test_script();
    /* Native -1 means no count; zero and all other negatives are invalid. */
    forge_test_choose("Choose", 1, 0);
    forge_test_choose("Choose", 1, -2);
    forge_test_choose("Choose", 1, LONG_MAX);
    for (i = 0; i < 39; ++i) forge_test_choose("Choose", i + 1, -1);
    init_isaac64(151501UL, rn2); tail = rn2(1000000);
    init_isaac64(151501UL, rn2);
    assert(!forge_allocate(&r, hammer, a, 40)); /* cancel final contribution */
    assert(rn2(1000000) == tail);
    assert(forge_choice_next == forge_choice_count);
    for (i = 0; i < 40; ++i)
        forge_test_same_object(&snapshots[i], forge_find(a[i].oid));
    forge_test_same_object(&hammer_snapshot, hammer);
    assert(enhancement_context_set(ENH_CONTEXT_SHOP) == ENH_CONTEXT_SHOP);
    memset(a, 0, sizeof a);
    for (i = 0; i < 40; ++i) a[i].oid = snapshots[i].o_id;
    forge_test_script();
    for (i = 0; i < 40; ++i) forge_test_choose("Choose", i + 1, 1);
    assert(forge_allocate(&r, hammer, a, 40));
    assert(forge_commit(&r, a, 40) == ECMD_TIME);
    assert(inv_cnt(FALSE) == 2);
    /* Successful insertion may relink inventory, but not change the hammer. */
    hammer_snapshot.nobj = hammer->nobj;
    forge_test_same_object(&hammer_snapshot, hammer);
    (void) enhancement_context_set(old);
    forge_test_clear();
    puts("PASS Step 15B 40-stack allocation, invalid/native counts, partial cancellation, semantic state and RNG tail");
}

static void
forge_test_capacity_ledger(void)
{
    struct forge_recipe r = { DAGGER, { { DAGGER, 1 }, { STILETTO, 1 } } };
    struct forge_allocation a[2];
    struct obj *obj, *first, *second, before[2];
    int q0, q1, merge, full, expected, cases = 0, total;
    for (q0 = 1; q0 <= 2; ++q0)
        for (q1 = 1; q1 <= 2; ++q1)
            for (merge = 0; merge < 2; ++merge)
                for (full = 0; full < 2; ++full) {
                    forge_test_clear(); forge_test_script();
                    memset(a, 0, sizeof a);
                    first = forge_test_item(DAGGER, q0);
                    first->nomerge = !merge;
                    second = forge_test_item(STILETTO, q1);
                    a[0].oid = first->o_id; a[0].quantity[0] = 1;
                    a[1].oid = second->o_id; a[1].quantity[1] = 1;
                    if (full)
                        while (inv_cnt(FALSE) < 52) forge_test_item(LONG_SWORD, 1);
                    before[0] = *first; before[1] = *second;
                    /* Explicit pack-limit contract, released slots, or a
                     * compatible surviving dagger. No production preflight
                     * or merge predicate is used to calculate this result. */
                    expected = !full || q0 == 1 || q1 == 1 || merge;
                    assert(forge_commit(&r, a, 2) == (expected ? ECMD_TIME : ECMD_OK));
                    if (!expected) {
                        forge_test_same_object(&before[0], first);
                        forge_test_same_object(&before[1], second);
                    } else {
                        total = 0;
                        for (obj = gi.invent; obj; obj = obj->nobj)
                            if (obj->otyp == DAGGER) total += (int) obj->quan;
                        assert(total == q0); /* one consumed, one produced */
                        obj = forge_find(a[1].oid);
                        assert(q1 == 1 ? !obj : obj && obj->quan == 1);
                        if (q0 == 1) assert(!forge_find(a[0].oid));
                        assert(inv_cnt(FALSE) <= 52);
                    }
                    ++cases;
                }
    forge_test_clear();
    printf("PASS Step 15B independent capacity ledger cases=%d (consumed and surviving merge targets)\n", cases);
}

static struct obj *
forge_test_item(int typ, long quantity)
{
    struct obj *obj = forge_output(typ);
    makeknown(typ);
    obj->quan = quantity;
    obj->owt = weight(obj);
    return addinv(obj);
}

static void
forge_test_clear(void)
{
    while (gi.invent)
        useupall(gi.invent);
}

static void
forge_test_eligibility(void)
{
    struct obj *obj, *other, saved;
    long counts[FORGE_REASON_COUNT] = { 0 };
    char unknown_screen[sizeof forge_screen];
    int known;

    forge_test_clear();
    obj = forge_test_item(LONG_SWORD, 2);
    assert(forge_available(&forge_recipes[0], NULL));
    obj->quan = 1; assert(!forge_available(&forge_recipes[0], NULL));
    other = forge_test_item(LONG_SWORD, 1);
    assert(forge_available(&forge_recipes[0], NULL)); /* distinct nonmergeable swords */
    assert(forge_reason(obj, obj) == FORGE_HAMMER);
    obj->owornmask = W_WEP;
    saved = *obj;
    assert(forge_reason(obj, NULL) == FORGE_EQUIPPED);
    assert(!memcmp(&saved, obj, sizeof saved));
    obj->owornmask = 0;
    obj->oartifact = ART_EXCALIBUR;
    assert(forge_reason(obj, NULL) == FORGE_PROTECTED); obj->oartifact = 0;
    obj->unpaid = 1; assert(forge_reason(obj, NULL) == FORGE_UNPAID); obj->unpaid = 0;
    obj->in_use = 1; assert(forge_reason(obj, NULL) == FORGE_ATTACHED); obj->in_use = 0;
    obj->dknown = 0; saved = *obj;
    assert(forge_quantity(LONG_SWORD, NULL, counts) == 1);
    assert(!counts[FORGE_UNKNOWN]);
    forge_test_script(); forge_inspect(&forge_recipes[0], NULL);
    Strcpy(unknown_screen, forge_screen);
    assert(!memcmp(&saved,obj,sizeof saved));
    freeinv(obj);
    forge_test_script(); forge_inspect(&forge_recipes[0], NULL);
    assert(!strcmp(unknown_screen, forge_screen)); /* invisible vs absent identical */
    obj = addinv(obj); obj->dknown = 1;
    known = objects[LONG_SWORD].oc_name_known;
    objects[LONG_SWORD].oc_name_known = 0;
    assert(!forge_available(&forge_recipes[0], NULL));
    objects[LONG_SWORD].oc_name_known = known;
    assert(enhancement_set(obj, OEP_FIRE, OQ_EXCEPTIONAL, FALSE));
    obj->spe=-5;obj->cursed=1;obj->oeroded=2;obj->greased=1;obj->opoisoned=1;
    obj->obranch_material=GOLD;
    obj=oname(obj,"chosen sword",ONAME_NO_FLAGS);
    saved=*obj;
    assert(forge_available(&forge_recipes[0], NULL));
    (void) forge_inventory_name(obj);
    assert(!memcmp(&saved,obj,sizeof saved));
    freeinv(obj);place_object(obj,u.ux,u.uy);
    assert(!forge_available(&forge_recipes[0],NULL));
    obj_extract_self(obj);obfree(obj,NULL);
    forge_test_clear();
    obj=forge_test_item(SACK,1);other=forge_output(DAGGER);
    add_to_container(obj,other);
    assert(forge_reason(obj,NULL)==FORGE_CONTENTS);
    assert(!forge_quantity(DAGGER,NULL,NULL));
    obj=forge_test_item(LOADSTONE,1);obj->cursed=1;saved=*obj;
    assert(forge_reason(obj,NULL)==FORGE_ATTACHED);
    assert(!memcmp(&saved,obj,sizeof saved));
    obj->cursed=0;
    obj=forge_test_item(LEASH,1);obj->leashmon=123;
    assert(forge_reason(obj,NULL)==FORGE_ATTACHED);obj->leashmon=0;
    obj=forge_test_item(WAR_HAMMER,1);
    assert(forge_reason(obj,obj)==FORGE_HAMMER);
    assert(forge_reason(obj,NULL)==FORGE_USABLE);
    forge_test_clear();
    puts("PASS Step 15B eligibility, mutable-state independence, unknown/absent diagnostic equivalence");
}

/* Inventory totals can exceed a native long even though individual stacks
 * fit. Availability must remain true and diagnostic totals must not wrap. */
static void
forge_test_quantity_limits(void)
{
    struct obj *a, *b;
    long counts[FORGE_REASON_COUNT] = { 0 };
    forge_test_clear();
    a = forge_test_item(LONG_SWORD, 1);
    b = forge_test_item(LONG_SWORD, 1);
    a->quan = LONG_MAX;
    b->quan = 1;
    assert(forge_available(&forge_recipes[0], NULL));
    assert(forge_quantity(LONG_SWORD, NULL, counts) == LONG_MAX);
    assert(counts[FORGE_USABLE] == LONG_MAX);
    a->quan = 1;
    forge_test_clear();
    puts("PASS Step 15B native-long quantity aggregation boundary");
}

static void
forge_test_transactions(void)
{
    struct obj *obj,*other, saved, snapshots[3];
    struct forge_allocation a[3] = {0};
    struct forge_recipe r = { KATANA, { { LONG_SWORD, 3 }, { DAGGER, 2 } } };
    unsigned id;
    int i;
    enum enhancement_context previous;

    forge_test_clear();
    obj=forge_test_item(LONG_SWORD,3);id=obj->o_id;
    a[0].oid=id;a[0].quantity[0]=a[0].quantity[1]=1;
    saved=*obj;
    forge_fail_construction=TRUE;
    assert(forge_commit(&forge_recipes[0],a,1)==ECMD_OK);
    forge_fail_construction=FALSE;
    assert(!memcmp(&saved,obj,sizeof saved));
    a[1]=a[0];assert(forge_commit(&forge_recipes[0],a,2)==ECMD_OK);
    a[0].quantity[1]=3;assert(forge_commit(&forge_recipes[0],a,1)==ECMD_OK);
    a[0].quantity[1]=0;assert(forge_commit(&forge_recipes[0],a,1)==ECMD_OK);
    a[0].quantity[1]=1;a[0].oid=0;
    assert(forge_commit(&forge_recipes[0],a,1)==ECMD_OK);a[0].oid=id;
    for(i=inv_cnt(FALSE);i<invlet_basic;++i) {
        other=forge_output(DAGGER);other->nomerge=1;addinv(other);
    }
    saved=*obj;
    assert(forge_commit(&forge_recipes[0],a,1)==ECMD_OK);
    assert(!memcmp(&saved,obj,sizeof saved));
    assert(inv_cnt(FALSE)==invlet_basic);
    /* A synthetic normal recipe exercises native output merging at capacity. */
    r=forge_recipes[0];r.output=DAGGER;
    for(other=gi.invent;other->otyp!=DAGGER;other=other->nobj) ;
    other->nomerge=0;
    assert(forge_commit(&r,a,1)==ECMD_TIME);
    assert(other->quan==2 && inv_cnt(FALSE)==invlet_basic);
    obj->quan=2;obj->owt=weight(obj);
    assert(forge_commit(&forge_recipes[0],a,1)==ECMD_TIME);
    assert(!forge_find(id) && inv_cnt(FALSE)==invlet_basic);
    forge_test_clear();
    r.output=KATANA;r.need[0].otyp=LONG_SWORD;r.need[0].quantity=3;
    r.need[1].otyp=DAGGER;r.need[1].quantity=2;
    memset(a,0,sizeof a);
    obj=forge_test_item(LONG_SWORD,2);a[0].oid=obj->o_id;a[0].quantity[0]=2;
    obj=forge_test_item(LONG_SWORD,2);a[1].oid=obj->o_id;a[1].quantity[0]=1;
    other=forge_test_item(DAGGER,3);a[2].oid=other->o_id;a[2].quantity[1]=2;
    assert(forge_available(&r,NULL));
    for(i=0;i<3;++i) {
        snapshots[i]=*forge_find(a[i].oid);
        a[i].quantity[0]=a[i].quantity[1]=0;
    }
    forge_test_script();
    forge_test_choose("Choose 3",1,1);
    forge_test_choose("Choose 2",1,1);
    forge_test_choose("Choose 1",2,1);
    forge_test_choose("Choose 2",3,2);
    forge_test_choose("Confirm crafting",2,-1);
    assert(forge_allocate(&r,NULL,a,3));
    assert(!forge_confirm(&r,a,3));
    for(i=0;i<3;++i) assert(!memcmp(&snapshots[i],forge_find(a[i].oid),sizeof snapshots[i]));
    assert(a[0].quantity[0]==2 && a[1].quantity[0]==1 && a[2].quantity[1]==2);
    assert(forge_commit(&r,a,3)==ECMD_TIME);
    assert(!forge_find(a[0].oid));assert(obj->quan==1 && other->quan==1);
    forge_test_clear();
    previous=enhancement_context_set(ENH_CONTEXT_FLOOR);
    for(i=0;i<2000;++i) {
        enum enhancement_context outer = (enum enhancement_context) (i % 4);
        (void) enhancement_context_set(outer);
        obj=forge_output(i%2?KATANA:PLATE_MAIL);
        assert(obj->quan==1 && !obj->oartifact && !obj->oextra);
        assert(!obj->o_enh_quality && !obj->o_enh_props && !obj->o_enh_known && !obj->o_enh_flags);
        assert(!obj->spe && !obj->cursed && !obj->blessed && !obj->obranch_material);
        assert(!obj->oeroded && !obj->oeroded2 && !obj->greased && !obj->opoisoned);
        assert(enhancement_context_set(outer)==outer);
        obfree(obj,NULL);
    }
    (void)enhancement_context_set(previous);
    puts("PASS Step 15B preflight faults, pack rejection/merge/released slot, multi-stack lifecycle, 2000 plain outputs");
}

static void
forge_test_navigation(void)
{
    struct obj *obj, saved;
    int i, j;
    long moves=svm.moves;
    forge_test_clear();
    obj=forge_test_item(LONG_SWORD,3);saved=*obj;
    for(i=0;i<6;++i) {
        forge_test_script();
        if(i>0) forge_test_choose("Use the forge",1,-1);
        if(i>1) forge_test_choose("Choose a category",1,-1);
        if(i>2) forge_test_choose("Weapons",1,-1);
        if(i>3) forge_test_choose("Choose 1",1,1);
        if(i>4) {
            forge_test_choose("Choose 1",1,1);
            forge_test_choose("Confirm crafting",2,-1);
        }
        assert(forge_menu(NULL)==ECMD_OK);
        assert(forge_choice_next==forge_choice_count);
        assert(!memcmp(&saved,obj,sizeof saved));
        assert(svm.moves==moves);
        if(i>1) {
            assert(strstr(forge_screen,"Tools (0)\nOther\n"));
            assert(strstr(forge_screen,"Weapons (1)"));
            assert(strstr(forge_screen,"katana - 2 long swords"));
            assert(!strstr(forge_screen,"[Available]")
                   && !strstr(forge_screen,"[Unavailable]"));
            assert(strstr(forge_screen,"Not available:\n- two-handed sword"));
            assert(forge_last_recipe_n > 1
                   && forge_last_recipe_ids[0] == 1
                   && forge_last_recipe_letters[0] == 'a');
            for (j = 1; j < forge_last_recipe_n; ++j)
                assert(forge_last_recipe_ids[j] == 0
                       && forge_last_recipe_letters[j] == 0);
        }
    }
    forge_test_script();
    forge_test_choose("Use the forge",1,-1);
    forge_test_choose("Choose a category",1,-1);
    forge_test_choose("Weapons",0,-1); /* unavailable rows are not selectable */
    forge_test_choose("Choose a category",0,-1);
    assert(forge_menu(NULL)==ECMD_OK);
    assert(strstr(forge_screen,"Not available:"));
    assert(!strstr(forge_screen,"[Available]")
           && !strstr(forge_screen,"[Unavailable]"));
    assert(!memcmp(&saved,obj,sizeof saved));
    forge_test_clear();
    forge_test_script();
    forge_test_choose("Use the forge",1,-1);
    forge_test_choose("Choose a category",1,-1);
    forge_test_choose("Weapons",0,-1);
    forge_test_choose("Choose a category",0,-1);
    assert(forge_menu(NULL)==ECMD_OK);
    assert(strstr(forge_screen,"Weapons (0)"));
    assert(forge_last_recipe_n > 0);
    for (i = 0; i < forge_last_recipe_n; ++i)
        assert(forge_last_recipe_ids[i] == 0
               && forge_last_recipe_letters[i] == 0);
    forge_test_clear();
    obj=forge_test_item(LONG_SWORD,3); saved=*obj;
    forge_test_script();
    forge_test_choose("Use the forge",1,-1);
    forge_test_choose("Choose a category",1,-1);
    forge_test_choose("Weapons",1,-1);
    forge_test_choose("Choose 1",1,2); /* excessive count: retry */
    forge_test_choose("Choose 1",1,1);
    forge_test_choose("Choose 1",1,1);
    forge_test_choose("Confirm crafting",1,-1);
    assert(forge_menu(NULL)==ECMD_TIME);
    assert(forge_choice_next==forge_choice_count && obj->quan==1);
    assert(strstr(forge_screen,"2 from "));
    assert(strstr(forge_screen,"3 long swords"));
    assert(svm.moves==moves); /* command return tells the main loop to tick once */
    obj=forge_test_item(TWO_HANDED_SWORD,1);
    assert(forge_available(&forge_recipes[2],NULL)); /* forged katana reusable */
    forge_test_clear();
    assert(forge_category(DAGGER)==0 && forge_category(PLATE_MAIL)==1);
    assert(forge_category(PICK_AXE)==2 && forge_category(ROCK)==3);
    puts("PASS Step 15B native menus, all cancellation/back paths, exact confirmation, one-action success and reforging");
}

#include "test_step15c.c"
#include "test_step15d_forge.c"
#include "test_step18a.c"
#include "test_step18b.c"
#include "test_step19.c"

void
step15b_test_main(void)
{
    struct forge_recipe pair[2];
    struct forge_allocation a[2];
    struct obj *obj, *other = 0, *output_capability;
    int i, j, expected_eroded, expected_quality;
    boolean has_eroded;
    step18b_test_main();
    step19_test_main();
    step18a_test_main();
    assert(forge_catalog_valid(forge_recipes, SIZE(forge_recipes)));
    pair[0] = pair[1] = forge_recipes[1];
    pair[1].need[0] = pair[0].need[1];
    pair[1].need[1] = pair[0].need[0];
    assert(!forge_catalog_valid(pair, 2));
    pair[1] = forge_recipes[0];
    pair[1].output = pair[0].output;
    assert(forge_catalog_valid(pair, 2));
    pair[1].need[0].quantity = 0;
    assert(!forge_catalog_valid(pair, 2));
    pair[0]=pair[1]=forge_recipes[0];
    pair[0].need[0].quantity=1;pair[0].need[1].quantity=3;
    pair[1].need[0].quantity=pair[1].need[1].quantity=2;
    assert(!forge_catalog_valid(pair,2)); /* same-type normalized totals */
    assert(!forge_type_allowed(STRANGE_OBJECT));
    assert(!forge_type_allowed(NUM_OBJECTS));
    assert(!forge_type_allowed(AMULET_OF_YENDOR));
    assert(!forge_type_allowed(FIRST_WORD));
    forge_test_script();
    forge_test_clear();
    for (i = 0; i < SIZE(forge_recipes); ++i) {
        const struct forge_recipe *r = &forge_recipes[i];
        memset(a, 0, sizeof a);
        obj = forge_test_item(r->need[0].otyp, 3);
        a[0].oid = obj->o_id;
        a[0].quantity[0] = 1;
        if (r->need[0].otyp == r->need[1].otyp) {
            a[0].quantity[1] = 1;
            j = 1;
        } else {
            other = forge_test_item(r->need[1].otyp, 1);
            a[1].oid = other->o_id;
            a[1].quantity[1] = 1;
            j = 2;
        }
        assert(forge_available(r, NULL));
        if (enhancement_eligible(obj) && obj->oclass != TOOL_CLASS)
            assert(enhancement_set(obj, 0,
                                   OQ_EXCEPTIONAL, FALSE));
        obj->spe=7;obj->cursed=1;obj->oeroded=2;obj->greased=1;
        obj->obranch_material=GOLD;obj=oname(obj,"ingredient",ONAME_NO_FLAGS);
        expected_eroded = 0;
        has_eroded = FALSE;
        if (forge_test_has_primary_erosion(obj)) {
            expected_eroded = obj->oeroded;
            has_eroded = TRUE;
        }
        if (j == 2 && forge_test_has_primary_erosion(other)) {
            expected_eroded = has_eroded ? min(expected_eroded, (int) other->oeroded)
                                          : other->oeroded;
            has_eroded = TRUE;
        }
        output_capability = forge_output(r->output);
        if (!forge_test_output_has_primary_erosion(output_capability))
            has_eroded = FALSE;
        expected_quality = OQ_STANDARD;
        if (enhancement_eligible(output_capability)
            && output_capability->oclass != TOOL_CLASS
            && enhancement_eligible(obj) && obj->oclass != TOOL_CLASS)
            expected_quality = OQ_EXCEPTIONAL;
        obfree(output_capability, NULL);
        if (!has_eroded)
            expected_eroded = 0;
        assert(forge_commit(r, a, j) == ECMD_TIME);
        assert(forge_find(a[0].oid)->quan == (j == 1 ? 1 : 2));
        if (j == 2) assert(!forge_find(a[1].oid));
        for (other = gi.invent; other && other->otyp != r->output; other = other->nobj) ;
        assert(other && other->quan == 1 && other->dknown);
        if (other->oclass == ARMOR_CLASS || other->oclass == WEAPON_CLASS)
            assert(other->spe == 7);
        else if (other->otyp == MAGIC_FLUTE || other->otyp == MAGIC_HARP)
            assert(other->spe >= 4 && other->spe <= 8);
        else assert(other->spe == 0);
        assert(!other->blessed && (int) other->cursed == (j == 1));
        assert(other->o_enh_quality == expected_quality);
        assert(!other->o_enh_props && !enhancement_slot_count(other));
        assert(!other->bknown && !other->rknown && !other->o_enh_known);
        assert(!other->oextra && !other->obranch_material
               && (int) other->oeroded == expected_eroded && !other->greased);
        assert(!objects[r->output].oc_uses_known || !other->known);
        assert(objects[r->output].oc_name_known);
        forge_test_clear();
    }
    printf("PASS Forge catalogue and all %d native crafting transactions\n",
           SIZE(forge_recipes));
    forge_test_eligibility();
    forge_test_quantity_limits();
    forge_test_transactions();
    forge_test_navigation();
    forge_test_ledger();
    forge_test_capacity_ledger();
    forge_test_many_stacks_and_counts();
    step15c_test_main();
    step15d_forge_tests();
    step15d_continuation_tests();
}
