/* Step 4.5: production function bodies, synthetic topology and file fixtures.
 * Run with run_ledger_runtime.ps1 from a Visual Studio developer shell.
 * Recovery payloads are opaque test bytes, not playable game saves.
 */
#include "hack.h"
#include "dgn_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef F_OK
#define F_OK 0
#endif

NEARDATA struct you u;
NEARDATA struct flag flags;
struct instance_globals_saved_b svb;
struct instance_globals_saved_d svd;
struct instance_globals_saved_l svl;
struct instance_globals_saved_n svn;
struct instance_globals_l gl;
struct instance_globals_s gs;
struct sinfo program_state;
uchar cscbuf[1];
static int failures;
static const int levels[] = { 1, 127, 128, 254, 255, 256, 257, 511,
                             512, 1024, 2048, MAXLINFO - 1 };
static int opened[MAXLINFO], largest_open;
static const char *output_name;

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); ++failures; \
} } while (0)
_Static_assert(MAXLEVEL == 200 && MAXLINFO == 3600, "Step 10 dungeon capacity; unchanged DoD");
_Static_assert(MAXULEV == 30 && EDITLEVEL == 5, "Step 3/4 limits; Step 10 stored-ID save epoch");
_Static_assert(sizeof(xint16) == 2 && LONG_MAX >= 0x7fff7fffL,
               "ledger pair carrier");

void panic(const char *fmt, ...) { (void) fmt; abort(); }
void raw_printf(const char *fmt, ...) { (void) fmt; }
static mapseen *find_mapseen_by_str(const char *s) { (void) s; return 0; }
s_level *find_level(const char *s) { (void) s; return 0; }
boolean In_V_tower(d_level *lev) { return (boolean) (lev->dnum == tower_dnum); }
char *eos(char *s) { return s + strlen(s); }
char *strstri(const char *s, const char *needle)
{
    size_t n = strlen(needle);
    for (; *s; ++s) if (!_strnicmp(s, needle, n)) return (char *) s;
    return 0;
}

static unsigned long incarnation(void)
{
    return ((unsigned long) VERSION_MAJOR << 24)
           | ((unsigned long) VERSION_MINOR << 16)
           | ((unsigned long) PATCHLEVEL << 8) | EDITLEVEL;
}
int get_critical_size_count(void) { return 0; }
boolean check_version(struct version_info *v, const char *s, boolean b,
                      unsigned long mask)
{
    (void) s; (void) b; (void) mask;
    return (boolean) (v->incarnation == incarnation());
}
void set_levelfile_name(char *name, int lev)
{
    char *p = strrchr(name, '.');
    if (!p) p = name + strlen(name);
    sprintf(p, ".%d", lev);
}
static NHFILE *new_file(int fd)
{
    NHFILE *f;
    if (fd < 0) return 0;
    f = calloc(1, sizeof *f);
    if (!f) abort();
    f->fd = fd;
    f->structlevel = TRUE;
    return f;
}
NHFILE *open_levelfile(int lev, char *err)
{
    (void) err;
    CHECK(lev >= 0 && lev < MAXLINFO);
    if (lev < 0 || lev >= MAXLINFO) return 0;
    ++opened[lev];
    if (lev > largest_open) largest_open = lev;
    set_levelfile_name(gl.lock, lev);
    return new_file(_open(gl.lock, _O_RDONLY | _O_BINARY));
}
void close_nhfile(NHFILE *f) { _close(f->fd); free(f); }
const char *fqname(const char *s, int prefix, int which)
{ (void) prefix; (void) which; return s; }
void set_savefile_name(boolean b) { (void) b; strcpy(gs.SAVEF, output_name); }
NHFILE *create_savefile(void)
{ return new_file(_open(output_name, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE)); }
int delete_savefile(void) { return _unlink(output_name); }
void delete_levelfile(int lev)
{ set_levelfile_name(gl.lock, lev); (void) _unlink(gl.lock); }
void bufon(int fd) { (void) fd; }
void bufoff(int fd) { (void) fd; }
static void put(int fd, const void *p, unsigned n)
{ CHECK(_write(fd, p, n) == (int) n); }
void sfo_int(NHFILE *f, int *p, const char *s)
{ (void) s; put(f->fd, p, sizeof *p); }
void sfo_char(NHFILE *f, char *p, const char *s, int n)
{ (void) s; put(f->fd, p, (unsigned) n); }
void store_version(NHFILE *f)
{
    char indicator = 'h', count = 0;
    struct version_info v = { 0 };
    v.incarnation = incarnation();
    put(f->fd, &indicator, 1); put(f->fd, &count, 1);
    put(f->fd, &v, sizeof v);
}
boolean copy_bytes(int from, int to)
{
    char buf[128];
    int n;
    while ((n = _read(from, buf, sizeof buf)) > 0)
        if (_write(to, buf, n) != n) return FALSE;
    return (boolean) (n == 0);
}

DISABLE_WARNING_UNREACHABLE_CODE
#include "ledger_runtime_functions.h"
RESTORE_WARNING_UNREACHABLE_CODE

static long pair(int a, int b)
{
    static branch br;
    memset(&svd, 0, sizeof svd);
    memset(&br, 0, sizeof br);
    br.end1.dnum = 0; br.end1.dlevel = (xint16) a;
    br.end2.dnum = 1; br.end2.dlevel = (xint16) b;
    strcpy(svd.dungeons[1].dname, "The Test Branch");
    svb.branches = &br;
    return find_branch("Test Branch", 0);
}
static void test_pairs(void)
{
    static const int vals[] = { 0, 1, 127, 128, 254, 255, 256, 257, 511,
                               512, 1024, 2048, 3199, 3200, 3599, 3600, 32767 };
    static const int pairs[][2] = { {1,254}, {1,255}, {1,256}, {2,1},
        {127,255}, {128,1}, {255,255}, {255,256}, {256,1}, {256,256},
        {511,512}, {512,1024} };
    size_t i, j;
    long prev = -1;
    struct proto_dungeon pd = { 0 };
    for (i = 0; i < SIZE(vals); ++i)
        for (j = 0; j < SIZE(vals); ++j) {
            long k = pair(vals[i], vals[j]);
            CHECK(k >= 0);
            CHECK(((k >> 16) & 0xffffL) == vals[i]);
            CHECK((k & 0xffffL) == vals[j]);
        }
    for (i = 0; i < SIZE(pairs); ++i) {
        long k = pair(pairs[i][0], pairs[i][1]);
        CHECK(k > prev); prev = k;
    }
    CHECK(find_branch("unknown", 0) == -1L);
    pd.n_brs = 2;
    pd.tmpbranch[0].name = "first";
    pd.tmpbranch[1].name = "second";
    CHECK(find_branch("first", &pd) == 0);
    CHECK(find_branch("second", &pd) == 1);
    puts("PASS: 225 packed pairs, requested numeric order, -1 and prototype indices");
}
static void test_navigation(void)
{
    static branch br;
    int i;
    size_t j;
    memset(&svd, 0, sizeof svd);
    memset(&svl, 0, sizeof svl);
    svn.n_dgns = MAXDUNGEON;
    for (i = 0; i < MAXDUNGEON; ++i) {
        svd.dungeons[i].ledger_start = i * MAXLEVEL;
        svd.dungeons[i].num_dunlevs = MAXLEVEL;
        svd.dungeons[i].depth_start = i * MAXLEVEL + 1;
    }
    --svd.dungeons[MAXDUNGEON - 1].num_dunlevs;
    CHECK(maxledgerno() == MAXLINFO - 1);
    medusa_level.dnum = 0; valley_level.dnum = 1;
    tower_dnum = MAXDUNGEON - 1;
    svb.branches = &br;
    for (j = 0; j < SIZE(levels); ++j) {
        int a = levels[j], b = a > 200 ? 128 : 257;
        d_level end = { ledger_to_dnum((xint16) a), ledger_to_dlev((xint16) a) };
        CHECK(ledger_no(&end) == a); CHECK(depth(&end) == a);
        br.end1 = end;
        br.end2.dnum = ledger_to_dnum((xint16) b);
        br.end2.dlevel = ledger_to_dlev((xint16) b);
        strcpy(svd.dungeons[br.end2.dnum].dname, "The Test Branch");
        u.uz = br.end1; wizard = TRUE;
        CHECK(lev_by_name("Test Branch") == a);
        CHECK(lev_by_name("branch to Test Branch") == a);
        u.uz = br.end2;
        CHECK(lev_by_name("Test Branch") == b);
        wizard = FALSE;
        svl.level_info[a].flags = svl.level_info[b].flags = 0;
        CHECK(lev_by_name("Test Branch") == 0);
        svl.level_info[a].flags = VISITED;
        CHECK(lev_by_name("Test Branch") == 0);
        svl.level_info[b].flags = VISITED;
        CHECK(lev_by_name("Test Branch") == b);
        u.uz = br.end1;
        CHECK(lev_by_name("Test Branch") == a);
    }
    puts("PASS: actual named teleport decoder, both endpoints, visibility, ledger/depth round trips through 3199");
}
static void payload(const char *name, unsigned char value)
{
    FILE *f = fopen(name, "wb");
    CHECK(f != 0); if (!f) return;
    CHECK(fwrite(&value, 1, 1, f) == 1); fclose(f);
}
static void fixture(const char *base, const char *save, int current, boolean old)
{
    char name[128], saved[SAVESIZE] = { 0 }, indicator = 'h', count = 0;
    char player[] = "T";
    int pid = 1, playerlen = sizeof player;
    struct version_info v = { 0 };
    FILE *f;
    size_t i;
    sprintf(name, "%s.0", base); strcpy(saved, save);
    v.incarnation = incarnation() - (old ? 1 : 0);
    f = fopen(name, "wb"); CHECK(f != 0); if (!f) return;
#define OUT(x) CHECK(fwrite(&(x), sizeof(x), 1, f) == 1)
    OUT(pid); OUT(current); OUT(saved); OUT(indicator); OUT(count);
    OUT(v); OUT(playerlen); OUT(player);
#undef OUT
    fputc('G', f); fclose(f);
    for (i = 0; i < SIZE(levels); ++i) {
        sprintf(name, "%s.%d", base, levels[i]);
        payload(name, (unsigned char) (i + 1));
    }
    /* Must not copy or clean a file beyond the shared storage bound. */
    sprintf(name, "%s.%d", base, MAXLINFO); payload(name, 'X');
}
static void verify(const char *base, const char *save, int current)
{
    FILE *f = fopen(save, "rb");
    char name[128];
    size_t i, current_index = 0;
    xint16 marker;
    CHECK(f != 0); if (!f) return;
    CHECK(fseek(f, 2L + sizeof(struct version_info) + sizeof(int) + 2L, SEEK_SET) == 0);
    for (i = 0; i < SIZE(levels); ++i) if (levels[i] == current) current_index = i;
    CHECK(fgetc(f) == (int) current_index + 1);
    CHECK(fgetc(f) == 'G');
    for (i = 0; i < SIZE(levels); ++i) {
        if (levels[i] != current) {
            CHECK(fread(&marker, sizeof marker, 1, f) == 1);
            CHECK(marker == levels[i]); CHECK(fgetc(f) == (int) i + 1);
        }
        sprintf(name, "%s.%d", base, levels[i]); CHECK(_access(name, 0) != 0);
    }
    CHECK(fgetc(f) == EOF); fclose(f);
    sprintf(name, "%s.0", base); CHECK(_access(name, 0) != 0);
    sprintf(name, "%s.%d", base, MAXLINFO); CHECK(_access(name, 0) == 0);
}
static void test_internal(void)
{
    static const int invalid[] = { -1, 0, MAXLINFO, 32767 };
    size_t i;
    for (i = 0; i < SIZE(levels); ++i) {
        char base[64], save[64];
        sprintf(base, "internal%d", levels[i]); sprintf(save, "%s.sav", base);
        fixture(base, save, levels[i], FALSE);
        strcpy(gl.lock, base); output_name = save;
        memset(opened, 0, sizeof opened); largest_open = -1;
        CHECK(recover_savefile());
        CHECK(largest_open == MAXLINFO - 1);
        CHECK(opened[0] == 1 && opened[levels[i]] == 1);
        CHECK(opened[256] == 1 && opened[3199] == 1);
        verify(base, save, levels[i]);
    }
    for (i = 0; i < SIZE(invalid); ++i) {
        fixture("invalid", "invalid.sav", invalid[i], FALSE);
        strcpy(gl.lock, "invalid"); output_name = "invalid.sav";
        CHECK(!recover_savefile()); CHECK(_access("invalid.0", 0) == 0);
        CHECK(_access("invalid.sav", 0) != 0);
    }
    puts("PASS: actual internal recovery, 12 current ledgers, high files/markers/cleanup, invalid bounds");
}
int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "--verify-standalone")) {
        verify("stand", "stand.sav", 256);
        CHECK(_access("badzero.0", 0) == 0 && _access("badzero.sav", 0) != 0);
        CHECK(_access("badnegative.0", 0) == 0 && _access("badnegative.sav", 0) != 0);
        CHECK(_access("badlimit.0", 0) == 0 && _access("badlimit.sav", 0) != 0);
        CHECK(_access("oldversion.0", 0) == 0 && _access("oldversion.sav", 0) != 0);
        puts("PASS: built standalone recover output, high markers/cleanup and rejected fixtures");
    } else {
        test_pairs(); test_navigation(); test_internal();
        fixture("stand", "stand.sav", 256, FALSE);
        fixture("badzero", "badzero.sav", 0, FALSE);
        fixture("badnegative", "badnegative.sav", -1, FALSE);
        fixture("badlimit", "badlimit.sav", MAXLINFO, FALSE);
        fixture("oldversion", "oldversion.sav", 256, TRUE);
    }
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
