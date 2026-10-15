static void
step22_native_config_registry_tests(void)
{
    struct monst actor, target;
    struct attack alt, base, *filtered;
    unsigned short ids[EX_MAX_ABILITIES];
    int seduce = SYSOPT_SEDUCE, mode, species, slot, prior, j, count;
    int previous[NATTK], replacements = 0, conditional = 0;
    memset(&target, 0, sizeof target); target.data = &mons[PM_GOBLIN];
    target.mhp = target.mhpmax = 1000;
    for (mode = 0; mode <= 1; ++mode) {
        SYSOPT_SEDUCE = mode;
        for (species = LOW_PM; species < NUMMONS; ++species) {
            memset(&actor, 0, sizeof actor); actor.data = &mons[species];
            actor.cham = NON_PM;
            actor.m_id = 232001; actor.mhp = actor.mhpmax = 1000;
            actor.mcansee = actor.mcanmove = 1;
            count = ex_ability_count(&actor, ids, SIZE(ids));
            assert(count <= EX_MAX_ABILITIES);
            for (prior = 0; prior < 2; ++prior) {
                for (j = 0; j < NATTK; ++j) previous[j] = prior ? M_ATTK_HIT : M_ATTK_MISS;
                for (slot = 0; slot < NATTK; ++slot) {
                    actor.ex_mute_until = 0;
                    init_isaac64(232000UL + species * NATTK + slot, rn2);
                    base = *getmattk(&actor, &target, slot, previous, &alt);
                    if (!mode && actor.data->mattk[slot].adtyp == AD_SSEX
                        && base.adtyp != AD_SSEX) ++replacements;
                    if (prior && base.adtyp == AD_STUN
                        && actor.data->mattk[slot].adtyp != AD_STUN) ++conditional;
                    if (base.adtyp == AD_PHYS || base.aatyp == AT_BOOM) continue;
                    actor.ex_mute_species = species; actor.ex_mute_until = svm.moves + 6;
                    for (j = 0; j < count; ++j) {
                        actor.ex_mute_ability = ids[j];
                        if (ex_muted(&actor, base.aatyp, base.adtyp)) break;
                    }
                    assert(j < count);
                    init_isaac64(232000UL + species * NATTK + slot, rn2);
                    filtered = getmattk(&actor, &target, slot, previous, &alt);
                    assert(filtered->adtyp == AD_PHYS && !actor.mcan);
                }
            }
        }
    }
    SYSOPT_SEDUCE = seduce;
    assert(replacements > 0 && conditional > 0);
    puts("PASS Step22 U7 native SEDUCE=0/1 concrete substitutions and prior-hit conditional stun replacements across every monster table remain selectable and suppressible without cancellation");
}
