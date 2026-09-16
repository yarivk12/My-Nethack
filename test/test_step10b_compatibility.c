/* Real enum declarations, historical save codecs, and version rejection.
 * Fixture IO only: no production hooks, maps, or branch registration. */
#include "hack.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

struct id_entry { int value; const char *name; };
#define DUMP_ENUMS
static const struct id_entry monster_ids[] = {
#include "monsters.h"
};
static const struct id_entry object_ids[] = {
#include "objects.h"
};
#undef DUMP_ENUMS
#define DUMP_ARTI_ENUM
static const struct id_entry artifact_ids[] = {
#include "artilist.h"
};
#undef DUMP_ARTI_ENUM

#ifndef STEP10B_BASELINE
struct restore_info restoreinfo;
struct instance_globals_c gc;
winid WIN_MESSAGE = WIN_ERR;
volatile struct window_procs windowprocs;
struct nomakedefs_s nomakedefs;
static FILE *payload;

void bwrite(int fd, const genericptr_t bytes, unsigned count)
{
    (void) fd;
    assert(fwrite(bytes, 1, count, payload) == count);
}
void mread(int fd, genericptr_t bytes, unsigned count)
{
    (void) fd;
    assert(fread(bytes, 1, count, payload) == count);
}
void pline(const char *fmt, ...) { (void) fmt; assert(0); }
void impossible(const char *fmt, ...) { (void) fmt; assert(0); }

/* MSVC's native UNUSED expands empty; production no-op normalizers use it. */
#pragma warning(push)
#pragma warning(disable: 4100)
#include "step10b_save_functions.h"
#pragma warning(pop)

static void save_roundtrip(void)
{
    struct you before = { 0 }, after = { 0 };
    struct levelflags level_before = { 0 }, level_after = { 0 };
    struct obj obj_before = { 0 }, obj_after = { 0 };
    NHFILE nhfp = { 0 };
    int sum, lethe;
    /* Existing adjacent bits and Step 9 state survive independently. */
    before.uevent.amulet_wish = 1;
    before.uevent.ascended = 1;
    before.uspare1 = 19;
    before.mith_slabs = 5;
    level_before.stormy = 1;
    level_before.fumaroles = 1;
    level_before.stasis_until = 123456L;
    obj_before.otyp = MIRRORBLADE;
    obj_before.oclass = WEAPON_CLASS;
    obj_before.quan = 1L;
    obj_before.owt = 40;
    obj_before.spe = 3;
    obj_before.obranch_material = GLASS;
    obj_before.obranch_size = MZ_SMALL + 1;
    obj_before.obranch_props = OBP_DEEP | OBP_CONCORDANT;
    for (sum = 0; sum <= 1; ++sum) {
        for (lethe = 0; lethe <= 1; ++lethe) {
            before.uevent.sum_entered = (unsigned) sum;
            level_before.lethe = (unsigned) lethe;
            payload = tmpfile();
            assert(payload);
            historical_sfo_you(&nhfp, &before, "you");
            historical_sfo_levelflags(&nhfp, &level_before, "level");
            historical_sfo_obj(&nhfp, &obj_before, "obj");
            assert(ftell(payload) == sizeof before + sizeof level_before
                                      + sizeof obj_before);
            rewind(payload);
            historical_sfi_you(&nhfp, &after, "you");
            historical_sfi_levelflags(&nhfp, &level_after, "level");
            historical_sfi_obj(&nhfp, &obj_after, "obj");
            assert(after.uevent.sum_entered == (unsigned) sum);
            assert(level_after.lethe == (unsigned) lethe);
            assert(!memcmp(&before, &after, sizeof before));
            assert(!memcmp(&level_before, &level_after, sizeof level_before));
            assert(!memcmp(&obj_before, &obj_after, sizeof obj_before));
            fclose(payload);
        }
    }
    puts("PASS native historical you/levelflags/object save codecs: all four state combinations");
}

static void topology_save_roundtrip(void)
{
    struct dungeon d_before[2] = { 0 }, d_after[2] = { 0 };
    struct branch b_before[3] = { 0 }, b_after[3] = { 0 };
    struct s_level l_before[5] = { 0 }, l_after[5] = { 0 };
    static const char *const variants[] = {
        "neulev", "leth-a-2", "leth-c-1", "leth-d-2", "nkai-a-1"
    };
    NHFILE nhfp = { 0 };
    int i;

    strcpy(d_before[0].dname, "Neutral Quest");
    d_before[0].entry_lev = 1;
    d_before[0].num_dunlevs = 8;
    d_before[0].depth_start = 199;
    strcpy(d_before[1].dname, "The Lost Cities");
    d_before[1].entry_lev = 2;
    d_before[1].num_dunlevs = 13;
    d_before[1].depth_start = 205;

    b_before[0].id = 30;
    b_before[0].type = BR_PORTAL;
    b_before[0].end1.dnum = 0;
    b_before[0].end1.dlevel = 199;
    b_before[0].end2.dnum = 15;
    b_before[0].end2.dlevel = 1;
    b_before[1].id = 31;
    b_before[1].type = BR_STAIR;
    b_before[1].end1.dnum = 15;
    b_before[1].end1.dlevel = 7;
    b_before[1].end2.dnum = 16;
    b_before[1].end2.dlevel = 2;
    b_before[2].id = 32;
    b_before[2].type = BR_STAIR;
    b_before[2].end1.dnum = 15;
    b_before[2].end1.dlevel = 4;
    b_before[2].end2.dnum = 15;
    b_before[2].end2.dlevel = 8;

    for (i = 0; i < SIZE(l_before); ++i) {
        strcpy(l_before[i].proto, variants[i]);
        l_before[i].dlevel.dnum = (xint16) (i ? 16 : 0);
        l_before[i].dlevel.dlevel = (xint16) (i ? (i == 1 ? 1
                                                   : i == 2 ? 3
                                                     : i == 3 ? 4 : 9)
                                                : 199);
    }

    payload = tmpfile();
    assert(payload);
    for (i = 0; i < SIZE(d_before); ++i)
        historical_sfo_dungeon(&nhfp, &d_before[i], "dungeon");
    for (i = 0; i < SIZE(b_before); ++i)
        historical_sfo_branch(&nhfp, &b_before[i], "branch");
    for (i = 0; i < SIZE(l_before); ++i)
        historical_sfo_s_level(&nhfp, &l_before[i], "s_level");
    rewind(payload);
    for (i = 0; i < SIZE(d_after); ++i)
        historical_sfi_dungeon(&nhfp, &d_after[i], "dungeon");
    for (i = 0; i < SIZE(b_after); ++i)
        historical_sfi_branch(&nhfp, &b_after[i], "branch");
    for (i = 0; i < SIZE(l_after); ++i)
        historical_sfi_s_level(&nhfp, &l_after[i], "s_level");
    assert(!memcmp(d_before, d_after, sizeof d_before));
    assert(!memcmp(b_before, b_after, sizeof b_before));
    assert(!memcmp(l_before, l_after, sizeof l_before));
    assert(b_after[0].end1.dlevel == 199);
    assert(b_after[2].end1.dlevel == 4 && b_after[2].end2.dlevel == 8);
    assert(!strcmp(l_after[0].proto, "neulev"));
    assert(!strcmp(l_after[1].proto, "leth-a-2"));
    assert(d_after[0].depth_start == 199 && d_after[1].depth_start == 205);
    fclose(payload);
    puts("PASS native topology save/restore: P, branches, alternates, Dispensary, depths");
}

static void epoch_gate(void)
{
    struct version_info version = { 0 };
    unsigned long epoch;
    nomakedefs.version_number = ((unsigned long) VERSION_MAJOR << 24)
        | ((unsigned long) VERSION_MINOR << 16)
        | ((unsigned long) PATCHLEVEL << 8) | EDITLEVEL;
    version.incarnation = nomakedefs.version_number;
    assert(check_version(&version, (const char *) 0, FALSE, 0));
    for (epoch = 0; epoch < EDITLEVEL; ++epoch) {
        version.incarnation = (nomakedefs.version_number & ~255UL) | epoch;
        assert(!check_version(&version, (const char *) 0, FALSE, 0));
        assert(!check_version(&version, (const char *) 0, FALSE, UTD_SKIP_SANITY1));
    }
    version.incarnation = nomakedefs.version_number + 1;
    assert(!check_version(&version, (const char *) 0, FALSE, 0));
    puts("PASS production check_version: prior epochs and future epoch rejected");
}
#endif

int main(void)
{
    int i;
    for (i = 0; i < SIZE(monster_ids); ++i)
        printf("ID monster %s %d\n", monster_ids[i].name, monster_ids[i].value);
    for (i = 0; i < SIZE(object_ids); ++i)
        printf("ID object %s %d\n", object_ids[i].name, object_ids[i].value);
    for (i = 0; i < SIZE(artifact_ids); ++i)
        printf("ID artifact %s %d\n", artifact_ids[i].name, artifact_ids[i].value);
#ifndef STEP10B_BASELINE
    _Static_assert(EDITLEVEL == 6, "Step 11 explicit room identity epoch");
    _Static_assert(MAXDUNGEON == 18 && MAXLINFO == 3600, "Step 10 capacity");
    _Static_assert(AFTER_LAST_ARTIFACT - 1 <= SCHAR_MAX, "saved artifact ID");
    _Static_assert(NUM_OBJECTS - 1 <= SHRT_MAX, "saved object ID");
    _Static_assert(MAX_GLYPH == 22 * NUMMONS + 2 * NUM_OBJECTS + 250,
                   "native glyph family arithmetic");
    printf("COUNTS NUMMONS=%d NUM_OBJECTS=%d ARTIFACT_SENTINEL=%d ARTIFACTS=%d MAX_GLYPH=%d\n",
           NUMMONS, NUM_OBJECTS, AFTER_LAST_ARTIFACT, NROFARTIFACTS, MAX_GLYPH);
    save_roundtrip();
    topology_save_roundtrip();
    epoch_gate();
#endif
    return 0;
}
