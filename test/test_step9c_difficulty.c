#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include "step9c_difficulty.h"

int main(int argc, char **argv) {
    static const int imported[] = {
        PM_CRYSTAL_OOZE, PM_DEEP_ONE, PM_DEEPER_ONE, PM_DEEPEST_ONE,
        PM_SELKIE, PM_SEAL, PM_OCEANID,
        PM_YURIAN, PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN,
        PM_MOTE_OF_LIGHT, PM_WATER_DOLPHIN, PM_SINGING_SAND, PM_LIVING_MIRAGE,
        PM_WRAITHWORM, PM_ALABASTER_ELF, PM_ALABASTER_ELF_ELDER,
        PM_SENTINEL_OF_MITHARDIR, PM_ALABASTER_MUMMY, PM_FIRST_WRAITHWORM,
        PM_ASPECT_OF_THE_SILENCE
    };
    int i;
    boolean report = argc > 1 && !strcmp(argv[1], "--report");
    monst_globals_init();
    for (i = 0; i < SIZE(imported); ++i) {
        struct permonst *pm = &mons[imported[i]];
        int calculated = mstrength(pm);
        printf("DIFFICULTY %d %d %d %s\n", imported[i], calculated,
               pm->difficulty, pm->pmnames[NEUTRAL]);
        if (!report) assert(pm->difficulty == calculated);
    }
    if (!report) puts("PASS all imported Mithardir difficulty ratings match native mstrength");
    return 0;
}
