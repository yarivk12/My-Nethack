/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Native fatal pipeline with exact threshold and seeded probability oracles. */
static void
step22_execution_contract_tests(void)
{
    struct obj *source, *armor, *amulet;
    struct monst *mon;
    struct ex_hit_context ctx;
    struct attack attack = { AT_WEAP, AD_PHYS, 1, 2 };
    struct mvitals vitals[2];
    int unique, seed, expected, successes[2] = { 0, 0 }, reduced;
    long experience;
    vitals[0] = svm.mvitals[PM_GOBLIN];
    vitals[1] = svm.mvitals[PM_CROESUS];
    source = inspect_item(LONG_SWORD);
    artifact_exists(source, "Excalibur", TRUE, ONAME_WISH);
    setuwep(source);
    ex_test_grant(source, EX_EXECUTION, EX_WIELDED, EX_HERO, W_WEP);
    for (unique = 0; unique < 2; ++unique) {
        mon = NULL;
        for (seed = 1; seed <= 100; ++seed) {
            if (!mon) {
                mon = makemon(&mons[unique ? PM_CROESUS : PM_GOBLIN],
                              u.ux + 1, u.uy, MM_ADJACENTOK);
                assert(mon && !ex_protected_monster(mon));
                assert(!!(mon->data->geno & G_UNIQ) == unique);
                while (mon->minvent) m_useup(mon, mon->minvent);
                mon->mpeaceful = mon->mtame = 0;
            }
            mon->mhpmax = 1000;
            mon->mhp = unique ? 151 : 251;
            ex_hit_begin(&ctx, mon, source, &attack);
            ex_hit_finish(&ctx, mon, 1);
            assert(!DEADMONSTER(mon)); /* exact boundary plus one */
            mon->mhp = unique ? 150 : 250;
            ex_hit_begin(&ctx, mon, source, &attack);
            ex_hit_finish(&ctx, mon, 0);
            assert(!DEADMONSTER(mon)); /* positive actual damage required */
            init_isaac64((unsigned long) (228000 + unique * 100 + seed), rn2);
            expected = rn2(100) < (unique ? 10 : 20);
            init_isaac64((unsigned long) (228000 + unique * 100 + seed), rn2);
            ex_hit_begin(&ctx, mon, source, &attack);
            ex_hit_finish(&ctx, mon, 1);
            assert(!!DEADMONSTER(mon) == expected);
            if (expected) { ++successes[unique]; mon = NULL; }
        }
        if (mon) mongone(mon);
    }
    assert(successes[0] > 0 && successes[1] > 0);
    mon = makemon(&mons[PM_GOBLIN], u.ux + 1, u.uy, MM_ADJACENTOK); assert(mon);
    while (mon->minvent) m_useup(mon, mon->minvent);
    mon->mpeaceful = mon->mtame = 0;
    armor = mksobj(PLATE_MAIL, FALSE, FALSE);
    assert(enhancement_set(armor, OEP_DR_IV, 0, FALSE));
    (void) mpickobj(mon, armor); armor->owornmask = W_ARM;
    mon->misc_worn_check |= W_ARM;
    reduced = enhancement_reduce(mon, 100, ENH_PHYSICAL);
    assert(reduced > 0 && reduced < 100);
    amulet = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE);
    (void) mpickobj(mon, amulet); amulet->owornmask = W_AMUL;
    mon->misc_worn_check |= W_AMUL;
    for (seed = 1; seed <= 100; ++seed) {
        init_isaac64((unsigned long) (228500 + seed), rn2);
        if (rn2(100) < 20) break;
    }
    assert(seed <= 100);
    mon->mhpmax = 1000; mon->mhp = 250;
    experience = u.uexp;
    ex_hit_begin(&ctx, mon, source, &attack);
    init_isaac64((unsigned long) (228500 + seed), rn2);
    ex_hit_finish(&ctx, mon, reduced);
    assert(!DEADMONSTER(mon) && !which_armor(mon, W_AMUL));
    assert(mon->mhp == mon->mhpmax && u.uexp == experience);
    /* The same fatal roll bypasses worn DR, once native lifesaving is gone. */
    mon->mhp = 250;
    ex_hit_begin(&ctx, mon, source, &attack);
    init_isaac64((unsigned long) (228500 + seed), rn2);
    ex_hit_finish(&ctx, mon, reduced);
    assert(DEADMONSTER(mon) && u.uexp > experience);
    setuwep(NULL); step22_combat_discard(source);
    svm.mvitals[PM_GOBLIN] = vitals[0];
    svm.mvitals[PM_CROESUS] = vitals[1];
    puts("PASS Step22 O5 native ordinary/optional-unique exact 25%/15% boundaries, seeded 20%/10% rolls, positive-damage gate, worn DR bypass, native lifesaving and XP (200 seeds228001..228200)");
}
