#include "hack.h"
#include <assert.h>
#include <stdio.h>
struct you u;
static int half_spell, half_phys, invulnerable, abstyp, adtyp;
static int antimagic, half_gas, blast, cloud;
#undef Half_spell_damage
#undef Half_physical_damage
#undef Invulnerable
#undef Maybe_Half_Phys
#undef You
#undef Antimagic
#undef Half_gas_damage
#define Half_spell_damage half_spell
#define Half_physical_damage half_phys
#define Invulnerable invulnerable
#define Maybe_Half_Phys(n) (half_phys ? ((n)+1)/2 : (n))
#define You(...) ((void)0)
#define Antimagic antimagic
#define Half_gas_damage half_gas
#include "step9c_vaul.h"
int main(void)
{
    int damage, timer, type, total=0, spell, spellcases=0, pathcases=0;
    static const int types[]={AD_PHYS,AD_ACID,AD_FIRE,AD_COLD,AD_ELEC,AD_MAGM};
    for(damage=0; damage<=1024; ++damage)
        for(timer=0; timer<2; ++timer) {
            u.mith_timers[MITH_VAUL]=timer;
            for(half_spell=0; half_spell<2; ++half_spell)
                for(abstyp=0; abstyp<30; abstyp+=10) {
                    assert(local_ray(damage)==donor_ray(damage)); ++total;
                }
            for(half_phys=0; half_phys<2; ++half_phys) {
                assert(local_rust(damage)==donor_rust(damage)); ++total;
                for(invulnerable=0; invulnerable<2; ++invulnerable)
                    for(type=0; type<SIZE(types); ++type) {
                        adtyp=types[type];
                        assert(local_blast(damage)==donor_blast(damage)); ++total;
                    }
            }
        }
    printf("PASS %d real beam/explosion/rust damage-path comparisons including stacked protections, zero damage and invulnerability\n",total);
    for(damage=0;damage<=1024;++damage)
        for(timer=0;timer<2;++timer)
            for(half_spell=0;half_spell<2;++half_spell)
                for(half_phys=0;half_phys<2;++half_phys) {
                    u.mith_timers[MITH_VAUL]=timer;abstyp=0;
                    for(spell=0;spell<SIZE(rerolled_spells);++spell) {
                        assert(rerolled_spells[spell](damage)==
                               (spell==2 ? donor_rust(damage) : donor_ray(damage)));
                        ++spellcases;
                    }
                    assert(vaul_blind()==200/(1+half_spell)/(1+timer));
                    assert(vaul_psychic(damage)==donor_ray(damage));
                    assert(vaul_striking(damage)==donor_ray(damage));
                    assert(vaul_piercer(damage)==donor_rust(damage));
                    assert(vaul_swallowed(damage)==donor_rust(damage));
                    assert(vaul_bag(damage)==donor_rust(damage));
                    pathcases += 5;
                    for(half_gas=0;half_gas<2;++half_gas)
                        for(blast=0;blast<2;++blast)
                            for(cloud=0;cloud<2;++cloud) {
                                int expected=damage;
                                if ((blast || cloud) && half_gas)
                                    expected=(expected+1)/2;
                                if(timer) expected=(expected+1)/2;
                                assert(vaul_poison(damage)==expected);
                                ++pathcases;
                            }
                    for(antimagic=0;antimagic<2;++antimagic)
                        assert(vaul_curse_bound()==6/(antimagic+half_spell+timer+1));
                }
    printf("PASS %d actual rerolled spell modifier comparisons and stacked blindness duration\n",spellcases);
    printf("PASS %d actual psychic/striking/piercer/swallowed/bag/poison damage cases and donor curse bounds\n",pathcases);
    return 0;
}
