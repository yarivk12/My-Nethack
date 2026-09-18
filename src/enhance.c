/* Generic equipment mechanics; no generation or progression policy. */
#include "hack.h"

staticfn uint32 enhancement_allowed(const struct obj *);
staticfn uint32 enhancement_active(const struct obj *);
staticfn const struct obj *enhancement_launcher(const struct obj *,
                                               const struct obj *,
                                               enum enhance_use);

boolean
enhancement_eligible(const struct obj *obj)
{
    return obj && !obj->oartifact
           && (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS
               || is_weptool(obj) || is_ammo(obj) || is_missile(obj));
}

staticfn uint32
enhancement_allowed(const struct obj *obj)
{
    if (!enhancement_eligible(obj))
        return 0;
    if (obj->oclass == ARMOR_CLASS)
        return OEP_WORN;
    return OEP_ELEMENTS | OEP_CUMBERSOME
           | ((is_launcher(obj) || is_ammo(obj) || is_missile(obj))
                  ? OEP_TRUEFLIGHT : 0);
}

boolean
enhancement_property_allowed(const struct obj *obj, uint32 props)
{
    return enhancement_eligible(obj)
           && !(props & ~enhancement_allowed(obj));
}

staticfn uint32
enhancement_active(const struct obj *obj)
{
    return obj ? obj->o_enh_props & enhancement_allowed(obj) : 0;
}

/* Only a matched shot transfers launcher properties.  Artifacts may deliver
 * ordinary enhanced ammo, but supply no generic properties themselves. */
staticfn const struct obj *
enhancement_launcher(const struct obj *obj, const struct obj *launcher,
                     enum enhance_use use)
{
    return use == ENHANCE_AMMO && obj && ammo_and_launcher(obj, launcher)
               ? launcher : (const struct obj *) 0;
}

void
enhancement_worn_off(struct obj *obj, struct monst *wearer)
{
    int p;
    long mask;

    if (!obj || wearer != &gy.youmonst)
        return; /* native monster Warning/Search/Stealth remain inert */
    mask = obj->owornmask & W_ARMOR;
    /* Clear only this slot. Preserve the object's native property when a
     * setter changes enhancements without removing the armor itself. */
    for (p = 0; p < 3; ++p) {
        int prop = p == 0 ? WARNING : p == 1 ? SEARCHING : STEALTH;
        if (objects[obj->otyp].oc_oprop != prop)
            u.uprops[prop].extrinsic &= ~mask;
    }
}

void
enhancement_worn_on(struct obj *obj, struct monst *wearer)
{
    uint32 props;
    long mask;

    if (!obj || wearer != &gy.youmonst)
        return;
    props = enhancement_active(obj);
    mask = obj->owornmask & W_ARMOR;
    if ((props & OEP_WARNING) && mask) {
        boolean observed = !Warning;
        EWarning |= mask;
        if (observed && Warning) {
            You_feel("sensitive to danger.");
            obj->o_enh_known |= OEP_WARNING;
        }
    }
    if ((props & OEP_SEARCHING) && mask) {
        boolean observed = !Searching;
        ESearching |= mask;
        if (observed && Searching) {
            You_feel("more observant.");
            obj->o_enh_known |= OEP_SEARCHING;
        }
    }
    if ((props & OEP_STEALTH) && mask) {
        boolean observed = !Stealth;
        EStealth |= mask;
        if (observed && Stealth) {
            You_feel("stealthy.");
            obj->o_enh_known |= OEP_STEALTH;
        }
    }
}

staticfn void
enhancement_changed(struct obj *obj)
{
    if (obj->unpaid)
        enhancement_rebill(obj);
    if (carried(obj)) {
        enhancement_worn_on(obj, &gy.youmonst);
        if (obj->owornmask & W_ARMOR)
            find_ac();
        update_inventory();
    }
}

void
enhancement_clear(struct obj *obj)
{
    if (!obj || !(obj->o_enh_props || obj->o_enh_known
                  || obj->o_enh_quality || obj->o_enh_flags))
        return;
    if (carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = obj->o_enh_known = 0;
    obj->o_enh_quality = obj->o_enh_flags = 0;
    enhancement_changed(obj);
}

/* Identity-changing conversions discard generic state; constructors still
 * zero the whole object. Remove old worn effects before changing identity. */
void
enhancement_change_type(struct obj *obj, int otyp)
{
    boolean changed = obj->o_enh_props || obj->o_enh_known
                      || obj->o_enh_quality || obj->o_enh_flags;
    if (changed && carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = obj->o_enh_known = 0;
    obj->o_enh_quality = obj->o_enh_flags = 0;
    obj->otyp = otyp;
    if (changed)
        enhancement_changed(obj);
}

void
enhancement_strip_for_artifact(struct obj *obj)
{
    if (obj && obj->oartifact)
        enhancement_clear(obj);
}

void
enhancement_normalize(struct obj *obj)
{
    uint32 props, known;
    uint8 quality, flags;

    if (!obj)
        return;
    if (!enhancement_eligible(obj)) {
        enhancement_clear(obj);
        return;
    }
    props = obj->o_enh_props & enhancement_allowed(obj);
    known = obj->o_enh_known & OEP_ALL;
    quality = obj->o_enh_quality <= OQ_EXCEPTIONAL
                  ? obj->o_enh_quality : OQ_STANDARD;
#ifdef DEBUG
    if (quality != obj->o_enh_quality)
        impossible("Invalid equipment enhancement quality");
#endif
    flags = obj->o_enh_flags & OEF_QUALITY_KNOWN;
    if (props == obj->o_enh_props && known == obj->o_enh_known
        && quality == obj->o_enh_quality && flags == obj->o_enh_flags)
        return;
    if (carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = props;
    obj->o_enh_known = known;
    obj->o_enh_quality = quality;
    obj->o_enh_flags = flags;
    enhancement_changed(obj);
}

boolean
enhancement_set(struct obj *obj, uint32 props,
                enum enhancement_quality quality, boolean known)
{
    if (!enhancement_property_allowed(obj, props)
        || quality < OQ_STANDARD || quality > OQ_EXCEPTIONAL)
        return FALSE;
    if (carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = props;
    obj->o_enh_known = known ? enhancement_allowed(obj) : 0;
    obj->o_enh_quality = (uint8) quality;
    obj->o_enh_flags = known ? OEF_QUALITY_KNOWN : 0;
    enhancement_changed(obj);
    return TRUE;
}

void
enhancement_identify(struct obj *obj)
{
    if (enhancement_eligible(obj)) {
        obj->o_enh_known = enhancement_allowed(obj);
        obj->o_enh_flags |= OEF_QUALITY_KNOWN;
    }
}

uint32
enhancement_visible_props(const struct obj *obj, boolean force_id)
{
    return enhancement_active(obj) & (force_id ? OEP_ALL
                                              : obj ? obj->o_enh_known : 0);
}

int
enhancement_quality_bonus(const struct obj *obj, enum enhance_use use)
{
    if (!enhancement_eligible(obj) || obj->o_enh_quality > OQ_EXCEPTIONAL
        || ((use == ENHANCE_ARMOR) != (obj->oclass == ARMOR_CLASS)))
        return 0;
    return obj->o_enh_quality;
}

int
enhancement_hit_bonus(const struct obj *obj, const struct obj *launcher,
                      const struct monst *target UNUSED, enum enhance_use use)
{
    uint32 props = enhancement_active(obj);
    int bonus;

    launcher = enhancement_launcher(obj, launcher, use);
    bonus = enhancement_quality_bonus(launcher ? launcher : obj, use);
    if (use == ENHANCE_ARMOR || (obj && obj->oclass == ARMOR_CLASS))
        return 0;
    props |= enhancement_active(launcher);
    if (props & OEP_CUMBERSOME)
        bonus -= 2;
    if (use != ENHANCE_MELEE && (props & OEP_TRUEFLIGHT))
        bonus += 2;
    return bonus;
}

int
enhancement_damage_bonus(const struct obj *obj,
                         const struct obj *launcher UNUSED,
                         const struct monst *target UNUSED, enum enhance_use use)
{
    return use == ENHANCE_ARMOR || use == ENHANCE_LAUNCHER
               ? 0 : enhancement_quality_bonus(obj, use);
}

/* Explicit player-attack observation, never called by pure hit queries. */
void
enhancement_observe_attack(struct obj *obj, struct obj *launcher,
                           enum enhance_use use)
{
    uint32 bits = OEP_CUMBERSOME;
    if (use != ENHANCE_MELEE)
        bits |= OEP_TRUEFLIGHT;
    if (obj)
        obj->o_enh_known |= enhancement_active(obj) & bits;
    if (enhancement_launcher(obj, launcher, use))
        launcher->o_enh_known |= enhancement_active(launcher) & bits;
}

int
enhancement_weapon_effects(struct obj *obj, const struct obj *launcher,
                           struct monst *target, int damage,
                           enum enhance_use use)
{
    uint32 props, bit;
    int extra = 0;
    boolean hero = target == &gy.youmonst, resisted, visible;

    if (!obj || !target || damage <= 0 || use == ENHANCE_ARMOR)
        return 0;
    launcher = enhancement_launcher(obj, launcher, use);
    props = (enhancement_active(obj) | enhancement_active(launcher))
            & OEP_ELEMENTS;
    for (bit = OEP_FIRE; bit <= OEP_SHOCK; bit <<= 1) {
        if (!(props & bit))
            continue;
        resisted = bit == OEP_FIRE ? (hero ? Fire_resistance : resists_fire(target))
                   : bit == OEP_COLD ? (hero ? Cold_resistance : resists_cold(target))
                                     : (hero ? Shock_resistance : resists_elec(target));
        if (resisted)
            continue;
        extra += rnd(4);
        visible = !Blind && (hero || canseemon(target));
        if (visible) {
            pline("%s %s struck by %s!", hero ? "You" : Monnam(target),
                  hero ? "are" : "is",
                  bit == OEP_FIRE ? "fire" : bit == OEP_COLD ? "frost" : "lightning");
        }
    }
    return extra;
}

/* Called only by the hero's confirmed-hit path after a visible elemental
 * message. gt.thrownobj is also used by monsters, so it is not ownership. */
void
enhancement_observe_hit(struct obj *obj, struct obj *launcher,
                        struct monst *target, enum enhance_use use)
{
    uint32 bits = OEP_ELEMENTS;
    if (Blind || !canseemon(target))
        return;
    if (resists_fire(target)) bits &= ~OEP_FIRE;
    if (resists_cold(target)) bits &= ~OEP_COLD;
    if (resists_elec(target)) bits &= ~OEP_SHOCK;
    if (obj) obj->o_enh_known |= enhancement_active(obj) & bits;
    if (enhancement_launcher(obj, launcher, use))
        launcher->o_enh_known |= enhancement_active(launcher) & bits;
}

long
enhancement_price_adjustment(const struct obj *obj)
{
    uint32 props = enhancement_active(obj), bit;
    long percent = 100L;
    if (!enhancement_eligible(obj))
        return percent;
    percent += 10L * enhancement_quality_bonus(obj,
                 obj->oclass == ARMOR_CLASS ? ENHANCE_ARMOR : ENHANCE_MELEE);
    for (bit = OEP_FIRE; bit < OEP_CUMBERSOME; bit <<= 1)
        if (props & bit)
            percent += 20L;
    return percent - ((props & OEP_CUMBERSOME) ? 10L : 0L);
}

long
enhancement_price(const struct obj *obj, long base)
{
    long percent = enhancement_price_adjustment(obj);
    if (!enhancement_eligible(obj))
        return base;
    return max(1L, (base / 100L) * percent + (base % 100L) * percent / 100L);
}

void
enhancement_prefix(const struct obj *obj, boolean force_id,
                    char *buf, size_t size)
{
    static const char *const names[] = {
        "fire ", "frost ", "shock ", "trueflight ", "warning ",
        "searching ", "stealth ", "cumbersome "
    };
    uint32 props = enhancement_visible_props(obj, force_id);
    int i, quality;
    size_t len;

    if (!size)
        return;
    buf[0] = '\0';
    if (!enhancement_eligible(obj))
        return;
    quality = (force_id || (obj->o_enh_flags & OEF_QUALITY_KNOWN))
                  ? obj->o_enh_quality : 0;
    if (quality == OQ_FINE || quality == OQ_EXCEPTIONAL)
        Snprintf(buf, size, "%s", quality == OQ_FINE ? "fine " : "exceptional ");
    for (i = 0; i < 8; ++i)
        if (props & (1U << i)) {
            len = strlen(buf);
            if (len < size - 1)
                Snprintf(buf + len, size - len, "%s", names[i]);
        }
}
