/* NetHack 5.0	mkroom.c	$NHDT-Date: 1781973056 2026/06/20 16:30:56 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.81 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2011. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Entry points:
 *      do_mkroom() -- make and stock a room of a given type
 *      nexttodoor() -- return TRUE if adjacent to a door
 *      has_dnstairs() -- return TRUE if given room has a down staircase
 *      has_upstairs() -- return TRUE if given room has an up staircase
 *      courtmon() -- generate a court monster
 *      save_rooms() -- save rooms into file fd
 *      rest_rooms() -- restore rooms from file fd
 *      cmap_to_type() -- convert S_xxx symbol to XXX topology code
 */

#include "hack.h"

#ifndef SFCTOOL
staticfn struct mkroom *pick_room(boolean);
staticfn void mkshop(void), mkzoo(int), mkswamp(void);
staticfn void mk_zoo_thronemon(coordxy, coordxy, boolean);
staticfn struct permonst *realzoomon(void);
staticfn void mktemple(void);
staticfn coord *shrine_pos(int);
staticfn struct permonst *morguemon(void);
staticfn struct permonst *squadmon(void);
#endif /* SFCTOOL */

staticfn void save_room(NHFILE *, struct mkroom *);
staticfn void rest_room(NHFILE *, struct mkroom *);

#ifndef SFCTOOL
staticfn boolean invalid_shop_shape(struct mkroom *sroom);

#define sq(x) ((x) * (x))

extern const struct shclass shtypes[]; /* defined in shknam.c */

/* make and stock a room of a given type */
void
do_mkroom(int roomtype)
{
    if (roomtype >= SHOPBASE) {
        mkshop(); /* someday, we should be able to specify shop type */
    } else {
        switch (roomtype) {
        case COURT:
            mkzoo(COURT);
            break;
        case ZOO:
            mkzoo(ZOO);
            break;
        case BEEHIVE:
            mkzoo(BEEHIVE);
            break;
        case MORGUE:
            mkzoo(MORGUE);
            break;
        case BARRACKS:
            mkzoo(BARRACKS);
            break;
        case SWAMP:
            mkswamp();
            break;
        case TEMPLE:
            mktemple();
            break;
        case LEPREHALL:
            mkzoo(LEPREHALL);
            break;
        case COCKNEST:
            mkzoo(COCKNEST);
            break;
        case ANTHOLE:
            mkzoo(ANTHOLE);
            break;
        default:
            impossible("Tried to make a room of type %d.", roomtype);
        }
    }
}

staticfn void
mkshop(void)
{
    struct mkroom *sroom;
    int i = -1;
    char *ep = (char *) 0; /* (init == lint suppression) */

    /* first determine shoptype */
    if (wizard) {
        ep = nh_getenv("SHOPTYPE");
        if (ep) {
            if (*ep == 'z' || *ep == 'Z') {
                mkzoo(ZOO);
                return;
            }
            if (*ep == 'm' || *ep == 'M') {
                mkzoo(MORGUE);
                return;
            }
            if (*ep == 'b' || *ep == 'B') {
                mkzoo(BEEHIVE);
                return;
            }
            if (*ep == 't' || *ep == 'T' || *ep == '\\') {
                mkzoo(COURT);
                return;
            }
            if (*ep == 's' || *ep == 'S') {
                mkzoo(BARRACKS);
                return;
            }
            if (*ep == 'a' || *ep == 'A') {
                mkzoo(ANTHOLE);
                return;
            }
            if (*ep == 'c' || *ep == 'C') {
                mkzoo(COCKNEST);
                return;
            }
            if (*ep == 'l' || *ep == 'L') {
                mkzoo(LEPREHALL);
                return;
            }
            if (*ep == '_') {
                mktemple();
                return;
            }
            if (*ep == '}') {
                mkswamp();
                return;
            }
            for (i = 0; shtypes[i].name; i++)
                if (*ep == def_oc_syms[(int) shtypes[i].symb].sym)
                    goto gottype;
            if (*ep == 'g' || *ep == 'G')
                i = 0;
            else if (*ep == 'v' || *ep == 'V')
                i = FODDERSHOP - SHOPBASE; /* veggy food */
            else
                i = -1;
        }
    }

 gottype:
    for (sroom = &svr.rooms[0];; sroom++) {
        /* return from this loop: cannot find any eligible room to be a shop
         * continue: sroom is ineligible
         * break: sroom is eligible
         */
        if (sroom->hx < 0)
            return;
        if (sroom - svr.rooms >= svn.nroom) {
            impossible("rooms[] not closed by -1?");
            return;
        }
        if (sroom->rtype != OROOM)
            continue;
        if (has_dnstairs(sroom) || has_upstairs(sroom)) {
#ifdef STEP11_TEST
            ++custom_generation.shop_stairs_blocked;
#endif
            continue;
        }
        if (sroom->doorct == 1 || (wizard && ep && sroom->doorct != 0)) {
            if (invalid_shop_shape(sroom)) {
#ifdef STEP11_TEST
                ++custom_generation.shop_invalid_shapes;
#endif
                continue;
            } else {
#ifdef STEP11_TEST
                ++custom_generation.shop_room_candidates;
#endif
                break;
            }
        }
#ifdef STEP11_TEST
        else
            ++custom_generation.shop_door_mismatch;
#endif
    }
    if (!sroom->rlit) {
        coordxy x, y;

        for (x = sroom->lx - 1; x <= sroom->hx + 1; x++)
            for (y = sroom->ly - 1; y <= sroom->hy + 1; y++)
                levl[x][y].lit = 1;
        sroom->rlit = 1;
    }

    if (i < 0) { /* shoptype not yet determined */
        int j;

        /* pick a shop type at random */
        for (j = rnd(100), i = 0; (j -= shtypes[i].prob) > 0; i++)
            continue;
    }
#ifdef STEP11_TEST
    custom_generation.shop_selected_type = SHOPBASE + i;
    custom_generation.shop_large = (boolean)
        ((sroom->hx - sroom->lx + 1) * (sroom->hy - sroom->ly + 1) > 20);
#endif
    sroom->rtype = SHOPBASE + i;
#ifdef STEP11_TEST
    custom_generation.shop_type = sroom->rtype;
#endif

    /* set room bits before stocking the shop */
#ifdef SPECIALIZATION
    topologize(sroom, FALSE); /* doesn't matter - this is a special room */
#else
    topologize(sroom);
#endif

    /* The shop used to be stocked here, but this no longer happens--all we do
       is set its rtype, and it gets stocked at the end of makelevel() along
       with other special rooms. */
    sroom->needfill = FILL_NORMAL;
}

/* pick an unused room, preferably with only one door */
staticfn struct mkroom *
pick_room(boolean strict)
{
    struct mkroom *sroom;
    int i = svn.nroom;

    for (sroom = &svr.rooms[rn2(svn.nroom)]; i--; sroom++) {
        if (sroom == &svr.rooms[svn.nroom])
            sroom = &svr.rooms[0];
        if (sroom->hx < 0)
            return (struct mkroom *) 0;
        if (sroom->rtype != OROOM)
            continue;
        if (!strict) {
            if (has_upstairs(sroom) || (has_dnstairs(sroom) && rn2(3)))
                continue;
        } else if (has_upstairs(sroom) || has_dnstairs(sroom))
            continue;
        if (sroom->doorct == 1 || !rn2(5) || wizard)
            return sroom;
    }
    return (struct mkroom *) 0;
}

staticfn void
mkzoo(int type)
{
    struct mkroom *sroom;

    if ((sroom = pick_room(FALSE)) != 0) {
        sroom->rtype = type;
        sroom->needfill = FILL_NORMAL;
    }
}

#ifdef STEP11_TEST
staticfn int step12_library_rn2(int);
#define LIBRARY_RN2(n) step12_library_rn2(n)
#else
#define LIBRARY_RN2(n) rn2(n)
#endif

/* Library is a native custom THEMEROOM, not a new core room type. */
staticfn boolean
library_floor(struct mkroom *room, int x, int y)
{
    int i;
    if (!isok(x, y) || x < room->lx || x > room->hx
        || y < room->ly || y > room->hy || levl[x][y].typ != ROOM
        || occupied(x, y)
        || (room->irregular
            && (levl[x][y].roomno != room - svr.rooms + ROOMOFFSET
                || levl[x][y].edge)))
        return FALSE;
    for (i = room->fdoor; i < room->fdoor + room->doorct; ++i)
        if (distmin(x, y, svd.doors[i].x, svd.doors[i].y) <= 1)
            return FALSE;
    return TRUE;
}

staticfn int
library_squares(struct mkroom *room)
{
    int x, y, count = 0;
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            count += library_floor(room, x, y);
    return count;
}

staticfn struct mkroom *
library_room(void)
{
    int i, start = LIBRARY_RN2(svn.nroom);
    for (i = 0; i < svn.nroom; ++i) {
        struct mkroom *room = &svr.rooms[(start + i) % svn.nroom];
        if (room->rtype == OROOM && !room->custom_id && room->needjoining
            && !room->nsubrooms && !has_upstairs(room) && !has_dnstairs(room)
            && library_squares(room) >= 12) {
            int x, y, free = 0;
            boolean chest = FALSE;
            /* A prepopulated ordinary host must not leak native chests or
             * let our chest cap consume the last unoccupied monster square. */
            for (x = room->lx; x <= room->hx; ++x)
                for (y = room->ly; y <= room->hy; ++y) {
                    if (sobj_at(CHEST, x, y)) chest = TRUE;
                    if (library_floor(room, x, y) && !MON_AT(x, y)) ++free;
                }
            if (chest || free <= (u.uz.dlevel < 100 ? 4 : 5)) continue;
            room->custom_id = CUSTOM_LIBRARY;
            room->rtype = THEMEROOM;
            room->needfill = FILL_NORMAL;
            return room;
        }
    }
    return (struct mkroom *) 0;
}

/* Zero-based themes and all theme state are generation-local. */
staticfn int
library_theme(int dl, int roll)
{
    if (dl < 30) return roll < 50 ? 0 : 1;
    if (dl < 60) return roll < 30 ? 0 : roll < 75 ? 1 : 2;
    if (dl < 100) return roll < 10 ? 0 : roll < 40 ? 1 : roll < 85 ? 2 : 3;
    if (dl < 150) return roll < 15 ? 1 : roll < 70 ? 2 : 3;
    return roll < 5 ? 1 : roll < 55 ? 2 : 3;
}

staticfn int
library_species(int theme, int choice)
{
    static const int pools[4][2] = {
        { PM_KOBOLD_SHAMAN, PM_ORC_SHAMAN },
        { PM_GNOMISH_WIZARD, PM_GNOMISH_WIZARD },
        { PM_LICH, PM_DEMILICH },
        { PM_MIND_FLAYER, PM_MASTER_MIND_FLAYER }
    };
    int pm = pools[theme][choice];
    if (svm.mvitals[pm].mvflags & G_GONE)
        pm = pools[theme][1 - choice];
    return (svm.mvitals[pm].mvflags & G_GONE) ? NON_PM : pm;
}

staticfn boolean
library_density(int dl)
{
    return dl < 60 ? !LIBRARY_RN2(4) : dl < 100 ? !LIBRARY_RN2(3) : LIBRARY_RN2(5) < 2;
}

staticfn struct obj *
library_item(int roll, int subroll)
{
    if (roll < 25) return enhancement_mkobj(SCROLL_CLASS, FALSE);
    if (roll < 65) return enhancement_mkobj(SPBOOK_CLASS, FALSE);
    if (roll < 85)
        return enhancement_mkobj(subroll < 33 ? WAND_CLASS
                     : subroll < 66 ? RING_CLASS : AMULET_CLASS, FALSE);
    return mksobj(roll < 95
                  ? (subroll < 50 ? MAGIC_MARKER : MAGIC_WHISTLE)
                  : (subroll < 50 ? MAGIC_LAMP : BAG_OF_HOLDING), TRUE, FALSE);
}

staticfn void
library_chest_state(struct obj *chest, int dl)
{
    chest->olocked = LIBRARY_RN2(100) < (dl < 60 ? 20 : dl < 100 ? 35 : 50);
    chest->otrapped = LIBRARY_RN2(100) < (dl < 60 ? 5 : dl < 100 ? 10 : 15);
}

staticfn void
library_monster(int theme, int x, int y)
{
    int pm;
    struct monst *mon;
    if (MON_AT(x, y)) return;
    pm = library_species(theme, theme == 1 ? 0 : LIBRARY_RN2(2));
    if (pm == NON_PM) return;
    mon = enhancement_makemon(&mons[pm], x, y, MM_ASLEEP | MM_NOGRP);
    if (mon) mon->msleeping = 1;
}

void
fill_library(struct mkroom *room)
{
    struct obj *chests[5], *chest;
    int dl = u.uz.dlevel, theme, x, y, i, j, n = 0;
    int guaranteed = dl < 60 ? 1 : 2, cap = dl < 100 ? 4 : 5;
    int left, pick, gx = -1, gy = -1;

    /* Native room lighting includes its bordering walls and doors. */
    for (x = room->lx - 1; x <= room->hx + 1; ++x)
        for (y = room->ly - 1; y <= room->hy + 1; ++y)
            if (isok(x, y)) levl[x][y].lit = 1;
    room->rlit = 1;
    theme = library_theme(dl, LIBRARY_RN2(100));

    /* Uniform rank selection without retries, shuffling, or a candidate list. */
    for (i = 0; i < guaranteed; ++i) {
        left = 0;
        for (x = room->lx; x <= room->hx; ++x)
            for (y = room->ly; y <= room->hy; ++y)
                if (library_floor(room, x, y) && !sobj_at(CHEST, x, y))
                    ++left;
        if (!left) panic("Library guaranteed chest has no floor");
        pick = LIBRARY_RN2(left);
        for (x = room->lx; x <= room->hx; ++x)
            for (y = room->ly; y <= room->hy; ++y)
                if (library_floor(room, x, y) && !sobj_at(CHEST, x, y)
                    && !pick--) {
                    chests[n++] = mksobj_at(CHEST, x, y, FALSE, FALSE);
                    goto chest_placed;
                }
 chest_placed:;
    }
    for (x = room->lx; x <= room->hx && n < cap; ++x)
        for (y = room->ly; y <= room->hy && n < cap; ++y)
            if (library_floor(room, x, y) && !sobj_at(CHEST, x, y)
                && library_density(dl))
                chests[n++] = mksobj_at(CHEST, x, y, FALSE, FALSE);

    for (i = 0; i < n; ++i) {
        int count = dl < 60 ? 1 + LIBRARY_RN2(3) : dl < 100 ? 2 + LIBRARY_RN2(3) : 2 + LIBRARY_RN2(4);
        chest = chests[i];
        for (j = 0; j < count; ++j) {
            int roll = LIBRARY_RN2(100), subroll = LIBRARY_RN2(100);
            (void) add_to_container(chest, library_item(roll, subroll));
        }
        chest->owt = weight(chest);
    }
    for (i = 0; i < n; ++i)
        library_chest_state(chests[i], dl);
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            if (library_floor(room, x, y) && !sobj_at(CHEST, x, y)
                && !MON_AT(x, y)) {
                gx = x; gy = y;
                library_monster(theme, x, y);
                goto guaranteed_monster;
            }
 guaranteed_monster:
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            if ((x != gx || y != gy) && library_floor(room, x, y)
                && !sobj_at(CHEST, x, y) && !MON_AT(x, y)
                && library_density(dl))
                library_monster(theme, x, y);
    for (x = room->lx; x <= room->hx; ++x)
        for (y = room->ly; y <= room->hy; ++y)
            if (library_floor(room, x, y) && !sobj_at(CHEST, x, y) && !LIBRARY_RN2(20))
                (void) enhancement_mkobj_at(LIBRARY_RN2(2) ? SCROLL_CLASS : SPBOOK_CLASS, x, y, FALSE);
}

#ifdef STEP11_TEST
#include "../test/test_step12_library.h"
#endif
#undef LIBRARY_RN2

/* Only the custom adapter may use this exhaustive, stair-safe fallback.
 * Vanilla mkzoo keeps its original preference rolls and failure behavior. */
struct mkroom *
custom_classic_room(unsigned id, int type)
{
    struct mkroom *sroom;
    int i;

    if (id == CUSTOM_LIBRARY) return library_room();
    sroom = pick_room(TRUE);

    if (sroom && (!sroom->needjoining || sroom->custom_id))
        sroom = (struct mkroom *) 0;
    if (!sroom)
        for (i = 0; i < svn.nroom; ++i) {
            struct mkroom *candidate = &svr.rooms[i];
            if (candidate->rtype == OROOM && !candidate->custom_id
                && candidate->needjoining && !has_upstairs(candidate)
                && !has_dnstairs(candidate)) {
                sroom = candidate;
                break;
            }
        }
    if (!sroom)
        return (struct mkroom *) 0;
    sroom->custom_id = (unsigned char) id;
    sroom->rtype = type;
    sroom->needfill = FILL_NORMAL;
    return sroom;
}

staticfn void
mk_zoo_thronemon(coordxy x, coordxy y, boolean giantcourt)
{
    int i = rnd(level_difficulty());
    int pm = giantcourt ? PM_TITAN : (i > 9) ? PM_OGRE_TYRANT
        : (i > 5) ? PM_ELVEN_MONARCH
        : (i > 2) ? PM_DWARF_RULER
        : PM_GNOME_RULER;
    struct monst *mon = enhancement_makemon(&mons[pm], x, y, NO_MM_FLAGS);

    if (mon) {
        mon->msleeping = 1;
        mon->mpeaceful = 0;
        set_malign(mon);
        /* Give him a sceptre to pound in judgment */
        (void) mongets(mon, MACE);
    }
}

/* Step 6B, 2026-09-06: population/rewards adapted from NerfHack dev
 * 0cb8781b0929b4617590a3b9fe78f972ef42c25f, src/mkroom.c.
 * Reuse vanilla COURT/ZOO room types and their persistence; the selected
 * owning room identifies which approved imported population to use. */
staticfn struct permonst *
realzoomon(void)
{
    int i = rn2(60) + rn2(3 * level_difficulty());
    int pm = (i > 115) ? PM_MASTODON : (i > 85) ? PM_PYTHON
        : (i > 70) ? PM_MUMAK : (i > 55) ? PM_TIGER
        : (i > 45) ? PM_PANTHER : (i > 25) ? PM_JAGUAR
        : (i > 15) ? PM_APE : PM_MONKEY;

    return (svm.mvitals[pm].mvflags & G_GONE) ? NULL : &mons[pm];
}

void
fill_zoo(struct mkroom *sroom)
{
    struct monst *mon;
    int sx, sy, i;
    int sh, goldlim = 0, type = sroom->rtype;
    int step6b_type = sroom->custom_id;
    boolean giantcourt = (type == COURT
                          && step6b_type == CUSTOM_GIANT_COURT);
    boolean realzoo = (type == ZOO
                      && step6b_type == CUSTOM_REAL_ZOO);
    boolean dragonlair = (type == ZOO
                          && step6b_type == CUSTOM_DRAGON_LAIR);
    coordxy tx = 0, ty = 0;
    int rmno = (int) ((sroom - svr.rooms) + ROOMOFFSET);
    coord mm;

    /* Note: This doesn't check needfill; it assumes the caller has already
       done that. */
    sh = sroom->fdoor;
    switch (type) {
    case COURT:
        if (svl.level.flags.is_maze_lev) {
            for (tx = sroom->lx; tx <= sroom->hx; tx++)
                for (ty = sroom->ly; ty <= sroom->hy; ty++)
                    if (IS_THRONE(levl[tx][ty].typ))
                        goto throne_placed;
        }
        i = 100;
        do { /* don't place throne on top of stairs */
            (void) somexyspace(sroom, &mm);
            tx = mm.x;
            ty = mm.y;
        } while (occupied(tx, ty) && --i > 0);
 throne_placed:
        mk_zoo_thronemon(tx, ty, giantcourt);
        break;
    case BEEHIVE:
        tx = sroom->lx + (sroom->hx - sroom->lx + 1) / 2;
        ty = sroom->ly + (sroom->hy - sroom->ly + 1) / 2;
        if (sroom->irregular) {
            /* center might not be valid, so put queen elsewhere */
            if ((int) levl[tx][ty].roomno != rmno || levl[tx][ty].edge) {
                (void) somexyspace(sroom, &mm);
                tx = mm.x;
                ty = mm.y;
            }
        }
        break;
    case ZOO:
    case LEPREHALL:
        goldlim = (dragonlair ? 1500 : 500) * level_difficulty();
        break;
    }

    for (sx = sroom->lx; sx <= sroom->hx; sx++)
        for (sy = sroom->ly; sy <= sroom->hy; sy++) {
            if (sroom->irregular) {
                if ((int) levl[sx][sy].roomno != rmno || levl[sx][sy].edge
                    || (sroom->doorct
                        && (distmin(sx, sy, svd.doors[sh].x, svd.doors[sh].y)
                            <= 1)))
                    continue;
            } else if (!SPACE_POS(levl[sx][sy].typ)
                       || (sroom->doorct
                           && ((sx == sroom->lx && svd.doors[sh].x == sx - 1)
                               || (sx == sroom->hx && svd.doors[sh].x
                                   == sx + 1)
                               || (sy == sroom->ly && svd.doors[sh].y
                                   == sy - 1)
                               || (sy == sroom->hy
                                   && svd.doors[sh].y == sy + 1))))
                continue;
            /* don't place monster on explicitly placed throne */
            if (type == COURT && IS_THRONE(levl[sx][sy].typ))
                continue;
            /* Do not turn an exhausted imported population into random
             * monsters via enhancement_makemon(NULL). Vanilla filling stays unchanged. */
            if (giantcourt || realzoo || dragonlair) {
                struct permonst *pm = giantcourt ? mkclass(S_GIANT, 0)
                    : dragonlair ? mkclass(S_DRAGON, 0) : realzoomon();
                mon = pm ? enhancement_makemon(pm, sx, sy, MM_ASLEEP | MM_NOGRP) : NULL;
            } else
            mon = enhancement_makemon((type == COURT)
                           ? courtmon()
                           : (type == BARRACKS)
                              ? squadmon()
                              : (type == MORGUE)
                                 ? morguemon()
                                 : (type == BEEHIVE)
                                     ? (sx == tx && sy == ty
                                         ? &mons[PM_QUEEN_BEE]
                                         : &mons[PM_KILLER_BEE])
                                     : (type == LEPREHALL)
                                         ? &mons[PM_LEPRECHAUN]
                                         : (type == COCKNEST)
                                             ? &mons[PM_COCKATRICE]
                                             : (type == ANTHOLE)
                                                 ? antholemon()
                                                 : (struct permonst *) 0,
                          sx, sy, MM_ASLEEP | MM_NOGRP);
            if (mon) {
                mon->msleeping = 1;
                if ((type == COURT || realzoo) && mon->mpeaceful) {
                    mon->mpeaceful = 0;
                    set_malign(mon);
                }
            }
            if (giantcourt && !rn2(111))
                (void) mksobj_at(TINNING_KIT, sx, sy, TRUE, FALSE);
            if (dragonlair && !rn2(20))
                (void) mksobj_at(
                    rnd_class(GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES),
                    sx, sy, FALSE, FALSE);
            switch (type) {
            case ZOO:
            case LEPREHALL:
                if (sroom->doorct) {
                    int distval = dist2(sx, sy,
                                        svd.doors[sh].x, svd.doors[sh].y);
                    i = sq(distval);
                } else
                    i = goldlim;
                if (i >= goldlim)
                    i = 5 * level_difficulty();
                goldlim -= i;
                (void) mkgold((long) rn1(i, 10), sx, sy);
                break;
            case MORGUE:
                if (!rn2(5))
                    (void) mk_tt_object(CORPSE, sx, sy);
                if (!rn2(10)) /* lots of treasure buried with dead */
                    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST, sx, sy,
                                     TRUE, FALSE);
                if (!rn2(5))
                    make_grave(sx, sy, (char *) 0);
                else if (moria_level(&u.uz) == 6 && !rn2(1000)) {
                    struct engr *ep;
                    boolean exists = FALSE;

                    for (ep = head_engr; ep; ep = ep->nxt_engr)
                        if (!strncmp(ep->engr_txt[actual_text], "Guest41,", 8))
                            exists = TRUE;
                    if (!exists)
                        make_grave(sx, sy,
                            "Guest41, Wherever you are, I hope you're doing fine");
                }
                break;
            case BEEHIVE:
                if (!rn2(3))
                    (void) mksobj_at(LUMP_OF_ROYAL_JELLY, sx, sy, TRUE,
                                     FALSE);
                break;
            case BARRACKS:
                if (!rn2(20)) /* the payroll and some loot */
                    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST, sx, sy,
                                     TRUE, FALSE);
                break;
            case COCKNEST:
                if (!rn2(3)) {
                    struct obj *sobj = mk_tt_object(STATUE, sx, sy);

                    if (sobj) {
                        for (i = rn2(5); i; i--)
                            (void) add_to_container(
                                sobj, enhancement_mkobj(RANDOM_CLASS, FALSE));
                        sobj->owt = weight(sobj);
                    }
                }
                break;
            case ANTHOLE:
                if (!rn2(3))
                    (void) enhancement_mkobj_at(FOOD_CLASS, sx, sy, FALSE);
                break;
            }
        }
    switch (type) {
    case COURT: {
        struct obj *chest, *gold;
        levl[tx][ty].typ = THRONE;
        (void) somexyspace(sroom, &mm);
        gold = mksobj(GOLD_PIECE, TRUE, FALSE);
        gold->quan = (long) rn1(50 * level_difficulty(), 10);
        gold->owt = weight(gold);
        /* the royal coffers */
        chest = mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE);
        add_to_container(chest, gold);
        chest->owt = weight(chest);
        chest->spe = 2; /* so it can be found later */
        svl.level.flags.has_court = 1;
        break;
    }
    case BARRACKS:
        svl.level.flags.has_barracks = 1;
        break;
    case ZOO:
        svl.level.flags.has_zoo = 1;
        break;
    case MORGUE:
        svl.level.flags.has_morgue = 1;
        break;
    case SWAMP:
        svl.level.flags.has_swamp = 1;
        break;
    case BEEHIVE:
        svl.level.flags.has_beehive = 1;
        break;
    }
}

/* make a swarm of undead around mm */
void
mkundead(
    coord *mm,
    boolean revive_corpses,
    int mm_flags)
{
    int cnt = (level_difficulty() + 1) / 10 + rnd(5);
    struct permonst *mdat;
    struct obj *otmp;
    coord cc;

    while (cnt--) {
        mdat = morguemon();
        if (mdat && enexto(&cc, mm->x, mm->y, mdat)
            && (!revive_corpses
                || !(otmp = sobj_at(CORPSE, cc.x, cc.y))
                || !revive(otmp, FALSE)))
            (void) enhancement_makemon(mdat, cc.x, cc.y, mm_flags);
    }
    svl.level.flags.graveyard = TRUE; /* reduced chance for undead corpse */
}

staticfn struct permonst *
morguemon(void)
{
    int i = rn2(100), hd = rn2(level_difficulty());

    if (hd > 10 && i < 10) {
        if (Inhell || In_endgame(&u.uz)) {
            return mkclass(S_DEMON, 0);
        } else {
            int ndemon_res = ndemon(A_NONE);
            if (ndemon_res != NON_PM)
                return &mons[ndemon_res];
            /* else do what? As is, it will drop to ghost/wraith/zombie */
        }
    }

    if (hd > 8 && i > 85)
        return mkclass(S_VAMPIRE, 0);

    return ((i < 20) ? &mons[PM_GHOST]
                     : (i < 40) ? &mons[PM_WRAITH]
                                : mkclass(S_ZOMBIE, 0));
}

struct permonst *
antholemon(void)
{
    int mtyp, indx, trycnt = 0;

    /* casts are for dealing with time_t */
    indx = (int) ((long) ubirthday % 3L);
    indx += level_difficulty();
    /* Same monsters within a level, different ones between levels */
    do {
        switch ((indx + trycnt) % 3) {
        case 0:
            mtyp = PM_SOLDIER_ANT;
            break;
        case 1:
            mtyp = PM_FIRE_ANT;
            break;
        default:
            mtyp = PM_GIANT_ANT;
            break;
        }
        /* try again if chosen type has been genocided or used up */
    } while (++trycnt < 3 && (svm.mvitals[mtyp].mvflags & G_GONE));

    return ((svm.mvitals[mtyp].mvflags & G_GONE) ? (struct permonst *) 0
                                             : &mons[mtyp]);
}

staticfn void
mkswamp(void) /* Michiel Huisjes & Fred de Wilde */
{
    struct mkroom *sroom;
    int i, eelct = 0;
    coordxy sx, sy;
    int rmno;

    for (i = 0; i < 5; i++) { /* turn up to 5 rooms swampy */
        sroom = &svr.rooms[rn2(svn.nroom)];
        if (sroom->hx < 0 || sroom->rtype != OROOM || has_upstairs(sroom)
            || has_dnstairs(sroom))
            continue;

        rmno = (int)(sroom - svr.rooms) + ROOMOFFSET;

        /* satisfied; make a swamp */
        sroom->rtype = SWAMP;
        for (sx = sroom->lx; sx <= sroom->hx; sx++)
            for (sy = sroom->ly; sy <= sroom->hy; sy++) {
                if (!IS_ROOM(levl[sx][sy].typ) || IS_FORGE(levl[sx][sy].typ)
                    || (int) levl[sx][sy].roomno != rmno)
                    continue;
                if (!OBJ_AT(sx, sy) && !MON_AT(sx, sy) && !t_at(sx, sy)
                    && !nexttodoor(sx, sy)) {
                    if ((sx + sy) % 2) {
                        del_engr_at(sx, sy);
                        levl[sx][sy].typ = POOL;
                        if (!eelct || !rn2(4)) {
                            /* mkclass() won't do, as we might get kraken */
                            (void) enhancement_makemon(rn2(5)
                                              ? &mons[PM_GIANT_EEL]
                                              : rn2(2)
                                                 ? &mons[PM_PIRANHA]
                                                 : &mons[PM_ELECTRIC_EEL],
                                           sx, sy, NO_MM_FLAGS);
                            eelct++;
                        }
                    } else if (!rn2(4)) /* swamps tend to be moldy */
                        (void) enhancement_makemon(mkclass(S_FUNGUS, 0), sx, sy,
                                       NO_MM_FLAGS);
                }
            }
        svl.level.flags.has_swamp = 1;
    }
}

/* return center of room, or a random free location
   if center is blocked */
staticfn coord *
shrine_pos(int roomno)
{
    static coord buf;
    int delta;
    struct mkroom *troom = &svr.rooms[roomno - ROOMOFFSET];

    /* if width and height are odd, placement will be the exact center;
       if either or both are even, center point is a hypothetical spot
       between map locations and placement will be adjacent to that */
    delta = troom->hx - troom->lx;
    buf.x = troom->lx + delta / 2;
    if ((delta % 2) && rn2(2))
        buf.x++;
    delta = troom->hy - troom->ly;
    buf.y = troom->ly + delta / 2;
    if ((delta % 2) && rn2(2))
        buf.y++;

    /* irregular room or the location is blocked */
    if (roomno != (int) levl[buf.x][buf.y].roomno
        || (levl[buf.x][buf.y].typ != ROOM
            && levl[buf.x][buf.y].typ != ICE
            && levl[buf.x][buf.y].typ != CLOUD))
        (void) somexyspace(troom, &buf);

    return &buf;
}

staticfn void
mktemple(void)
{
    struct mkroom *sroom;
    coord *shrine_spot;
    struct rm *lev;

    if (!(sroom = pick_room(TRUE)))
        return;

    /* set up Priest and shrine */
    sroom->rtype = TEMPLE;
    /*
     * In temples, shrines are blessed altars
     * located in the center of the room
     */
    shrine_spot = shrine_pos((int) ((sroom - svr.rooms) + ROOMOFFSET));
    lev = &levl[shrine_spot->x][shrine_spot->y];
    lev->typ = ALTAR;
    lev->altarmask = induced_align(80);
    priestini(&u.uz, sroom, shrine_spot->x, shrine_spot->y, FALSE);
    lev->altarmask |= AM_SHRINE;
    svl.level.flags.has_temple = 1;
}

boolean
nexttodoor(int sx, int sy)
{
    int dx, dy;
    struct rm *lev;

    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++) {
            if (!isok(sx + dx, sy + dy))
                continue;
            lev = &levl[sx + dx][sy + dy];
            if (IS_DOOR(lev->typ) || lev->typ == SDOOR)
                return TRUE;
        }
    return FALSE;
}

boolean
has_dnstairs(struct mkroom *sroom)
{
    stairway *stway = gs.stairs;

    while (stway) {
        if (!stway->up && inside_room(sroom, stway->sx, stway->sy))
            return TRUE;
        stway = stway->next;
    }
    return FALSE;
}

boolean
has_upstairs(struct mkroom *sroom)
{
    stairway *stway = gs.stairs;

    while (stway) {
        if (stway->up && inside_room(sroom, stway->sx, stway->sy))
            return TRUE;
        stway = stway->next;
    }
    return FALSE;
}

int
somex(struct mkroom *croom)
{
    return rn1(croom->hx - croom->lx + 1, croom->lx);
}

int
somey(struct mkroom *croom)
{
    return rn1(croom->hy - croom->ly + 1, croom->ly);
}

boolean
inside_room(struct mkroom *croom, coordxy x, coordxy y)
{
    if (croom->irregular) {
        int i = (int) ((croom - svr.rooms) + ROOMOFFSET);
        return (!levl[x][y].edge && (int) levl[x][y].roomno == i);
    }

    return (boolean) (x >= croom->lx - 1 && x <= croom->hx + 1
                      && y >= croom->ly - 1 && y <= croom->hy + 1);
}

/* return a coord c inside mkroom croom, but not in a subroom.
   returns TRUE if any such space found.
   can return a non-accessible location, eg. inside a wall
   if a themed room is not irregular, but has some non-room terrain */
boolean
somexy(struct mkroom *croom, coord *c)
{
    int try_cnt = 0;
    int i;

    if (croom->irregular) {
        i = (int) ((croom - svr.rooms) + ROOMOFFSET);

        while (try_cnt++ < 100) {
            c->x = somex(croom);
            c->y = somey(croom);
            if (!levl[c->x][c->y].edge && (int) levl[c->x][c->y].roomno == i)
                return TRUE;
        }
        /* try harder; exhaustively search until one is found */
        for (c->x = croom->lx; c->x <= croom->hx; c->x++)
            for (c->y = croom->ly; c->y <= croom->hy; c->y++)
                if (!levl[c->x][c->y].edge
                    && (int) levl[c->x][c->y].roomno == i)
                    return TRUE;
        return FALSE;
    }

    if (!croom->nsubrooms) {
        c->x = somex(croom);
        c->y = somey(croom);
        return TRUE;
    }

    /* Check that coords doesn't fall into a subroom or into a wall */

    while (try_cnt++ < 100) {
        c->x = somex(croom);
        c->y = somey(croom);
        if (IS_WALL(levl[c->x][c->y].typ))
            continue;
        for (i = 0; i < croom->nsubrooms; i++)
            if (inside_room(croom->sbrooms[i], c->x, c->y))
                goto you_lose;
        break;
 you_lose:
        ;
    }
    if (try_cnt >= 100)
        return FALSE;
    return TRUE;
}

/* like somexy(), but returns an accessible location */
boolean
somexyspace(struct mkroom* croom, coord *c)
{
    int trycnt = 0;
    boolean okay;

    do {
        okay = somexy(croom, c) && isok(c->x, c->y) && !occupied(c->x, c->y)
            && (levl[c->x][c->y].typ == ROOM
                || levl[c->x][c->y].typ == CORR
                || levl[c->x][c->y].typ == ICE);
    } while (trycnt++ < 100 && !okay);
    return okay;
}

/*
 * Search for a special room given its type (zoo, court, etc...)
 *      Special values :
 *              - ANY_SHOP
 *              - ANY_TYPE
 */
struct mkroom *
search_special(schar type)
{
    struct mkroom *croom;

    for (croom = &svr.rooms[0]; croom->hx >= 0; croom++)
        if ((type == ANY_TYPE && croom->rtype != OROOM)
            || (type == ANY_SHOP && croom->rtype >= SHOPBASE)
            || croom->rtype == type)
            return croom;
    for (croom = &gs.subrooms[0]; croom->hx >= 0; croom++)
        if ((type == ANY_TYPE && croom->rtype != OROOM)
            || (type == ANY_SHOP && croom->rtype >= SHOPBASE)
            || croom->rtype == type)
            return croom;
    return (struct mkroom *) 0;
}

struct permonst *
courtmon(void)
{
    int i = rn2(60) + rn2(3 * level_difficulty());

    if (i > 100)
        return mkclass(S_DRAGON, 0);
    else if (i > 95)
        return mkclass(S_GIANT, 0);
    else if (i > 85)
        return mkclass(S_TROLL, 0);
    else if (i > 75)
        return mkclass(S_CENTAUR, 0);
    else if (i > 60)
        return mkclass(S_ORC, 0);
    else if (i > 45)
        return &mons[PM_BUGBEAR];
    else if (i > 30)
        return &mons[PM_HOBGOBLIN];
    else if (i > 15)
        return mkclass(S_GNOME, 0);
    else
        return mkclass(S_KOBOLD, 0);
}

static const struct {
    unsigned pm;
    unsigned prob;
} squadprob[] = { { PM_SOLDIER, 80 },
                  { PM_SERGEANT, 15 },
                  { PM_LIEUTENANT, 4 },
                  { PM_CAPTAIN, 1 } };

/* return soldier types. */
staticfn struct permonst *
squadmon(void)
{
    int sel_prob, i, cpro, mndx;

    sel_prob = rnd(80 + level_difficulty());

    cpro = 0;
    for (i = 0; i < SIZE(squadprob); i++) {
        cpro += squadprob[i].prob;
        if (cpro > sel_prob) {
            mndx = squadprob[i].pm;
            goto gotone;
        }
    }
    mndx = ROLL_FROM(squadprob).pm;
 gotone:
    if (!(svm.mvitals[mndx].mvflags & G_GONE))
        return &mons[mndx];
    else
        return (struct permonst *) 0;
}

/*
 * save_room : A recursive function that saves a room and its subrooms
 * (if any).
 */
staticfn void
save_room(NHFILE *nhfp, struct mkroom *r)
{
    short i;

    /*
     * Well, I really should write only useful information instead
     * of writing the whole structure. That is I should not write
     * the gs.subrooms pointers, but who cares ?
     */
    Sfo_mkroom(nhfp, r, "room-mkroom");
    for (i = 0; i < r->nsubrooms; i++) {
        save_room(nhfp, r->sbrooms[i]);
    }
}

/*
 * save_rooms : Save all the rooms on disk!
 */
void
save_rooms(NHFILE *nhfp)
{
    short i;

    /* First, write the number of rooms */
    Sfo_int(nhfp, &svn.nroom, "room-nroom");
    for (i = 0; i < svn.nroom; i++)
        save_room(nhfp, &svr.rooms[i]);
}
#endif /* !SFCTOOL */

staticfn void
rest_room(NHFILE *nhfp, struct mkroom *r)
{
    short i;

    Sfi_mkroom(nhfp, r, "room-mkroom");

    for (i = 0; i < r->nsubrooms; i++) {
        r->sbrooms[i] = &gs.subrooms[gn.nsubroom];
        rest_room(nhfp, &gs.subrooms[gn.nsubroom]);
        gs.subrooms[gn.nsubroom++].resident = (struct monst *) 0;
    }
}

/*
 * rest_rooms : That's for restoring rooms. Read the rooms structure from
 * the disk.
 */
void
rest_rooms(NHFILE *nhfp)
{
    short i;

    Sfi_int(nhfp, &svn.nroom, "room-nroom");

    gn.nsubroom = 0;
    for (i = 0; i < svn.nroom; i++) {
        rest_room(nhfp, &svr.rooms[i]);
        svr.rooms[i].resident = (struct monst *) 0;
    }
    svr.rooms[svn.nroom].hx = -1; /* restore ending flags */
    gs.subrooms[gn.nsubroom].hx = -1;
}

#ifndef SFCTOOL
/* convert a display symbol for terrain into topology type;
   used for remembered terrain when mimics pose as furniture */
int
cmap_to_type(int sym)
{
    int typ = STONE; /* catchall */

    switch (sym) {
    case S_stone:
        typ = STONE;
        break;
    case S_vwall:
        typ = VWALL;
        break;
    case S_hwall:
        typ = HWALL;
        break;
    case S_tlcorn:
        typ = TLCORNER;
        break;
    case S_trcorn:
        typ = TRCORNER;
        break;
    case S_blcorn:
        typ = BLCORNER;
        break;
    case S_brcorn:
        typ = BRCORNER;
        break;
    case S_crwall:
        typ = CROSSWALL;
        break;
    case S_tuwall:
        typ = TUWALL;
        break;
    case S_tdwall:
        typ = TDWALL;
        break;
    case S_tlwall:
        typ = TLWALL;
        break;
    case S_trwall:
        typ = TRWALL;
        break;
    case S_ndoor:  /* no door (empty doorway) */
    case S_vodoor: /* open door in vertical wall */
    case S_hodoor: /* open door in horizontal wall */
    case S_vcdoor: /* closed door in vertical wall */
    case S_hcdoor:
        typ = DOOR;
        break;
    case S_bars:
        typ = IRONBARS;
        break;
    case S_tree:
        typ = TREE;
        break;
    case S_room:
    case S_darkroom:
        typ = ROOM;
        break;
    case S_corr:
    case S_litcorr:
        typ = CORR;
        break;
    case S_upstair:
    case S_dnstair:
        typ = STAIRS;
        break;
    case S_upladder:
    case S_dnladder:
        typ = LADDER;
        break;
    case S_altar:
        typ = ALTAR;
        break;
    case S_grave:
        typ = GRAVE;
        break;
    case S_throne:
        typ = THRONE;
        break;
    case S_sink:
        typ = SINK;
        break;
    case S_fountain:
        typ = FOUNTAIN;
        break;
    case S_pool:
        typ = POOL;
        break;
    case S_ice:
        typ = ICE;
        break;
    case S_lava:
        typ = LAVAPOOL;
        break;
    case S_vodbridge: /* open drawbridge spanning north/south */
    case S_hodbridge:
        typ = DRAWBRIDGE_DOWN;
        break;        /* east/west */
    case S_vcdbridge: /* closed drawbridge in vertical wall */
    case S_hcdbridge:
        typ = DBWALL;
        break;
    case S_air:
        typ = AIR;
        break;
    case S_cloud:
        typ = CLOUD;
        break;
    case S_water:
        typ = WATER;
        break;
    case S_lavawall:
        typ = LAVAWALL;
        break;
    default:
        break; /* not a cmap symbol? */
    }
    return typ;
}

/* Step 10C-C Outlands post-processing.  The pinned donor owns these features
 * in mkroom.c and invokes them from makemaz() after a special map is loaded.
 * Keep the same ownership here.  The protected path is a local generation
 * guard, not persistent state: it preserves the two resolved portal cells and
 * one ordinary four-way walking route between them while features are placed. */
static boolean outlands_protected[COLNO][ROWNO];
static struct rm outlands_protected_rm[COLNO][ROWNO];

#ifdef STEP10C_C_TEST
extern void step10c_c_note_feature(const char *);
#define OUTLANDS_NOTE(s) step10c_c_note_feature(s)
#else
#define OUTLANDS_NOTE(s) ((void) 0)
#endif

staticfn boolean
outlands_walkable(coordxy x, coordxy y)
{
    int typ;

    if (!isok(x, y))
        return FALSE;
    typ = levl[x][y].typ;
    return (boolean) (ACCESSIBLE(typ) && !IS_POOL(typ) && typ != WATER
                      && typ != LAVAPOOL && typ != LAVAWALL);
}

staticfn boolean
outlands_route_candidate(coordxy x, coordxy y)
{
    int typ;

    if (!isok(x, y))
        return FALSE;
    typ = levl[x][y].typ;
    return (boolean) (outlands_walkable(x, y) || typ == TREE || typ == GRASS
                      || typ == SOIL || typ == SAND || typ == PUDDLE);
}

staticfn boolean
outlands_connector(coordxy x, coordxy y)
{
    struct trap *ttmp;

    if (!isok(x, y))
        return FALSE;
    if (levl[x][y].typ == STAIRS || levl[x][y].typ == LADDER)
        return TRUE;
    ttmp = t_at(x, y);
    return (boolean) (ttmp && ttmp->ttyp == MAGIC_PORTAL);
}

staticfn void
outlands_protect_route(void)
{
    coordxy qx[COLNO * ROWNO], qy[COLNO * ROWNO];
    short prev[COLNO * ROWNO];
    coord portals[2];
    int head = 0, tail = 0, count = 0, i, x, y, nx, ny, cur, next;
    static const schar dx[4] = { 1, -1, 0, 0 };
    static const schar dy[4] = { 0, 0, 1, -1 };

    (void) memset(outlands_protected, 0, sizeof outlands_protected);
    (void) memset(outlands_protected_rm, 0, sizeof outlands_protected_rm);
    for (x = 1; x < COLNO; ++x)
        for (y = 0; y < ROWNO; ++y) {
            struct trap *ttmp;

            if (outlands_connector((coordxy) x, (coordxy) y)) {
                outlands_protected[x][y] = TRUE;
                outlands_protected_rm[x][y] = levl[x][y];
            }
            ttmp = t_at((coordxy) x, (coordxy) y);
            if (ttmp && ttmp->ttyp == MAGIC_PORTAL) {
                if (count < 2) {
                    portals[count].x = (coordxy) x;
                    portals[count].y = (coordxy) y;
                }
                ++count;
            }
        }
    if (count < 2)
        return;

    for (i = 0; i < SIZE(prev); ++i)
        prev[i] = -1;
    cur = portals[0].y * COLNO + portals[0].x;
    prev[cur] = cur;
    qx[tail] = portals[0].x;
    qy[tail++] = portals[0].y;
    while (head < tail) {
        x = qx[head];
        y = qy[head++];
        if (x == portals[1].x && y == portals[1].y)
            break;
        cur = y * COLNO + x;
        for (i = 0; i < 4; ++i) {
            nx = x + dx[i];
            ny = y + dy[i];
            if (!outlands_route_candidate((coordxy) nx, (coordxy) ny))
                continue;
            next = ny * COLNO + nx;
            if (prev[next] >= 0)
                continue;
            prev[next] = (short) cur;
            qx[tail] = (coordxy) nx;
            qy[tail++] = (coordxy) ny;
        }
    }
    cur = portals[1].y * COLNO + portals[1].x;
    if (prev[cur] < 0)
        return;
    while (TRUE) {
        x = cur % COLNO;
        y = cur / COLNO;
        outlands_protected[x][y] = TRUE;
        if (!outlands_walkable((coordxy) x, (coordxy) y)) {
            levl[x][y].typ = GRASS;
            levl[x][y].lit = 1;
        }
        outlands_protected_rm[x][y] = levl[x][y];
        if (prev[cur] == cur)
            break;
        cur = prev[cur];
    }
}

staticfn void
outlands_restore_route(void)
{
    int x, y;

    for (x = 1; x < COLNO; ++x)
        for (y = 0; y < ROWNO; ++y)
            if (outlands_protected[x][y])
                levl[x][y] = outlands_protected_rm[x][y];
}

staticfn boolean
outlands_ground(int typ)
{
    return (boolean) (typ == TREE || typ == ROOM || typ == GRASS
                      || typ == SOIL || typ == SAND || typ == PUDDLE);
}

staticfn boolean
outlands_area(coordxy x, coordxy y, int w, int h, boolean trees_only)
{
    int i, j, typ;

    for (i = 0; i < w; ++i)
        for (j = 0; j < h; ++j) {
            if (!isok(x + i, y + j) || outlands_protected[x + i][y + j]
                || t_at(x + i, y + j))
                return FALSE;
            typ = levl[x + i][y + j].typ;
            if (trees_only ? (typ != TREE && typ != GRASS)
                           : !outlands_ground(typ))
                return FALSE;
        }
    return TRUE;
}

staticfn void
outlands_relocate(coordxy x, coordxy y)
{
    struct monst *mtmp = m_at(x, y);

    if (mtmp)
        (void) rloc(mtmp, RLOC_NOMSG);
}

staticfn void
outlands_terrain(coordxy x, coordxy y, schar typ, boolean lit)
{
    if (!isok(x, y) || outlands_protected[x][y]
        || outlands_connector(x, y) || t_at(x, y))
        return;
    outlands_relocate(x, y);
    levl[x][y].typ = typ;
    levl[x][y].lit = lit;
}

staticfn void
outlands_wall_box(coordxy x, coordxy y, int w, int h, schar floor)
{
    int i, j;

    for (i = 0; i < w; ++i)
        for (j = 0; j < h; ++j)
            outlands_terrain(x + i, y + j,
                             (i == 0 || j == 0 || i == w - 1 || j == h - 1)
                                 ? HWALL : floor,
                             TRUE);
    wallification(x, y, x + w - 1, y + h - 1);
}

staticfn void
outlands_door(coordxy x, coordxy y)
{
    if (!outlands_protected[x][y] && !t_at(x, y)) {
        levl[x][y].typ = DOOR;
        levl[x][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
    }
}

void
neuliquify(coordxy x, coordxy y, boolean edge)
{
    int typ, monster = PM_JELLYFISH, dep;

    if (!isok(x, y) || outlands_protected[x][y] || outlands_connector(x, y)
        || t_at(x, y))
        return;
    if (svl.level.flags.has_shop && *in_rooms(x, y, SHOPBASE))
        return;
    typ = levl[x][y].typ;
    if (typ != TREE && typ != GRASS && typ != SOIL && typ != SAND)
        return;
    OUTLANDS_NOTE("river-cell");
    if (typ != TREE || (!edge && rn2(6))) {
        levl[x][y].typ = (typ == TREE) ? POOL : PUDDLE;
        if (typ == TREE)
            outlands_relocate(x, y);
    }
    dep = depth(&u.uz);
    if (levl[x][y].typ == POOL) {
        if (!rn2(max(1, 85 - dep))) {
            if (dep > 19 && !rn2(3))
                monster = PM_ELECTRIC_EEL;
            else if (dep > 15 && !rn2(3))
                monster = PM_GIANT_EEL;
            else if (dep > 11 && !rn2(2))
                monster = PM_SHARK;
            else if (dep > 7 && rn2(4))
                monster = PM_PIRANHA;
            (void) enhancement_makemon(&mons[monster], x, y, NO_MM_FLAGS);
        }
        if (!rn2(max(1, 140 - dep)))
            (void) enhancement_mkobj_at(RANDOM_CLASS, x, y, FALSE);
        else if (!rn2(max(1, 100 - dep)))
            (void) mkgold((long) rn1(10 * level_difficulty(), 10), x, y);
    }
    levl[x][y].lit = 1;
}

void
mkneuriver(void)
{
    int center, width, prog, fill;
    boolean edge;

    OUTLANDS_NOTE("river");
    if (!rn2(4)) {
        center = rn2(ROWNO - 12) + 6;
        width = rn2(4) + 4;
        for (prog = 1; prog < COLNO; ++prog) {
            edge = TRUE;
            for (fill = center - width / 2; fill <= center + width / 2;
                 ++fill) {
                neuliquify((coordxy) prog, (coordxy) fill, edge);
                edge = (boolean) (fill == center + width / 2 - 1);
            }
            if (!rn2(3)) {
                if (!rn2(2) && width > 4)
                    --width;
                else if (width < 7)
                    ++width;
            }
            if (!rn2(3)) {
                if (!rn2(2) && center - width / 2 > 1)
                    --center;
                else if (center + width / 2 < ROWNO - 1)
                    ++center;
            }
            center = max(4, min(center, ROWNO - 5));
        }
    } else {
        center = rn2(COLNO - 14) + 7;
        width = rn2(4) + 5;
        for (prog = 0; prog < ROWNO; ++prog) {
            edge = TRUE;
            for (fill = center - width / 2; fill <= center + width / 2;
                 ++fill) {
                neuliquify((coordxy) fill, (coordxy) prog, edge);
                edge = (boolean) (fill == center + width / 2 - 1);
            }
            if (!rn2(3)) {
                if (!rn2(2) && width > 5)
                    --width;
                else if (width < 8)
                    ++width;
            }
            if (!rn2(3)) {
                if (!rn2(2) && center - width / 2 > 1)
                    --center;
                else if (center + width / 2 < COLNO - 1)
                    ++center;
            }
            center = max(5, min(center, COLNO - 6));
        }
    }
}

staticfn struct obj *
outlands_name_artifact(int otyp, int artinum, coordxy x, coordxy y)
{
    struct obj *otmp = mksobj(otyp, FALSE, FALSE);

    if (!otmp)
        return (struct obj *) 0;
    otmp = oname(otmp, artiname(artinum), ONAME_RANDOM);
    otmp->spe = max(1, otmp->spe);
    otmp->cursed = otmp->blessed = 0;
    place_object(otmp, x, y);
    return otmp;
}

void
mkkamereltowers(void)
{
    int x = 0, y = 0, tx = 0, ty = 0, tries = 0;
    int i, j, c, edge;
    boolean left = (boolean) rn2(2), good = FALSE;
    int slant = rn2(3);
    struct obj *otmp;
    /* The donor's Lillend identity is not present locally; retain every other
     * member of its fixed serpent/statue pool rather than invent a fallback. */
    static const int snakes[] = {
        PM_PYTHON, PM_COBRA, PM_PIT_VIPER,
        PM_PYTHON, PM_COBRA, PM_PIT_VIPER,
        PM_PYTHON, PM_COBRA, PM_PIT_VIPER,
        PM_LONG_WORM, PM_PURPLE_WORM, PM_COUATL,
        PM_RED_NAGA, PM_BLACK_NAGA, PM_GOLDEN_NAGA, PM_GUARDIAN_NAGA,
        PM_GIANT_EEL, PM_ELECTRIC_EEL, PM_SALAMANDER, PM_MARILITH
    };

    OUTLANDS_NOTE("kamerel");
    edge = left ? rn1(20, 20) : COLNO - rn1(20, 20);
    for (j = 0; j < ROWNO; ++j) {
        if (left) {
            for (i = 1; i < edge; ++i)
                if (isok(i, j) && outlands_ground(levl[i][j].typ)
                    && !outlands_protected[i][j] && !t_at(i, j)) {
                    if (levl[i][j].typ != TREE || edge - i > rn2(6))
                        outlands_terrain(i, j, PUDDLE, TRUE);
                }
        } else {
            for (i = COLNO - 1; i > edge; --i)
                if (isok(i, j) && outlands_ground(levl[i][j].typ)
                    && !outlands_protected[i][j] && !t_at(i, j)) {
                    if (levl[i][j].typ != TREE || i - edge > rn2(6))
                        outlands_terrain(i, j, PUDDLE, TRUE);
                }
        }
        if (rn2(4))
            edge += rn2(3) - slant;
    }
    while (!good && tries++ < 500) {
        x = left ? 4 + rnd(3) + rn2(3) : COLNO - (4 + rnd(3) + rn2(3));
        y = 6 + rn2(10);
        good = TRUE;
        for (i = -3; i <= 3; ++i)
            for (j = -3; j <= 3; ++j)
                if (!isok(x + i, y + j) || levl[x + i][y + j].typ != PUDDLE
                    || outlands_protected[x + i][y + j])
                    good = FALSE;
    }
    if (!good)
        return;
    tx = x;
    ty = y;
    for (i = -3; i <= 3; ++i)
        for (j = -3; j <= 3; ++j)
            if (dist2(x + i, y + j, x, y) <= 14)
                outlands_terrain(x + i, y + j, HWALL, TRUE);
    for (i = -2; i <= 2; ++i)
        for (j = -2; j <= 2; ++j)
            if (dist2(x + i, y + j, x, y) <= 5) {
                outlands_terrain(x + i, y + j, CORR, FALSE);
                if (i || j) {
                    (void) mkcorpstat(STATUE, (struct monst *) 0,
                                      &mons[snakes[rn2(SIZE(snakes))]],
                                      x + i, y + j, FALSE);
                    (void) mkcorpstat(STATUE, (struct monst *) 0,
                                      &mons[PM_AMM_KAMEREL], x + i, y + j,
                                      TRUE);
                }
            }
    for (i = -3; i <= 3; ++i)
        for (j = -3; j <= 3; ++j)
            if (levl[x + i][y + j].typ == HWALL
                && (otmp = mksobj(MIRROR, FALSE, FALSE)) != 0) {
                otmp->obranch_size = MZ_GIGANTIC + 1;
                place_object(otmp, x + i, y + j);
            }
    wallification(x - 3, y - 3, x + 3, y + 3);
    switch (rn2(3)) {
    case 0:
        (void) outlands_name_artifact(KHAKKHARA,
                                      ART_STAFF_OF_TWELVE_MIRRORS, x, y);
        break;
    case 1:
        (void) outlands_name_artifact(MIRRORBLADE, ART_SANSARA_MIRROR, x, y);
        break;
    default:
        (void) outlands_name_artifact(DOUBLE_LIGHTSABER,
                                      ART_INFINITY_S_MIRRORED_ARC, x, y);
        break;
    }
    c = 1 + rn2(3);
    while (c-- > 0) {
        boolean placed = FALSE;
        for (tries = 0; tries < 50 && !placed; ++tries) {
            x = tx + rn2(17) - 8;
            y = ty + rn2(17) - 8;
            if (!outlands_area(x - 2, y - 2, 5, 5, FALSE))
                continue;
            outlands_wall_box(x - 2, y - 2, 5, 5, CORR);
            (void) enhancement_makemon(&mons[PM_AMM_KAMEREL], x, y, MM_ADJACENTOK);
            placed = TRUE;
        }
    }
    c = rnd(4) + rn2(4);
    while (c-- > 0) {
        x = tx + rn2(25) - 12;
        y = ty + rn2(25) - 12;
        if (isok(x, y) && levl[x][y].typ == PUDDLE
            && !outlands_protected[x][y] && !m_at(x, y) && !t_at(x, y))
            (void) enhancement_makemon(&mons[PM_HUDOR_KAMEREL], x, y, MM_ADJACENTOK);
    }
}

void
mkminorspire(void)
{
    int x = 0, y = 0, ix, iy, tries, i, j, c;
    boolean good = FALSE;

    OUTLANDS_NOTE("spire");
    for (tries = 0; tries < 50 && !good; ++tries) {
        x = rn2(COLNO - 6) + 3;
        y = rn2(ROWNO - 5) + 2;
        good = outlands_area(x - 1, y - 1, 3, 3, FALSE);
    }
    if (!good)
        return;
    ix = x;
    iy = y;
    for (i = -10; i <= 10; ++i)
        for (j = -10; j <= 10; ++j)
            if (isok(ix + i, iy + j) && dist2(ix, iy, ix + i, iy + j) < 105
                && !outlands_protected[ix + i][iy + j]
                && outlands_ground(levl[ix + i][iy + j].typ)) {
                if (levl[ix + i][iy + j].typ != TREE
                    || dist2(ix, iy, ix + i, iy + j) < rnd(8) * rnd(8) + 36)
                    outlands_terrain(ix + i, iy + j, PUDDLE, TRUE);
            }
    outlands_wall_box(x - 1, y - 1, 3, 3, HWALL);
    outlands_terrain(x, y, CORR, FALSE);
    (void) mksobj_at(ROBE, x, y, TRUE, FALSE);
    switch (rn2(6)) {
    case 4:
        (void) outlands_name_artifact(LONG_SWORD, ART_MIRROR_BRAND, x, y);
        break;
    case 3:
        (void) outlands_name_artifact(PLATE_MAIL, ART_SOULMIRROR, x, y);
        break;
    default:
        (void) mksobj_at(KHAKKHARA, x, y, FALSE, FALSE);
        (void) mksobj_at(AMULET_OF_REFLECTION, x, y, FALSE, FALSE);
        break;
    }
    c = rnd(4) + rn2(4);
    while (c-- > 0) {
        x = ix + rn2(25) - 12;
        y = iy + rn2(25) - 12;
        if (isok(x, y) && levl[x][y].typ == PUDDLE && !m_at(x, y)
            && !t_at(x, y) && !outlands_protected[x][y])
            (void) enhancement_makemon(&mons[PM_HUDOR_KAMEREL], x, y, MM_ADJACENTOK);
    }
    c = rnd(3) + rn2(3);
    while (c-- > 0) {
        x = ix + rn2(25) - 12;
        y = iy + rn2(25) - 12;
        if (isok(x, y) && levl[x][y].typ == PUDDLE && !m_at(x, y)
            && !t_at(x, y) && !outlands_protected[x][y])
            (void) enhancement_makemon(&mons[PM_SHARAB_KAMEREL], x, y, MM_ADJACENTOK);
    }
}

staticfn void
mkfishinghut(boolean left)
{
    int x, y, tries, i, j, pathto;
    struct obj *otmp;

    for (tries = 0; tries < 500; ++tries) {
        x = rn2(COLNO / 2) + 1 + (left ? 0 : COLNO / 2);
        y = rn2(ROWNO - 5);
        if (!outlands_area(x, y, 4, 4, FALSE))
            continue;
        pathto = 0;
        for (i = -1; i <= 4; ++i)
            for (j = -1; j <= 4; ++j)
                if (isok(x + i, y + j)
                    && levl[x + i][y + j].typ == PUDDLE)
                    ++pathto;
        if (!pathto)
            continue;
        outlands_wall_box(x, y, 4, 4, CORR);
        for (i = 1; i < 3; ++i)
            for (j = 1; j < 3; ++j) {
                if (!rn2(9))
                    (void) mksobj_at(SPEAR, x + i, y + j, TRUE, FALSE);
                if (!rn2(9)) {
                    otmp = mksobj(SLIME_MOLD, TRUE, FALSE);
                    if (otmp) {
                        otmp->quan = (long) rnd(4);
                        otmp->owt = weight(otmp);
                        place_object(otmp, x + i, y + j);
                    }
                }
                if (!rn2(9))
                    (void) mksobj_at(POT_BOOZE, x + i, y + j, TRUE, FALSE);
            }
        for (i = 1 + rn2(3); i > 0; --i)
            (void) enhancement_makemon(&mons[PM_DEEP_ONE], x + rnd(2), y + rnd(2),
                           MM_ADJACENTOK);
        if (left)
            outlands_door(x + 3, y + 2);
        else
            outlands_door(x, y + 2);
        return;
    }
}

void
mkwell(boolean left)
{
    int x, y, tries, i, j, pathto;

    OUTLANDS_NOTE("well");
    for (tries = 0; tries < 500; ++tries) {
        x = rn2(COLNO / 2) + 1 + (left ? 0 : COLNO / 2);
        y = rn2(ROWNO - 5);
        if (!outlands_area(x - 1, y - 1, 3, 3, FALSE))
            continue;
        pathto = 0;
        if (isok(x, y - 2) && outlands_walkable(x, y - 2))
            ++pathto;
        if (isok(x, y + 2) && outlands_walkable(x, y + 2))
            ++pathto;
        if (isok(x - 2, y) && outlands_walkable(x - 2, y))
            ++pathto;
        if (isok(x + 2, y) && outlands_walkable(x + 2, y))
            ++pathto;
        if (!pathto)
            continue;
        for (i = -1; i <= 1; ++i)
            for (j = -1; j <= 1; ++j)
                outlands_terrain(x + i, y + j, CORR, TRUE);
        outlands_terrain(x, y, POOL, TRUE);
        (void) mksobj_at(RAKUYO, x, y, TRUE, FALSE);
        return;
    }
}

void
mkfishingvillage(void)
{
    boolean left = (boolean) rn2(2);
    int i, j, edge, slant = rn2(3), shelf = rn1(5, 5), n;

    OUTLANDS_NOTE("fishing");
    edge = left ? rn1(20, 20) : COLNO - rn1(20, 20);
    for (j = 0; j < ROWNO; ++j) {
        if (left) {
            for (i = 1; i < edge; ++i)
                if (outlands_ground(levl[i][j].typ)
                    && !outlands_protected[i][j] && !t_at(i, j))
                    outlands_terrain(i, j,
                                     i < edge - shelf ? MOAT : PUDDLE, TRUE);
        } else {
            for (i = COLNO - 1; i > edge; --i)
                if (outlands_ground(levl[i][j].typ)
                    && !outlands_protected[i][j] && !t_at(i, j))
                    outlands_terrain(i, j,
                                     i > edge + shelf ? MOAT : PUDDLE, TRUE);
        }
        if (rn2(4))
            edge += rn2(3) - slant;
    }
    n = 4 + rnd(4) + rn2(4);
    while (n-- > 0)
        mkfishinghut(left);
    mkwell(left);
}

void
mkpluhomestead(void)
{
    int x, y, tries, i, j, pathto;

    for (tries = 0; tries < 500; ++tries) {
        x = rn2(COLNO - 6) + 1;
        y = rn2(ROWNO - 5);
        if (!outlands_area(x, y, 5, 5, TRUE))
            continue;
        pathto = 0;
        if (isok(x + 2, y - 1) && levl[x + 2][y - 1].typ == GRASS)
            ++pathto;
        if (isok(x + 2, y + 5) && levl[x + 2][y + 5].typ == GRASS)
            ++pathto;
        if (isok(x - 1, y + 2) && levl[x - 1][y + 2].typ == GRASS)
            ++pathto;
        if (isok(x + 5, y + 2) && levl[x + 5][y + 2].typ == GRASS)
            ++pathto;
        if (!pathto)
            continue;
        outlands_wall_box(x, y, 5, 5, CORR);
        for (i = 1; i < 4; ++i)
            for (j = 1; j < 4; ++j)
                if (!rn2(3))
                    (void) enhancement_mkobj_at(rn2(2) ? WEAPON_CLASS
                                           : rn2(2) ? TOOL_CLASS : ARMOR_CLASS,
                                    x + i, y + j, FALSE);
        for (i = rnd(3) + rn2(2); i > 0; --i)
            (void) enhancement_makemon(&mons[PM_PLUMACH_RILMANI], x + rnd(3), y + rnd(3),
                           MM_ADJACENTOK);
        switch (rn2(pathto)) {
        case 0: outlands_door(x + 2, y); break;
        case 1: outlands_door(x + 2, y + 4); break;
        case 2: outlands_door(x, y + 2); break;
        default: outlands_door(x + 4, y + 2); break;
        }
        OUTLANDS_NOTE("homestead-success");
        return;
    }
}

staticfn void
outlands_fill_building(coordxy x, coordxy y, int w, int h, int rtype)
{
    struct mkroom *room;

    if (svn.nroom >= MAXNROFROOMS)
        return;
    outlands_wall_box(x, y, w, h, rtype >= SHOPBASE ? ROOM : CORR);
    outlands_door(x + w - 1, y + h / 2);
    flood_fill_rm(x + 1, y + 1, svn.nroom + ROOMOFFSET, TRUE, TRUE);
    add_room(x + 1, y + 1, x + w - 2, y + h - 2, TRUE, (schar) rtype,
             TRUE);
    room = &svr.rooms[svn.nroom - 1];
    add_door(x + w - 1, y + h / 2, room);
    if (rtype >= SHOPBASE) {
        stock_room(rtype - SHOPBASE, room);
        (void) step10b_designate_plumach_shopkeeper(room->resident);
    } else if (rtype == BARRACKS || rtype == COURT)
        fill_zoo(room);
}

void
mkpluvillage(void)
{
    int x = 0, y = 0, tries, n, i, j, nshacks = 0, sizebig1 = 0,
        sizebig2 = 0, sizetot = 0;
    boolean good = FALSE;

    OUTLANDS_NOTE("plumach-village");
    for (tries = 0; tries < 50 && !good; ++tries) {
        nshacks = rnd(3) + rn2(3);
        if (rn2(2)) {
            sizebig1 = 1 + rnd(3) + 2;
            sizebig2 = 2 + rnd(3) + 2;
        } else {
            sizebig1 = 2 + rnd(3) + 2;
            sizebig2 = 1 + rnd(3) + 2;
        }
        sizetot = sizebig1 + nshacks * 5 + sizebig2 + 1;
        x = rn2(COLNO - sizetot) + 1;
        y = rn2(ROWNO - 11);
        good = outlands_area(x, y, sizetot + 1, 11, TRUE);
    }
    if (!good)
        return;
    for (i = sizebig1; i < sizetot - sizebig2; ++i)
        for (j = 1; j < 10; ++j)
            outlands_terrain(x + i, y + j, GRASS, TRUE);
    outlands_fill_building(x, y + 3, sizebig1, 5,
                           rn2(7) == 0 ? SHOPBASE + rn2(UNIQUESHOP - SHOPBASE)
                                       : (rn2(4) == 0 ? BARRACKS : OROOM));
    outlands_fill_building(x + sizetot - sizebig2, y + 3, sizebig2 + 1, 5,
                           rn2(7) == 0 ? SHOPBASE + rn2(UNIQUESHOP - SHOPBASE)
                                       : (rn2(4) == 0 ? COURT : OROOM));
    for (n = 0; n < nshacks; ++n) {
        i = x + sizebig1 + 1 + n * 5;
        if (outlands_area(i, y, 4, 4, TRUE)) {
            outlands_wall_box(i, y, 4, 4, CORR);
            outlands_door(i + 2, y + 3);
            (void) enhancement_makemon(&mons[PM_PLUMACH_RILMANI], i + 1, y + 1,
                           MM_ADJACENTOK);
        }
        if (outlands_area(i, y + 7, 4, 4, TRUE)) {
            outlands_wall_box(i, y + 7, 4, 4, CORR);
            outlands_door(i + 2, y + 7);
            (void) enhancement_makemon(&mons[PM_PLUMACH_RILMANI], i + 1, y + 8,
                           MM_ADJACENTOK);
        }
    }
}

void
mkferrutower(void)
{
    int x = 0, y = 0, tries, i, j, size = 8;
    boolean good = FALSE;

    OUTLANDS_NOTE("ferrumach");
    if (!rn2(20))
        size += rnd(4);
    for (tries = 0; tries < 500 && !good; ++tries) {
        x = rn2(COLNO - size) + 1;
        y = rn2(ROWNO - size);
        good = outlands_area(x, y, size, size, TRUE);
    }
    if (!good || svn.nroom >= MAXNROFROOMS)
        return;
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
            outlands_terrain(x + i, y + j,
                             (i < 2 || j < 2 || i >= size - 2
                              || j >= size - 2) ? HWALL : ROOM,
                             TRUE);
    wallification(x, y, x + size - 1, y + size - 1);
    outlands_door(x + 1, y + size / 2);
    flood_fill_rm(x + size / 2, y + size / 2,
                  svn.nroom + ROOMOFFSET, TRUE, TRUE);
    add_room(x + 2, y + 2, x + size - 3, y + size - 3, TRUE, BARRACKS,
             TRUE);
    add_door(x + 1, y + size / 2, &svr.rooms[svn.nroom - 1]);
    fill_zoo(&svr.rooms[svn.nroom - 1]);
}

void
mkinvertzigg(void)
{
    int x = 0, y = 0, tries, i, j, size = 15;
    boolean good = FALSE;
    struct obj *chest, *otmp;

    OUTLANDS_NOTE("ziggurat");
    for (tries = 0; tries < 500 && !good; ++tries) {
        x = rn2(COLNO - size) + 1;
        y = rn2(ROWNO - size);
        good = outlands_area(x, y, size, size, TRUE);
    }
    if (!good)
        return;
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j) {
            schar typ = GRASS;
            if (i >= 1 && j >= 1 && i < size - 1 && j < size - 1)
                typ = HWALL;
            if (i >= 2 && j >= 2 && i < size - 2 && j < size - 2)
                typ = CORR;
            if (i >= 4 && j >= 4 && i < size - 4 && j < size - 4)
                typ = HWALL;
            if (i >= 5 && j >= 5 && i < size - 5 && j < size - 5)
                typ = ROOM;
            outlands_terrain(x + i, y + j, typ, typ != CORR);
        }
    wallification(x, y, x + size - 1, y + size - 1);
    outlands_door(x + 1, y + size / 2);
    outlands_door(x + 4, y + size / 2);
    if (rn2(3)) {
        chest = mksobj_at(CHEST, x + size / 2, y + size / 2, TRUE, TRUE);
        if (chest) {
            chest->obranch_material = IRON;
            if ((otmp = mksobj(TORCH, TRUE, FALSE)) != 0)
                (void) add_to_container(chest, otmp);
            if ((otmp = mksobj(SHADOWLANDER_S_TORCH, TRUE, FALSE)) != 0)
                (void) add_to_container(chest, otmp);
            if ((otmp = mksobj(WAN_STRIKING, TRUE, FALSE)) != 0)
                (void) add_to_container(chest, otmp);
            (void) bury_an_obj(chest, (boolean *) 0);
        }
    } else {
        levl[x + size / 2][y + size / 2].typ = ALTAR;
        levl[x + size / 2][y + size / 2].altarmask = Align2amask(A_NONE);
    }
    if (!montoostrong(PM_SHATTERED_ZIGGURAT_WIZARD,
                      (level_difficulty() + u.ulevel) / 2 + 5)) {
        (void) enhancement_makemon(&mons[PM_SHATTERED_ZIGGURAT_WIZARD],
                       x + size / 2, y + size / 2, MM_ADJACENTOK);
        for (i = rnd(6) + rnd(4); i > 0; --i)
            (void) enhancement_makemon(&mons[PM_SHATTERED_ZIGGURAT_KNIGHT],
                           x + size / 2, y + size / 2, MM_ADJACENTOK);
    } else
        for (i = rnd(4); i > 0; --i)
            (void) enhancement_makemon(&mons[PM_SHATTERED_ZIGGURAT_KNIGHT],
                           x + size / 2, y + size / 2, MM_ADJACENTOK);
    for (i = rn1(6, 6); i > 0; --i)
        (void) enhancement_makemon(&mons[PM_SHATTERED_ZIGGURAT_CULTIST],
                       x + size / 2, y + size / 2, MM_ADJACENTOK);
}

void
place_neutral_features(void)
{
    int n;

    outlands_protect_route();
    OUTLANDS_NOTE("invocation");
    if (!rn2(30)) {
        mkkamereltowers();
        if (!rn2(16))
            mkfishingvillage();
    } else if (!rn2(16)) {
        mkminorspire();
    } else if (!rn2(16)) {
        mkfishingvillage();
    }
    if (!rn2(8))
        mkneuriver();
    if (!rn2(8))
        mkpluvillage();
    if (!rn2(16))
        mkinvertzigg();
    if (!rn2(8))
        mkferrutower();
    if (!rn2(3)) {
        n = rnd(4) + rn2(4);
        while (n-- > 0) {
            OUTLANDS_NOTE("homestead-attempt");
            mkpluhomestead();
        }
    }
    outlands_restore_route();
}

/* With the introduction of themed rooms, there are certain room shapes that
 * may generate a door, the square just inside the door, and only one other
 * ROOM square touching that one. E.g.
 *   ---
 * ---..
 * +....
 * ---..
 *   ---
 * This means that if the room becomes a shop, the shopkeeper will move
 * between those two squares nearest the door without ever allowing the
 * player to get past them.
 * Before approving sroom as a shop, check for this circumstance, and if it
 * exists, don't consider it as valid for a shop.
 *
 * Note that the invalidity of the shape derives from the position of its door
 * already being chosen. It's quite possible that if the door were somewhere
 * else on the perimeter of this room, it would work fine as a shop.*/
staticfn boolean
invalid_shop_shape(struct mkroom *sroom)
{
    coordxy x, y;
    coordxy doorx = svd.doors[sroom->fdoor].x;
    coordxy doory = svd.doors[sroom->fdoor].y;
    coordxy insidex = 0, insidey = 0, insidect = 0;

    /* First, identify squares inside the room and next to the door. */
    for (x = max(doorx - 1, sroom->lx);
         x <= min(doorx + 1, sroom->hx); x++) {
        for (y = max(doory - 1, sroom->ly);
             y <= min(doory + 1, sroom->hy); y++) {
            if (levl[x][y].typ == ROOM) {
                insidex = x;
                insidey = y;
                insidect++;
            }
        }
    }
    if (insidect < 1) {
        impossible("invalid_shop_shape: no squares inside door?");
        return TRUE;
    }
    /* if insidect > 1, then the shopkeeper already has alternate
     * squares to move to so we don't need to check further. */
    if (insidect == 1) {
        /* But if it is 1, scan all adjacent squares for other squares
         * that are part of this room. */
        insidect = 0;
        for (x = max(insidex - 1, sroom->lx);
             x <= min(insidex + 1, sroom->hx); x++) {
            for (y = max(insidey - 1, sroom->ly);
                 y <= min(insidey + 1, sroom->hy); y++) {
                if (x == insidex && y == insidey)
                    continue;
                if (levl[x][y].typ == ROOM)
                    insidect++;
            }
        }
        if (insidect == 1) {
            /* shopkeeper standing just inside the door can only move
             * to one other square; this cannot be a shop. */
            return TRUE;
        }
    }
    return FALSE;
}
#endif /* !SFCTOOL */

/*mkroom.c*/
