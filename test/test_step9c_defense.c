#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct you u;
struct instance_globals_y gy;
struct obj *uarm, *uarmc, *uarmu, *uarmh, *uarmg, *uarmf;
static int roll, rolls;
int rn2(int n) { assert(n > 0); ++rolls; return roll % n; }
int rnd(int n) { return rn2(n) + 1; }
struct obj *which_armor(struct monst *m, long mask) {
    struct obj *o;
    for (o = m->minvent; o; o = o->nobj)
        if (o->owornmask & mask) return o;
    return 0;
}
#include "step9c_defense.h"

int main(void) {
    struct monst mon = { 0 };
    struct obj armor = { 0 }, weapon = { 0 };
    int i, sum, bits, permanent;
    monst_globals_init(); objects_globals_init();
    for(i=0;i<90;++i) {
        int baseline,boosted;
        mon.data=&mons[PM_HUMAN];mon.mhp=1000;rolls=0;
        kick_commit(&mon,i);baseline=mon.mhp;
        assert(baseline==1000-i&&!rolls);
        u.mith_timers[MITH_AESH]=1;mon.mhp=1000;
        kick_commit(&mon,i);boosted=mon.mhp;
        assert(boosted==baseline-10&&!rolls);
        u.mith_timers[MITH_AESH]=0;
        mon.data=&mons[PM_LIVING_MIRAGE];mon.mhp=1000;
        kick_commit(&mon,i);assert(mon.mhp==1000-(i>0));
    }
    puts("PASS actual kick damage commit: Aesh, mirage defense and native no-effect damage/RNG");
    u.umonnum = PM_HUMAN; gy.youmonst.data = &mons[PM_HUMAN];
    mon.data = &mons[PM_HUMAN]; weapon.otyp = LONG_SWORD;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 17) == 17);
    assert(!rolls);
    armor.otyp = PLATE_MAIL; armor.spe = 7; armor.owornmask = W_ARM;
    mon.minvent = &armor;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 17) == 17);
    assert(!rolls && !mith_armor_dr(&armor));
    puts("PASS native bodies/equipment: unchanged damage and RNG");

    armor.otyp = LIVING_ARMOR; armor.spe = 0;
    assert(mith_armor_dr(&armor) == 2);
    armor.obranch_material = MINERAL;
    assert(mith_armor_dr(&armor) == 4);
    armor.oeroded = 1; armor.spe = 3;
    assert(mith_armor_dr(&armor) == 5);
    armor.obranch_material = 0; armor.oeroded = 0; armor.spe = -3;
    assert(mith_armor_dr(&armor) == 1);
    armor.spe = 0;
    for (i = 0; i < 5; ++i) {
        roll = i; assert(mith_roll_dr(&mon) == (i < 2 ? 2 : 0));
    }
    mon.minvent = 0;
    for (i = 0; i < 18; ++i) {
        int slot;
        u.mith_syllables[5] = i;
        for (slot = sum = 0; slot < 5; ++slot) {
            roll = slot; sum += mith_roll_dr(&gy.youmonst);
        }
        assert(sum == i); /* exact aggregate of the five donor body slots */
    }
    u.mith_syllables[5] = 0;
    mon.data = &mons[PM_ALABASTER_MUMMY];
    mon.mspare1 = 6L << MITH_SYLLABLE_SHIFT;
    assert(mith_roll_dr(&mon) == 14);
    mon.mspare1 = 0; assert(mith_roll_dr(&mon) == 4);
    mon.data = &mons[PM_ASPECT_OF_THE_SILENCE];
    assert(mith_roll_dr(&mon) == 6);
    mon.mcan = 1; assert(mith_roll_dr(&mon) == 3); mon.mcan = 0;
    puts("PASS imported armor material/erosion/enchantment, five locations, Vaul and natural/aura DR");

    mon.data = &mons[PM_CRYSTAL_OOZE]; weapon.otyp = LONG_SWORD;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 40);
    weapon.otyp = MACE;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 5);
    mon.data = &mons[PM_SENTINEL_OF_MITHARDIR];
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 40);
    weapon.otyp = LONG_SWORD;
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 20) == 5);
    mon.data = &mons[PM_ASPECT_OF_THE_SILENCE];
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 40) == 4);
    mon.data = &mons[PM_LIVING_MIRAGE];
    assert(mith_physical_damage(&mon, &weapon, AT_WEAP, 40) == 1);
    assert(mith_physical_damage(&mon, 0, AT_BITE, 40) == 40);
    puts("PASS donor slash/blunt vulnerabilities, quarter resistance, mirage weapon immunity");
    {
        int dead,physical,skilled,expected;
        mon.data=&mons[PM_SENTINEL_OF_MITHARDIR];weapon.otyp=LONG_SWORD;
        for(dead=0;dead<2;++dead)for(physical=0;physical<2;++physical)
          for(skilled=0;skilled<2;++skilled) {
            expected=dead||(!physical&&!skilled) ? 20
              : mith_physical_damage(&mon,skilled?&weapon:0,AT_WEAP,20);
            assert(hero_hit(&mon,&weapon,20,skilled,physical,dead)==expected);
          }
        mon.data=&mons[PM_HUMAN];rolls=0;
        assert(hero_hit(&mon,0,20,FALSE,TRUE,FALSE)==20&&!rolls);
        assert(hero_hit(&mon,0,0,FALSE,FALSE,FALSE)==0&&!rolls);
        puts("PASS improvised physical hit defense, blunt classification, consumed object safety and native no-op");
    }

    for (permanent = 0; permanent <= 1; ++permanent)
        for (bits = 0; bits < 32; ++bits) {
            boolean light = (bits & TEMP_LIT) != 0;
            boolean dark = (bits & MITH_DARK1) != 0;
            boolean inner = (bits & MITH_DARK2) != 0;
            boolean expected = light ? !dark || (permanent && !inner)
                                     : permanent && !dark && !inner;
            assert(!!mith_viz_lit(permanent, bits) == expected);
        }
    puts("PASS complete ordinary-sight darkness/light overlap truth table");
    {
        int species,dam,kind,expected,savedrolls,cases=0;
        static const int targets[]={PM_HUMAN,PM_ALABASTER_MUMMY,
            PM_CRYSTAL_OOZE,PM_SENTINEL_OF_MITHARDIR,PM_ASPECT_OF_THE_SILENCE};
        static const int missiles[]={ARROW,DAGGER,BOULDER,ACID_VENOM};
        for(species=0;species<SIZE(targets);++species)
            for(kind=0;kind<SIZE(missiles);++kind)
                for(dam=0;dam<=100;++dam) for(roll=0;roll<5;++roll) {
                    mon.data=&mons[targets[species]];weapon.otyp=missiles[kind];
                    mon.minvent=&armor;armor.spe=3;armor.otyp=BARNACLE_ARMOR;
                    rolls=0;
                    expected=kind==3 ? dam : mith_physical_damage(&mon,&weapon,AT_WEAP,dam);
                    savedrolls=rolls;rolls=0;
                    assert(missile_mon(&mon,&weapon,dam)==expected && rolls==savedrolls);
                    gy.youmonst.data=mon.data;u.umonnum=targets[species];uarm=&armor;
                    rolls=0;
                    expected=kind==3 ? dam : mith_physical_damage(&gy.youmonst,&weapon,AT_WEAP,dam);
                    savedrolls=rolls;rolls=0;
                    assert(missile_hero(&weapon,dam,kind==3)==expected && rolls==savedrolls);
                    cases+=2;
                }
        printf("PASS %d actual hero/monster projectile dispatch cases: scoped DR/resistance, acid exclusion and single RNG application\n",cases);
    }
    return 0;
}
