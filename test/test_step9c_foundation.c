/* Compile real type tables; guard index, generation, and saved-state boundaries. */
#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct you u;
struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
static boolean spot_occupied;
boolean occupied(coordxy x, coordxy y) {
    (void) x; (void) y; return spot_occupied;
}
static int roll;
int rn2(int n) { assert(n > 0); return roll++ % n; }
void impossible(const char *msg, ...) { (void) msg; assert(0); }
char *simpleonames(struct obj *obj) { (void) obj; return "fixture"; }
int eaten_stat(int n, struct obj *obj) { (void) obj; return n; }
#include "step9c_foundation.h"

int main(void)
{
    const int imported[] = {
        PM_CRYSTAL_OOZE, PM_DEEP_ONE, PM_DEEPER_ONE, PM_DEEPEST_ONE,
        PM_SELKIE, PM_SEAL, PM_OCEANID,
        PM_YURIAN, PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN,
        PM_MOTE_OF_LIGHT, PM_WATER_DOLPHIN, PM_SINGING_SAND, PM_LIVING_MIRAGE,
        PM_WRAITHWORM, PM_ALABASTER_ELF, PM_ALABASTER_ELF_ELDER,
        PM_SENTINEL_OF_MITHARDIR, PM_ALABASTER_MUMMY, PM_FIRST_WRAITHWORM,
        PM_ASPECT_OF_THE_SILENCE
    };
    const int expected[] = { 167, 166, 167, 167, 167, 166 };
    int counts[6] = { 0 }, i, mask, typ, n, sum, active;
    struct obj gear = { 0 };

    monst_globals_init();
    objects_globals_init();
    for (typ = 0; typ < MAX_TYPE; ++typ) {
        levl[10][10].typ = (schar) typ;
        spot_occupied = FALSE;
        assert(bad_location(10, 10, 0, 0, 0, 0)
               == !(typ == ROOM || typ == AIR || typ == PUDDLE
                    || typ == SAND || typ == SOIL || typ == GRASS));
        assert(bad_location(10, 10, 9, 9, 11, 11));
        spot_occupied = TRUE;
        assert(bad_location(10, 10, 0, 0, 0, 0));
    }
    spot_occupied = FALSE;
    svl.level.flags.is_maze_lev = TRUE;
    levl[10][10].typ = CORR;
    assert(!bad_location(10, 10, 0, 0, 0, 0));
    puts("PASS portal terrain eligibility, occupied/excluded rejection and native maze corridors");
    assert(SIZE(terrain_descr) == xWATERWALL + 1);
    assert(SIZE(type_names) == MAX_TYPE);
    for (i = 0; i < MAX_TYPE; ++i) {
        assert(type_names[i] && *type_names[i]);
        assert(levltyp[i] && *levltyp[i]);
        assert(terrain_descr[i] && *terrain_descr[i]);
    }
    assert(!strcmp(terrain_descr[ICEWALL], "Ice-wall"));
    assert(!strcmp(terrain_descr[CRYSTALICEWALL], "Crystal-ice-wall"));
    assert(!strcmp(terrain_descr[xFLOOR], "Floor"));
    assert(!strcmp(terrain_descr[xWATERWALL], "WaterWall"));
    assert(!strcmp(levltyp[PUDDLE], "shallow water"));
    assert(!strcmp(type_names[GRASS], "GRASS"));
    assert(PUDDLE != BOG && ICED_PUDDLE != ICED_BOG);
    puts("PASS complete terrain/status/debug lookup tables and distinct shallow water");
    for (i = 0; i < SIZE(imported); ++i) {
        assert(imported[i] < SPECIAL_PM);
        assert(mons[imported[i]].geno & G_NOGEN);
        assert(mons[imported[i]].pmidx == imported[i]);
    }
    assert(mons[PM_FIRST_WRAITHWORM].geno & G_UNIQ);
    assert(!(mons[PM_ASPECT_OF_THE_SILENCE].geno & G_UNIQ));
    assert(mith_anhydrous(&mons[PM_SENTINEL_OF_MITHARDIR]));
    assert(mith_watery(&mons[PM_WATER_DOLPHIN]));
    assert(!mith_anhydrous(&mons[PM_ALABASTER_ELF]));
    puts("PASS all twenty-two imported species enabled, outside quest range, and NOGEN");
    for (i = 0; i < 1000; ++i)
        ++counts[mith_tile_type() - SYLLABLE_OF_STRENGTH__AESH];
    for (i = 0; i < 6; ++i) {
        assert(counts[i] == expected[i]);
        typ = SYLLABLE_OF_STRENGTH__AESH + i;
        assert(objects[typ].oc_prob == 0 && objects[typ].oc_weight == 3);
        assert(objects[typ].oc_cost == 300 && objects[typ].oc_material == MINERAL);
    }
    for (mask = 0; mask < 8; ++mask) {
        u.mith_slabs = mask;
        for (i = 0; i < 18; ++i) {
            typ = mith_slab_type();
            if (mask == 7) assert(typ == STRANGE_OBJECT);
            else {
                assert(typ >= FIRST_WORD && typ <= NURTURING_WORD);
                assert(!(mask & (1 << (typ - FIRST_WORD))));
                assert(objects[typ].oc_unique && objects[typ].oc_nowish);
            }
            assert(u.mith_slabs == (unsigned) mask);
        }
    }
    puts("PASS exact syllable weights and all eight slab uniqueness states");
    for (n = 0; n <= 32767; n += 97) {
        sum = 0;
        for (i = 1; i <= 90; ++i) {
            svm.moves = 999999900L + i;
            sum += mith_regen_increment(n);
        }
        assert(sum == n);
    }
    for (active = 0; active <= 1; ++active)
        for (n = 0; n < 300; ++n) {
            u.mith_syllables[MITH_AESH] = n;
            u.mith_timers[MITH_AESH] = active;
            roll = 0;
            sum = 0;
            for (i = 0; i < 3; ++i)
                sum += mith_aesh_bonus();
            assert(sum == n + (active ? 30 : 0));
        }
    puts("PASS exact Aesh rounding and 90-turn regeneration at late-game turn limits");
    gear.otyp = QUARTERSTAFF;
    gear.oclass = WEAPON_CLASS;
    gear.quan = 1;
    assert(weight(&gear) == (int) objects[QUARTERSTAFF].oc_weight);
    gear.obranch_material = METAL;
    assert(obj_material(&gear) == METAL && !is_rustprone(&gear));
    n = objects[QUARTERSTAFF].oc_weight * 70 / 30;
    assert(weight(&gear) == n);
    gear.obranch_size = MZ_LARGE + 1;
    assert(weight(&gear) == n * 2);
    gear.obranch_size = MZ_HUGE + 1;
    assert(weight(&gear) == n * 3);
    gear.otyp = MASK;
    gear.oclass = TOOL_CLASS;
    gear.obranch_material = MINERAL;
    gear.obranch_size = 0;
    assert(weight(&gear) == (int) objects[MASK].oc_weight * 50 / 15);
    gear.otyp = MOON_AXE;
    gear.oclass = WEAPON_CLASS;
    gear.obranch_material = 0;
    for (i = 0; i <= 4; ++i) {
        gear.usecount = i;
        assert(weight(&gear) == (int) objects[MOON_AXE].oc_weight / 4 * max(1, i));
    }
    puts("PASS donor material densities, oversized gear and all five moon-axe phase weights");
    return 0;
}
