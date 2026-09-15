/* Native databases and extracted production code; no fake branch topology. */
#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct instance_globals_saved_m svm;
static int roll, selected, calls, ndice, sides;
int rn2(int n) { assert(n == 12); ++calls; return roll; }
int d(int n, int size) { ndice=n; sides=size; return n * size; }
struct obj *mongets(struct monst *m, int typ)
{ (void) m; selected=typ; return (struct obj *) 0; }
int buzzmu(struct monst *m, struct attack *a)
{ (void) m; (void) a; return 11; }
int castmu(struct monst *m, struct attack *a, boolean attacking, boolean found)
{ (void) m; (void) a; assert(attacking && found); return 22; }
#include "step10b_ogre.h"

int main(void)
{
    struct monst mage = { 0 }, target = { 0 }, ordinary = { 0 };
    struct permonst *ptr;
    int i;
    monst_globals_init();
    objects_globals_init();
    assert(PM_OGRE_MAGE == 430 && NUMMONS >= 441);
    ptr = &mons[PM_OGRE_MAGE];
    mage.data = ptr; mage.m_lev = 7; mage.mhp = mage.mhpmax = 60;
    mage.mcanmove = 1;
    target.data = &mons[PM_GIANT_ANT]; target.mhp = 30; target.mcanmove = 1;
    assert(ptr->pmidx == PM_OGRE_MAGE && ptr->mlet == S_OGRE);
    assert(!strcmp(ptr->pmnames[NEUTRAL], "ogre mage"));
    assert(ptr->difficulty == 10 && ptr->mcolor == CLR_BLUE);
    assert(polyok(ptr));
    assert(ptr->mlevel == 7 && ptr->mmove == 10 && ptr->ac == 5
           && ptr->mr == 0 && ptr->maligntyp == -3);
    assert(ptr->cwt == 2200 && ptr->cnutrit == 500 && ptr->msize == MZ_LARGE);
    assert(ptr->msound == MS_GRUNT && ptr->mresists == 0 && ptr->mconveys == 0);
    assert(ptr->geno == (G_GENO | G_NOGEN | 1));
    assert(!(ptr->geno & (G_UNIQ | G_NOCORPSE | G_SGROUP | G_LGROUP)));
    assert(ptr->mflags1 == (M1_HUMANOID | M1_CARNIVORE));
    assert(ptr->mflags2 == (M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT | M2_ORC));
    assert(ptr->mflags3 == (M3_INFRAVISIBLE | M3_INFRAVISION));
    assert(ptr->mattk[0].aatyp == AT_WEAP && ptr->mattk[0].adtyp == AD_PHYS
           && ptr->mattk[0].damn == 2 && ptr->mattk[0].damd == 5);
    assert(ptr->mattk[1].aatyp == AT_MAGC && ptr->mattk[1].adtyp == AD_SPEL
           && ptr->mattk[1].damn == 2 && ptr->mattk[1].damd == 6);
    for (i=2; i<NATTK; ++i)
        assert(ptr->mattk[i].aatyp == AT_NONE && ptr->mattk[i].adtyp == AD_PHYS
               && !ptr->mattk[i].damn && !ptr->mattk[i].damd);
    for (roll=0; roll<12; ++roll) {
        calls=0; selected=0;
        ogre_equipment(&mage);
        assert(calls == 1 && selected == (roll ? CLUB : BATTLE_AXE));
    }
    for (i=1; i<=127; ++i) {
        mage.m_lev = (uchar) i;
        (void) mith_spell_damage(&mage, &ptr->mattk[1]);
        assert(ndice == min(10, i/3+1)+2 && sides == 6);
    }
    assert(spell_eligible(&mage, &target, &ptr->mattk[1]) == M_ATTK_HIT);
    assert(spell_eligible(&mage, &target, &ptr->mattk[0]) == M_ATTK_MISS);
    ordinary=mage; ordinary.data=&mons[PM_OGRE];
    assert(hero_spell_route(&mage, &ptr->mattk[1], FALSE) == 22);
    assert(hero_spell_route(&mage, &ptr->mattk[1], TRUE) == 22);
    assert(hero_spell_route(&ordinary, &ptr->mattk[1], FALSE) == 22);
    assert(hero_spell_route(&ordinary, &ptr->mattk[1], TRUE) == 11);
    assert(spell_eligible(&ordinary, &target, &ptr->mattk[1]) == M_ATTK_MISS);
    mage.mhp=0;
    assert(spell_eligible(&mage, &target, &ptr->mattk[1]) == M_ATTK_MISS);
    mage.mhp=60; mage.mcanmove=0;
    assert(spell_eligible(&mage, &target, &ptr->mattk[1]) == M_ATTK_MISS);
    mage.mcanmove=1; target.mhp=0;
    assert(spell_eligible(&mage, &target, &ptr->mattk[1]) == M_ATTK_MISS);
    puts("PASS ogre mage: full declaration, append-only ID, 12 equipment outcomes, 127 spell levels, hero adjacent/ranged routing, target/death/helpless gates and ordinary-ogre isolation");
    return 0;
}
