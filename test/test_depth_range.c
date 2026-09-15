/* Focused structural regression test for signed level/depth widening. */

#include "hack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_IS_INT(expr) _Generic((expr), int: 1, default: 0)
#define TYPE_IS(expr, type) _Generic((expr), type: 1, default: 0)

struct lchoice_capacity_layout {
    int idx;
    xint16 lev[MAXLINFO];
    int playerlev[MAXLINFO];
    xint16 dgn[MAXLINFO];
    char menuletter;
};

_Static_assert(sizeof(xint16) == 2, "xint16 must be two bytes");
_Static_assert(MAXLEVEL == 200, "Step 4 requires MAXLEVEL 200");
_Static_assert(MAXLINFO == 3600, "Step 10 MAXLINFO must scale to 3600");
_Static_assert(sizeof(struct lchoice_capacity_layout) == 28808,
               "unexpected print_dungeon lchoice stack layout");
_Static_assert(TYPE_IS_INT(depth((d_level *) 0)), "depth must return int");
_Static_assert(TYPE_IS_INT(deepest_lev_reached(FALSE)),
               "deepest_lev_reached must return int");
_Static_assert(TYPE_IS_INT(level_difficulty()),
               "level_difficulty must return int");
_Static_assert(TYPE_IS_INT(lev_by_name((const char *) 0)),
               "lev_by_name must return int");
_Static_assert(TYPE_IS(&print_dungeon,
                       int (*)(boolean, xint16 *, xint16 *)),
               "print_dungeon signature mismatch");
_Static_assert(TYPE_IS(&savelev, void (*)(NHFILE *, xint16)),
               "savelev signature mismatch");
_Static_assert(TYPE_IS(&getlev, void (*)(NHFILE *, int, xint16)),
               "getlev signature mismatch");

/* Minimal globals used by the linked production depth functions. */
NEARDATA struct you u;
struct instance_globals_saved_b svb;
struct instance_globals_saved_d svd;
struct instance_globals_saved_n svn;
NEARDATA winid WIN_MESSAGE = WIN_ERR;
struct sinfo program_state;

struct critical_sizes_with_names {
    uchar ucsize;
    const char *nm;
};

extern struct critical_sizes_with_names critical_sizes[];
extern int get_critical_size_count(void);
extern void historical_sfo_xint16(NHFILE *, xint16 *, const char *);
extern void historical_sfi_xint16(NHFILE *, xint16 *, const char *);

static int failures;

#define CHECK(cond, label)                                                    \
    do {                                                                      \
        if (!(cond)) {                                                        \
            fprintf(stderr, "FAIL: %s\n", label);                            \
            ++failures;                                                       \
        }                                                                     \
    } while (0)

void
panic(const char *fmt UNUSED, ...)
{
    abort();
}

void
impossible(const char *fmt UNUSED, ...)
{
    abort();
}

ATTRNORETURN void
nh_terminate(int status)
{
    exit(status);
}

ATTRNORETURN void
error(const char *fmt UNUSED, ...)
{
    abort();
}

int
delete_savefile(void)
{
    return 0;
}

int
nhclose(int fd)
{
    return close(fd);
}

int
rn2(int x UNUSED)
{
    return 0;
}

static unsigned long
current_incarnation(void)
{
    return ((unsigned long) VERSION_MAJOR << 24)
           | ((unsigned long) VERSION_MINOR << 16)
           | ((unsigned long) PATCHLEVEL << 8)
           | (unsigned long) EDITLEVEL;
}

static void
test_depths(void)
{
    static const int values[] = { 126, 127, 128, 129, 150, 200, 255 };
    d_level lev = { 0, 0 }, where = { 0, 126 };
    size_t i;

    memset(&u, 0, sizeof u);
    memset(&svb, 0, sizeof svb);
    memset(&svd, 0, sizeof svd);
    memset(&svn, 0, sizeof svn);

    svd.dungeons[0].depth_start = 1;
    svd.dungeons[0].entry_lev = 1;
    svd.dungeons[0].num_dunlevs = 255;
    astral_level.dnum = MAXDUNGEON - 1;

    for (i = 0; i < SIZE(values); ++i) {
        lev.dlevel = (xint16) values[i];
        CHECK(depth(&lev) == values[i], "physical depth boundary");
        u.uz = lev;
        CHECK(level_difficulty() == values[i],
              "level difficulty boundary");
    }

    svn.n_dgns = 1;
    svd.dungeons[0].dunlev_ureached = 255;
    CHECK(deepest_lev_reached(FALSE) == 255, "deepest depth 255");

    CHECK(!strcmp(level_distance(&where), "far above"),
          "level distance beyond +127");

    u.uz.dlevel = 126;
    where.dlevel = 255;
    CHECK(!strcmp(level_distance(&where), "far below"),
          "level distance beyond -127");
}

static void
test_ledgers(void)
{
    static const int values[] = { 126, 127, 128, 129, 150, 200, 254, 255 };
    d_level lev;
    size_t i;
    int count = 0;
    xint16 marker;

    memset(&svd, 0, sizeof svd);
    memset(&svn, 0, sizeof svn);
    svn.n_dgns = 2;
    svd.dungeons[0].ledger_start = 0;
    svd.dungeons[0].num_dunlevs = 200;
    svd.dungeons[1].ledger_start = 200;
    svd.dungeons[1].num_dunlevs = 55;

    for (i = 0; i < SIZE(values); ++i) {
        marker = (xint16) values[i];
        lev.dnum = ledger_to_dnum(marker);
        lev.dlevel = ledger_to_dlev(marker);
        CHECK(ledger_no(&lev) == marker, "ledger round trip");
    }

    for (marker = 127; marker <= 255; ++marker)
        ++count;
    CHECK(count == 129, "ledger iteration 127 through 255");
}

static void
test_marker_representation(void)
{
    static const int values[] = { -1, 0, 127, 128, 150, 200, 255 };
    FILE *fp = tmpfile();
    NHFILE nhfp;
    size_t i;
    xint16 marker;

    CHECK(fp != NULL, "open temporary marker stream");
    if (!fp)
        return;

    memset(&nhfp, 0, sizeof nhfp);
    nhfp.fd = fileno(fp);

    for (i = 0; i < SIZE(values); ++i) {
        marker = (xint16) values[i];
        historical_sfo_xint16(&nhfp, &marker, "test-marker");
    }
    CHECK(lseek(nhfp.fd, 0L, SEEK_CUR) == (long) (SIZE(values) * 2),
          "serialized marker width");
    (void) lseek(nhfp.fd, 0L, SEEK_SET);
    for (i = 0; i < SIZE(values); ++i) {
        marker = 0;
        historical_sfi_xint16(&nhfp, &marker, "test-marker");
        CHECK((int) marker == values[i], "marker signed round trip");
    }
    fclose(fp);
}

static void
test_filenames(void)
{
    static const int values[] = { 99, 100, 127, 128, 150, 200, 255 };
    char names[SIZE(values)][32];
    size_t i, j;

    for (i = 0; i < SIZE(values); ++i) {
        int len = snprintf(names[i], sizeof names[i], "player.%d", values[i]);

        CHECK(len > 0 && len < (int) sizeof names[i], "level filename fits");
        for (j = 0; j < i; ++j)
            CHECK(strcmp(names[i], names[j]) != 0,
                  "level filename suffix unique");
    }
}

static int
write_checkpoint(const char *filename, const char *recovered,
                 unsigned long incarnation)
{
    FILE *fp = fopen(filename, "wb");
    struct version_info version_data;
    char savename[SAVESIZE], indicator = 'h';
    char cscount = (char) get_critical_size_count();
    char plname[] = "T";
    int hpid = 1, savelev = 1, pltmpsiz = (int) sizeof plname;
    int i;

    if (!fp)
        return 0;
    memset(savename, 0, sizeof savename);
    snprintf(savename, sizeof savename, "%s", recovered);
    version_data.incarnation = incarnation;
    version_data.feature_set = 0UL;
    version_data.entity_count = 0UL;

    if (fwrite(&hpid, sizeof hpid, 1, fp) != 1
        || fwrite(&savelev, sizeof savelev, 1, fp) != 1
        || fwrite(savename, sizeof savename, 1, fp) != 1
        || fwrite(&indicator, sizeof indicator, 1, fp) != 1
        || fwrite(&cscount, sizeof cscount, 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    for (i = 0; i < (int) cscount; ++i)
        if (fwrite(&critical_sizes[i].ucsize, 1, 1, fp) != 1) {
            fclose(fp);
            return 0;
        }
    if (fwrite(&version_data, sizeof version_data, 1, fp) != 1
        || fwrite(&pltmpsiz, sizeof pltmpsiz, 1, fp) != 1
        || fwrite(plname, sizeof plname, 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    return fclose(fp) == 0;
}

static int
write_save_header(const char *filename, unsigned long incarnation)
{
    FILE *fp = fopen(filename, "wb");
    struct version_info version_data;
    char indicator = 'h', cscount = (char) get_critical_size_count();
    int i;

    if (!fp)
        return 0;
    version_data.incarnation = incarnation;
    version_data.feature_set = 0UL;
    version_data.entity_count = 0UL;
    if (fwrite(&indicator, sizeof indicator, 1, fp) != 1
        || fwrite(&cscount, sizeof cscount, 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    for (i = 0; i < (int) cscount; ++i)
        if (fwrite(&critical_sizes[i].ucsize, 1, 1, fp) != 1) {
            fclose(fp);
            return 0;
        }
    if (fwrite(&version_data, sizeof version_data, 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    return fclose(fp) == 0;
}

static int
write_level_payload(const char *filename, int payload)
{
    FILE *fp = fopen(filename, "wb");
    unsigned char byte = (unsigned char) payload;

    if (!fp)
        return 0;
    if (fwrite(&byte, sizeof byte, 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    return fclose(fp) == 0;
}

int
main(int argc, char **argv)
{
    test_depths();
    test_ledgers();
    test_marker_representation();
    test_filenames();

    if (argc == 2 && !strcmp(argv[1], "--fixtures")) {
        static const int levels[] = { 99, 100, 127, 128, 150, 200, 255 };
        char filename[32];
        size_t i;

        CHECK(write_checkpoint("oldlock.0", "old-recovered",
                               current_incarnation() - 1),
              "write old checkpoint fixture");
        CHECK(write_checkpoint("newlock.0", "new-recovered",
                               current_incarnation()),
              "write new checkpoint fixture");
        CHECK(write_save_header("oldsave.NetHack-saved-game",
                                current_incarnation() - 1),
              "write old save fixture");
        CHECK(write_level_payload("newlock.1", 'C'),
              "write current level payload");
        for (i = 0; i < SIZE(levels); ++i) {
            snprintf(filename, sizeof filename, "newlock.%d", levels[i]);
            CHECK(write_level_payload(filename, (int) i + 1),
                  "write high level payload");
        }
    }

    if (failures) {
        fprintf(stderr, "%d depth-range test(s) failed\n", failures);
        return EXIT_FAILURE;
    }
    printf("depth-range tests passed (incarnation 0x%08lx)\n",
           current_incarnation());
    return EXIT_SUCCESS;
}
