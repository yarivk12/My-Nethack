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
    int dod, sheol_dnum, dragon_dnum, mithardir_dnum, neutral_dnum,
        lost_dnum;
    int sheol, dragon, mithardir, neutral, collision;
    int step9_count = 0, step9d_count = 0;
    int castle_count, neutral_floors = 0, lost_floors = 0, alternates = 0;
    branch *br, *lost_branch = 0, *dispensary_branch = 0;
    s_level *chalv2, *neulev, *slev, *lbyrnth;

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
    neutral_dnum = named_dungeon("Neutral Quest");
    lost_dnum = named_dungeon("The Lost Cities");
    if (dod < 0 || sheol_dnum < 0 || dragon_dnum < 0 || mithardir_dnum < 0
        || neutral_dnum < 0 || lost_dnum < 0
        || named_dungeon("The Dispensary") >= 0)
        return fprintf(stderr, "FAIL native topology: missing/extra Step 10 dungeon\n"),
               2;

    sheol = dragon = mithardir = neutral = -1;
    for (br = svb.branches; br; br = br->next) {
        if (br->end1.dnum == dod && br->end2.dnum == sheol_dnum)
            sheol = br->end1.dlevel;
        else if (br->end1.dnum == dod && br->end2.dnum == dragon_dnum)
            dragon = br->end1.dlevel;
        else if (br->end1.dnum == dod && br->end2.dnum == mithardir_dnum)
            mithardir = br->end1.dlevel;
        else if (br->end1.dnum == dod && br->end2.dnum == neutral_dnum) {
            neutral = br->end1.dlevel;
            if (br->type != BR_PORTAL || br->end2.dlevel != 1)
                return fprintf(stderr, "FAIL native topology: Neutral connector\n"), 3;
        } else if (br->end1.dnum == neutral_dnum
                   && br->end2.dnum == lost_dnum)
            lost_branch = br;
        else if (br->end1.dnum == neutral_dnum
                 && br->end2.dnum == neutral_dnum)
            dispensary_branch = br;
    }
    if (sheol < 0 || dragon < 0 || mithardir < 0 || neutral < 0)
        return fprintf(stderr, "FAIL native topology: missing scheduled parent\n"), 4;

    for (br = svb.branches; br; br = br->next)
        if (br->end1.dnum == dod) {
            if (br->end2.dnum == sheol_dnum || br->end2.dnum == dragon_dnum
                || br->end2.dnum == mithardir_dnum)
                ++step9_count;
            else if (br->end2.dnum < 0)
                ++step9d_count;
        }

    collision = (sheol == dragon) + (sheol == mithardir)
        + (dragon == mithardir) + (neutral == sheol) + (neutral == dragon)
        + (neutral == mithardir);
    chalv2 = find_level("chalv2");
    neulev = find_level("neulev");
    lbyrnth = find_level("lbyrnth");
    castle_count = count_special("castle", dod);
    if (!chalv2 || chalv2->dlevel.dnum != dod
        || chalv2->dlevel.dlevel != mithardir)
        return fprintf(stderr, "FAIL native topology: Mithardir approach not rebased\n"),
               4;

    for (slev = svs.sp_levchn; slev; slev = slev->next) {
        if (slev->dlevel.dnum == neutral_dnum && slev != lbyrnth)
            ++neutral_floors;
        if (slev->dlevel.dnum == lost_dnum) {
            ++lost_floors;
            if (!strcmp(slev->proto, "leth-a-1")
                || !strcmp(slev->proto, "leth-a-2")
                || !strcmp(slev->proto, "leth-c-1")
                || !strcmp(slev->proto, "leth-c-2")
                || !strcmp(slev->proto, "leth-d-1")
                || !strcmp(slev->proto, "leth-d-2")
                || !strcmp(slev->proto, "nkai-a-1")
                || !strcmp(slev->proto, "nkai-a-2"))
                ++alternates;
        }
    }

    if (step9_count != 3 || step9d_count != 0 || collision != 0
        || sheol < 30 || sheol > 199 || dragon < 30 || dragon > 199
        || mithardir < 30 || mithardir > 199 || neutral < 30 || neutral > 199
        || castle_count != 1 || svn.n_dgns > MAXDUNGEON
        || svd.dungeons[dod].num_dunlevs != 200
        || stronghold_level.dnum != dod || stronghold_level.dlevel != 200
        || !neulev || neulev->dlevel.dnum != dod
        || neulev->dlevel.dlevel != neutral
        || neutral_floors != 7 || lost_floors != 13 || alternates != 4
        || !lbyrnth || lbyrnth->dlevel.dnum != neutral_dnum
        || lbyrnth->dlevel.dlevel != 8
        || !lost_branch || lost_branch->end1.dlevel != 7
        || lost_branch->end2.dlevel != 2 || lost_branch->type != BR_STAIR
        || !dispensary_branch || dispensary_branch->end1.dlevel < 2
        || dispensary_branch->end1.dlevel > 6
        || dispensary_branch->end2.dlevel != 8
        || depth(&lbyrnth->dlevel) != neutral + dispensary_branch->end1.dlevel
        || svd.dungeons[neutral_dnum].depth_start != neutral
        || svd.dungeons[lost_dnum].depth_start != neutral + 6)
        return fprintf(stderr, "FAIL native topology: invariant violation\n"), 5;

    printf("TOPOLOGY|sheol=%d|dragon=%d|mithardir=%d|neutral=%d|dispensary_parent=%d|step9_count=%d|step9d_count=%d|collisions=%d|castle_count=%d|chalv2=%d|neulev=%d|dungeons=%d|neutral_floors=%d|lost_floors=%d|alternates=%d|max_depth=%d|step9_parent_at_111=%d\n",
           sheol, dragon, mithardir, neutral, dispensary_branch->end1.dlevel,
           step9_count, step9d_count, collision, castle_count,
           chalv2->dlevel.dlevel, neulev->dlevel.dlevel, svn.n_dgns,
           neutral_floors, lost_floors, alternates, neutral + 18,
           (sheol == 111) + (dragon == 111) + (mithardir == 111));
    return 0;
}
