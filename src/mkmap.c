/* NetHack 5.0	mkmap.c	$NHDT-Date: 1781973055 2026/06/20 16:30:55 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.47 $ */
/* Copyright (c) J. C. Collet, M. Stephenson and D. Cohrs, 1992   */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "sp_lev.h"

#define HEIGHT (ROWNO - 1)
#define WIDTH (COLNO - 2)

staticfn void init_map(schar);
staticfn void init_fill(schar, schar);
staticfn schar get_map(coordxy, coordxy, schar);
staticfn void pass_one(schar, schar);
staticfn void pass_two(schar, schar);
staticfn void pass_three(schar, schar);
staticfn void join_map_cleanup(void);
staticfn void join_map(schar, schar);
staticfn void finish_map(schar, schar, boolean, boolean, boolean);
staticfn void remove_room(unsigned);
void mkmap(lev_init *);

staticfn void
init_map(schar bg_typ)
{
    coordxy x, y;

    for (x = 1; x < COLNO; x++)
        for (y = 0; y < ROWNO; y++) {
            levl[x][y].roomno = NO_ROOM;
            levl[x][y].typ = bg_typ;
            levl[x][y].lit = FALSE;
        }
}

staticfn void
init_fill(schar bg_typ, schar fg_typ)
{
    coordxy x, y;
    long limit, count;

    limit = (WIDTH * HEIGHT * 2) / 5;
    count = 0;
    while (count < limit) {
        x = (coordxy) rn1(WIDTH - 1, 2);
        y = (coordxy) rnd(HEIGHT - 1);
        if (levl[x][y].typ == bg_typ) {
            levl[x][y].typ = fg_typ;
            count++;
        }
    }
}

staticfn schar
get_map(coordxy col, coordxy row, schar bg_typ)
{
    if (col <= 0 || row < 0 || col > WIDTH || row >= HEIGHT)
        return bg_typ;
    return levl[col][row].typ;
}

staticfn const int dirs[16] = {
    -1, -1 /**/, -1,  0 /**/, -1, 1 /**/, 0, -1 /**/,
     0,  1 /**/,  1, -1 /**/,  1, 0 /**/, 1,  1
};

staticfn void
pass_one(schar bg_typ, schar fg_typ)
{
    coordxy x, y;
    short count, dr;

    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++) {
            for (count = 0, dr = 0; dr < 8; dr++)
                if (get_map(x + dirs[dr * 2], y + dirs[(dr * 2) + 1], bg_typ)
                    == fg_typ)
                    count++;

            switch (count) {
            case 0: /* death */
            case 1:
            case 2:
                levl[x][y].typ = bg_typ;
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                levl[x][y].typ = fg_typ;
                break;
            default:
                break;
            }
        }
}

#define new_loc(i, j) *(gn.new_locations + ((j) * (WIDTH + 1)) + (i))

staticfn void
pass_two(schar bg_typ, schar fg_typ)
{
    coordxy x, y;
    short count, dr;

    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++) {
            for (count = 0, dr = 0; dr < 8; dr++)
                if (get_map(x + dirs[dr * 2], y + dirs[(dr * 2) + 1], bg_typ)
                    == fg_typ)
                    count++;
            if (count == 5)
                new_loc(x, y) = bg_typ;
            else
                new_loc(x, y) = get_map(x, y, bg_typ);
        }

    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++)
            levl[x][y].typ = new_loc(x, y);
}

staticfn void
pass_three(schar bg_typ, schar fg_typ)
{
    coordxy x, y;
    short count, dr;

    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++) {
            for (count = 0, dr = 0; dr < 8; dr++)
                if (get_map(x + dirs[dr * 2], y + dirs[(dr * 2) + 1], bg_typ)
                    == fg_typ)
                    count++;
            if (count < 3)
                new_loc(x, y) = bg_typ;
            else
                new_loc(x, y) = get_map(x, y, bg_typ);
        }

    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++)
            levl[x][y].typ = new_loc(x, y);
}

/*
 * use a flooding algorithm to find all locations that should
 * have the same rm number as the current location.
 * if anyroom is TRUE, use IS_ROOM to check room membership instead of
 * exactly matching levl[sx][sy].typ and walls are included as well.
 */
void
flood_fill_rm(
    coordxy sx,
    coordxy sy,
    int rmno,
    boolean lit,
    boolean anyroom)
{
    coordxy i, nx;
    schar fg_typ = levl[sx][sy].typ;

    /* back up to find leftmost uninitialized location */
    while (sx > 0 && (anyroom ? IS_ROOM(levl[sx][sy].typ)
                              : levl[sx][sy].typ == fg_typ)
           && (int) levl[sx][sy].roomno != rmno)
        sx--;
    sx++; /* compensate for extra decrement */

    /* assume sx,sy is valid */
    if (sx < gm.min_rx)
        gm.min_rx = sx;
    if (sy < gm.min_ry)
        gm.min_ry = sy;

    for (i = sx; i <= WIDTH && levl[i][sy].typ == fg_typ; i++) {
        levl[i][sy].roomno = rmno;
        levl[i][sy].lit = lit;
        if (anyroom) {
            /* add walls to room as well */
            coordxy ii, jj;
            for (ii = (i == sx ? i - 1 : i); ii <= i + 1; ii++)
                for (jj = sy - 1; jj <= sy + 1; jj++)
                    if (isok(ii, jj) && (IS_WALL(levl[ii][jj].typ)
                                         || IS_DOOR(levl[ii][jj].typ)
                                         || levl[ii][jj].typ == SDOOR)) {
                        levl[ii][jj].edge = 1;
                        if (lit)
                            levl[ii][jj].lit = lit;

                        if (levl[ii][jj].roomno == NO_ROOM)
                            levl[ii][jj].roomno = rmno;
                        else if ((int) levl[ii][jj].roomno != rmno)
                            levl[ii][jj].roomno = SHARED;
                    }
        }
        gn.n_loc_filled++;
    }
    nx = i;

    if (isok(sx, sy - 1)) {
        for (i = sx; i < nx; i++)
            if (levl[i][sy - 1].typ == fg_typ) {
                if ((int) levl[i][sy - 1].roomno != rmno)
                    flood_fill_rm(i, sy - 1, rmno, lit, anyroom);
            } else {
                if ((i > sx || isok(i - 1, sy - 1))
                    && levl[i - 1][sy - 1].typ == fg_typ) {
                    if ((int) levl[i - 1][sy - 1].roomno != rmno)
                        flood_fill_rm(i - 1, sy - 1, rmno, lit, anyroom);
                }
                if ((i < nx - 1 || isok(i + 1, sy - 1))
                    && levl[i + 1][sy - 1].typ == fg_typ) {
                    if ((int) levl[i + 1][sy - 1].roomno != rmno)
                        flood_fill_rm(i + 1, sy - 1, rmno, lit, anyroom);
                }
            }
    }
    if (isok(sx, sy + 1)) {
        for (i = sx; i < nx; i++)
            if (levl[i][sy + 1].typ == fg_typ) {
                if ((int) levl[i][sy + 1].roomno != rmno)
                    flood_fill_rm(i, sy + 1, rmno, lit, anyroom);
            } else {
                if ((i > sx || isok(i - 1, sy + 1))
                    && levl[i - 1][sy + 1].typ == fg_typ) {
                    if ((int) levl[i - 1][sy + 1].roomno != rmno)
                        flood_fill_rm(i - 1, sy + 1, rmno, lit, anyroom);
                }
                if ((i < nx - 1 || isok(i + 1, sy + 1))
                    && levl[i + 1][sy + 1].typ == fg_typ) {
                    if ((int) levl[i + 1][sy + 1].roomno != rmno)
                        flood_fill_rm(i + 1, sy + 1, rmno, lit, anyroom);
                }
            }
    }

    if (nx > gm.max_rx)
        gm.max_rx = nx - 1; /* nx is just past valid region */
    if (sy > gm.max_ry)
        gm.max_ry = sy;
}

/* join_map uses temporary rooms; clean up after it */
staticfn void
join_map_cleanup(void)
{
    coordxy x, y;

    for (x = 1; x < COLNO; x++)
        for (y = 0; y < ROWNO; y++)
            levl[x][y].roomno = NO_ROOM;
    svn.nroom = gn.nsubroom = 0;
    svr.rooms[svn.nroom].hx = gs.subrooms[gn.nsubroom].hx = -1;
}

staticfn void
join_map(schar bg_typ, schar fg_typ)
{
    struct mkroom *croom, *croom2;

    coordxy x, y, sx, sy;
    coord sm, em;

    /* first, use flood filling to find all of the regions that need joining
     */
    for (x = 2; x <= WIDTH; x++)
        for (y = 1; y < HEIGHT; y++) {
            if (levl[x][y].typ == fg_typ && levl[x][y].roomno == NO_ROOM) {
                gm.min_rx = gm.max_rx = x;
                gm.min_ry = gm.max_ry = y;
                gn.n_loc_filled = 0;
                flood_fill_rm(x, y, svn.nroom + ROOMOFFSET, FALSE, FALSE);
                if (gn.n_loc_filled > 3) {
                    add_room(gm.min_rx, gm.min_ry, gm.max_rx, gm.max_ry,
                             FALSE, OROOM, TRUE);
                    svr.rooms[svn.nroom - 1].irregular = TRUE;
                    if (svn.nroom >= (MAXNROFROOMS * 2))
                        goto joinm;
                } else {
                    /*
                     * it's a tiny hole; erase it from the map to avoid
                     * having the player end up here with no way out.
                     */
                    for (sx = gm.min_rx; sx <= gm.max_rx; sx++)
                        for (sy = gm.min_ry; sy <= gm.max_ry; sy++)
                            if ((int) levl[sx][sy].roomno
                                == svn.nroom + ROOMOFFSET) {
                                levl[sx][sy].typ = bg_typ;
                                levl[sx][sy].roomno = NO_ROOM;
                            }
                }
            }
        }

 joinm:
    /*
     * Ok, now we can actually join the regions with fg_typ's.
     * The rooms are already sorted due to the previous loop,
     * so don't call sort_rooms(), which can screw up the roomno's
     * validity in the levl structure.
     */
    for (croom = &svr.rooms[0], croom2 = croom + 1;
         croom2 < &svr.rooms[svn.nroom]; ) {
        /* pick random starting and end locations for "corridor" */
        if (!somexy(croom, &sm) || !somexy(croom2, &em)) {
            /* ack! -- the level is going to be busted */
            /* arbitrarily pick centers of both rooms and hope for the best */
            impossible("No start/end room loc in join_map.");
            sm.x = croom->lx + ((croom->hx - croom->lx) / 2);
            sm.y = croom->ly + ((croom->hy - croom->ly) / 2);
            em.x = croom2->lx + ((croom2->hx - croom2->lx) / 2);
            em.y = croom2->ly + ((croom2->hy - croom2->ly) / 2);
        }

        (void) dig_corridor(&sm, &em, NULL, FALSE, fg_typ, bg_typ);

        /* choose next region to join */
        /* only increment croom if croom and croom2 are non-overlapping */
        if (croom2->lx > croom->hx
            || ((croom2->ly > croom->hy || croom2->hy < croom->ly)
                && rn2(3))) {
            croom = croom2;
        }
        croom2++; /* always increment the next room */
    }
    join_map_cleanup();
}

staticfn void
finish_map(
    schar fg_typ,
    schar bg_typ,
    boolean lit,
    boolean walled,
    boolean icedpools)
{
    coordxy x, y;

    if (walled)
        wallify_map(1, 0, COLNO-1, ROWNO-1);

    if (lit) {
        for (x = 1; x < COLNO; x++)
            for (y = 0; y < ROWNO; y++)
                if ((!IS_OBSTRUCTED(fg_typ) && levl[x][y].typ == fg_typ)
                    || (!IS_OBSTRUCTED(bg_typ) && levl[x][y].typ == bg_typ)
                    || (bg_typ == TREE && levl[x][y].typ == bg_typ)
                    || (walled && IS_WALL(levl[x][y].typ)))
                    levl[x][y].lit = TRUE;
        for (x = 0; x < svn.nroom; x++)
            svr.rooms[x].rlit = 1;
    }
    /* light lava even if everything's otherwise unlit;
       ice might be frozen pool rather than frozen moat */
    for (x = 1; x < COLNO; x++)
        for (y = 0; y < ROWNO; y++) {
            if (levl[x][y].typ == LAVAPOOL)
                levl[x][y].lit = TRUE;
            else if (levl[x][y].typ == ICE)
                levl[x][y].icedpool = icedpools ? ICED_POOL : ICED_MOAT;
        }
}

/*
 * TODO: If we really want to remove rooms after a map is plopped down
 * in a special level, this needs to be rewritten - the maps may have
 * holes in them ("x" mapchar), leaving parts of rooms still on the map.
 *
 * When level processed by join_map is overlaid by a MAP, some rooms may no
 * longer be valid.  All rooms in the region lx <= x < hx, ly <= y < hy are
 * removed.  Rooms partially in the region are truncated.  This function
 * must be called before the REGIONs or ROOMs of the map are processed, or
 * those rooms will be removed as well.  Assumes roomno fields in the
 * region are already cleared, and roomno and irregular fields outside the
 * region are all set.
 */
void
remove_rooms(coordxy lx, coordxy ly, coordxy hx, coordxy hy)
{
    int i;
    struct mkroom *croom;

    for (i = svn.nroom - 1; i >= 0; --i) {
        croom = &svr.rooms[i];
        if (croom->hx < lx || croom->lx >= hx || croom->hy < ly
            || croom->ly >= hy)
            continue; /* no overlap */

        if (croom->lx < lx || croom->hx >= hx || croom->ly < ly
            || croom->hy >= hy) { /* partial overlap */
            /* TODO: ensure remaining parts of room are still joined */

            if (!croom->irregular)
                impossible("regular room in joined map");
        } else {
            /* total overlap, remove the room */
            remove_room((unsigned) i);
        }
    }
}

/*
 * Remove roomno from the rooms array, decrementing nroom.
 * The last room is swapped with the being-removed room and locations
 * within it have their roomno field updated.  Other rooms are unaffected.
 * Assumes level structure contents corresponding to roomno have already
 * been reset.
 * Currently handles only the removal of rooms that have no subrooms.
 */
staticfn void
remove_room(unsigned int roomno)
{
    struct mkroom *croom = &svr.rooms[roomno];
    struct mkroom *maxroom = &svr.rooms[--svn.nroom];
    coordxy x, y;
    unsigned oroomno;

    if (croom != maxroom) {
        /* since the order in the array only matters for making corridors,
         * copy the last room over the one being removed on the assumption
         * that corridors have already been dug. */
        *croom = *maxroom;

        /* since maxroom moved, update affected level roomno values */
        oroomno = svn.nroom + ROOMOFFSET;
        roomno += ROOMOFFSET;
        for (x = croom->lx; x <= croom->hx; ++x)
            for (y = croom->ly; y <= croom->hy; ++y) {
                if (levl[x][y].roomno == oroomno)
                    levl[x][y].roomno = roomno;
            }
    }

    maxroom->hx = -1; /* just like add_room */
}

#define N_P1_ITER 1 /* tune map generation via this value */
#define N_P2_ITER 1 /* tune map generation via this value */
#define N_P3_ITER 2 /* tune map smoothing via this value */

boolean
litstate_rnd(int litstate)
{
    if (litstate < 0)
        return (rnd(1 + abs(depth(&u.uz))) < 11 && rn2(77)) ? TRUE : FALSE;
    return (boolean) litstate;
}

void
mkmap(lev_init *init_lev)
{
    schar bg_typ = init_lev->bg, fg_typ = init_lev->fg;
    boolean smooth = init_lev->smoothed, join = init_lev->joined;
    xint16 lit = init_lev->lit, walled = init_lev->walled;
    int i;

    lit = litstate_rnd(lit);

    gn.new_locations = (char *) alloc((WIDTH + 1) * HEIGHT);

    init_map(bg_typ);
    init_fill(bg_typ, fg_typ);

    for (i = 0; i < N_P1_ITER; i++)
        pass_one(bg_typ, fg_typ);

    for (i = 0; i < N_P2_ITER; i++)
        pass_two(bg_typ, fg_typ);

    if (smooth)
        for (i = 0; i < N_P3_ITER; i++)
            pass_three(bg_typ, fg_typ);

    if (join)
        join_map(bg_typ, fg_typ);

    finish_map(fg_typ, bg_typ, (boolean) lit, (boolean) walled,
               init_lev->icedpools);
    /* a walled, joined level is cavernous, not mazelike -dlc */
    if (walled && join) {
        svl.level.flags.is_maze_lev = FALSE;
        svl.level.flags.is_cavernous_lev = TRUE;
    }
    free(gn.new_locations);
}

/*mkmap.c*/

/* Step 9A: UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a
 * src/mksheol.c. Voronoi/Bezier generator, retained procedurally. */
/* Minimum distance between points when using voronoi diagram to place
 * water, ice and solid ground.
 *
 * This is euclidean distance to second power.
 * (i.e. 2 == 4) */
#define MINIMUM_VORONOI_DISTANCE 4

/* See sheol.txt for Sheol description.
 * This file deals with generating the levels. */

/* sheol_init_level_base_voronoi() uses this. */
typedef struct spatchcoord
{
    coord c;
    schar typ;
} patchcoord;

/* probability of making a floor:
 * prob / out_of or if guaranteed is 1 then it's 100% chance */
typedef struct sfloorprob
{
    int prob;
    int out_of;

    int guaranteed;
} floorprob;

/* These are chances which type to use for each tile in a voronoi diagram. */
staticfn schar sheol_typs[10] = { ICEWALL,
                          CRYSTALICEWALL, CRYSTALICEWALL, CRYSTALICEWALL,
                          POOL, POOL,
                          STONE, STONE, STONE, STONE };

staticfn schar sheol_opentyps[10] = { ICEWALL,
                              CRYSTALICEWALL, CRYSTALICEWALL, STONE,
                              POOL, POOL,
                              ROOM, ICE, ICE, ROOM };

staticfn void sheol_init_level_base_voronoi(schar* vtyps, int numtyps, int numpoints);
staticfn int sheol_check_voronoi_winner(patchcoord* coords, int num_coords,
                                coordxy x, coordxy y);
staticfn void sheol_carve_path(floorprob* probs);
staticfn void sheol_fuzzy_circle(coordxy x, coordxy y,
                         int guaranteed_passage_radius, int fallout,
                         floorprob* floorprobs);

staticfn void sheol_finalize_map(void);
staticfn int sheol_under_middle(void);
staticfn int sheol_at_middle(void);

/* Return values from sheol_plug_unreachable_places and sheol_verify_stairs_place. */
/*  Any value >0 means "ok" */
#define STAT_REJECT 0       /* "please reject the map, it's too bad" */
#define STAT_SEMIPLUGGED  1 /* Some places are only reachable behind ice */
#define STAT_ALLREACHABLE 2 /* Every place is reachable by walking or
                               levitating */
#define STAT_STAIRSOK     3 /*  Stairs have adequate space. */

staticfn int sheol_plug_unreachable_places(void);
staticfn int sheol_verify_stairs_place(void);

staticfn void sheol_mkopensheol(void);
staticfn void sheol_place_clouds(void);
staticfn void sheol_shake_position(int* x, int* y);

void
mksheol(void)
{
    int i1, i2;
    int testval;
    floorprob* probs;

    /* Sometimes make an almost open level instead */
    if (!sheol_under_middle() &&
        !sheol_at_middle() && !rn2(3)) {
        sheol_mkopensheol();
        return;
    }

    probs = (floorprob*) alloc(sizeof(floorprob) * COLNO * ROWNO);

again:
    memset(probs, 0, sizeof(floorprob) * COLNO * ROWNO);

    sheol_init_level_base_voronoi(sheol_typs, 10, 300);

    /* Then, carve a "path" from somewhere left of the level to the right
     * of the level. */
    sheol_carve_path(probs);

    for (i1 = 1; i1 < COLNO; ++i1) {
        for (i2 = 0; i2 < ROWNO; ++i2) {
            testval = 1;
            if (probs[i1 + i2 * COLNO].out_of != 0) {
                testval = rn2(probs[i1+i2*COLNO].out_of);
            }
            if (testval < probs[i1+i2*COLNO].prob ||
                probs[i1+i2*COLNO].guaranteed)
            {
                if (levl[i1][i2].typ == ICEWALL ||
                    levl[i1][i2].typ == CRYSTALICEWALL)
                    levl[i1][i2].typ = ICE;
                else if (levl[i1][i2].typ != POOL) {
                    levl[i1][i2].typ = ROOM;
                }

            } else if (levl[i1][i2].typ == POOL) {
                levl[i1][i2].typ = ICEWALL;
            }
        }
    }

    /* Sometimes, put a lot of clouds somewhere on the level. */
    if (!rn2(5)) {
        sheol_place_clouds();
    }

    if (sheol_verify_stairs_place() == STAT_REJECT) {
        goto again;
    }

    sheol_finalize_map();

    if (sheol_plug_unreachable_places() == STAT_REJECT) {
        goto again;
    }

    free(probs);
}

staticfn void
sheol_init_level_base_voronoi(schar* vtyps, int numtyps, int numpoints)
{
    int patches, i1, i2;
    int winner_patch;
    patchcoord* points;
    int i1_tries=0, i1_last_try=0;

    /* We use a voronoi diagram to put ice and solid ground on the level.
    * Maybe it looks interesting? I hope so. That's kind of the point. */
    points = (patchcoord*) alloc(sizeof(patchcoord)*numpoints);
    patches = numpoints;

    for (i1 = 0; i1 < patches; ++i1) {
        if (i1_last_try != i1) {
            i1_tries = 0;
            i1_last_try = i1;
        }
        points[i1].c.x = rn1(COLNO-1, 1);
        points[i1].c.y = rn2(ROWNO);
        points[i1].typ = vtyps[rn2(numtyps)];

        /* Continue loop when no point positioned far enough away from
         * the other points has been found. We don't need a perfect
         * match here as level properties are checked further up
         * anyway. */
        if (i1_tries++ > 1000) {
            continue;
        }

        /* Don't want them to be too close to other points... */
        for (i2 = 0; i2 < i1; ++i2) {
            if (dist2(points[i1].c.x, points[i1].c.y,
                      points[i2].c.x, points[i2].c.y) <
                MINIMUM_VORONOI_DISTANCE) {
                --i1;
                break;
            }
        }
    }

    for (i1 = 1; i1 < COLNO; ++i1) {
        for (i2 = 0; i2 < ROWNO; ++i2) {
            winner_patch = sheol_check_voronoi_winner(points, patches, i1, i2);
            levl[i1][i2].typ = points[winner_patch].typ;
        }
    }

    free(points);
}

staticfn int
sheol_check_voronoi_winner(patchcoord *coords, int num_coords, coordxy x, coordxy y)
{
    int i1;
    int winner = 0, winner_distance;
    int d;

    winner_distance = 10000;

    for (i1 = 0; i1 < num_coords; ++i1) {
        d = dist2(coords[i1].c.x, coords[i1].c.y, x, y);
        if (d < winner_distance) {
            winner_distance = d;
            winner = i1;
        }
    }

    return winner;
}

typedef struct sbezcoord
{
    int x, y;         /* coordinate */
} bezcoord;

staticfn void
sheol_carve_path(floorprob *floorprobs)
{
    /*
     * We'll draw a bezier curve from the left of the level to the right.
     * We first pick a rough path from left to right by picking points.
     * Then we'll make a smooth path using a bezier curve by going through
     * those points.
     *
     * We use every second coordinate as a control point.
     */

    bezcoord points[100];
    int i1, i2, num_points, c1_x, c1_y, c2_x, c2_y;
    int r_i2;
    int x, y;

    int sample_points;

    int path_x, path_y;
    int tries;

    /* Attempt to make ~random point paths to the end. */
    while (1) {
        path_x = -2;
        path_y = ROWNO / 2;

        for (i1 = 0; i1 < 100; ++i1) {
            tries = 0;
            while (tries < 100) {
                tries++;
                points[i1].x = path_x;
                points[i1].y = path_y;

                if (!rn2(2)) {
                    points[i1].x += rn2(11)-5;
                } else {
                    points[i1].x += rn2(6)-2;
                }
                points[i1].y += rn2(11)-5;

                if (points[i1].y < 0 ||
                    points[i1].y >= ROWNO ||
                    points[i1].x < 1 ||
                    points[i1].x >= COLNO)
                    continue;
                for (i2 = 0; i2 < i1; ++i2) {
                    if (dist2(points[i2].x, points[i2].y, points[i1].x, points[i1].y) <= 16) {
                        break;
                    }
                }
                if (i2 < i1) {
                    continue;
                }
                break;
            }
            if (tries >= 100) {
                path_x = -2;
                path_y = ROWNO / 2;
                i1 = -1;
                continue;
            }
            if (i1 > 0 && points[i1-1].x >= COLNO-5) {
                break;
            }
            path_x = points[i1].x;
            path_y = points[i1].y;
        }
        if (i1 >= 100) {
            continue;
        }
        break;
    }

    num_points = i1;

    for (i1 = 0; i1 < num_points - 2; i1 += 2) {
        sample_points = isqrt(dist2(points[i1].x, points[i1].y,
                                    points[i1+1].x, points[i1+1].y)+
                              dist2(points[i1+1].x, points[i1+1].y,
                                    points[i1+2].x, points[i1+2].y))*20;
        for (i2 = 0; i2 <= sample_points; ++i2) {
            r_i2 = sample_points - i2;
            #define BEZNUMERICAL(targ, m, idx1, idx2) \
    targ = points[idx1].m * i2 / sample_points; \
    targ += points[idx2].m * r_i2 / sample_points;

            BEZNUMERICAL(c1_x, x, i1, i1+1);
            BEZNUMERICAL(c1_y, y, i1, i1+1);
            BEZNUMERICAL(c2_x, x, i1+1, i1+2);
            BEZNUMERICAL(c2_y, y, i1+1, i1+2);

            x = c1_x * i2 / sample_points;
            x += c2_x * r_i2 / sample_points;
            y = c1_y * i2 / sample_points;
            y += c2_y * r_i2 / sample_points;

            #undef BEZNUMERICAL

            if (sheol_under_middle()) {
                sheol_fuzzy_circle(x, y, 1, 0, floorprobs);
            } else {
                sheol_fuzzy_circle(x, y, 1, 2, floorprobs);
            }
        }
    }
}


/* Makes a passable circle centered at x, y.
 * To radius guaranteed_passage, guarantees passability.
 *
 * After that, linearly makes it less likely that a passable square is
 * made. This makes a sort of "rough" edge. */
staticfn void
sheol_fuzzy_circle(coordxy x, coordxy y, int guaranteed_passage_radius, int fallout, floorprob *floorprobs)
{
    int i1, i2;
    int fallout_2 = fallout * fallout;
    int d;

    if (fallout < guaranteed_passage_radius) {
        fallout = guaranteed_passage_radius;
    }

    for (i1 = x - fallout; i1 <= x + fallout; ++i1) {
        for (i2 = y - fallout; i2 <= y + fallout; ++i2) {
            if (i1 < 1 || i1 >= COLNO ||
                i2 < 0 || i2 >= ROWNO)
                continue;
            d = dist2(i1, i2, x, y);
            if (d > fallout_2) {
                continue;
            }
            if (d <= guaranteed_passage_radius) {
                floorprobs[i1 + i2 * COLNO].guaranteed = 1;
            } else {
                d -= guaranteed_passage_radius;
                d = (d * 100) /
                    ((fallout_2 -
                      guaranteed_passage_radius));
                d = 100 - d;
                floorprobs[i1+i2*COLNO].prob += d;
                floorprobs[i1+i2*COLNO].out_of += 100;
            }
        }
    }
}

#define VALID_PASSABLE(x, y) (levl[x][y].typ == ICE || \
                              levl[x][y].typ == POOL || \
                              levl[x][y].typ == ROOM)
#define VALID_PASSABLE2(x, y) (VALID_PASSABLE(x, y) || \
                               IS_ANY_ICEWALL(levl[x][y].typ))

staticfn int
sheol_plug_unreachable_places(void)
{
    char fillmap[COLNO][ROWNO];
    int not_passable;
    int x, y;
    int flood_x, flood_y;
    int tries;
    int done;

    not_passable = 0;

    memset(fillmap, 0, sizeof(fillmap));

    tries = 100;
    while (tries > 0) {
        tries--;
        x = rn1(COLNO-2, 1);
        y = rn2(ROWNO);
        if (levl[x][y].typ != ROOM &&
            levl[x][y].typ != ICE)
            continue;
        break;
    }
    if (tries <= 0) {
        return STAT_REJECT;
    }

    /* flood fill */
    done = 0;
    flood_x = x;
    flood_y = y;
    fillmap[flood_x][flood_y] = 1;

    while (!done) {
        done = 1;
        for (x = 2; x < COLNO-1; ++x) {
            for (y = 1; y < ROWNO-1; ++y) {
                if (!fillmap[x][y]) {
                    continue;
                }
                if (fillmap[x+1][y] == 0 &&
                    VALID_PASSABLE(x+1, y)) {
                    fillmap[x+1][y] = 1;
                    done = 0;
                }
                if (fillmap[x-1][y] == 0 &&
                    VALID_PASSABLE(x-1, y)) {
                    fillmap[x-1][y] = 1;
                    done = 0;
                }
                if (fillmap[x][y-1] == 0 &&
                    VALID_PASSABLE(x, y-1)) {
                    fillmap[x][y-1] = 1;
                    done = 0;
                }
                if (fillmap[x][y+1] == 0 &&
                    VALID_PASSABLE(x, y+1)) {
                    fillmap[x][y+1] = 1;
                    done = 0;
                }
            }
        }
    }

    for (x = 1; x < COLNO; ++x) {
        for (y = 0; y < ROWNO; ++y) {
            if (VALID_PASSABLE(x, y) && fillmap[x][y] != 1) {
                not_passable = 1;
                break;
            }
        }
    }

    if (!not_passable) {
        return STAT_ALLREACHABLE;
    }

    /* flood fill again, but go through ice this time */
    done = 0;
    not_passable = 0;
    memset(fillmap, 0, sizeof(fillmap));
    fillmap[flood_x][flood_y] = 1;

    while (!done) {
        done = 1;
        for (x = 2; x < COLNO-1; ++x) {
            for (y = 1; y < ROWNO-1; ++y) {
                if (!fillmap[x][y]) {
                    continue;
                }
                if (fillmap[x+1][y] == 0 &&
                    VALID_PASSABLE2(x+1, y)) {
                    fillmap[x+1][y] = 1;
                    done = 0;
                }
                if (fillmap[x-1][y] == 0 &&
                    VALID_PASSABLE2(x-1, y)) {
                    fillmap[x-1][y] = 1;
                    done = 0;
                }
                if (fillmap[x][y-1] == 0 &&
                    VALID_PASSABLE2(x, y-1)) {
                    fillmap[x][y-1] = 1;
                    done = 0;
                }
                if (fillmap[x][y+1] == 0 &&
                    VALID_PASSABLE2(x, y+1)) {
                    fillmap[x][y+1] = 1;
                    done = 0;
                }
            }
        }
    }

    for (x = 1; x < COLNO; ++x) {
        for (y = 0; y < ROWNO; ++y) {
            if (VALID_PASSABLE(x, y) && fillmap[x][y] != 1) {
                not_passable = 1;
                break;
            }
        }
    }

    if (not_passable) {
        return STAT_REJECT;
    }
    return STAT_SEMIPLUGGED;
}

/* Return 1 if below the middle Sheol level. */
staticfn int
sheol_under_middle(void) {
    return u.uz.dlevel > 2;
}

/* Return 0 if right at the middle level. */
staticfn int
sheol_at_middle(void) {
    return u.uz.dlevel == 2;
}

staticfn void
sheol_mkopensheol(void)
{
    /*  This one's simple. */
    int num_points = 300;
    int tries = 0;
again:
    do {
        sheol_init_level_base_voronoi(sheol_opentyps, 10, num_points);
    } while (sheol_verify_stairs_place() != STAT_STAIRSOK);
    tries++;
    if (tries > 100 && num_points > 100) {
        num_points--;
    }

    sheol_finalize_map();

    if (sheol_plug_unreachable_places() == STAT_REJECT) {
        goto again;
    }
}

staticfn void
sheol_finalize_map(void) {
    int i1, i2;

    for (i1 = 1; i1 < COLNO; ++i1) {
        for (i2 = 0; i2 < ROWNO; ++i2) {
            levl[i1][i2].lit = TRUE; /* lit things up */
            if (IS_OBSTRUCTED(levl[i1][i2].typ) &&
                !IS_ANY_ICEWALL(levl[i1][i2].typ))
                levl[i1][i2].wall_info |= W_NONDIGGABLE;
        }
    }

    wallify_map(1, 0, COLNO-1, ROWNO-1);

    svl.level.flags.is_maze_lev = FALSE;
    svl.level.flags.is_cavernous_lev = TRUE;
}

staticfn int
sheol_verify_stairs_place(void) {
    int i1, i2;

    /* Make sure there is ROOM somewhere in both sides of the level */
    for (i1 = 1; i1 <= 15; ++i1) {
        for (i2 = 0; i2 < ROWNO; ++i2) {
            if (levl[i1][i2].typ == ROOM) {
                break;
            }
        }
        if (i2 < ROWNO) {
            break;
        }
    }
    if (i1 > 15) {
        return STAT_REJECT;
    }

    for (i1 = 65; i1 < COLNO; ++i1) {
        for (i2 = 0; i2 < ROWNO; ++i2) {
            if (levl[i1][i2].typ == ROOM) {
                break;
            }
        }
        if (i2 < ROWNO) {
            break;
        }
    }
    if (i1 >= COLNO) {
        return STAT_REJECT;
    }

    return STAT_STAIRSOK;
}

staticfn void
sheol_place_clouds(void) {
    int num_clouds;
    int x, y;
    int tries;

    num_clouds = rn2(25) + rn2(40) + 5;

    while (1) {
        x = rn1(COLNO-2, 1);
        y = rn2(ROWNO);
        if (levl[x][y].typ == ICE ||
            levl[x][y].typ == ROOM)
            break;
    }

    tries = 200;
    while (num_clouds > 0 && tries > 0) {
        if (levl[x][y].typ == ICE ||
            levl[x][y].typ == ROOM) {
            levl[x][y].typ = CLOUD;
            num_clouds--;
            tries = 200;
        } else {
            --tries;
            sheol_shake_position(&x, &y);
        }
    }
}

staticfn void
sheol_shake_position(int *x, int *y)
{
    int i1;
    int old_x, old_y;
    int tries;

    int dx, dy;

    old_x = (*x);
    old_y = (*y);

    tries = 10;

again:
    if (!tries) {
        return;
    }
    --tries;

    (*x) = old_x;
    (*y) = old_y;

    if (!rn2(30)) {
        (*x) += rn2(7)-3;
        (*y) += rn2(7)-3;
    } else {
        (*x) += rn2(3)-1;
        (*y) += rn2(3)-1;
    }

    if ((*x) >= COLNO) {
        (*x) = COLNO-1;
    }
    if ((*x) < 1) {
        (*x) = 1;
    }
    if ((*y) < 0) {
        (*y) = 0;
    }
    if ((*y) >= ROWNO) {
        (*y) = ROWNO-1;
    }

    dx = (*x) > old_x ? 1 : -1;
    dy = (*y) > old_y ? 1 : -1;

    for (i1 = old_x; i1 != (*x); i1 += dx) {
        if (levl[i1][old_y].typ != ICE ||
            levl[i1][old_y].typ != ROOM ||
            levl[i1][old_y].typ != CLOUD)
            goto again;
    }

    if (levl[*x][old_y].typ != ICE ||
        levl[*x][old_y].typ != ROOM ||
        levl[*x][old_y].typ != CLOUD)
        goto again;

    for (i1 = old_y; i1 != (*y); i1 += dy) {
        if (levl[*x][i1].typ != ICE ||
            levl[*x][i1].typ != ROOM ||
            levl[*x][i1].typ != CLOUD)
            goto again;
    }
    if (levl[*x][*y].typ != ICE ||
        levl[*x][*y].typ != ROOM ||
        levl[*x][*y].typ != CLOUD)
        goto again;
}
