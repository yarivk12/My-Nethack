/* Exact production/donor generator comparison. The runner compiles this once
 * per generator with the same deterministic RNG, map storage and wallifier.
 * Actual wall topology and travel are checked separately in packaged games. */
#include "hack.h"
#include "sp_lev.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct you u;
struct instance_globals_saved_l svl;
static unsigned rng;
long *alloc(unsigned n) { return (long *) calloc(1, n); }
int rn2(int n) {
    assert(n > 0);
    rng = rng * 1664525U + 1013904223U;
    return (int) ((rng >> 1) % (unsigned) n);
}
int dist2(coordxy x, coordxy y, coordxy a, coordxy b) {
    return (x-a)*(x-a)+(y-b)*(y-b);
}
int isqrt(int n) {
    int r = 0;
    while ((r+1)*(r+1) <= n) ++r;
    return r;
}
void wallify_map(int x1, int y1, int x2, int y2) {
    assert(x1==1 && y1==0 && x2==COLNO-1 && y2==ROWNO-1);
}

#include "step9a_generator.h"

int main(void) {
    int seed, floor, x, y, left, right, ice, crystal;
    for (seed = 1; seed <= 64; ++seed) {
        for (floor = 1; floor <= 3; ++floor) {
            unsigned long long hash = 1469598103934665603ULL;
            memset(&svl, 0, sizeof svl);
            rng = (unsigned) (seed * 97 + floor);
            u.uz.dlevel = floor;
            mksheol();
            left=right=ice=crystal=0;
            for (x=1; x<COLNO; ++x) for (y=0; y<ROWNO; ++y) {
                int typ=levl[x][y].typ;
                assert(typ>=0 && typ<MAX_TYPE);
                hash=(hash ^ (unsigned) typ)*1099511628211ULL;
                hash=(hash ^ levl[x][y].wall_info)*1099511628211ULL;
                left += x<=15 && typ==ROOM;
                right += x>=65 && typ==ROOM;
                ice += typ==ICEWALL;
                crystal += typ==CRYSTALICEWALL;
            }
            assert(left && right && ice && crystal);
            assert(svl.level.flags.is_cavernous_lev);
            assert(!svl.level.flags.is_maze_lev);
            printf("%d %d %llu\n",seed,floor,hash);
        }
    }
    return 0;
}
