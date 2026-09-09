#include "hack.h"
#include <assert.h>
#include <stdio.h>
struct you u;
struct instance_globals_y gy;
static int roll,rolls;
int rn2(int n) { assert(n>0); ++rolls; return roll%n; }
int rnd(int n) { return rn2(n)+1; }
int d(int n,int x) { int sum=0; while(n--) sum+=rnd(x); return sum; }
int touch_artifact(struct obj *o,struct monst *m) { (void)o;(void)m; return TRUE; }
boolean mon_hates_silver(struct monst *m) { (void)m; return FALSE; }
#undef resists_ston
#define resists_ston(m) TRUE
struct obj *which_armor(struct monst *m,long slot) {
    struct obj *o;
    for(o=m->minvent;o;o=o->nobj) if(o->owornmask&slot) return o;
    return 0;
}
#include "step9c_iron.h"
int main(void) {
    struct monst mon={0}; struct obj o={0};
    int pm,material,lev,expected,saved,cases=0;
    monst_globals_init();objects_globals_init();
    u.umonnum=u.umonster=PM_HUMAN;o.otyp=DAGGER;o.oclass=WEAPON_CLASS;
    for(pm=LOW_PM;pm<NUMMONS;++pm) for(material=LIQUID;material<=MINERAL;++material)
      for(lev=1;lev<=30;++lev) {
        mon.data=&mons[pm];mon.m_lev=lev;o.obranch_material=material;
        for(roll=0;roll<32;++roll) {
            rolls=0;expected=donor_iron(&mon,&o);saved=rolls;rolls=0;
            assert(mith_iron_damage(&mon,obj_material(&o))==expected && rolls==saved);
            ++cases;
        }
        mon.misc_worn_check=0;
        assert(can_touch_safely(&mon,&o)==
          ((!mith_hates_iron(mon.data)||obj_material(&o)!=IRON)&&native_touch(&mon,&o)));
        mon.misc_worn_check=W_ARMG;
        assert(can_touch_safely(&mon,&o)==native_touch(&mon,&o));
      }
    for(pm=LOW_PM;pm<NUMMONS;++pm) {
        gy.youmonst.data=&mons[pm];u.umonnum=pm;u.ulevel=30;
        o.obranch_material=IRON;
        for(roll=0;roll<64;++roll)
            assert(mith_iron_damage(&gy.youmonst,IRON)==donor_iron(&gy.youmonst,&o));
    }
    {
        struct monst attacker={0};struct obj glove={0};int type;
        mon.data=&mons[PM_ALABASTER_ELF];mon.m_lev=15;
        attacker.data=&mons[PM_IRON_GOLEM];glove.otyp=LEATHER_GLOVES;
        glove.owornmask=W_ARMG;roll=7;
        assert(mith_iron_contact(&attacker,&mon,AT_WEAP,0)==8);
        attacker.minvent=&glove;
        assert(!mith_iron_contact(&attacker,&mon,AT_WEAP,0));
        assert(mith_iron_contact(&attacker,&mon,AT_BITE,0)==8);
        glove.obranch_material=IRON;attacker.data=&mons[PM_HUMAN];
        assert(mith_iron_contact(&attacker,&mon,AT_CLAW,0)==8);
        assert(!mith_iron_contact(&attacker,&mon,AT_CLAW,&o));
        attacker.minvent=0;attacker.data=&mons[PM_IRON_GOLEM];
        for(type=0;type<=AT_MAGC;++type) {
            int expected=(type==AT_WEAP||type==AT_CLAW||type==AT_KICK
              ||type==AT_BUTT||type==AT_BITE||type==AT_STNG||type==AT_TUCH
              ||type==AT_HUGS||type==AT_TENT) ? 8 : 0;
            assert(mith_iron_contact(&attacker,&mon,type,0)==expected);
        }
        mon.data=&mons[PM_HUMAN];rolls=0;
        assert(!mith_iron_contact(&attacker,&mon,AT_WEAP,0)&&!rolls);
    }
    printf("PASS %d pinned iron damage/RNG cases, all species handling/gloves and hero forms\n",cases);
    return 0;
}
