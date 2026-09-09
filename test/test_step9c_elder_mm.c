#include "hack.h"
#include "artifact.h"
#include <assert.h>
#include <stdio.h>
struct you u;
struct instance_globals_saved_m svm;
struct instance_globals_saved_l svl;
struct instance_globals_b gb;
struct instance_globals_t gt;
#define MCASTU_ENUM
enum spells {
#include "mcastu.h"
};
#undef MCASTU_ENUM
#define MCASTU_INIT
static struct { int level,flags; } mcast_data[]={
#include "mcastu.h"
};
#undef MCASTU_INIT
#undef canseemon
#define canseemon(m) FALSE
#undef pline_mon
#define pline_mon(...) ((void)0)
#undef Upolyd
#define Upolyd FALSE
static int selected, selections, damage_calls, beam_calls, heal_calls, far_heal;
static int magic_resist, blind_resist, resist_roll, fumble, aligned=1;
static coordxy heal_x,heal_y;
static struct monst *beam_victim;
static int kill_beam_target,kill_beam_caster;
static struct obj *test_armor;
static int armor_resist,quest_armor,removed,curses,summons,deaths,lifesave;
static struct monst *real_caster;
int rn2(int n) { assert(n>0);return fumble ? 0 : n-1; }
int rnd(int n) { return rn2(n)+1; }
int sgn(int n) { return n<0 ? -1 : n>0; }
boolean resists_magm(struct monst *m) { (void)m;return (boolean)magic_resist; }
boolean resists_blnd(struct monst *m) { (void)m;return (boolean)blind_resist; }
int resist(struct monst *m,char c,int damage,int tell) { (void)m;(void)c;(void)damage;(void)tell;return resist_roll; }
void shieldeff(coordxy x,coordxy y) { (void)x;(void)y; }
boolean linedup(coordxy ax,coordxy ay,coordxy bx,coordxy by,int boulders) {
    assert(boulders==0);gt.tbx=(schar)(ax-bx);gt.tby=(schar)(ay-by);return (boolean)aligned;
}
void dobuzz(int type,int dice,coordxy x,coordxy y,int dx,int dy,
            boolean hero_target,boolean tell,boolean inside) {
    assert(type==BZ_M_SPELL(BZ_OFS_AD(AD_SLEE)) && dice==4);
    assert(x==10 && y==3 && dx==1 && dy==0 && gb.buzzer);
    assert(!hero_target && !tell && !inside);
    ++beam_calls;
    if(kill_beam_caster) gb.buzzer->mhp=0;
    if(kill_beam_target) beam_victim->mhp=0;
}
static int mith_elder_spell(void) { return selected; }
static int mith_spell_damage(struct monst *m,struct attack *a) {
    assert(m && a->adtyp==AD_SPEL);++damage_calls;return 11;
}
static void mcast_disappear(struct monst *m) { m->minvis=1; }
static void mith_mass_cure(struct monst *m,boolean far,coordxy x,coordxy y) {
    assert(m);++heal_calls;far_heal=far;heal_x=x;heal_y=y;
}
static int m_cure_self(struct monst *m,int damage) { m->mhp=min(m->mhpmax,m->mhp+18);return damage; }
void mon_adjust_speed(struct monst *m,int amount,struct obj *o) {
    assert(amount==1 && !o);m->permspeed=MFAST;
}
struct obj *some_armor(struct monst *m) { (void)m;return test_armor; }
boolean is_quest_artifact(struct obj *o) { (void)o;return (boolean)quest_armor; }
boolean obj_resists(struct obj *o,int a,int b) { (void)o;assert(a==0 && b==90);return (boolean)armor_resist; }
void m_useupall(struct monst *m,struct obj *o) { assert(m && o==test_armor);test_armor=0;++removed; }
boolean spec_ability(struct obj *o,unsigned long f) { (void)o;assert(f==SPFX_INTEL);return FALSE; }
void unbless(struct obj *o) { o->blessed=0;++curses; }
void curse(struct obj *o) { o->cursed=1;++curses; }
int nasty(struct monst *m) {
    assert(m!=real_caster && m->data==real_caster->data && m->mux==12 && m->muy==3);
    ++summons;return 1;
}
void monkilled(struct monst *m,const char *reason,int type) {
    assert(!*reason && type==AD_SPEL);++deaths;
    if(lifesave) { m->mhp=20;m->mhpmax=max(20,m->mhpmax); }
}
static int test_choose_spell(struct monst *,struct monst *);
#include "step9c_elder_mm.h"
static int test_choose_spell(struct monst *a,struct monst *b) {
    ++selections;return selected>=0 ? selected : mith_mm_choose_spell(a,b);
}
static struct monst make_mon(int type,coordxy x) {
    struct monst m={0};m.data=&mons[type];m.mx=x;m.my=3;m.mhp=40;m.mhpmax=80;
    if(type==PM_ALABASTER_MUMMY) m.mspare1=(MITH_KRAU+1)<<MITH_SYLLABLE_SHIFT;
    m.m_lev=10;m.mcanmove=1;m.mcansee=1;return m;
}
int main(void) {
    struct monst caster,target,friend,enemy;
    struct attack attack={AT_MAGC,AD_SPEL,0,4};
    int spells[]={MCAST_DISAPPEAR,MCAST_CONFUSE_YOU,MCAST_BLIND_YOU,MCAST_MITH_SLEEP,
                   MCAST_MITH_CURE_CLOSE,MCAST_MITH_CURE_FAR,MCAST_AGGRAVATION};
    int i,result;
    monst_globals_init();objects_globals_init();
    for(i=0;i<SIZE(spells);++i) {
        caster=make_mon(PM_ALABASTER_ELF_ELDER,10);target=make_mon(PM_ORC,12);
        friend=make_mon(PM_ORC,15);enemy=make_mon(PM_ORC,16);
        target.mpeaceful=1;friend.msleeping=enemy.msleeping=1;enemy.mpeaceful=1;
        caster.nmon=&target;target.nmon=&friend;friend.nmon=&enemy;fmon=&caster;
        selected=spells[i];selections=damage_calls=beam_calls=heal_calls=0;
        assert(mith_castmm(&caster,&target,&attack)==M_ATTK_HIT);
        assert(selections==1 && damage_calls==1 && caster.mspec_used==2 && !gb.buzzer);
        assert(target.mhp==40); /* support spells must discard their damage roll */
        if(selected==MCAST_DISAPPEAR) assert(caster.minvis);
        if(selected==MCAST_CONFUSE_YOU) assert(target.mconf);
        if(selected==MCAST_BLIND_YOU) assert(target.mblinded==127 && !target.mcansee);
        if(selected==MCAST_MITH_SLEEP) assert(beam_calls==1);
        if(selected==MCAST_MITH_CURE_CLOSE || selected==MCAST_MITH_CURE_FAR)
            assert(heal_calls==1 && far_heal==(selected==MCAST_MITH_CURE_FAR)
                   && heal_x==12 && heal_y==3);
        if(selected==MCAST_AGGRAVATION) assert(!friend.msleeping && enemy.msleeping);
    }
    caster=make_mon(PM_ALABASTER_ELF_ELDER,10);target=make_mon(PM_ORC,12);fmon=&caster;
    selected=MCAST_MITH_SLEEP;aligned=0;selections=0;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS && selections==40);
    aligned=1;caster.mcan=1;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS && !caster.mspec_used);
    caster.mcan=0;caster.mspec_used=1;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS);
    caster.mspec_used=0;fumble=1;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS && caster.mspec_used==2);
    fumble=0;
    for(i=0;i<3;++i) {
        caster.mspec_used=0;selected=MCAST_CONFUSE_YOU;
        magic_resist=i==0;resist_roll=i==1;target.mconf=0;
        assert(mith_castmm(&caster,&target,&attack)==M_ATTK_HIT);
        assert(target.mconf==(i==2));
    }
    magic_resist=resist_roll=0;selected=MCAST_MITH_SLEEP;beam_victim=&target;
    caster.mspec_used=0;kill_beam_target=kill_beam_caster=1;
    result=mith_castmm(&caster,&target,&attack);
    assert((result&M_ATTK_DEF_DIED) && (result&M_ATTK_AGR_DIED) && !gb.buzzer);
    caster=make_mon(PM_LICH,10);target=make_mon(PM_ORC,12);selections=0;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS && !selections);
    puts("PASS elder monster-target spell effects, target coordinates, cooldown/cancellation/fumble, resistance, beam death flags and native exclusion");
    kill_beam_target=kill_beam_caster=0;
    for(i=0;i<11;++i) {
        static int mummy_spells[]={MCAST_PSI_BOLT,MCAST_CURE_SELF,MCAST_HASTE_SELF,
            MCAST_STUN_YOU,MCAST_DISAPPEAR,MCAST_WEAKEN_YOU,MCAST_DESTRY_ARMR,
            MCAST_CURSE_ITEMS,MCAST_AGGRAVATION,MCAST_SUMMON_MONS,MCAST_DEATH_TOUCH};
        struct obj armor={0}, item={0};
        caster=make_mon(PM_ALABASTER_MUMMY,10);target=make_mon(PM_ORC,12);fmon=&caster;
        caster.nmon=&target;real_caster=&caster;caster.mux=2;caster.muy=1;
        armor.otyp=PLATE_MAIL;armor.oclass=ARMOR_CLASS;test_armor=&armor;
        item.otyp=DAGGER;item.oclass=WEAPON_CLASS;target.minvent=&item;
        selected=mummy_spells[i];removed=curses=summons=deaths=0;
        result=mith_castmm(&caster,&target,&attack);
        assert(result & M_ATTK_HIT);
        if(selected==MCAST_PSI_BOLT) assert(target.mhp==29);
        if(selected==MCAST_CURE_SELF) assert(caster.mhp==58 && target.mhp==40);
        if(selected==MCAST_HASTE_SELF) assert(caster.permspeed==MFAST && target.mhp==40);
        if(selected==MCAST_STUN_YOU) assert(target.mstun && target.mhp==40);
        if(selected==MCAST_WEAKEN_YOU) assert(target.mhp==20 && target.mhpmax==60);
        if(selected==MCAST_DESTRY_ARMR) assert(removed==1);
        if(selected==MCAST_CURSE_ITEMS) assert(curses==1 && item.cursed);
        if(selected==MCAST_SUMMON_MONS) assert(summons==1 && caster.mux==2 && caster.muy==1);
        if(selected==MCAST_DEATH_TOUCH) assert(deaths==1 && (result&M_ATTK_DEF_DIED));
    }
    caster=make_mon(PM_ALABASTER_MUMMY,10);target=make_mon(PM_ORC,12);
    caster.mspare1=(MITH_NAEN+1)<<MITH_SYLLABLE_SHIFT;caster.mspec_used=40;fumble=1;
    selected=MCAST_PSI_BOLT;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_HIT && !caster.mspec_used);
    caster.mcan=1;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_MISS);
    caster.mcan=0;fumble=0;selected=MCAST_DEATH_TOUCH;lifesave=1;
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_HIT && target.mhp==20);
    lifesave=0;target=make_mon(PM_IRON_GOLEM,12);
    assert(mith_castmm(&caster,&target,&attack)==M_ATTK_HIT && target.mhp==40);
    target=make_mon(PM_ORC,12);caster.m_lev=10;selected=-1;
    assert(mith_mm_choose_spell(&caster,&target)==MCAST_DESTRY_ARMR);
    assert(mith_mm_useless(&caster,&target,MCAST_CLONE_WIZ));
    caster.mtame=1;
    assert(mith_mm_useless(&caster,&target,MCAST_SUMMON_MONS));
    puts("PASS mummy spell effects, actual native-list selection, Naen cooldown/fumble, cancellation, target proxy, death immunity and lifesaving");
    return 0;
}
