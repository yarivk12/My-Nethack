/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Step 22 engine, deliberately without production artifact bindings.
 * Copyright 2026. NetHack license, see license. */
#include "hack.h"
#include "artifact.h"

static unsigned ex_secondary_depth;

int ex_secondary(void) { return ex_secondary_depth != 0; }
void ex_secondary_begin(void) { ++ex_secondary_depth; }
void ex_secondary_end(void) { if (ex_secondary_depth) --ex_secondary_depth; }

static struct obj *
ex_find(struct obj *list, unsigned id)
{
    struct obj *o, *found;
    for (o = list; o; o = o->nobj) {
        if (o->o_id == id) return o;
        if (o->cobj && (found = ex_find(o->cobj, id)) != 0) return found;
    }
    return NULL;
}

int
ex_active(const struct obj *o, int property, const struct monst *actor)
{
    const struct obj *top = o;
    int permission;
    long mask;
    if (!o || !o->oartifact || property < 0 || property >= EX_COUNT
        || !(o->exotic.grants & (1UL << property))) return 0;
    if (!actor || actor == &gy.youmonst) permission = EX_HERO;
    else permission = actor->mtame ? EX_PET : EX_MONSTER;
    if (!(o->exotic.actors[property] & permission)) return 0;
    while (top->where == OBJ_CONTAINED) top = top->ocontainer;
    if (permission == EX_HERO) {
        if (top->where != OBJ_INVENT) return 0;
    } else if (top->where != OBJ_MINVENT || top->ocarry != actor) return 0;
    mask = o->owornmask;
    switch (o->exotic.activation[property]) {
    case EX_DIRECT: return top == o;
    case EX_CARRIED: return 1;
    case EX_WIELDED:
        if (permission == EX_HERO)
            return o == uwep || (u.twoweap && o == uswapwep);
        return MON_WEP(actor) == o;
    case EX_WORN:
        return top == o && (mask & o->exotic.slots[property]) != 0;
    case EX_MANUAL: return top == o;
    case EX_INVOKED: return o->exotic.invoked != 0;
    default: return 0;
    }
}

static void
ex_collect(struct ex_sources *s, struct obj *list, struct monst *actor)
{
    struct obj *o;
    unsigned long grants;
    int p;
    for (o = list; o; o = o->nobj) {
        grants = 0;
        for (p = 0; p < EX_COUNT; ++p)
            if (ex_active(o, p, actor)) grants |= 1UL << p;
        if (grants && s->count < EX_MAX_SOURCES) {
            s->ids[s->count] = o->o_id;
            s->grants[s->count++] = grants;
        }
        if (o->cobj) ex_collect(s, o->cobj, actor);
    }
}

void
ex_snapshot(struct ex_sources *s, struct monst *actor)
{
    unsigned i, j, id;
    unsigned long grants;
    s->count = 0;
    ex_collect(s, (!actor || actor == &gy.youmonst) ? gi.invent
                                                          : actor->minvent,
               actor);
    for (i = 1; i < s->count; ++i) {
        id = s->ids[i]; grants = s->grants[i]; j = i;
        while (j && s->ids[j - 1] > id) {
            s->ids[j] = s->ids[j - 1]; s->grants[j] = s->grants[j - 1]; --j;
        }
        s->ids[j] = id; s->grants[j] = grants;
    }
}

struct obj *
ex_source(const struct ex_sources *s, unsigned i, int p, struct monst *actor)
{
    struct obj *o;
    if (p < 0 || p >= EX_COUNT || i >= s->count
        || !(s->grants[i] & (1UL << p))) return NULL;
    o = ex_find((!actor || actor == &gy.youmonst) ? gi.invent : actor->minvent,
                s->ids[i]);
    return ex_active(o, p, actor) ? o : NULL;
}

int
ex_has(int p)
{
    struct ex_sources s;
    unsigned i;
    ex_snapshot(&s, &gy.youmonst);
    for (i = 0; i < s.count; ++i)
        if (ex_source(&s, i, p, &gy.youmonst)) return 1;
    return 0;
}

int
ex_roll(int p)
{
    static const unsigned char chance[EX_COUNT] = {
        20,20,20,20,20,20,30,33,100,100,20,100,100,100,0,100,100,25
    };
    return p >= 0 && p < EX_COUNT && rn2(100) < chance[p];
}

int
ex_protected_monster(const struct monst *m)
{
    return !m || is_rider(m->data) || m->iswiz || m->isshk || m->ispriest
           || m->isgd || m->data->msound == MS_LEADER
           || m->data->msound == MS_NEMESIS || m->data->msound == MS_GUARDIAN
           || (svq.quest_status.leader_m_id && m->m_id == svq.quest_status.leader_m_id)
           || m->data == &mons[PM_MEDUSA] || m->data == &mons[PM_VLAD_THE_IMPALER];
}

int
ex_protected_object(const struct obj *o)
{
    const struct obj *c;
    if (!o) return 0;
    if ((o->oartifact && o->oartifact == gu.urole.questarti)
        || o->otyp == AMULET_OF_YENDOR
        || o->otyp == BELL_OF_OPENING || o->otyp == CANDELABRUM_OF_INVOCATION
        || o->otyp == SPE_BOOK_OF_THE_DEAD
        || o->oartifact == ART_SILVER_KEY || o->oartifact == ART_NECRONOMICON
        || (o->oartifact >= ART_FIRST_KEY_OF_NEUTRALITY
            && o->oartifact <= ART_THIRD_KEY_OF_NEUTRALITY)) return 1;
    for (c = o->cobj; c; c = c->nobj)
        if (ex_protected_object(c)) return 1;
    return 0;
}

int
ex_protected_tile(int x, int y)
{
    struct trap *t;
    if (!isok(x,y) || stairway_at(x,y) || invocation_pos(x,y)) return 1;
    t = t_at(x,y);
    if (t && (t->ttyp == MAGIC_PORTAL || t->ttyp == VIBRATING_SQUARE)) return 1;
    return IS_DRAWBRIDGE(levl[x][y].typ) || IS_ALTAR(levl[x][y].typ)
           || IS_FORGE(levl[x][y].typ);
}

void
ex_resources(struct obj *o)
{
    static const int properties[2] = {EX_DIG, EX_TRANSMUTE};
    static const int caps[2] = {5,10}, intervals[2] = {100,10};
    int k;
    long gained;
    for (k = 0; k < 2; ++k) {
        if (!(o->exotic.grants & (1UL << properties[k]))) continue;
        if (!(o->exotic.initialized & (1 << k))) {
            o->exotic.initialized |= 1 << k;
            o->exotic.charges[k] = caps[k];
            o->exotic.charge_turn[k] = svm.moves;
        }
        if (o->exotic.charges[k] >= caps[k]) {
            o->exotic.charges[k] = caps[k];
            o->exotic.charge_turn[k] = svm.moves;
            continue;
        }
        gained = (svm.moves - o->exotic.charge_turn[k]) / intervals[k];
        if (gained <= 0) continue;
        o->exotic.charges[k] = (unsigned char) min(caps[k], o->exotic.charges[k] + gained);
        o->exotic.charge_turn[k] += gained * intervals[k];
        if (o->exotic.charges[k] == caps[k]) o->exotic.charge_turn[k] = svm.moves;
    }
}

int
ex_spend(struct obj *o, int pool, int amount)
{
    int cap = pool == 0 ? 5 : 10;
    if (!o || pool < 0 || pool > 1 || amount < 1) return 0;
    ex_resources(o);
    if (o->exotic.charges[pool] < amount) return 0;
    if (o->exotic.charges[pool] == cap) o->exotic.charge_turn[pool] = svm.moves;
    o->exotic.charges[pool] -= amount;
    return 1;
}

#ifdef STEP15_TEST
void
ex_test_grant(struct obj *o, int p, int mode, int actors, long slot)
{
    if (!o || !o->oartifact || p < 0 || p >= EX_COUNT) return;
    o->exotic.grants |= 1UL << p;
    o->exotic.activation[p] = mode;
    o->exotic.actors[p] = actors;
    o->exotic.slots[p] = slot;
    ex_resources(o);
}
void ex_test_clear(struct obj *o) { memset(&o->exotic, 0, sizeof o->exotic); }
#endif

/* Property modules share the typed source and lifecycle contract above. */
#include "exotic_combat.inc"
#include "exotic_utility.inc"
#include "exotic_items.inc"
#include "exotic_defense.inc"
