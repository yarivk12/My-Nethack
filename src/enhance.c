/* Shared generic equipment mechanics and natural-generation policy. */
#include "hack.h"

staticfn uint32 enhancement_allowed(const struct obj *);
staticfn uint32 enhancement_active(const struct obj *);
staticfn const struct obj *enhancement_launcher(const struct obj *,
                                               const struct obj *,
                                               enum enhance_use);

/* Catalog order is the canonical same-tier naming priority. Element values
 * are native resistance properties; Primordial uses all three components. */
const struct enhancement_entry enhancement_catalog[24] = {
    { OEP_FIRE, 1, 0, FIRE_RES, 1, 4, "smoldering", "of Embers" },
    { OEP_COLD, 1, 0, COLD_RES, 1, 4, "chilled", "of Rime" },
    { OEP_SHOCK, 1, 0, SHOCK_RES, 1, 4, "sparking", "of Static" },
    { OEP_TRUEFLIGHT, 1, 0, 0, 0, 0, "trueflight", "of Trueflight" },
    { OEP_FIRE_II, 2, 0, FIRE_RES, 3, 4, "blazing", "of the Inferno" },
    { OEP_COLD_II, 2, 0, COLD_RES, 3, 4, "glacial", "of the Blizzard" },
    { OEP_SHOCK_II, 2, 0, SHOCK_RES, 3, 4, "thunderous", "of the Tempest" },
    { OEP_FIRE_III, 3, 0, FIRE_RES, 5, 6, "cataclysmic", "of Hellfire" },
    { OEP_COLD_III, 3, 0, COLD_RES, 5, 6, "stygian", "of Absolute Zero" },
    { OEP_SHOCK_III, 3, 0, SHOCK_RES, 5, 6, "voltaic", "of Heaven's Wrath" },
    { OEP_PRIMORDIAL, 4, 0, -1, 5, 6, "primordial", "of the Elements" },
    { OEP_SEARCHING, 1, SEARCHING, 0, 0, 0, "keen", "of Searching" },
    { OEP_WARNING, 1, WARNING, 0, 0, 0, "watchful", "of Warning" },
    { OEP_STEALTH, 1, STEALTH, 0, 0, 0, "silent", "of Stealth" },
    { OEP_FIRE_RES, 2, FIRE_RES, 0, 0, 0, "emberward", "of Fire Resistance" },
    { OEP_COLD_RES, 2, COLD_RES, 0, 0, 0, "frostward", "of Cold Resistance" },
    { OEP_SHOCK_RES, 2, SHOCK_RES, 0, 0, 0, "stormward", "of Shock Resistance" },
    { OEP_POISON_RES, 2, POISON_RES, 0, 0, 0, "venomward", "of Poison Resistance" },
    { OEP_SPEED, 3, FAST, 0, 0, 0, "swift", "of Speed" },
    { OEP_REGEN, 3, REGENERATION, 0, 0, 0, "renewing", "of Regeneration" },
    { OEP_DISPLACED, 3, DISPLACED, 0, 0, 0, "shifting", "of Displacement" },
    { OEP_SLOW_DIGEST, 3, SLOW_DIGESTION, 0, 0, 0, "sustaining", "of Slow Digestion" },
    { OEP_MAGIC_RES, 4, ANTIMAGIC, 0, 0, 0, "arcane", "of Magic Resistance" },
    { OEP_REFLECTION, 4, REFLECTING, 0, 0, 0, "mirrored", "of Reflection" }
};

boolean
enhancement_native_property(const struct obj *obj, int prop)
{
    return objects[obj->otyp].oc_oprop == prop
        || (obj->otyp == ALCHEMY_SMOCK && (prop == POISON_RES || prop == ACID_RES))
        /* Secondary native powers from dragon_armor_handling(). */
        || ((obj->otyp == BLUE_DRAGON_SCALES || obj->otyp == BLUE_DRAGON_SCALE_MAIL)
            && prop == FAST)
        || ((obj->otyp == WHITE_DRAGON_SCALES || obj->otyp == WHITE_DRAGON_SCALE_MAIL)
            && prop == SLOW_DIGESTION)
        || (Is_chromatic_armor(obj)
            && ((prop >= FIRE_RES && prop <= STONE_RES)
                || prop == REFLECTING || prop == ANTIMAGIC));
}

boolean
enhancement_eligible(const struct obj *obj)
{
    return obj && !obj->oartifact
           && (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS);
}

staticfn uint32
enhancement_allowed(const struct obj *obj)
{
    if (!enhancement_eligible(obj))
        return 0;
    if (obj->oclass == ARMOR_CLASS) {
        uint32 allowed = OEP_WORN;
        int i;
        for (i = 0; i < SIZE(enhancement_catalog); ++i)
            if (enhancement_catalog[i].native_property
                && enhancement_native_property(obj, enhancement_catalog[i].native_property))
                allowed &= ~enhancement_catalog[i].bit;
        return allowed;
    }
    return OEP_ELEMENTS
           | ((is_launcher(obj) || is_ammo(obj) || is_missile(obj)
               || is_spear(obj))
                  ? OEP_TRUEFLIGHT : 0);
}

boolean
enhancement_property_allowed(const struct obj *obj, uint32 props)
{
    uint32 rest = props & (props - 1U);
    return enhancement_eligible(obj) && !(rest & (rest - 1U))
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

boolean
enhancement_confers(const struct obj *obj, int prop)
{
    int i;
    uint32 bits = enhancement_active(obj);
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (enhancement_catalog[i].native_property == prop
            && (bits & enhancement_catalog[i].bit))
            return TRUE;
    return FALSE;
}

boolean
enhancement_mon_confers(const struct monst *mon, int prop)
{
    const struct obj *obj;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if ((obj->owornmask & W_ARMOR) && enhancement_confers(obj, prop))
            return TRUE;
    return FALSE;
}

boolean
enhancement_elemental_contact(const struct obj *obj, const struct obj *launcher,
                              enum enhance_use use)
{
    return ((enhancement_active(obj)
             | enhancement_active(enhancement_launcher(obj, launcher, use)))
            & OEP_ELEMENTS) != 0;
}

/* A native observable event can identify a single attributable source.
 * Redundant intrinsic, native, or enhanced sources cannot identify each other. */
void
enhancement_observe_worn(int prop)
{
    struct obj *obj, *source = 0;
    int i;
    long sources = u.uprops[prop].extrinsic;
    if (u.uprops[prop].intrinsic || u.uprops[prop].blocked)
        return;
    for (obj = gi.invent; obj; obj = obj->nobj)
        if ((obj->owornmask & W_ARMOR) && enhancement_confers(obj, prop)) {
            if (source) return;
            source = obj;
        }
    if (!source || (sources & ~source->owornmask)) return;
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (enhancement_catalog[i].native_property == prop)
            source->o_enh_known |= source->o_enh_props & enhancement_catalog[i].bit;
}

void
enhancement_worn_off(struct obj *obj, struct monst *wearer)
{
    int i, prop;
    long mask;
    if (!obj || wearer != &gy.youmonst)
        return;
    mask = obj->owornmask & W_ARMOR;
    for (i = 0; i < SIZE(enhancement_catalog); ++i) {
        prop = enhancement_catalog[i].native_property;
        if (prop && !enhancement_native_property(obj, prop)) {
            u.uprops[prop].extrinsic &= ~mask;
            if (mask && (obj->o_enh_props & enhancement_catalog[i].bit))
                monstunseesu_prop(prop);
        }
    }
}

void
enhancement_worn_on(struct obj *obj, struct monst *wearer)
{
    int i, prop;
    long mask;
    uint32 bits;
    boolean warning = Warning, searching = Searching, stealth = Stealth;
    if (!obj || wearer != &gy.youmonst)
        return;
    mask = obj->owornmask & W_ARMOR;
    bits = enhancement_active(obj);
    for (i = 0; i < SIZE(enhancement_catalog); ++i) {
        prop = enhancement_catalog[i].native_property;
        if (prop && (bits & enhancement_catalog[i].bit))
            u.uprops[prop].extrinsic |= mask;
    }
    if (mask && !warning && Warning) {
        You_feel("sensitive to danger.");
        enhancement_observe_worn(WARNING);
    }
    if (mask && !searching && Searching) {
        You_feel("more observant.");
        enhancement_observe_worn(SEARCHING);
    }
    if (mask && !stealth && Stealth) {
        You_feel("stealthy.");
        enhancement_observe_worn(STEALTH);
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
    if (obj->otyp == otyp)
        return;
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
    if (!enhancement_property_allowed(obj, props))
        props = 0; /* corrupt state must not bypass the two-property limit */
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

/* Hit accuracy alone supplies no attributable evidence of Trueflight. */
void
enhancement_observe_attack(struct obj *obj UNUSED, struct obj *launcher UNUSED,
                           enum enhance_use use UNUSED)
{
}

staticfn boolean
enhancement_resisted(struct monst *target, int element)
{
    boolean hero = target == &gy.youmonst;
    return element == FIRE_RES ? (hero ? Fire_resistance : resists_fire(target))
        : element == COLD_RES ? (hero ? Cold_resistance : resists_cold(target))
        : (hero ? Shock_resistance : resists_elec(target));
}

int
enhancement_weapon_effects(struct obj *obj, const struct obj *launcher,
                           struct monst *target, int damage UNUSED,
                           enum enhance_use use)
{
    const struct obj *sources[2];
    int source, i, e, element, extra = 0;
    if (!obj || !target || use == ENHANCE_ARMOR)
        return 0;
    sources[0] = obj;
    sources[1] = enhancement_launcher(obj, launcher, use);
    for (source = 0; source < 2; ++source) {
        uint32 bits = enhancement_active(sources[source]);
        for (i = 0; i < SIZE(enhancement_catalog); ++i) {
            const struct enhancement_entry *entry = &enhancement_catalog[i];
            if (!(bits & entry->bit) || !entry->element)
                continue;
            for (e = 0; e < (entry->element == -1 ? 3 : 1); ++e) {
                element = entry->element == -1
                    ? (e == 0 ? FIRE_RES : e == 1 ? COLD_RES : SHOCK_RES)
                    : entry->element;
                if (enhancement_resisted(target, element))
                    continue;
                extra += d(entry->dice, entry->sides);
                if (!Blind && (target == &gy.youmonst || canseemon(target)))
                    pline("%s %s struck by %s!",
                        target == &gy.youmonst ? "You" : Monnam(target),
                        target == &gy.youmonst ? "are" : "is",
                        element == FIRE_RES ? "fire" : element == COLD_RES ? "frost" : "lightning");
            }
        }
    }
    return extra;
}

void
enhancement_observe_hit(struct obj *obj, struct obj *launcher,
                        struct monst *target, enum enhance_use use)
{
    int i;
    uint32 bits = 0;
    if (Blind || !canseemon(target))
        return;
    for (i = 0; i < SIZE(enhancement_catalog); ++i) {
        int e = enhancement_catalog[i].element;
        if (e && (e == -1
            ? (!enhancement_resisted(target, FIRE_RES)
               || !enhancement_resisted(target, COLD_RES)
               || !enhancement_resisted(target, SHOCK_RES))
            : !enhancement_resisted(target, e)))
            bits |= enhancement_catalog[i].bit;
    }
    if (obj) obj->o_enh_known |= enhancement_active(obj) & bits;
    if (enhancement_launcher(obj, launcher, use))
        launcher->o_enh_known |= enhancement_active(launcher) & bits;
}

long
enhancement_price_adjustment(const struct obj *obj)
{
    uint32 props = enhancement_active(obj);
    int i;
    long percent = 100L;
    if (!enhancement_eligible(obj))
        return percent;
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (props & enhancement_catalog[i].bit)
            percent += 50L << (enhancement_catalog[i].tier - 1);
    return percent * (100L + 10L * enhancement_quality_bonus(obj,
        obj->oclass == ARMOR_CLASS ? ENHANCE_ARMOR : ENHANCE_MELEE)) / 100L;
}

long
enhancement_price(const struct obj *obj, long base)
{
    long percent = enhancement_price_adjustment(obj);
    if (!enhancement_eligible(obj))
        return base;
    return max(1L, (base / 100L) * percent + (base % 100L) * percent / 100L);
}

/* Stable tier-first order; equal tiers retain catalog priority. */
staticfn void
enhancement_names(const struct obj *obj, boolean force_id, int *first, int *second)
{
    int i;
    uint32 bits = enhancement_visible_props(obj, force_id);
    *first = *second = -1;
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (bits & enhancement_catalog[i].bit) {
            if (*first < 0 || enhancement_catalog[i].tier > enhancement_catalog[*first].tier) {
                *second = *first;
                *first = i;
            } else {
                *second = i;
            }
        }
}

void
enhancement_prefix(const struct obj *obj, boolean force_id, char *buf, size_t size)
{
    int first, second, quality;
    size_t len;
    if (!size) return;
    *buf = '\0';
    if (!enhancement_eligible(obj)) return;
    enhancement_names(obj, force_id, &first, &second);
    quality = force_id || (obj->o_enh_flags & OEF_QUALITY_KNOWN) ? obj->o_enh_quality : 0;
    if (quality == OQ_FINE || quality == OQ_EXCEPTIONAL)
        Snprintf(buf, size, "%s", quality == OQ_FINE ? "fine " : "exceptional ");
    len = strlen(buf);
    if (first >= 0 && len < size - 1)
        Snprintf(buf + len, size - len, "%s ", enhancement_catalog[first].prefix);
}

void
enhancement_suffix(const struct obj *obj, boolean force_id, char *buf, size_t size)
{
    int first, second;
    if (!size) return;
    *buf = '\0';
    enhancement_names(obj, force_id, &first, &second);
    if (second >= 0)
        Snprintf(buf, size, " %s", enhancement_catalog[second].suffix);
}

static const struct enhancement_band enhancement_bands[5] = {
    { 6, 65, 30, 50, 15, { 70, 15, 10, 5 } },
    { 10, 50, 40, 60, 20, { 55, 20, 15, 10 } },
    { 16, 40, 40, 70, 30, { 40, 25, 20, 15 } },
    { 22, 30, 40, 80, 40, { 25, 30, 25, 20 } },
    { 28, 20, 40, 90, 50, { 10, 35, 30, 25 } }
};

const struct enhancement_band *
enhancement_depth_band(int dep)
{
    dep = max(1, min(199, dep));
    return &enhancement_bands[dep < 30 ? 0 : dep < 60 ? 1 : dep < 100 ? 2
                              : dep < 150 ? 3 : 4];
}

void
enhancement_generate(struct obj *obj, int dep)
{
    const struct enhancement_band *band = enhancement_depth_band(dep);
    int quality, presence, roll, slots, slot, tier, i, count, candidates[24];
    uint32 props = 0;
    if (!enhancement_eligible(obj) || rn2(100) >= band->gate)
        return;
    do {
        roll = rn2(100);
        quality = roll < band->standard ? OQ_STANDARD
            : roll < band->standard + band->fine ? OQ_FINE : OQ_EXCEPTIONAL;
        presence = rn2(100) < band->presence;
    } while (quality == OQ_STANDARD && !presence);
    slots = presence ? (rn2(100) < band->two ? 2 : 1) : 0;
    for (slot = 0; slot < slots; ++slot) {
        roll = rn2(100);
        for (tier = 1; tier < 4 && roll >= band->tier[tier - 1]; ++tier)
            roll -= band->tier[tier - 1];
        for (; tier > 0; --tier) {
            count = 0;
            for (i = 0; i < SIZE(enhancement_catalog); ++i)
                if (enhancement_catalog[i].tier == tier
                    && !(props & enhancement_catalog[i].bit)
                    && enhancement_property_allowed(obj, props | enhancement_catalog[i].bit))
                    candidates[count++] = i;
            if (count) {
                props |= enhancement_catalog[candidates[rn2(count)]].bit;
                break;
            }
        }
        if (!tier)
            impossible("Natural enhancement exhausted tier one");
    }
    (void) enhancement_set(obj, props, (enum enhancement_quality) quality, FALSE);
}

/* Synchronous creation scope, default-deny. Only original constructors call
 * enhancement_created, never movement, restore, split, or ownership code. */
static enum enhancement_context creation_context = ENH_CONTEXT_NONE;

enum enhancement_context
enhancement_context_set(enum enhancement_context context)
{
    enum enhancement_context old = creation_context;
    creation_context = context;
    return old;
}

void
enhancement_created(struct obj *obj)
{
    if (creation_context != ENH_CONTEXT_NONE)
        enhancement_generate(obj, depth(&u.uz));
}

struct obj *
enhancement_mkobj(int oclass, boolean artif)
{
    enum enhancement_context old = enhancement_context_set(ENH_CONTEXT_FLOOR);
    struct obj *obj = mkobj(oclass, artif);
    (void) enhancement_context_set(old);
    return obj;
}

struct obj *
enhancement_mkobj_at(char oclass, coordxy x, coordxy y, boolean artif)
{
    enum enhancement_context old = enhancement_context_set(ENH_CONTEXT_FLOOR);
    struct obj *obj = mkobj_at(oclass, x, y, artif);
    (void) enhancement_context_set(old);
    return obj;
}

struct obj *
enhancement_mksobj_at(int otyp, coordxy x, coordxy y, boolean init, boolean artif)
{
    enum enhancement_context old = enhancement_context_set(ENH_CONTEXT_SHOP);
    struct obj *obj = mksobj_at(otyp, x, y, init, artif);
    (void) enhancement_context_set(old);
    return obj;
}

struct monst *
enhancement_makemon(struct permonst *ptr, coordxy x, coordxy y, mmflags_nht flags)
{
    return makemon(ptr, x, y, flags | MM_NATURAL);
}
