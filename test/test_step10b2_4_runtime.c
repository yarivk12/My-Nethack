#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct instance_globals_saved_l svl;
struct instance_globals_saved_k svk;
struct instance_globals_m gm;
struct instance_globals_y gy;

int
sgn(int value)
{
    return (value > 0) - (value < 0);
}

static int grow_count, explosion_count, cloud_count, throw_count;
static int familiar_create_count, familiar_flee_count;
static mmflags_nht familiar_create_flags;
static int explosion_type, explosion_damage, explosion_olet, explosion_visual;
static int cloud_size, cloud_damage, throw_range;
static coordxy throw_dx, throw_dy;
static struct obj test_object;
static NhRegion test_cloud;
static struct monst test_familiar;

struct monst *
makemon(struct permonst *ptr, coordxy x, coordxy y, mmflags_nht flags)
{
    nhUse(x); nhUse(y);
    ++familiar_create_count;
    familiar_create_flags = flags;
    (void) memset(&test_familiar, 0, sizeof test_familiar);
    test_familiar.data = ptr;
    return &test_familiar;
}

int
canseemon(struct monst *mon)
{
    nhUse(mon);
    return 0;
}

char *
Monnam(struct monst *mon)
{
    nhUse(mon);
    return "witch";
}

void
pline_mon(struct monst *mon, const char *fmt, ...)
{
    nhUse(mon); nhUse(fmt);
}

void
monflee(struct monst *mon, int duration, boolean first, boolean message)
{
    nhUse(duration); nhUse(first); nhUse(message);
    ++familiar_flee_count;
    mon->mflee = 1;
}

struct permonst *
grow_up(struct monst *mon, struct monst *victim)
{
    nhUse(victim);
    ++grow_count;
    ++mon->mhpmax;
    ++mon->mhp;
    return mon->data;
}

int
d(int number, int sides)
{
    return number * sides;
}

void
explode(coordxy x, coordxy y, int type, int damage, char olet, int visual)
{
    nhUse(x); nhUse(y);
    ++explosion_count;
    explosion_type = type;
    explosion_damage = damage;
    explosion_olet = olet;
    explosion_visual = visual;
}

NhRegion *
create_gas_cloud(coordxy x, coordxy y, int size, int damage)
{
    nhUse(x); nhUse(y);
    ++cloud_count;
    cloud_size = size;
    cloud_damage = damage;
    test_cloud.ttl = 0;
    return &test_cloud;
}

struct obj *
mksobj(int otyp, boolean init, boolean artif)
{
    nhUse(init); nhUse(artif);
    (void) memset(&test_object, 0, sizeof test_object);
    test_object.otyp = (short) otyp;
    return &test_object;
}

void
curse(struct obj *obj)
{
    obj->cursed = 1;
}

boolean
linedup(coordxy ax, coordxy ay, coordxy dx, coordxy dy, int flags)
{
    nhUse(ax); nhUse(ay); nhUse(dx); nhUse(dy); nhUse(flags);
    return TRUE;
}

void
m_throw(struct monst *mon, coordxy x, coordxy y, coordxy dx, coordxy dy,
        int range, struct obj *obj)
{
    nhUse(mon); nhUse(x); nhUse(y);
    assert(obj == &test_object);
    ++throw_count;
    throw_dx = dx;
    throw_dy = dy;
    throw_range = range;
}

int
mith_silver_arrow(struct monst *magr, struct monst *mdef)
{
    nhUse(magr); nhUse(mdef);
    return 42;
}

/* EXTRACTED */

static void
reset_mon(struct monst *mon, int pm, int hp)
{
    (void) memset(mon, 0, sizeof *mon);
    mon->data = &mons[pm];
    mon->mhp = mon->mhpmax = hp;
}

int
main(void)
{
    struct monst deep, deeper, deepest, unrelated, attacker, defender;
    struct monst witch, ordinary;
    struct attack load = { AT_ARRW, AD_LOAD, 1, 1 };
    struct attack silver = { AT_ARRW, AD_SLVR, 1, 4 };

    monst_globals_init();
    reset_mon(&deep, PM_DEEP_ONE, 100);
    reset_mon(&deeper, PM_DEEPER_ONE, 100);
    reset_mon(&deepest, PM_DEEPEST_ONE, 100);
    reset_mon(&unrelated, PM_HUMAN, 100);
    deep.nmon = &deeper;
    deeper.nmon = &deepest;
    deepest.nmon = &unrelated;
    svl.level.monlist = &deep;
    mith_deep_soul(&mons[PM_FATHER_DAGON]);
    assert(deep.mhpmax == 108 && deeper.mhpmax == 108);
    assert(deepest.mhpmax == 108 && unrelated.mhpmax == 100);
    assert(grow_count == 3);
    mith_deep_soul(&mons[PM_MOTHER_HYDRA]);
    assert(deep.mhpmax == 116 && deepest.mhpmax == 116);
    assert(grow_count == 6);
    mith_deep_soul(&mons[PM_DEEP_ONE]);
    assert(deep.mhpmax == 118 && deepest.mhpmax == 118);
    assert(grow_count == 9);

    step10b_cthulhu_death_effect(7, 9);
    assert(explosion_count == 1 && explosion_type == PHYS_EXPL_TYPE);
    assert(explosion_damage == 64 && explosion_olet == MON_EXPLODE);
    assert(explosion_visual == EXPL_NOXIOUS);
    assert(cloud_count == 1 && STEP10B_CTHULHU_GAS_RADIUS == 2);
    assert(cloud_size == 5 && cloud_damage == 30 && test_cloud.ttl == 30);
    assert(!svk.killer.name[0]);

    reset_mon(&attacker, PM_CENTER_OF_ALL, 100);
    reset_mon(&defender, PM_HUMAN, 100);
    attacker.mx = 2; attacker.my = 3;
    defender.mx = 6; defender.my = 3;
    assert(mith_internal_projectile(&attacker, &defender, &load)
           == M_ATTK_HIT);
    assert(throw_count == 1 && test_object.otyp == LOADSTONE);
    assert(test_object.cursed && throw_dx == 1 && throw_dy == 0);
    assert(throw_range == 8 && gm.mtarget == 0);
    assert(mith_internal_projectile(&attacker, &defender, &silver) == 42);

    reset_mon(&ordinary, PM_HUMAN, 20);
    ordinary.m_id = 41U;
    assert(step10b_create_witch_familiar(&ordinary) == 0);
    assert(familiar_create_count == 0);

    reset_mon(&witch, PM_APPRENTICE_WITCH, 31);
    witch.mhpmax = 47;
    witch.m_id = 42U;
    witch.mpeaceful = 1;
    witch.mx = 3; witch.my = 4;
    assert(step10b_create_witch_familiar(&witch) == &test_familiar);
    assert(familiar_create_count == 1);
    assert(test_familiar.data == &mons[PM_WITCH_S_FAMILIAR]);
    assert(familiar_create_flags == (MM_ADJACENTOK | MM_NOCOUNTBIRTH));
    assert(test_familiar.m_lev == witch.m_lev);
    assert(test_familiar.mhp == 31 && test_familiar.mhpmax == 47);
    assert(test_familiar.mpeaceful == 1);
    assert((unsigned long) test_familiar.mspare1 == 42UL);
    svl.level.monlist = &test_familiar;
    assert(!step10b_witch_needs_familiar(&witch));
    test_familiar.mspare1 = 43L;
    assert(step10b_witch_needs_familiar(&witch));

    test_familiar.nmon = &witch;
    step10b_familiar_died(42U);
    assert(witch.mflee && familiar_flee_count == 1);
    assert(witch.mspec_used == 0);

    reset_mon(&witch, PM_WITCH, 40);
    witch.m_id = 52U;
    test_familiar.nmon = &witch;
    step10b_familiar_died(52U);
    assert(witch.mflee && familiar_flee_count == 2);
    assert(witch.mspec_used == 10);

    reset_mon(&witch, PM_COVEN_LEADER, 60);
    witch.m_id = 62U;
    test_familiar.nmon = &witch;
    step10b_familiar_died(62U);
    assert(!witch.mflee && familiar_flee_count == 2);
    assert(witch.mspec_used == 4 && witch.mavenge);

    puts("PASS Step 10B2-4 runtime familiar/soul/death/projectile paths");
    return 0;
}
