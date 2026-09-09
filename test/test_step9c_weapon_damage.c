#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t rng;
int rn2(int n) { assert(n > 0); rng=rng*1664525U+1013904223U; return (rng>>8)%n; }
int rnd(int n) { return rn2(n)+1; }
int d(int n, int sides) { int sum=0; while(n-- > 0) sum+=rnd(sides); return sum; }
#define is_lightsaber(o) FALSE
#define litsaber(o) FALSE
#define is_bludgeon(o) ((objects[(o)->otyp].oc_dir & WHACK) != 0)
#define is_slashing(o) ((objects[(o)->otyp].oc_dir & SLASH) != 0)
#define is_stabbing(o) ((objects[(o)->otyp].oc_dir & PIERCE) != 0)
#include "step9c_weapon_damage.h"

int main(void)
{
    struct obj obj = {0};
    int i, size, material, large, phase, seed, actual, expected, total=0;
    uint32_t after;
    objects_globals_init(); obj.oclass=WEAPON_CLASS;
    for(i=0; i<SIZE(tested_types); ++i) {
        obj.otyp=tested_types[i];
        for(size=0; size<=MZ_GIGANTIC+1; ++size)
            for(material=0; material<=SHELL; ++material)
                for(large=0; large<2; ++large)
                    for(phase=0; phase<(obj.otyp==MOON_AXE ? 5 : 1); ++phase)
                        for(seed=1; seed<=16; ++seed) {
                            obj.obranch_size=size; obj.obranch_material=material;
                            obj.usecount=phase; obj.spe=seed-8;
                            rng=seed; actual=mith_weapon_dice(&obj,large); after=rng;
                            rng=seed; expected=donor_dice(&obj,large);
                            if(actual!=expected || rng!=after) {
                                fprintf(stderr,"dice mismatch type=%d size=%d material=%d large=%d phase=%d seed=%d local=%d donor=%d\n",
                                        obj.otyp,size,material,large,phase,seed,actual,expected);
                                return 1;
                            }
                            ++total;
                        }
    }
    printf("PASS %d pinned donor weapon-dice/RNG comparisons across sizes, materials, targets, phases and enchantments\n",total);
    return 0;
}
