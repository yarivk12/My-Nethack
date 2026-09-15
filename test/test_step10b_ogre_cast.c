/* Reuse the established spell-effect fixture adapters, not game hooks. */
#define main step9c_spell_regressions
#include "test_step9c_elder_mm.c"
#undef main

int main(void)
{
    struct monst caster, target;
    struct attack *attack;
    int i;
    assert(step9c_spell_regressions() == 0);
    for (i=0; i<3; ++i) {
        caster=make_mon(PM_OGRE_MAGE,10); target=make_mon(PM_ORC,12);
        caster.m_lev=7; attack=&caster.data->mattk[1];
        selected=i==0 ? MCAST_PSI_BOLT : i==1 ? MCAST_HASTE_SELF : MCAST_CURE_SELF;
        fumble=magic_resist=resist_roll=0; selections=damage_calls=0;
        assert(mith_castmm(&caster,&target,attack) == M_ATTK_HIT);
        assert(selections==1 && damage_calls==1 && caster.mspec_used==3);
        if (i==0) assert(target.mhp==29);
        if (i==1) assert(caster.permspeed==MFAST && target.mhp==40);
        if (i==2) assert(caster.mhp==58 && target.mhp==40);
        /* Control-path tests need a still-useful spell after self-buffs. */
        selected=MCAST_PSI_BOLT;
        assert(mith_castmm(&caster,&target,attack) == M_ATTK_MISS);
        caster.mspec_used=0; caster.mcan=1;
        assert(mith_castmm(&caster,&target,attack) == M_ATTK_MISS);
        caster.mcan=0; fumble=1;
        assert(mith_castmm(&caster,&target,attack) == M_ATTK_MISS);
        assert(caster.mspec_used==3);
    }
    puts("PASS ogre mage actual spell dispatch: damage, haste, healing, cooldown, cancellation and fumble");
    return 0;
}
