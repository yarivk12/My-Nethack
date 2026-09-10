/* Compile-gated native Step 9 topology probe.
 *
 * This is linked into a diagnostic build of the real Windows NetHack target.
 * It calls the same init_objects(), role_init(), and init_dungeons() sequence
 * used by newgame(), then reads the production branch and special-level chains
 * in memory.  The normal Release target neither compiles this file nor exposes
 * the environment-variable entry point.
 */
#include "hack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
named_dungeon(const char *name)
{
    int i;

    for (i = 0; i < svn.n_dgns; ++i)
        if (!strcmp(svd.dungeons[i].dname, name))
            return i;
    return -1;
}

static int
count_special(const char *proto, int dnum)
{
    int count = 0;
    s_level *slev;

    for (slev = svs.sp_levchn; slev; slev = slev->next)
        if (!strcmp(slev->proto, proto) && slev->dlevel.dnum == dnum)
            ++count;
    return count;
}

int
step9_topology_test_main(void)
{
    int dod, sheol_dnum, dragon_dnum, mithardir_dnum;
    int sheol, dragon, mithardir, collision;
    int step9_count = 0, step9d_count = 0;
    int castle_count;
    branch *br;
    s_level *chalv2;

    /* Match the production newgame() initialization order up to init_dungeons.
     * Leaving role/race/gender/alignment unspecified lets role_init() select a
     * valid fresh character without any menu or PTY input. */
    init_objects();
    flags.pantheon = -1;
    flags.initrole = ROLE_NONE;
    flags.initrace = ROLE_NONE;
    flags.initgend = ROLE_NONE;
    flags.initalign = ROLE_NONE;
    (void) strcpy(svp.plname, "step9-topology");
    svp.pl_character[0] = '\0';
    role_init();
    init_dungeons();

    dod = named_dungeon("The Dungeons of Doom");
    sheol_dnum = named_dungeon("Sheol");
    dragon_dnum = named_dungeon("The Dragon Caves");
    mithardir_dnum = named_dungeon("Mithardir");
    if (dod < 0 || sheol_dnum < 0 || dragon_dnum < 0 || mithardir_dnum < 0)
        return fprintf(stderr, "FAIL native topology: missing Step 9 dungeon\n"),
               2;

    sheol = dragon = mithardir = -1;
    for (br = svb.branches; br; br = br->next) {
        if (br->end1.dnum != dod)
            continue;
        if (br->end2.dnum == sheol_dnum)
            sheol = br->end1.dlevel;
        else if (br->end2.dnum == dragon_dnum)
            dragon = br->end1.dlevel;
        else if (br->end2.dnum == mithardir_dnum)
            mithardir = br->end1.dlevel;
    }
    if (sheol < 0 || dragon < 0 || mithardir < 0)
        return fprintf(stderr, "FAIL native topology: missing Step 9 parent\n"),
               3;

    for (br = svb.branches; br; br = br->next)
        if (br->end1.dnum == dod) {
            if (br->end2.dnum == sheol_dnum || br->end2.dnum == dragon_dnum
                || br->end2.dnum == mithardir_dnum)
                ++step9_count;
            else if (br->end2.dnum < 0)
                ++step9d_count;
        }

    collision = (sheol == dragon) + (sheol == mithardir)
        + (dragon == mithardir);
    chalv2 = find_level("chalv2");
    castle_count = count_special("castle", dod);
    if (!chalv2 || chalv2->dlevel.dnum != dod
        || chalv2->dlevel.dlevel != mithardir)
        return fprintf(stderr, "FAIL native topology: Mithardir approach not rebased\n"),
               4;

    if (step9_count != 3 || step9d_count != 0 || collision != 0
        || sheol < 30 || sheol > 199 || dragon < 30 || dragon > 199
        || mithardir < 30 || mithardir > 199 || castle_count != 1)
        return fprintf(stderr, "FAIL native topology: invariant violation\n"), 5;

    printf("TOPOLOGY|sheol=%d|dragon=%d|mithardir=%d|step9_count=%d|step9d_count=%d|collisions=%d|castle_count=%d|chalv2=%d|step9_parent_at_111=%d\n",
           sheol, dragon, mithardir, step9_count, step9d_count, collision,
           castle_count, chalv2->dlevel.dlevel,
           (sheol == 111) + (dragon == 111) + (mithardir == 111));
    return 0;
}
