/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Included by the native runtime harness after its object/window helpers. */
#include "exotic_combat.h"
static void
step22_combat_discard(struct obj *obj)
{
    assert(obj && obj->where == OBJ_INVENT);
    setnotworn(obj);
    freeinv(obj);
    obfree(obj, NULL);
}
#include "test_step22_hands.c"
#include "test_step22_execution.c"
#include "test_step22_composite.c"
#include "test_step22_loot.c"
#include "test_step22_config.c"
#include "test_step22_innate.c"
#include "test_step22_natural.c"
static void
step22_native_physical_tests(void)
{
    struct monst *mon;
    struct obj *obj, *gloves = NULL, *ring = NULL;
    struct ex_hit_context ctx;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 2 };
    int scenario, seed, expected, before, role = Role_switch;
    unsigned oldkills = svm.mvitals[PM_GOBLIN].died;
    struct prop props[LAST_PROP + 1];
    memcpy(props, u.uprops, sizeof props);
    memset(u.uprops, 0, sizeof props);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK);
    assert(mon && !Upolyd);
    mon->mpeaceful = mon->mtame = 0;
    levl[mon->mx][mon->my].lit = 1;
    vision_reset();
    vision_recalc(0);
    assert(canseemon(mon));
    for (scenario = 0; scenario < 6; ++scenario) {
        obj = NULL;
        gu.urole.mnum = scenario == 0 ? PM_HEALER : scenario == 1 ? PM_ROGUE : role;
        if (scenario < 2) {
            svm.mvitals[PM_GOBLIN].died = 18;
            obj = addinv(mksobj(KNIFE, FALSE, FALSE));
            setuwep(obj);
            mon->mflee = 1;
        } else if (scenario == 2) {
            assert(newcham(mon, &mons[PM_VAMPIRE], NO_NC_FLAGS));
            gloves = addinv(mksobj(LEATHER_GLOVES, FALSE, FALSE));
            gloves->blessed = 1; setworn(gloves, W_ARMG);
        } else if (scenario == 3) {
            int ringtype;
            for (ringtype = RIN_ADORNMENT; ringtype <= RIN_PROTECTION_FROM_SHAPE_CHAN; ++ringtype)
                if (objects[ringtype].oc_material == SILVER) break;
            assert(ringtype <= RIN_PROTECTION_FROM_SHAPE_CHAN);
            ring = mksobj(ringtype, FALSE, FALSE);
            ring = addinv(ring); setworn(ring, W_RINGL);
            assert(obj_material(ring) == SILVER);
        } else {
            obj = addinv(mksobj(scenario == 4 ? BOW : LEASH, FALSE, FALSE));
            setuwep(obj);
        }
        for (seed = 1; seed <= 60; ++seed) {
            mon->mhp = mon->mhpmax = 10000;
            mon->mflee = 1;
            ex_hit_begin(&ctx, mon, obj, &attack);
            ctx.native_skill = scenario < 2;
            init_isaac64((unsigned long) (225000 + scenario * 100 + seed), rn2);
            expected = enhancement_reduce(mon, ex_native_physical_roll(&ctx, mon), ENH_PHYSICAL);
            init_isaac64((unsigned long) (225000 + scenario * 100 + seed), rn2);
            before = mon->mhp;
            assert(hmon(mon, obj, HMON_MELEE, 10));
            assert(before - mon->mhp == expected);
        }
        if (obj) { setuwep(NULL); step22_combat_discard(obj); }
        if (gloves) { step22_combat_discard(gloves); gloves = NULL; }
        if (ring) { step22_combat_discard(ring); ring = NULL; }
    }
    mongone(mon);
    gu.urole.mnum = role; svm.mvitals[PM_GOBLIN].died = oldkills;
    memcpy(u.uprops, props, sizeof props);
    puts("PASS Step 22 O4 native physical calculator matches real hmon for Healer anatomy, Rogue backstab, blessed gloves, silver ring, improper launcher and improvised object (360 paired seeds225001..225560)");
}

static void
step22_native_mute_tests(void)
{
    struct monst *mon;
    struct ex_hit_context contact;
    struct attack strike = { AT_CLAW, AD_PHYS, 1, 2 }, cast = { AT_MAGC, AD_SPEL, 0, 0 };
    struct attack alt, *selected;
    struct prop props[LAST_PROP + 1];
    int previous[NATTK] = { 0 }, i, frozen = 0, trials = 0, hp = u.uhp;
    memcpy(props, u.uprops, sizeof props);
    memset(u.uprops, 0, sizeof props);
    mon = makemon(&mons[PM_GELATINOUS_CUBE], u.ux + 1, u.uy, MM_ADJACENTOK);
    assert(mon);
    for (i = 1; i <= 100; ++i) {
        mon->ex_mute_until = 0; mon->ex_mute_ability = 0;
        init_isaac64((unsigned long) (222000 + i), rn2);
        ex_hit_begin(&contact, mon, NULL, &strike);
        ex_apply_mute(mon);
        if (!ex_muted(mon, AT_NONE, AD_PLYS)) { ex_contact_end(mon); continue; }
        ++trials;
        gm.multi = 0;
        (void) passive(mon, NULL, TRUE, TRUE, AT_CLAW, FALSE);
        if (gm.multi < 0) ++frozen;
        gm.multi = 0;
        ex_hit_begin(&contact, mon, NULL, &strike);
        (void) passive(mon, NULL, TRUE, TRUE, AT_CLAW, FALSE);
        assert(gm.multi == 0); /* same mute stops the next real contact */
    }
    assert(trials > 25 && frozen > 10);
    assert(newcham(mon, &mons[PM_LICH], NO_NC_FLAGS));
    for (i = 1; i <= 100; ++i) {
        mon->ex_mute_until = 0;
        ex_apply_mute(mon);
        if (ex_muted(mon, AT_MAGC, AD_SPEL)) break;
    }
    assert(i <= 100);
    mon->mspec_used = 0;
    assert(castmu(mon, &cast, TRUE, TRUE) == M_ATTK_MISS);
    assert(mon->mspec_used == 0 && u.uhp == hp);
    selected = getmattk(mon, &gy.youmonst, 1, previous, &alt);
    assert(selected->aatyp == AT_NONE && selected->damn == 0 && selected->damd == 0);
    assert(newcham(mon, &mons[PM_MIND_FLAYER], NO_NC_FLAGS));
    for (i = 1; i <= 100; ++i) {
        mon->ex_mute_until = 0; ex_apply_mute(mon);
        if (ex_muted(mon, AT_TENT, AD_DRIN)) break;
    }
    assert(i <= 100);
    selected = getmattk(mon, &gy.youmonst, 1, previous, &alt);
    assert(selected->aatyp == AT_TENT && selected->adtyp == AD_PHYS
           && selected->damn == mon->data->mattk[1].damn
           && selected->damd == mon->data->mattk[1].damd);
    mongone(mon);
    gm.multi = 0; memcpy(u.uprops, props, sizeof props);
    puts("PASS Step 22 U7 native passive original-contact/subsequent suppression, castmu complete channel cancellation, and concrete getmattk mixed physical preservation (seeds 222001..222100)");
}

static void
step22_native_chain_tests(void)
{
    struct obj *source, *paper;
    struct monst *primary, *secondary, *victims[4];
    struct ex_hit_context committed;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 2 };
    int x, y, dx, dy, clear, seed, chains = 0, before, i, scenario, base;
    coord oldhero;
    oldhero.x = u.ux; oldhero.y = u.uy;
    for (x = 4; x < COLNO - 8; ++x) {
        for (y = 3; y < ROWNO - 3; ++y) {
            if (levl[x - 1][y].typ != ROOM || levl[x][y].typ != ROOM
                || levl[x + 1][y].typ != ROOM || levl[x + 2][y].typ != ROOM
                || levl[x + 3][y].typ != ROOM || levl[x + 4][y].typ != ROOM
                || levl[x][y + 1].typ != ROOM || levl[x + 1][y + 1].typ != ROOM
                || t_at(x, y) || t_at(x + 1, y)) continue;
            clear = 1;
            for (dx = -2; dx <= 6; ++dx) for (dy = -2; dy <= 2; ++dy)
                if (m_at(x + dx, y + dy)) clear = 0;
            if (clear) break;
        }
        if (y < ROWNO - 3) break;
    }
    assert(x < COLNO - 8);
    u.ux = x - 1; u.uy = y;
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Fire Brand", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_CHAIN, EX_WIELDED, EX_HERO, W_WEP);
    setuwep(source);
    secondary = makemon(&mons[PM_GOBLIN], x + 1, y, NO_MM_FLAGS);
    assert(secondary && secondary->mx == x + 1 && secondary->my == y);
    secondary->mpeaceful = secondary->mtame = 0;
    secondary->mhp = secondary->mhpmax = 10000;
    paper = mksobj(SCR_IDENTIFY, FALSE, FALSE);
    (void) mpickobj(secondary, paper);
    for (seed = 1; seed <= 100; ++seed) {
        primary = makemon(&mons[PM_GOBLIN], x, y, NO_MM_FLAGS);
        assert(primary && primary->mx == x && primary->my == y);
        primary->mpeaceful = primary->mtame = 0;
        primary->mhp = 1;
        before = secondary->mhp;
        gb.bhitpos.x = x; gb.bhitpos.y = y;
        init_isaac64((unsigned long) (221000 + seed), rn2);
        assert(!hmon(primary, source, HMON_MELEE, 10));
        assert(DEADMONSTER(primary));
        assert(secondary->mhp <= before && before - secondary->mhp <= 6);
        if (secondary->mhp < before) ++chains;
        assert(paper->where == OBJ_MINVENT && paper->ocarry == secondary);
    }
    assert(chains > 12 && chains < 50);
    victims[0] = secondary;
    for (i = 1; i < 4; ++i) {
        victims[i] = makemon(&mons[PM_GOBLIN], x + i + 1, y, NO_MM_FLAGS);
        assert(victims[i] && victims[i]->mx == x + i + 1 && victims[i]->my == y);
    }
    for (scenario = 0; scenario < 3; ++scenario) {
        if (scenario == 1) assert(newcham(victims[1], &mons[PM_FIRE_ELEMENTAL], NO_NC_FLAGS));
        if (scenario == 2) {
            assert(newcham(victims[1], &mons[PM_GOBLIN], NO_NC_FLAGS));
        }
        for (i = 0; i < 4; ++i) {
            victims[i]->mhp = victims[i]->mhpmax = 10000;
            victims[i]->mpeaceful = scenario == 2 && i == 1;
            victims[i]->mtame = 0;
        }
        primary = makemon(&mons[PM_GOBLIN], x, y, NO_MM_FLAGS); assert(primary);
        for (seed = 1; seed <= 100; ++seed) {
            init_isaac64((unsigned long) (226000 + scenario * 100 + seed), rn2);
            ex_hit_begin(&committed, primary, source, &attack);
            if (committed.chains) break;
        }
        assert(seed <= 100 && committed.chains == 1);
        base = committed.chain[0].base;
        primary->mhp = 0; killed(primary);
        ex_hit_finish(&committed, primary, 1);
        assert(victims[0]->mhp == 10000 - max(1, base * 3 / 4));
        if (!scenario) {
            assert(victims[1]->mhp == 10000 - max(1, base * 9 / 16));
            assert(victims[2]->mhp == 10000 - max(1, base * 27 / 64));
        } else assert(victims[1]->mhp == 10000 && victims[2]->mhp == 10000);
        assert(victims[3]->mhp == 10000); /* never a fourth hop */
    }
    for (i = 0; i < 4; ++i) mongone(victims[i]);
    /* The diagonal victim is legal floor; the orthogonal corner tiles are
     * barriers, not phantom monsters embedded in walls. */
    secondary = makemon(&mons[PM_GOBLIN], x + 1, y + 1, NO_MM_FLAGS); assert(secondary);
    secondary->mpeaceful = secondary->mtame = 0;
    secondary->mhp = secondary->mhpmax = 10000;
    primary = makemon(&mons[PM_GOBLIN], x, y, NO_MM_FLAGS); assert(primary);
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64((unsigned long) (226500 + seed), rn2);
        ex_hit_begin(&committed, primary, source, &attack);
        if (committed.chains) break;
    }
    assert(seed <= 100);
    levl[x + 1][y].typ = STONE; levl[x][y + 1].typ = STONE;
    primary->mhp = 0; killed(primary); ex_hit_finish(&committed, primary, 1);
    assert(secondary->mhp == 10000);
    levl[x + 1][y].typ = ROOM; levl[x][y + 1].typ = ROOM;
    mongone(secondary);
    setuwep(NULL); step22_combat_discard(source);
    u.ux = oldhero.x; u.uy = oldhero.y;
    puts("PASS Step 22 native hmon physical killing blows propagate precommitted O6 elemental HP-only chains after primary death; secondary paper survives (seeds 221001..221100)");
    puts("PASS Step22 O6 native committed resolver chains: exact 3-hop falloff/min1, no fourth hop, full resistance terminates, friendly conduit stops and blocked diagonal corner rejects (seeds226001..226600)");
}

static void
step22_native_strike_tests(void)
{
    struct obj *source, *weapon;
    struct monst *mon;
    int seed, baseline, frenzy = 0, execution = 0;
    long oldexp;
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Excalibur", TRUE, ONAME_WISH);
    setuwep(source);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK);
    assert(mon);
    mon->mpeaceful = mon->mtame = 0;
    while (mon->minvent) m_useup(mon, mon->minvent);
    for (seed = 1; seed <= 200; ++seed) {
        ex_test_clear(source);
        mon->mhp = mon->mhpmax = 10000;
        init_isaac64((unsigned long) seed, rn2);
        assert(hmon(mon, source, HMON_MELEE, 10));
        baseline = 10000 - mon->mhp;
        ex_test_grant(source, EX_FRENZY, EX_WIELDED, EX_HERO, W_WEP);
        mon->mhp = mon->mhpmax = 10000;
        init_isaac64((unsigned long) seed, rn2);
        assert(hmon(mon, source, HMON_MELEE, 10));
        assert(10000 - mon->mhp >= baseline);
        if (10000 - mon->mhp > baseline) ++frenzy;
    }
    assert(frenzy > 18 && frenzy < 65);
    ex_test_clear(source);
    ex_test_grant(source, EX_DISARM, EX_DIRECT, EX_HERO, 0L);
    weapon = mksobj(DAGGER, FALSE, FALSE);
    weapon->cursed = 0; (void) mpickobj(mon, weapon);
    mon->weapon_check = NEED_HTH_WEAPON;
    (void) mon_wield_item(mon);
    assert(MON_WEP(mon) == weapon);
    for (seed = 1; seed <= 100 && MON_WEP(mon); ++seed) {
        mon->mhp = mon->mhpmax = 10000;
        init_isaac64((unsigned long) (220500 + seed), rn2);
        assert(hmon(mon, source, HMON_MELEE, 10));
    }
    assert(!MON_WEP(mon) && weapon->where == OBJ_FLOOR
           && !weapon->owornmask && weapon->ox == mon->mx && weapon->oy == mon->my);
    obj_extract_self(weapon); obfree(weapon, NULL);
    ex_test_clear(source);
    ex_test_grant(source, EX_EXECUTION, EX_WIELDED, EX_HERO, W_WEP);
    oldexp = u.uexp;
    for (seed = 1; seed <= 100; ++seed) {
        mon->mhpmax = 1000; mon->mhp = 100;
        init_isaac64((unsigned long) (220700 + seed), rn2);
        if (!hmon(mon, source, HMON_MELEE, 10)) { execution++; break; }
        assert(mon->mhp > 0 && mon->mhp < 100);
    }
    assert(execution == 1 && DEADMONSTER(mon) && u.uexp > oldexp);
    setuwep(NULL); step22_combat_discard(source);
    puts("PASS Step 22 real hmon primary strikes: O4 paired native damage seeds 1..200, O1A native unwield/drop, O5 native death/XP pipeline (220501..220800)");
}

static void
step22_combat_tests(void)
{
    struct monst *mon;
    struct obj *source, *other, *weapon, *bag, *inner, *protected;
    struct ex_hit_context hit;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 6 };
    unsigned short abilities[NATTK + 5];
    unsigned char oldflags[SPECIAL_PM];
    int i, candidate, changed = 0, refreshed = 0, applied = 0;
    long now = svm.moves;
    inspect_window_setup();
    mon = makemon(&mons[PM_TROLL], u.ux + 1, u.uy, MM_ADJACENTOK);
    assert(mon);
    mon->mhpmax = 100; mon->mhp = 50;
    svm.moves = 1000;
    mon->ex_regen_until = 1005;
    mon_regen(mon, FALSE); assert(mon->mhp == 50);
    mon->mspec_used = 3;
    mon_regen(mon, FALSE); assert(mon->mhp == 50 && mon->mspec_used == 2);
    assert(healmon(mon, 7, 0) == 7 && mon->mhp == 57);
    svm.moves = 1005;
    mon_regen(mon, FALSE); assert(mon->mhp == 58);
    mon->mhp = 50;
    for (i = 0; i < 5; ++i) {
        svm.moves = 1000 + i;
        mcalcdistress();
        assert(mon->mhp == 50);
    }
    svm.moves = 1005;
    mcalcdistress(); assert(mon->mhp == 51);
    mon->mlstmv = 1000; mon->mhp = 50;
    svm.moves = 1011;
    mon_catchup_elapsed_time(mon, 10);
    assert(mon->mhp == 56); /* ticks 1005..1010 are not suppressed */
    for (i = 1; i <= 400; ++i) {
        init_isaac64((unsigned long) i, rn2);
        candidate = ex_poly_candidate(mon);
        assert(candidate == NON_PM
            || (mons[candidate].mlevel <= mon->data->mlevel
                && mons[candidate].difficulty <= mon->data->difficulty
                && polyok(&mons[candidate])
                && !(svm.mvitals[candidate].mvflags & (G_GENOD | G_EXTINCT))));
        if (candidate != NON_PM) changed++;
    }
    assert(changed > 300);
    for (i = LOW_PM; i < SPECIAL_PM; ++i) {
        oldflags[i] = svm.mvitals[i].mvflags;
        svm.mvitals[i].mvflags |= G_GENOD;
    }
    assert(ex_poly_candidate(mon) == NON_PM);
    for (i = LOW_PM; i < SPECIAL_PM; ++i)
        svm.mvitals[i].mvflags = oldflags[i];
    mon->ex_regen_until = svm.moves + 5;
    ex_apply_mute(mon);
    assert(!mon->ex_mute_until); /* plain troll has no eligible special */
    assert(newcham(mon, &mons[PM_MIND_FLAYER], NO_NC_FLAGS));
    assert(ex_ability_count(mon, abilities, SIZE(abilities)) == 2);
    for (i = 1; i <= 100; ++i) {
        mon->ex_mute_until = 0; mon->ex_mute_ability = 0;
        init_isaac64((unsigned long) i, rn2);
        ex_apply_mute(mon);
        assert(mon->ex_mute_until >= svm.moves + 2
               && mon->ex_mute_until <= svm.moves + 6);
        assert(ex_muted(mon, AT_TENT, AD_DRIN)
               || ex_innate_muted(mon, EX_INNATE_PSYCHIC));
        mon->ex_mute_until = svm.moves + 6;
        ex_apply_mute(mon);
        assert(mon->ex_mute_until == svm.moves + 6);
        refreshed++;
    }
    assert(newcham(mon, &mons[PM_GOBLIN], NO_NC_FLAGS));
    assert(!mon->ex_mute_until && !mon->ex_mute_ability
           && mon->ex_regen_until == svm.moves + 5);
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Fire Brand", TRUE, ONAME_WISH);
    ex_test_grant(source, EX_ANTIREGEN, EX_DIRECT, EX_HERO, 0L);
    for (i = 1; i <= 500; ++i) {
        mon->ex_regen_until = 0;
        init_isaac64((unsigned long) i, rn2);
        ex_hit_begin(&hit, mon, source, &attack);
        ex_hit_finish(&hit, mon, 1);
        if (mon->ex_regen_until) {
            assert(mon->ex_regen_until == svm.moves + 5); ++applied;
        }
    }
    assert(applied > 60 && applied < 145);
    /* Snapshot does not acquire a later grant. A removed captured source
     * cannot apply status after the event boundary. */
    ex_test_clear(source);
    ex_hit_begin(&hit, mon, source, &attack);
    ex_test_grant(source, EX_ANTIREGEN, EX_DIRECT, EX_HERO, 0L);
    mon->ex_regen_until = 0;
    ex_hit_finish(&hit, mon, 1); assert(!mon->ex_regen_until);
    ex_hit_begin(&hit, mon, source, &attack);
    freeinv(source);
    ex_hit_finish(&hit, mon, 1); assert(!mon->ex_regen_until);
    source = addinv(source);
    /* T1: nested and deeply nested progression objects reject the whole
     * top-level entry before the uniform inventory sampler executes. */
    while (mon->minvent) m_useup(mon, mon->minvent);
    bag = mksobj(SACK, FALSE, FALSE);
    inner = mksobj(SACK, FALSE, FALSE);
    protected = mksobj(BELL_OF_OPENING, FALSE, FALSE);
    add_to_container(inner, protected); add_to_container(bag, inner);
    (void) mpickobj(mon, bag);
    weapon = mksobj(DAGGER, FALSE, FALSE); weapon->quan = 4;
    weapon->owt = weight(weapon); (void) mpickobj(mon, weapon);
    ex_steal(mon);
    assert(weapon->where != OBJ_MINVENT && bag->where == OBJ_MINVENT
           && protected->where == OBJ_CONTAINED && protected->ocontainer == inner
           && inner->ocontainer == bag && weapon->quan == 4);
    if (weapon->where == OBJ_INVENT) step22_combat_discard(weapon);
    else if (weapon->where == OBJ_FLOOR) { obj_extract_self(weapon); obfree(weapon, NULL); }
    /* Multiple explicitly configured native intrinsic sources commit before
     * damage. They retain separate rolled bases and original impact data. */
    ex_test_clear(source);
    ex_test_grant(source, EX_CHAIN, EX_DIRECT, EX_HERO, 0L);
    source->exotic.chain_native = 1;
    other = addinv(mksobj(LONG_SWORD, FALSE, FALSE));
    artifact_exists(other, "Frost Brand", TRUE, ONAME_WISH);
    ex_test_grant(other, EX_CHAIN, EX_DIRECT, EX_HERO, 0L);
    other->exotic.chain_native = 1;
    applied = 0;
    for (i = 1; i <= 400; ++i) {
        init_isaac64((unsigned long) (220000 + i), rn2);
        ex_hit_begin(&hit, mon, source, &attack);
        if (hit.chains == 2) {
            assert(hit.chain[0].base > 0 && hit.chain[1].base > 0);
            assert(hit.chain[0].element != hit.chain[1].element);
            ++applied;
        }
    }
    assert(applied > 12 && applied < 65);
    ex_test_clear(source); ex_test_clear(other);
    ex_test_grant(source, EX_POLYMORPH, EX_DIRECT, EX_HERO, 0L);
    ex_test_grant(other, EX_POLYMORPH, EX_CARRIED, EX_HERO, 0L);
    ex_test_grant(source, EX_MUTE, EX_DIRECT, EX_HERO, 0L);
    ex_test_grant(other, EX_MUTE, EX_CARRIED, EX_HERO, 0L);
    applied = 0;
    for (i = 1; i <= 1000; ++i) {
        int before_changes;
        assert(!DEADMONSTER(mon));
        if (mon->data != &mons[PM_TROLL])
            assert(newcham(mon, &mons[PM_TROLL], NO_NC_FLAGS));
        mon->mhp = mon->mhpmax = 10000;
        mon->ex_regen_until = svm.moves + 5;
        mon->ex_mute_until = 0;
        before_changes = ex_test_poly_changes;
        init_isaac64((unsigned long) (223000 + i), rn2);
        ex_hit_begin(&hit, mon, source, &attack);
        ex_hit_finish(&hit, mon, 1);
        assert(mon->data->mlevel <= mons[PM_TROLL].mlevel
               && mon->data->difficulty <= mons[PM_TROLL].difficulty);
        assert(mon->ex_regen_until == svm.moves + 5);
        if (mon->ex_mute_until > svm.moves)
            assert(mon->ex_mute_species == monsndx(mon->data));
        if (ex_test_poly_changes - before_changes >= 2) ++applied;
    }
    assert(applied > 5 && ex_test_poly_attempts > 200);
    step22_combat_discard(other); step22_combat_discard(source);
    mongone(mon);
    svm.moves = now;
    step15b_window_setup();
    assert(refreshed == 100);
    puts("PASS Step 22 combat: O3 native regeneration/catchup/healing, T1 nested protection and whole stacks, T2 finite bounds, U7 dedup/nonshortening/poly clearing, source snapshots, independent native O6 commitments (seeds 1..500, 220001..220400)");
    step22_native_strike_tests();
    step22_native_physical_tests();
    step22_hand_attribution_tests();
    step22_execution_contract_tests();
    step22_native_mute_tests();
    step22_native_chain_tests();
    step22_composite_strike_tests();
    step22_native_loot_contract_tests();
    step22_native_config_registry_tests();
    step22_native_innate_dispatch_tests();
    step22_native_kick_natural_tests();
}
