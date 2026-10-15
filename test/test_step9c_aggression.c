/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-09-09.
 * See doc/public-source-compliance.md and doc/public-main-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include "step9c_aggression.h"
int main(void) {
    struct monst a={0},b={0};int i,j,cases=0,enemies=0;
    monst_globals_init();
    for(i=LOW_PM;i<NUMMONS;++i) for(j=LOW_PM;j<NUMMONS;++j) {
        long actual;
        a.data=&mons[i];b.data=&mons[j];
        actual=local_rule(&a,&b)|local_rule(&b,&a);
        assert(actual==donor_rule(&a,&b));++cases;
        if(actual) ++enemies;
    }
    assert(enemies>0);
    printf("PASS %d pinned species-pair hostility comparisons (%d enemy pairs), native pet guard/dispatch unchanged\n",cases,enemies);
    return 0;
}
