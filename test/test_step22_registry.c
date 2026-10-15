/* Exercise the canonical registry against every real monster attack table. */
extern void step22_test_mind_blast(struct monst *);
static void
step22_registry_tests(void)
{
    struct monst actor, target;
    struct attack base, alt, *filtered;
    unsigned short ids[EX_MAX_ABILITIES];
    int species, slot, count, j, k, previous[NATTK] = { 0 };
    int attacks = 0, pure = 0, mixed = 0, dispatches = 0;
    long hp = u.uhp, now = svm.moves;
    memset(&target, 0, sizeof target);
    target.data = &mons[PM_GOBLIN]; target.mhp = target.mhpmax = 1000;
    target.mx = u.ux + 2; target.my = u.uy;
    {
        struct monst *goblin=makemon(&mons[PM_GOBLIN],u.ux+1,u.uy,NO_MINVENT | MM_NOMSG);
        assert(goblin);
        goblin->mhpmax=100;goblin->mhp=50;
        goblin->ex_regen_until=18;goblin->mlstmv=19;svm.moves=21;
        mon_catchup_elapsed_time(goblin,2);
        assert(goblin->mhp==50);
        mongone(goblin);svm.moves=now;
    }
    for (species = LOW_PM; species < NUMMONS; ++species) {
        memset(&actor, 0, sizeof actor);
        actor.cham = NON_PM;
        actor.data = &mons[species]; actor.m_id = 226001;
        actor.m_lev = max(1, actor.data->mlevel);
        actor.mhp = actor.mhpmax = 1000;
        actor.mcansee = actor.mcanmove = TRUE;
        actor.mx = u.ux + 1; actor.my = u.uy;
        actor.mux = u.ux; actor.muy = u.uy;
        count = ex_ability_count(&actor, ids, SIZE(ids));
        assert(count >= 0 && count <= EX_MAX_ABILITIES);
        for (j = 0; j < count; ++j) for (k = j + 1; k < count; ++k)
            assert(ids[j] != ids[k]);
        for (slot = 0; slot < NATTK; ++slot) {
            actor.ex_mute_until = 0;
            init_isaac64(226000UL + species * NATTK + slot, rn2);
            base = *getmattk(&actor, &target, slot, previous, &alt);
            if (base.adtyp == AD_PHYS || base.aatyp == AT_BOOM) continue;
            for (j = 0; j < count; ++j) {
                actor.ex_mute_species = species;
                actor.ex_mute_until = now + 6;
                actor.ex_mute_ability = ids[j];
                if (ex_muted(&actor, base.aatyp, base.adtyp)) break;
            }
            assert(j < count);
            for (k = 0; k < count; ++k)
                assert(ex_innate_muted(&actor, ids[k]) == (k == j));
            init_isaac64(226000UL + species * NATTK + slot, rn2);
            filtered = getmattk(&actor, &target, slot, previous, &alt);
            assert(filtered->adtyp == AD_PHYS && !actor.mcan);
            if (base.aatyp == AT_MAGC || base.aatyp == AT_GAZE
                || base.aatyp == AT_BREA || base.aatyp == AT_SPIT
                || base.aatyp == AT_EXPL || base.aatyp == AT_ARRW) {
                assert(filtered->aatyp == AT_NONE && !filtered->damn && !filtered->damd);
                ++pure;
            } else {
                assert(filtered->damn == base.damn && filtered->damd == base.damd);
                ++mixed;
            }
            switch (base.aatyp) {
            case AT_MAGC:
                assert(castmu(&actor, &base, TRUE, TRUE) == M_ATTK_MISS);
                assert(!actor.mspec_used); ++dispatches; break;
            case AT_GAZE:
                assert(gazemu(&actor, &base) == M_ATTK_MISS); ++dispatches; break;
            case AT_BREA:
                assert(breamm(&actor, &base, &target) == M_ATTK_MISS); ++dispatches; break;
            case AT_SPIT:
                assert(spitmm(&actor, &base, &target) == M_ATTK_MISS); ++dispatches; break;
            case AT_ARRW:
                assert(mith_internal_projectile(&actor, &target, &base) == M_ATTK_MISS);
                ++dispatches; break;
            default: break;
            }
            assert(u.uhp == hp && actor.mhp == 1000 && target.mhp == 1000);
            ++attacks;
        }
    }
    {
        int species_list[] = {PM_WEREWOLF, PM_HUMAN_WEREWOLF, PM_MIND_FLAYER,
            PM_HEZROU, PM_STEAM_VORTEX, PM_FOG_CLOUD, PM_SHRIEKER, PM_ERINYS};
        int ability_list[] = {EX_INNATE_TRANSFORM, EX_INNATE_TRANSFORM,
            EX_INNATE_PSYCHIC, EX_INNATE_STENCH, EX_INNATE_STENCH,
            EX_INNATE_STENCH, EX_INNATE_SHRIEK, EX_INNATE_SHRIEK};
        for (j=0;j<SIZE(species_list);++j) {
            int nextdraw, regions=svn.n_regions;
            memset(&actor,0,sizeof actor);
            actor.cham=NON_PM;
            actor.data=&mons[species_list[j]];actor.m_id=226002;
            actor.mx=u.ux+1;actor.my=u.uy;actor.mcansee=actor.mcanmove=1;
            actor.mhp=actor.mhpmax=100;actor.m_lev=10;
            actor.ex_mute_species=species_list[j];actor.ex_mute_until=now+6;
            actor.ex_mute_ability=ability_list[j];
            init_isaac64(229000UL+j,rn2);nextdraw=rn2(100000);
            init_isaac64(229000UL+j,rn2);
            if (j<2) {were_change(&actor);new_were(&actor);}
            else if(j==2) step22_test_mind_blast(&actor);
            else if(j<6) {m_everyturn_effect(&actor);m_postmove_effect(&actor);}
            else m_respond(&actor);
            assert(actor.data==&mons[species_list[j]] && u.uhp==hp
                   && svn.n_regions==regions && rn2(100000)==nextdraw);
        }
        puts("PASS Step22 T8 actual innate were transformation, psychic blast, three vapor/stench and two vocal alert dispatch paths suppressed without RNG or collateral effects");
    }
    assert(attacks > 0 && pure > 0 && mixed > 0 && dispatches > 0);
    printf("PASS Step22 T8 all %d native monster tables: %d distinct-slot selections, %d pure and %d mixed suppressions, %d real spell/gaze/breath/spit/projectile entrypoint cancellations; physical dice and unrelated capabilities retained\n",
           NUMMONS - LOW_PM, attacks, pure, mixed, dispatches);
}
