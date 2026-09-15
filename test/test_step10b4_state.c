#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct instance_globals_saved_m svm;
struct permonst mons[NUMMONS + 1];
static int rng_value;

int
rn2(int bound)
{
    assert(bound > 0);
    return rng_value % bound;
}

int
newcham(struct monst *mon, struct permonst *ptr, unsigned flags)
{
    nhUse(flags);
    mon->data = ptr;
    return TRUE;
}

void
set_malign(struct monst *mon)
{
    mon->malign = mon->mpeaceful ? -1 : 1;
}

#include "step10b4_state_helpers.h"

int
main(void)
{
    struct monst pet = { 0 }, shk = { 0 }, priest = { 0 };
    struct mextra petextra = { 0 }, shkextra = { 0 }, prextra = { 0 };
    struct edog edog = { 0 };
    struct eshk eshk = { 0 };
    struct epri epri = { 0 };
    struct permonst carnivore = { 0 };

    carnivore.mflags1 = M1_CARNIVORE;
    pet.data = &carnivore;
    pet.mextra = &petextra;
    petextra.edog = &edog;
    pet.mtame = 10;
    pet.mpeaceful = 1;
    pet.mhp = 20;
    svm.moves = 1000L;
    edog.hungrytime = 1000L;
    rng_value = 0;
    step10b_pet_separation_catchup(&pet, 300, STEP10B_CTX_NONE);
    assert(pet.mtame == 8 && pet.mpeaceful == 1);

    pet.mtame = 10;
    pet.mpeaceful = 1;
    pet.mhp = 1;
    edog.hungrytime = 0L;
    step10b_pet_separation_catchup(&pet, 300, STEP10B_CTX_GATE);
    assert(pet.mtame == 10 && pet.mpeaceful == 1);
    assert(edog.hungrytime == 1500L);

    shk.mextra = &shkextra;
    shkextra.eshk = &eshk;
    shk.isshk = 1;
    shk.mpeaceful = 1;
    shk.data = &mons[PM_SHOPKEEPER];
    eshk.shoproom = 7;
    strcpy(eshk.shknam, "Plum Test");
    assert(step10b_designate_plumach_shopkeeper(&shk));
    assert(shk.data == &mons[PM_PLUMACH_RILMANI]);
    assert(shk.isshk && shk.mextra->eshk == &eshk);
    assert(eshk.shoproom == 7 && !strcmp(eshk.shknam, "Plum Test"));
    assert(step10b_designate_plumach_shopkeeper(&shk));

    priest.mextra = &prextra;
    prextra.epri = &epri;
    priest.ispriest = 1;
    priest.mpeaceful = 1;
    priest.msleeping = 1;
    priest.data = &mons[PM_ALIGNED_CLERIC];
    epri.shroom = 9;
    epri.shralign = A_NEUTRAL;
    assert(step10b_designate_bridge_priest(&priest));
    assert(priest.data == &mons[PM_BLASPHEMOUS_LURKER]);
    assert(priest.ispriest && !priest.isminion);
    assert(!priest.mpeaceful && !priest.msleeping && priest.malign == 1);
    assert(priest.mextra->epri == &epri && epri.shroom == 9);
    assert(epri.shralign == A_NEUTRAL);

    shk.isshk = 0;
    priest.ispriest = 0;
    assert(!step10b_designate_plumach_shopkeeper(&shk));
    assert(!step10b_designate_bridge_priest(&priest));

    puts("PASS Step 10B4 pet and role-extension state transitions");
    return 0;
}
