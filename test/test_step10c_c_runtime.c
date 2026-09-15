/* Compile-gated native Step 10C-C production-generation probe. */
#include "hack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_ISAAC64
extern void init_isaac64(unsigned long, int (*)(int));
#endif

struct outlands_counts {
    long invocation, kamerel, spire, fishing, well, river, village;
    long ziggurat, ferrumach, homestead_attempts, homestead_successes;
    long river_cells;
};
static struct outlands_counts observed;

struct outlands_payload {
    long monsters, objects, deep_one, deeper_one, amm_kamerel;
    long hudor_kamerel, sharab_kamerel, plumach_rilmani;
    long ziggurat_wizard, ziggurat_knight, ziggurat_cultist;
    long mirror, robe, khakkhara, reflection_amulet, rakuyo, chest;
    long puddle, pool, moat, doors, altars, barracks, courts, shops;
};

void
step10c_c_note_feature(const char *name)
{
    if (!strcmp(name, "invocation")) ++observed.invocation;
    else if (!strcmp(name, "kamerel")) ++observed.kamerel;
    else if (!strcmp(name, "spire")) ++observed.spire;
    else if (!strcmp(name, "fishing")) ++observed.fishing;
    else if (!strcmp(name, "well")) ++observed.well;
    else if (!strcmp(name, "river")) ++observed.river;
    else if (!strcmp(name, "plumach-village")) ++observed.village;
    else if (!strcmp(name, "ziggurat")) ++observed.ziggurat;
    else if (!strcmp(name, "ferrumach")) ++observed.ferrumach;
    else if (!strcmp(name, "homestead-attempt")) ++observed.homestead_attempts;
    else if (!strcmp(name, "homestead-success")) ++observed.homestead_successes;
    else if (!strcmp(name, "river-cell")) ++observed.river_cells;
}

static boolean
normal_walkable(coordxy x, coordxy y)
{
    int typ;
    if (!isok(x, y)) return FALSE;
    typ = levl[x][y].typ;
    return (boolean) (ACCESSIBLE(typ) && !IS_POOL(typ) && typ != WATER
                      && typ != LAVAPOOL && typ != LAVAWALL);
}

static void
count_object_payload(struct obj *list, struct outlands_payload *payload)
{
    struct obj *obj;

    for (obj = list; obj; obj = obj->nobj) {
        ++payload->objects;
        switch (obj->otyp) {
        case MIRROR: ++payload->mirror; break;
        case ROBE: ++payload->robe; break;
        case KHAKKHARA: ++payload->khakkhara; break;
        case AMULET_OF_REFLECTION: ++payload->reflection_amulet; break;
        case RAKUYO: ++payload->rakuyo; break;
        case CHEST: ++payload->chest; break;
        default: break;
        }
        count_object_payload(obj->cobj, payload);
    }
}

static void
collect_payload(struct outlands_payload *payload)
{
    struct monst *mon;
    int x, y, roomno;

    (void) memset(payload, 0, sizeof *payload);
    for (mon = fmon; mon; mon = mon->nmon) {
        ++payload->monsters;
        switch (mon->mnum) {
        case PM_DEEP_ONE: ++payload->deep_one; break;
        case PM_DEEPER_ONE: ++payload->deeper_one; break;
        case PM_AMM_KAMEREL: ++payload->amm_kamerel; break;
        case PM_HUDOR_KAMEREL: ++payload->hudor_kamerel; break;
        case PM_SHARAB_KAMEREL: ++payload->sharab_kamerel; break;
        case PM_PLUMACH_RILMANI: ++payload->plumach_rilmani; break;
        case PM_SHATTERED_ZIGGURAT_WIZARD: ++payload->ziggurat_wizard; break;
        case PM_SHATTERED_ZIGGURAT_KNIGHT: ++payload->ziggurat_knight; break;
        case PM_SHATTERED_ZIGGURAT_CULTIST: ++payload->ziggurat_cultist; break;
        default: break;
        }
    }
    count_object_payload(fobj, payload);
    count_object_payload(svl.level.buriedobjlist, payload);
    for (x = 1; x < COLNO; ++x)
        for (y = 0; y < ROWNO; ++y) {
            switch (levl[x][y].typ) {
            case PUDDLE: ++payload->puddle; break;
            case POOL: ++payload->pool; break;
            case MOAT: ++payload->moat; break;
            default: break;
            }
            if (levl[x][y].typ == DOOR)
                ++payload->doors;
            if (levl[x][y].typ == ALTAR)
                ++payload->altars;
        }
    for (roomno = 0; roomno < svn.nroom; ++roomno) {
        int rtype = svr.rooms[roomno].rtype;
        if (rtype == BARRACKS)
            ++payload->barracks;
        else if (rtype == COURT)
            ++payload->courts;
        else if (rtype >= SHOPBASE)
            ++payload->shops;
    }
}

static int
validate_generated(d_level *expected, struct outlands_payload *payload)
{
    coord portals[2], queue[COLNO * ROWNO];
    boolean seen[COLNO][ROWNO] = { { FALSE } };
    int x, y, nportal = 0, head = 0, tail = 0, i;
    static const schar dx[4] = { 1, -1, 0, 0 };
    static const schar dy[4] = { 0, 0, 1, -1 };
    struct monst *mon;
    struct obj *obj;

    if (!on_level(&u.uz, expected)) return 10;
    for (x = 1; x < COLNO; ++x)
        for (y = 0; y < ROWNO; ++y) {
            struct trap *trap = t_at((coordxy) x, (coordxy) y);
            if (levl[x][y].typ < STONE || levl[x][y].typ >= MAX_TYPE)
                return 11;
            if (trap && trap->ttyp == MAGIC_PORTAL) {
                if (nportal < 2) {
                    portals[nportal].x = (coordxy) x;
                    portals[nportal].y = (coordxy) y;
                }
                ++nportal;
            }
        }
    if (nportal != 2 || !normal_walkable(portals[0].x, portals[0].y)
        || !normal_walkable(portals[1].x, portals[1].y)) return 12;
    queue[tail++] = portals[0];
    seen[portals[0].x][portals[0].y] = TRUE;
    while (head < tail) {
        coord cur = queue[head++];
        for (i = 0; i < 4; ++i) {
            coordxy nx = cur.x + dx[i], ny = cur.y + dy[i];
            if (!normal_walkable(nx, ny) || seen[nx][ny]) continue;
            seen[nx][ny] = TRUE;
            queue[tail].x = nx;
            queue[tail++].y = ny;
        }
    }
    if (!seen[portals[1].x][portals[1].y]) return 13;
    for (mon = fmon; mon; mon = mon->nmon)
        if (mon->mnum < LOW_PM || mon->mnum >= NUMMONS) return 14;
    for (obj = fobj; obj; obj = obj->nobj)
        if (obj->otyp <= STRANGE_OBJECT || obj->otyp >= NUM_OBJECTS) return 15;
    collect_payload(payload);
    return 0;
}

static void
add_payload(struct outlands_payload *total,
            const struct outlands_payload *sample)
{
    total->monsters += sample->monsters;
    total->objects += sample->objects;
    total->deep_one += sample->deep_one;
    total->deeper_one += sample->deeper_one;
    total->amm_kamerel += sample->amm_kamerel;
    total->hudor_kamerel += sample->hudor_kamerel;
    total->sharab_kamerel += sample->sharab_kamerel;
    total->plumach_rilmani += sample->plumach_rilmani;
    total->ziggurat_wizard += sample->ziggurat_wizard;
    total->ziggurat_knight += sample->ziggurat_knight;
    total->ziggurat_cultist += sample->ziggurat_cultist;
    total->mirror += sample->mirror;
    total->robe += sample->robe;
    total->khakkhara += sample->khakkhara;
    total->reflection_amulet += sample->reflection_amulet;
    total->rakuyo += sample->rakuyo;
    total->chest += sample->chest;
    total->puddle += sample->puddle;
    total->pool += sample->pool;
    total->moat += sample->moat;
    total->doors += sample->doors;
    total->altars += sample->altars;
    total->barracks += sample->barracks;
    total->courts += sample->courts;
    total->shops += sample->shops;
}

int
step10c_c_test_main(void)
{
    const char *levname = getenv("NETHACK_OUTLANDS_LEVEL");
    const char *countstr = getenv("NETHACK_OUTLANDS_COUNT");
    const char *seedstr = getenv("NETHACK_OUTLANDS_SEED");
    const char *trace = getenv("NETHACK_OUTLANDS_TRACE");
    s_level *slev;
    int count = countstr ? atoi(countstr) : 1, sample, rc;
    unsigned long seed = seedstr ? strtoul(seedstr, NULL, 10) : 1UL;
    struct outlands_payload payload_total = { 0 }, payload;

    if (!levname || strcmp(levname, "out1") && strcmp(levname, "out2")
        && strcmp(levname, "out3") && strcmp(levname, "out4"))
        return fprintf(stderr, "invalid Outlands level\n"), 2;
    has_strong_rngseed = FALSE;
#ifdef USE_ISAAC64
    init_isaac64(0x10ccUL, rn2);
    init_isaac64(0x5a5a4a96UL, rn2_on_display_rng);
#endif
    init_objects();
    flags.pantheon = -1;
    flags.initrole = flags.initrace = flags.initgend = flags.initalign = ROLE_NONE;
    (void) strcpy(svp.plname, "outlands-probe");
    svp.pl_character[0] = '\0';
    role_init();
    init_dungeons();
    init_artifacts();
    u.ulevel = 1;
    l_nhcore_init();
    vision_init();
    slev = find_level(levname);
    if (!slev) return fprintf(stderr, "missing %s\n", levname), 3;
    for (sample = 0; sample < count; ++sample) {
        struct outlands_counts before = observed;

#ifdef USE_ISAAC64
        init_isaac64(seed + (unsigned long) sample, rn2);
        init_isaac64((seed + (unsigned long) sample) ^ 0x5a5a5a5aUL,
                     rn2_on_display_rng);
#endif
        while (gf.ftrap) deltrap(gf.ftrap);
        u.uz = slev->dlevel;
        level_status_init();
        oinit();
        clear_level_structures();
        makemaz(slev->proto);
        if ((rc = validate_generated(&slev->dlevel, &payload)) != 0)
            return fprintf(stderr, "sample %d validation %d\n", sample, rc), rc;
        add_payload(&payload_total, &payload);
        if (trace)
            printf("OUTLANDS_SAMPLE|sample=%d|seed=%lu|kamerel=%ld|spire=%ld|fishing=%ld|well=%ld|river=%ld|village=%ld|ziggurat=%ld|ferrumach=%ld|homestead_attempts=%ld|homestead_successes=%ld|river_cells=%ld|semantic_monsters=%ld|semantic_objects=%ld|deep_one=%ld|deeper_one=%ld|amm_kamerel=%ld|hudor_kamerel=%ld|sharab_kamerel=%ld|plumach_rilmani=%ld|ziggurat_wizard=%ld|ziggurat_knight=%ld|ziggurat_cultist=%ld|mirrors=%ld|robes=%ld|khakkhara=%ld|reflection_amulets=%ld|rakuyo=%ld|chests=%ld|puddles=%ld|pools=%ld|moats=%ld|doors=%ld|altars=%ld|barracks=%ld|courts=%ld|shops=%ld\n",
                   sample, seed + (unsigned long) sample,
                   observed.kamerel - before.kamerel,
                   observed.spire - before.spire,
                   observed.fishing - before.fishing,
                   observed.well - before.well,
                   observed.river - before.river,
                   observed.village - before.village,
                   observed.ziggurat - before.ziggurat,
                   observed.ferrumach - before.ferrumach,
                   observed.homestead_attempts - before.homestead_attempts,
                   observed.homestead_successes - before.homestead_successes,
                   observed.river_cells - before.river_cells,
                   payload.monsters, payload.objects, payload.deep_one,
                   payload.deeper_one, payload.amm_kamerel,
                   payload.hudor_kamerel, payload.sharab_kamerel,
                   payload.plumach_rilmani, payload.ziggurat_wizard,
                   payload.ziggurat_knight, payload.ziggurat_cultist,
                   payload.mirror, payload.robe, payload.khakkhara,
                   payload.reflection_amulet, payload.rakuyo, payload.chest,
                   payload.puddle, payload.pool, payload.moat, payload.doors,
                   payload.altars, payload.barracks, payload.courts,
                   payload.shops);
    }
    if (observed.invocation != count)
        return fprintf(stderr, "invocation mismatch %ld/%d\n",
                       observed.invocation, count), 20;
    printf("OUTLANDS|level=%s|instances=%d|kamerel=%ld|spire=%ld|fishing=%ld|well=%ld|river=%ld|village=%ld|ziggurat=%ld|ferrumach=%ld|homestead_attempts=%ld|homestead_successes=%ld|river_cells=%ld|semantic_monsters=%ld|semantic_objects=%ld|deep_one=%ld|deeper_one=%ld|amm_kamerel=%ld|hudor_kamerel=%ld|sharab_kamerel=%ld|plumach_rilmani=%ld|ziggurat_wizard=%ld|ziggurat_knight=%ld|ziggurat_cultist=%ld|mirrors=%ld|robes=%ld|khakkhara=%ld|reflection_amulets=%ld|rakuyo=%ld|chests=%ld|puddles=%ld|pools=%ld|moats=%ld|doors=%ld|altars=%ld|barracks=%ld|courts=%ld|shops=%ld\n",
           levname, count, observed.kamerel, observed.spire, observed.fishing,
           observed.well, observed.river, observed.village, observed.ziggurat,
           observed.ferrumach, observed.homestead_attempts,
           observed.homestead_successes, observed.river_cells,
           payload_total.monsters, payload_total.objects,
           payload_total.deep_one, payload_total.deeper_one,
           payload_total.amm_kamerel, payload_total.hudor_kamerel,
           payload_total.sharab_kamerel, payload_total.plumach_rilmani,
           payload_total.ziggurat_wizard, payload_total.ziggurat_knight,
           payload_total.ziggurat_cultist, payload_total.mirror,
           payload_total.robe, payload_total.khakkhara,
           payload_total.reflection_amulet, payload_total.rakuyo,
           payload_total.chest, payload_total.puddle, payload_total.pool,
           payload_total.moat, payload_total.doors, payload_total.altars,
           payload_total.barracks, payload_total.courts, payload_total.shops);
    return 0;
}
