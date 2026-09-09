#include "hack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

struct you u;
struct instance_globals_saved_l svl;
struct instance_globals_n gn;
static struct rm donor_map[COLNO][ROWNO], expected[COLNO][ROWNO];
static char donor_locations[(COLNO-1)*(ROWNO-1)];
static uint32_t rng;
static int moves, objects_moved, wallified;
int rn2(int n) { assert(n > 0); rng = rng*1664525U+1013904223U; return (rng>>8)%n; }
int rnd(int n) { return rn2(n)+1; }
int isok(coordxy x, coordxy y) { return x >= 1 && x < COLNO && y >= 0 && y < ROWNO; }
boolean In_mithardir(const d_level *l) { return l->dnum == 14; }
boolean In_mithardir_desert(const d_level *l) {
    return In_mithardir(l) && l->dlevel >= 2 && l->dlevel <= 4;
}
boolean rloc(struct monst *m, unsigned flags) {
    assert(flags == RLOC_NOMSG); ++moves;
    svl.level.monsters[m->mx][m->my] = 0; return TRUE;
}
boolean rloco(struct obj *o) {
    ++objects_moved; svl.level.objects[o->ox][o->oy] = 0; return TRUE;
}
static void rlocos_at(int x, int y) {
    struct obj *o;
    for (o = svl.level.objects[x][y]; o; o = o->nexthere) ++objects_moved;
    svl.level.objects[x][y] = 0;
}
void wallification(coordxy x1, coordxy y1, coordxy x2, coordxy y2) {
    assert(x1 == 1 && y1 == 0 && x2 == COLNO-1 && y2 == ROWNO-1);
    ++wallified;
}
#include "step9c_wastes.h"

int main(void) {
    unsigned seed;
    uint32_t final_rng;
    int x, y, kind, n;
    struct monst mon = { 0 };
    struct obj one = { 0 }, two = { 0 };
    gn.new_locations = donor_locations;
    for (kind = 0; kind < 2; ++kind)
        for (seed = 1; seed <= 512; ++seed) {
            schar bg = kind ? PUDDLE : SAND, fg = kind ? MOAT : STONE;
            memset(&svl, 0, sizeof svl); memset(donor_map, 0, sizeof donor_map);
            rng = seed; init_map(bg); init_fill(bg, fg); pass_one(bg, fg); pass_two(bg, fg);
            final_rng = rng;
            rng = seed; donor_init_map(bg); donor_init_fill(bg, fg);
            donor_pass_one(bg, fg); donor_pass_two(bg, fg);
            assert(final_rng == rng);
            for (x = 0; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
                assert(levl[x][y].typ == donor_map[x][y].typ);
        }
    puts("PASS 1024 donor/native Wastes and Elshava cellular maps and RNG traces");
    for (seed = 1; seed <= 256; ++seed) {
        memset(&svl, 0, sizeof svl); rng = seed;
        for (x = 1; x < COLNO; ++x) for (y = 0; y < ROWNO; ++y)
            levl[x][y].typ = rn2(3) ? SAND : VWALL;
        memcpy(donor_map, levl, sizeof donor_map);
        u.uz.dnum = 14; u.uz.dlevel = 2;
        wallified = 0; mith_finish_wastes(); assert(wallified == 1);
        memcpy(expected, levl, sizeof expected); memcpy(levl, donor_map, sizeof donor_map);
        wallified = 0; donor_cleanup(); assert(wallified == 1);
        for (y = 0; y < ROWNO; ++y) {
            assert(!expected[0][y].lit);
            assert(levl[0][y].lit); /* donor lights its unused boundary */
            levl[0][y].lit = FALSE;
        }
        assert(!memcmp(expected, levl, sizeof expected));
    }
    puts("PASS 256 donor/native dangling-wall cleanup maps and lighting");
    memset(&svl, 0, sizeof svl);
    mon.mx = one.ox = two.ox = 10; mon.my = one.oy = two.oy = 10;
    one.nexthere = &two;
    svl.level.monsters[10][10] = &mon; svl.level.objects[10][10] = &one;
    u.uz.dnum = 14; u.uz.dlevel = 1;
    moves = objects_moved = 0; mith_finish_wastes();
    assert(moves == 1 && objects_moved == 2);
    for (n = 0; n < 2; ++n) {
        u.uz.dnum = n ? 14 : 0; u.uz.dlevel = n ? 5 : 2;
        wallified = moves = objects_moved = 0; mith_finish_wastes();
        assert(!wallified && !moves && !objects_moved);
    }
    puts("PASS overwritten-rock relocation and non-Wastes/native-level guards");
    return 0;
}
