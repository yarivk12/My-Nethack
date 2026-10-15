/* Exact native hand and stance attribution for snapshotted Frenzy rolls. */
static void
step22_hand_attribution_tests(void)
{
    struct skills saved[P_NUM_SKILLS];
    struct monst *mon, *pony = NULL;
    struct obj *main, *off, *striker;
    struct ex_hit_context ctx;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 2 };
    int scenario, seed, damage, oldtwo = gt.twohits;
    coord ponypos = { 0, 0 };
    assert(!gi.invent && !u.usteed && !u.twoweap);
    memcpy(saved, u.weapon_skills, sizeof saved);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK); assert(mon);
    mon->mpeaceful = mon->mtame = 0;
    for (scenario = 0; scenario < 4; ++scenario) {
        main = addinv(mksobj(scenario == 0 ? RAKUYO
                              : scenario == 3 ? TWO_HANDED_SWORD : LONG_SWORD,
                            FALSE, FALSE));
        main->spe = 3; setuwep(main);
        off = NULL; striker = main; gt.twohits = 0;
        if (scenario == 1 || scenario == 2) {
            off = addinv(mksobj(DAGGER, FALSE, FALSE));
            off->spe = 3; setuswapwep(off); u.twoweap = TRUE;
            P_SKILL(P_DAGGER) = P_EXPERT;
            P_SKILL(P_TWO_WEAPON_COMBAT) = P_UNSKILLED;
            P_SKILL(P_RIDING) = P_EXPERT;
            striker = off; gt.twohits = 2;
        }
        if (scenario == 2) {
            pony = makemon(&mons[PM_PONY], u.ux + 2, u.uy, MM_ADJACENTOK); assert(pony);
            ponypos.x = pony->mx; ponypos.y = pony->my;
            remove_monster(pony->mx, pony->my);
            pony->mx = u.ux; pony->my = u.uy; u.usteed = pony;
        }
        for (seed = 1; seed <= 60; ++seed) {
            mon->mhp = mon->mhpmax = 10000;
            ex_hit_begin(&ctx, mon, striker, &attack);
            init_isaac64((unsigned long) (227000 + scenario * 100 + seed), rn2);
            damage = enhancement_reduce(mon, ex_native_physical_roll(&ctx, mon), ENH_PHYSICAL);
            init_isaac64((unsigned long) (227000 + scenario * 100 + seed), rn2);
            assert(hmon(mon, striker, HMON_MELEE, 10));
            assert(10000 - mon->mhp == damage);
        }
        /* The calculation survives disappearance of the actual held source;
         * it does not temporarily install a stack object as a hero weapon. */
        ex_hit_begin(&ctx, mon, striker, &attack);
        init_isaac64(227900UL + scenario, rn2);
        damage = ex_native_physical_roll(&ctx, mon);
        if (off) { setuswapwep(NULL); step22_combat_discard(off); }
        setuwep(NULL); step22_combat_discard(main); u.twoweap = FALSE;
        init_isaac64(227900UL + scenario, rn2);
        assert(ex_native_physical_roll(&ctx, mon) == damage);
        if (pony) {
            u.usteed = NULL;
            place_monster(pony, ponypos.x, ponypos.y);
            mongone(pony); pony = NULL;
        }
    }
    gt.twohits = oldtwo; memcpy(u.weapon_skills, saved, sizeof saved);
    mongone(mon);
    puts("PASS Step22 O4 exact native Rakuyo/mainhand, dual offhand minskill, mounted-dual skill and two-handed strength attribution; consumed-source snapshots retain original roll (240 paired seeds227001..227360)");
}
