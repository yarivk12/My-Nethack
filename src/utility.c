/* Ordinary Tool / Utility effects at native engine boundaries. */
#include "hack.h"

struct utility_snapshot {
    struct utility_snapshot *next;
    struct obj *source;
    int interval;
};
static struct utility_snapshot *utility_sources;

staticfn void
utility_register_source(struct obj *obj)
{
    struct utility_snapshot *sample;
    for (sample = utility_sources; sample; sample = sample->next)
        if (sample->source == obj)
            return;
    sample = (struct utility_snapshot *) alloc(sizeof *sample);
    sample->source = obj;
    sample->interval = utility_purification_interval(obj);
    sample->next = utility_sources;
    utility_sources = sample;
}

/* Complete only the one sampled turn, including an object which was saved
 * on the departed level before that turn elapsed. This is never catch-up. */
void
utility_purification_restore(struct obj *obj)
{
    if (obj->o_purification_sampled
        && obj->o_purification_sampled <= svm.moves) {
        if (utility_purification_interval(obj) && obj->o_purification_remaining)
            --obj->o_purification_remaining;
        obj->o_purification_sampled = 0L;
    } else if (obj->o_purification_sampled) {
        /* A mid-turn restore can contain sampled sources which are now on
         * the floor, in a container, or carried by a monster. */
        utility_register_source(obj);
    }
}

/* Sources are top-level hero inventory; recipients have separate scopes. */
boolean
utility_active(int property)
{
    struct obj *obj;
    for (obj = gi.invent; obj; obj = obj->nobj)
        if (carried(obj) && enhancement_has(obj, property))
            return TRUE;
    return FALSE;
}

boolean
utility_erosion_protected(const struct obj *obj, int type)
{
    int minimum, tier;
    if (!obj || !carried(obj))
        return FALSE;
    switch (type) {
    case ERODE_RUST: minimum = 0; break;
    case ERODE_ROT: minimum = 1; break;
    case ERODE_CORRODE: minimum = 2; break;
    case ERODE_BURN: minimum = 3; break;
    default: return FALSE;
    }
    for (tier = 3; tier >= minimum; --tier)
        if (utility_active(EP_EROSION_I + tier))
            return TRUE;
    return FALSE;
}

boolean
utility_curse_protected(const struct obj *obj)
{
    const struct obj *outer = obj;
    if (!obj || enhancement_equipped_target(obj))
        return FALSE;
    while (outer->where == OBJ_CONTAINED)
        outer = outer->ocontainer;
    if (!carried(outer))
        return FALSE;
    return utility_active(EP_CURSE_IV)
           || (carried(obj) && utility_active(EP_CURSE_II));
}

void
utility_discernment_refresh(void)
{
    struct obj *obj;
    if (!utility_active(EP_DISCERNMENT))
        return;
    for (obj = gi.invent; obj; obj = obj->nobj)
        obj->bknown = 1;
}

int
utility_purification_interval(const struct obj *obj)
{
    int tier;
    for (tier = 0; tier < 4; ++tier)
        if (enhancement_has(obj, EP_PURIFICATION_I + tier))
            return 400 - tier * 100;
    return 0;
}

/* Snapshots live only between native turns. Objects remain valid after
 * dropping, containment and stealing; deallocation removes the reference. */
void
utility_turn_cancel(void)
{
    struct utility_snapshot *sample;
    while ((sample = utility_sources) != NULL) {
        utility_sources = sample->next;
        if (sample->source)
            sample->source->o_purification_sampled = 0L;
        free((genericptr_t) sample);
    }
    svc.context.purification_snapshot_turn = 0L;
}

void
utility_forget_object(struct obj *obj)
{
    struct utility_snapshot *sample;
    for (sample = utility_sources; sample; sample = sample->next)
        if (sample->source == obj)
            sample->source = NULL;
}

void
utility_turn_snapshot(void)
{
    struct obj *obj;
    if (svc.context.purification_snapshot_turn == svm.moves + 1L)
        return;
    svc.context.purification_snapshot_turn = svm.moves + 1L;
    for (obj = gi.invent; obj; obj = obj->nobj) {
        int interval = utility_purification_interval(obj);
        utility_purification_restore(obj);
        if (interval) {
            obj->o_purification_sampled = svm.moves + 1L;
            utility_register_source(obj);
        }
    }
}

void
utility_turn_tick(void)
{
    struct utility_snapshot *sample;
    for (sample = utility_sources; sample; sample = sample->next) {
        struct obj *obj = sample->source;
        if (obj && utility_purification_interval(obj) == sample->interval)
            utility_purification_restore(obj);
    }
    utility_turn_cancel();
}

/* Native uncurse() accepts every cursed object, including entire stacks.
 * Reservoir sampling chooses uniformly without a second inventory cache. */
staticfn void
utility_cursed_target(struct obj *chain, struct obj **target, int *count)
{
    struct obj *obj;
    for (obj = chain; obj; obj = obj->nobj) {
        if (obj->cursed && !rn2(++*count))
            *target = obj;
        if (obj->cobj)
            utility_cursed_target(obj->cobj, target, count);
    }
}

void
utility_turn_end(void)
{
    struct obj *source;
    boolean changed = FALSE;
    for (source = gi.invent; source; source = source->nobj) {
        int interval = utility_purification_interval(source), count = 0;
        struct obj *target = NULL;
        if (!interval || source->o_purification_remaining)
            continue;
        utility_cursed_target(gi.invent, &target, &count);
        if (!target)
            continue;
        uncurse(target);
        source->o_purification_remaining = (unsigned short) interval;
        changed = TRUE;
    }
    if (changed)
        update_inventory();
}

int
utility_excavating_effort(struct obj *pick, int before, int native,
                          int threshold)
{
    if (!enhancement_has(pick, EP_EXCAVATING))
        return native;
    /* Native completion checks use strict greater-than thresholds. */
    if (before + native <= threshold && before + native * 2 > threshold)
        enhancement_learn(pick, EP_EXCAVATING);
    return native * 2;
}
