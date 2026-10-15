static void
step22_status_combat_tests(void)
{
    struct monst *attacker, *defender;
    struct mvitals cat = svm.mvitals[PM_HOUSECAT];
    struct mvitals saved[2];
    unsigned short ids[EX_MAX_ABILITIES];
    int kinds[2] = {PM_COCKATRICE, PM_GELATINOUS_CUBE};
    int effects[2] = {AD_STON, AD_PLYS};
    int kind, trial, muted, j, count, native_events[2] = {0,0};
    struct obj *o, *next;
    for(kind=0;kind<2;++kind) {
        saved[kind]=svm.mvitals[kinds[kind]];
        svm.mvitals[kinds[kind]].mvflags &= ~G_GONE;
        defender=makemon(&mons[kinds[kind]],u.ux+3,u.uy,NO_MINVENT | MM_NOMSG);
        assert(defender);defender->mhp=defender->mhpmax=1000000;
        count=ex_ability_count(defender,ids,SIZE(ids));
        for(j=0;j<count;++j) {
            defender->ex_mute_ability=ids[j];
            defender->ex_mute_species=kinds[kind];
            defender->ex_mute_until=svm.moves+6;
            if(ex_muted(defender,AT_NONE,effects[kind]))break;
        }
        assert(j<count);
        for(trial=1;trial<=100;++trial) for(muted=0;muted<2;++muted) {
            svm.mvitals[PM_HOUSECAT].mvflags &= ~G_GONE;
            attacker=makemon(&mons[PM_HOUSECAT],defender->mx+1,defender->my,
                             NO_MINVENT | MM_NOMSG);
            assert(attacker);attacker->m_lev=50;
            attacker->mhp=attacker->mhpmax=10000;
            attacker->mpeaceful=defender->mpeaceful=FALSE;
            attacker->msleeping=defender->msleeping=FALSE;
            defender->ex_mute_until=muted?svm.moves+6:0;
            init_isaac64(232000UL+kind*100+trial,rn2);
            (void)mattackm(attacker,defender);
            if(muted) assert(!DEADMONSTER(attacker) && attacker->mcanmove
                              && !attacker->mfrozen && !defender->mcan);
            else if(DEADMONSTER(attacker) || !attacker->mcanmove || attacker->mfrozen)
                ++native_events[kind];
            if(!DEADMONSTER(attacker))mongone(attacker);
        }
        assert(native_events[kind]>10);
        mongone(defender);
        svm.mvitals[kinds[kind]]=saved[kind];
    }
    svm.mvitals[PM_HOUSECAT]=cat;
    for(o=fobj;o;o=next) {
        next=o->nobj;
        if(o->ox==u.ux+4 && o->oy==u.uy && o->otyp==STATUE
           && o->corpsenm==PM_HOUSECAT){obj_extract_self(o);obfree(o,NULL);}
    }
    puts("PASS Step22 T8 400 actual monster-versus-monster contacts: living cockatrice stoning and gelatinous paralysis occur in controls and selected mute suppresses them without global cancellation (232001..232200)");
}
