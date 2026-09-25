/* Shared generic equipment mechanics and natural-generation policy. */
#include "hack.h"

staticfn uint64 enhancement_allowed(const struct obj *);
staticfn uint64 enhancement_active(const struct obj *);
staticfn uint64 socket_bits(const struct obj *);
staticfn const struct obj *enhancement_launcher(const struct obj *,
                                               const struct obj *,
                                               enum enhance_use);

/* Catalog order is the canonical same-tier naming priority. Element values
 * are native resistance properties; Primordial uses all three components. */
const struct enhancement_entry enhancement_catalog[59] = {
    { OEP_FIRE, 1, 0, FIRE_RES, 1, 4, "Smoldering", "of Embers" },
    { OEP_COLD, 1, 0, COLD_RES, 1, 4, "Chilled", "of Rime" },
    { OEP_SHOCK, 1, 0, SHOCK_RES, 1, 4, "Sparking", "of Static" },
    { OEP_TRUEFLIGHT, 1, 0, 0, 0, 0, "Trueflight", "of Trueflight" },
    { OEP_FIRE_II, 2, 0, FIRE_RES, 3, 4, "Blazing", "of the Inferno" },
    { OEP_COLD_II, 2, 0, COLD_RES, 3, 4, "Glacial", "of the Blizzard" },
    { OEP_SHOCK_II, 2, 0, SHOCK_RES, 3, 4, "Thunderous", "of the Tempest" },
    { OEP_FIRE_III, 3, 0, FIRE_RES, 5, 6, "Cataclysmic", "of Hellfire" },
    { OEP_COLD_III, 3, 0, COLD_RES, 5, 6, "Stygian", "of Absolute Zero" },
    { OEP_SHOCK_III, 3, 0, SHOCK_RES, 5, 6, "Voltaic", "of Heaven's Wrath" },
    { OEP_PRIMORDIAL, 4, 0, -1, 5, 6, "Primordial", "of the Elements" },
    { OEP_SEARCHING, 1, SEARCHING, 0, 0, 0, "Keen", "of Searching" },
    { OEP_WARNING, 1, WARNING, 0, 0, 0, "Watchful", "of Warning" },
    { OEP_STEALTH, 1, STEALTH, 0, 0, 0, "Silent", "of Stealth" },
    { OEP_FIRE_RES, 2, FIRE_RES, 0, 0, 0, "Emberward", "of Fire Resistance" },
    { OEP_COLD_RES, 2, COLD_RES, 0, 0, 0, "Frostward", "of Cold Resistance" },
    { OEP_SHOCK_RES, 2, SHOCK_RES, 0, 0, 0, "Stormward", "of Shock Resistance" },
    { OEP_POISON_RES, 2, POISON_RES, 0, 0, 0, "Venomward", "of Poison Resistance" },
    { OEP_SPEED, 3, FAST, 0, 0, 0, "Swift", "of Speed" },
    { OEP_REGEN, 3, REGENERATION, 0, 0, 0, "Renewing", "of Regeneration" },
    { OEP_DISPLACED, 3, DISPLACED, 0, 0, 0, "Shifting", "of Displacement" },
    { OEP_SLOW_DIGEST, 3, SLOW_DIGESTION, 0, 0, 0, "Sustaining", "of Slow Digestion" },
    { OEP_MAGIC_RES, 4, ANTIMAGIC, 0, 0, 0, "Arcane", "of Magic Resistance" },
    { OEP_REFLECTION, 4, REFLECTING, 0, 0, 0, "Mirrored", "of Reflection" },
    { OEP_STR_I, 1, 0, 0, 1, 2, "Strong", "of Strength", ES_STR },
    { OEP_STR_II, 2, 0, 0, 1, 3, "Mighty", "of Might", ES_STR },
    { OEP_STR_III, 3, 0, 0, 2, 2, "Titanic", "of Power", ES_STR },
    { OEP_STR_IV, 4, 0, 0, 2, 3, "Herculean", "of Giants", ES_STR },
    { OEP_DEX_I, 1, 0, 0, 1, 2, "Nimble", "of Dexterity", ES_DEX },
    { OEP_DEX_II, 2, 0, 0, 1, 3, "Deft", "of Agility", ES_DEX },
    { OEP_DEX_III, 3, 0, 0, 2, 2, "Precise", "of Precision", ES_DEX },
    { OEP_DEX_IV, 4, 0, 0, 2, 3, "Peerless", "of Mastery", ES_DEX },
    { OEP_VAMPIRIC_I, 1, 0, 0, 0, 0, "Bloodkissed", "of the Bloodkiss" },
    { OEP_VAMPIRIC_II, 2, 0, 0, 0, 0, "Leeching", "of the Leech" },
    { OEP_VAMPIRIC_III, 3, 0, 0, 0, 0, "Crimson", "of the Rose" },
    { OEP_VAMPIRIC_IV, 4, 0, 0, 0, 0, "Dread-Vampire's", "of Exsanguination" },
    { OEP_ACID_I, 1, 0, ACID_RES, 1, 4, "Corrosive", "of Corrosion" },
    { OEP_ACID_II, 2, 0, ACID_RES, 3, 4, "Caustic", "of Dissolution" },
    { OEP_ACID_III, 3, 0, ACID_RES, 5, 6, "Flesh-Eating", "of the Crucible" },
    { OEP_ANARCHIC_I, 1, 0, 0, 2, 4, "Chaotic", "of Discord" },
    { OEP_ANARCHIC_II, 2, 0, 0, 5, 4, "Brutal", "of Anarchy" },
    { OEP_AXIOMATIC_I, 1, 0, 0, 2, 4, "Righteous", "of Order" },
    { OEP_AXIOMATIC_II, 2, 0, 0, 5, 4, "Immutable", "of the Arbiter" },
    { OEP_STONING_I, 1, 0, 0, 0, 0, "Calcifying", "of Calcification" },
    { OEP_STONING_II, 2, 0, 0, 0, 0, "Petrifying", "of Petrification" },
    { OEP_STONING_III, 3, 0, 0, 0, 0, "Gorgon's", "of the Gorgon" },
    { OEP_STONING_IV, 4, 0, 0, 0, 0, "Medusa's", "of Medusa" },
    { OEP_WARDING, 3, 0, 0, 0, 0, "Sanctified", "of Warding" },
    { OEP_CASTING_II, 2, 0, 0, 0, 0, "Mystic", "of Casting" },
    { OEP_CASTING_IV, 4, 0, 0, 0, 0, "Sorcerous", "of the Archmage" },
    { OEP_LIGHTNESS_I, 1, 0, 0, 0, 0, "Lightweight", "of Lightness" },
    { OEP_LIGHTNESS_II, 2, 0, 0, 0, 0, "Feathered", "of Featherweight" },
    { OEP_LIGHTNESS_III, 3, 0, 0, 0, 0, "Weightless", "of Weightlessness" },
    { OEP_THORNS_I, 1, 0, 0, 0, 0, "Barbed", "of Thorns" },
    { OEP_THORNS_II, 2, 0, 0, 0, 0, "Spiked", "of Retribution" },
    { OEP_DR_I, 1, 0, 0, 0, 0, "Hardened", "of Resilience" },
    { OEP_DR_II, 2, 0, 0, 0, 0, "Stalwart", "of Preservation" },
    { OEP_DR_III, 3, 0, 0, 0, 0, "Aegis-Bound", "of Aegis" },
    { OEP_DR_IV, 4, 0, 0, 0, 0, "Bulwarked", "of Safeguarding" }
};

/* Socket-only entries share the ordinary engine's property descriptors.
 * IDs 1..32 always refer directly to enhancement_catalog. */
static const struct enhancement_entry socket_extra[] = {
    { 0, 1, 0, 0, 1, 4, "Max HP I", "", ES_HP },
    { 0, 3, 0, 0, 5, 4, "Max HP III", "", ES_HP },
    { 0, 4, 0, 0, 8, 4, "Max HP IV", "", ES_HP },
    { 0, 1, 0, 0, 1, 2, "Protection I", "", ES_PROTECTION },
    { 0, 3, 0, 0, 2, 3, "Protection III", "", ES_PROTECTION },
    { 0, 4, 0, 0, 4, 2, "Protection IV", "", ES_PROTECTION },
    { 0, 1, 0, 0, 1, 2, "CON I", "", ES_CON },
    { 0, 2, 0, 0, 1, 3, "CON II", "", ES_CON },
    { 0, 3, 0, 0, 2, 2, "CON III", "", ES_CON },
    { 0, 4, 0, 0, 2, 3, "CON IV", "", ES_CON },
    { 0, 1, 0, 0, 2, 4, "Mana I", "", ES_MANA },
    { 0, 2, 0, 0, 4, 4, "Mana II", "", ES_MANA },
    { 0, 3, 0, 0, 7, 4, "Mana III", "", ES_MANA },
    { 0, 4, 0, 0, 9, 6, "Mana IV", "", ES_MANA },
    { 0, 1, 0, 0, 1, 2, "INT I", "", ES_INT },
    { 0, 2, 0, 0, 1, 3, "INT II", "", ES_INT },
    { 0, 3, 0, 0, 2, 2, "INT III", "", ES_INT },
    { 0, 4, 0, 0, 2, 3, "INT IV", "", ES_INT },
    { 0, 1, 0, 0, 1, 2, "WIS I", "", ES_WIS },
    { 0, 2, 0, 0, 1, 3, "WIS II", "", ES_WIS },
    { 0, 3, 0, 0, 2, 2, "WIS III", "", ES_WIS },
    { 0, 4, 0, 0, 2, 3, "WIS IV", "", ES_WIS },
    { 0, 1, 0, 0, 1, 2, "CHA I", "", ES_CHA },
    { 0, 2, 0, 0, 1, 3, "CHA II", "", ES_CHA },
    { 0, 3, 0, 0, 2, 2, "CHA III", "", ES_CHA },
    { 0, 4, 0, 0, 2, 3, "CHA IV", "", ES_CHA },
    { 0, 1, SLEEP_RES, 0, 0, 0, "Sleep Resistance", "", ES_NONE },
    { 0, 2, SEE_INVIS, 0, 0, 0, "See Invisible", "", ES_NONE },
    { 0, 2, JUMPING, 0, 0, 0, "Jumping", "", ES_NONE },
    { 0, 2, MAGICAL_BREATHING, 0, 0, 0, "Magical Breathing", "", ES_NONE },
    { 0, 3, TELEPAT, 0, 0, 0, "Telepathy", "", ES_NONE },
    { 0, 4, TELEPORT_CONTROL, 0, 0, 0, "Teleport Control", "", ES_NONE },
    { 0, 4, POLYMORPH_CONTROL, 0, 0, 0, "Polymorph Control", "", ES_NONE },
    { 0, 4, FLYING, 0, 0, 0, "Flying", "", ES_NONE }
};

const struct enhancement_entry *
equipment_property(int id)
{
    if (id <= EP_NONE || id >= EP_COUNT) return 0;
    if (id >= EP_VAMPIRIC_I) return &enhancement_catalog[32 + id - EP_VAMPIRIC_I];
    return id <= 32 ? &enhancement_catalog[id - 1] : &socket_extra[id - 33];
}

const char *
equipment_property_name(int id)
{
    static const char *const names[32] = {
        "Fire I", "Cold I", "Shock I", "Trueflight",
        "Fire II", "Cold II", "Shock II", "Fire III", "Cold III", "Shock III",
        "Primordial", "Searching", "Warning", "Stealth", "Fire Resistance",
        "Cold Resistance", "Shock Resistance", "Poison Resistance", "Very Fast",
        "Regeneration", "Displacement", "Slow Digestion", "Magic Resistance",
        "Reflection", "Strength I", "Strength II", "Strength III", "Strength IV",
        "Dexterity I", "Dexterity II", "Dexterity III", "Dexterity IV"
    };
    static const char *const offensive_names[] = {
        "Vampiric I", "Vampiric II", "Vampiric III", "Vampiric IV", "Acid I", "Acid II", "Acid III", "Anarchic I", "Anarchic II", "Axiomatic I", "Axiomatic II", "Stoning I", "Stoning II", "Stoning III", "Stoning IV"
    };
    const struct enhancement_entry *e = equipment_property(id);
    if (id >= EP_WARDING && id < EP_COUNT) return e->prefix;
    if (id >= EP_VAMPIRIC_I && id < EP_WARDING) return offensive_names[id - EP_VAMPIRIC_I];
    return !e ? "empty" : id <= 32 ? names[id - 1] : e->prefix;
}

int
socket_capacity(const struct obj *obj)
{
    int cls;
    if (!obj || obj->oartifact) return 0;
    cls = objects[obj->otyp].oc_class;
    if (cls == WEAPON_CLASS) {
        if (is_ammo(obj) || is_missile(obj)) return 0;
        return is_launcher(obj) || bimanual(obj) ? 2 : 1;
    }
    if (cls == ARMOR_CLASS) return is_helmet(obj) || is_suit(obj) ? 2 : 1;
    return cls == RING_CLASS || cls == AMULET_CLASS ? 1 : 0;
}

void
socket_init(struct obj *obj)
{
    obj->o_socket_capacity = (uint8) socket_capacity(obj);
    memset(obj->o_sockets, 0, sizeof obj->o_sockets);
}

int
socket_count(const struct obj *obj)
{
    int i, count = 0;
    if (obj) for (i = 0; i < min(obj->o_socket_capacity, 2); ++i)
        count += obj->o_sockets[i].property != EP_NONE;
    return count;
}

int
socket_gem_tier(int typ)
{
    switch (typ) {
    case DILITHIUM_CRYSTAL: case DIAMOND: return 4;
    case RUBY: case JACINTH: case SAPPHIRE: return 3;
    case BLACK_OPAL: case EMERALD: case TURQUOISE: case CITRINE:
    case AQUAMARINE: case AMBER: case TOPAZ: return 2;
    case JET: case OPAL: case CHRYSOBERYL: case GARNET: case AMETHYST:
    case JASPER: case FLUORITE: case JADE: case OBSIDIAN: case AGATE: return 1;
    default: return 0;
    }
}

staticfn boolean
socket_allowed(const struct obj *obj, int id)
{
    const struct enhancement_entry *e = equipment_property(id);
    int cls = objects[obj->otyp].oc_class;
    if (!socket_capacity(obj) || !e
        || (e->native_property && enhancement_native_property(obj, e->native_property))
        || (e->stat == ES_PROTECTION && enhancement_native_property(obj, PROTECTION))
        || (e->stat == ES_CHA && enhancement_native_property(obj, ADORNED)))
        return FALSE;
    if (cls == WEAPON_CLASS)
        return (id <= EP_PRIMORDIAL && (id != EP_TRUEFLIGHT || is_launcher(obj)))
               || (id >= EP_STR_I && id <= EP_DEX_IV)
               || (e->bit & (OEP_ACID | OEP_ALIGNMENT));
    if (cls == ARMOR_CLASS)
        return id == EP_STEALTH || id == EP_WARNING
            || (id >= EP_FIRE_RES && id <= EP_POISON_RES)
            || id == EP_DISPLACED || id == EP_MAGIC_RES
            || (id >= EP_HP_I && id <= EP_CON_IV);
    return id == EP_STEALTH || id == EP_SEARCHING || id == EP_REGEN
        || id == EP_SPEED || id == EP_REFLECTION || (id >= EP_MANA_I && id <= EP_FLYING);
}

int
socket_candidates(const struct obj *obj, int tier, int removed, int *out)
{
    int id, i, n = 0;
    for (id = 1; id < EP_COUNT; ++id) {
        const struct enhancement_entry *e = equipment_property(id);
        if (e->tier != tier || !socket_allowed(obj, id)
            || (e->bit && (obj->o_enh_props & e->bit))) continue;
        for (i = 0; i < min(obj->o_socket_capacity, 2); ++i)
            if (i != removed && obj->o_sockets[i].property == id) break;
        if (i < min(obj->o_socket_capacity, 2)) continue;
        if (e->bit & OEP_ALIGNMENT) {
            if (obj->o_enh_props & OEP_ALIGNMENT) continue;
            for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
                const struct enhancement_entry *other = equipment_property(obj->o_sockets[i].property);
                if (i != removed && other && (other->bit & OEP_ALIGNMENT)) break;
            }
            if (i < min(obj->o_socket_capacity, 2)) continue;
        }
        if (out) out[n] = id;
        ++n;
    }
    return n;
}

/* Restore/sanity normalization is deterministic and has no gameplay callbacks. */
void
socket_normalize(struct obj *obj)
{
    int i, cap = socket_capacity(obj);
    if (!enhancement_eligible(obj)) {
        obj->o_enh_props = obj->o_enh_known = 0;
        obj->o_stoning_remaining = 0;
        obj->o_stoning_turn = 0;
        obj->o_enh_quality = obj->o_enh_flags = 0;
    } else {
        struct obj ordinary = *obj;
        obj->o_enh_props &= enhancement_allowed(obj);
        memset(ordinary.o_sockets, 0, sizeof ordinary.o_sockets);
        if (!enhancement_property_allowed(&ordinary, obj->o_enh_props)) obj->o_enh_props = 0;
        if (obj->o_enh_quality > OQ_EXCEPTIONAL) obj->o_enh_quality = OQ_STANDARD;
        obj->o_enh_flags &= OEF_QUALITY_KNOWN;
    }
    if (!(obj->o_enh_props & OEP_STONING)) {
        obj->o_stoning_remaining = 0;
        obj->o_stoning_turn = 0;
    }
    if (obj->o_socket_capacity > cap) obj->o_socket_capacity = (uint8) cap;
    for (i = 0; i < 2; ++i) {
        struct item_socket *s = &obj->o_sockets[i];
        const struct enhancement_entry *e = equipment_property(s->property);
        if (i >= obj->o_socket_capacity || !e || !socket_allowed(obj, s->property)
            || (e->bit && (obj->o_enh_props & e->bit))
            || ((e->bit & OEP_ALIGNMENT) && (obj->o_enh_props & OEP_ALIGNMENT))
            || (i && equipment_property(obj->o_sockets[0].property)
                && (e->bit & OEP_ALIGNMENT)
                && (equipment_property(obj->o_sockets[0].property)->bit & OEP_ALIGNMENT))
            || (i && obj->o_sockets[0].property == s->property)
            || (e->stat && (s->value < e->dice || s->value > e->dice * e->sides)))
            memset(s, 0, sizeof *s);
        else {
            s->known = !!s->known;
            if (!e->stat) s->value = 0;
        }
    }
    for (i = 0; i < 8; ++i) {
        const struct enhancement_entry *e = &enhancement_catalog[24 + i];
        if (!(obj->o_enh_props & e->bit)) obj->o_enh_values[i] = 0;
        else if (obj->o_enh_values[i] < e->dice
                 || obj->o_enh_values[i] > e->dice * e->sides) {
            obj->o_enh_props &= ~e->bit;
            obj->o_enh_values[i] = 0;
        }
    }
}

void
socket_label(const struct obj *obj, int slot, char *buf, size_t size)
{
    const struct item_socket *s = &obj->o_sockets[slot];
    const struct enhancement_entry *e = equipment_property(s->property);
    if (!e || !s->known)
        Snprintf(buf, size, "%s", e ? "unknown" : "empty");
    else if (e->stat)
        Snprintf(buf, size, "%s +%u", equipment_property_name(s->property), s->value);
    else Snprintf(buf, size, "%s", equipment_property_name(s->property));
}

staticfn uint64
socket_bits(const struct obj *obj)
{
    uint64 bits = 0;
    int i;
    if (obj && socket_capacity(obj))
        for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
            const struct enhancement_entry *e = equipment_property(obj->o_sockets[i].property);
            if (e) bits |= e->bit;
        }
    return bits;
}

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

staticfn uint64
enhancement_allowed(const struct obj *obj)
{
    if (!enhancement_eligible(obj))
        return 0;
    if (obj->oclass == ARMOR_CLASS) {
        uint64 allowed = OEP_WORN | OEP_WARDING | OEP_DR;
        if (armor_spell_penalty(obj)) allowed |= OEP_CASTING;
        if (objects[obj->otyp].oc_weight >= 100) allowed |= OEP_LIGHTNESS;
        if (is_suit(obj) || is_shield(obj) || is_helmet(obj)
            || is_gloves(obj) || is_boots(obj)) allowed |= OEP_THORNS;
        int i;
        for (i = 0; i < SIZE(enhancement_catalog); ++i)
            if (enhancement_catalog[i].native_property
                && enhancement_native_property(obj, enhancement_catalog[i].native_property))
                allowed &= ~enhancement_catalog[i].bit;
        return allowed;
    }
    return OEP_ELEMENTS | OEP_ATTRIBUTES | OEP_VAMPIRIC
           | (!is_ammo(obj) ? OEP_ALIGNMENT : 0)
           | (!is_ammo(obj) && obj->quan == 1L ? OEP_STONING : 0)
           | ((is_launcher(obj) || is_ammo(obj) || is_missile(obj)
               || is_spear(obj))
                  ? OEP_TRUEFLIGHT : 0);
}

boolean
enhancement_property_allowed(const struct obj *obj, uint64 props)
{
    uint64 rest = props & (props - 1U);
    uint64 families[] = { OEP_VAMPIRIC, OEP_ALIGNMENT, OEP_STONING,
                          OEP_CASTING, OEP_LIGHTNESS, OEP_THORNS, OEP_DR };
    int i;
    for (i = 0; i < SIZE(families); ++i) {
        uint64 bits = props & families[i];
        if (bits & (bits - 1ULL)) return FALSE;
        if (bits && (socket_bits(obj) & families[i])) return FALSE;
    }
    return enhancement_eligible(obj) && !(rest & (rest - 1U))
           && !(props & ~enhancement_allowed(obj));
}

staticfn uint64
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
    uint64 bits = enhancement_active(obj);
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (enhancement_catalog[i].native_property == prop
            && (bits & enhancement_catalog[i].bit))
            return TRUE;
    if (obj && socket_capacity(obj))
        for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
            const struct enhancement_entry *e = equipment_property(obj->o_sockets[i].property);
            if (e && e->native_property == prop) return TRUE;
        }
    return FALSE;
}

boolean
enhancement_mon_confers(const struct monst *mon, int prop)
{
    const struct obj *obj;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if ((obj->owornmask & (W_ARMOR | W_RING | W_AMUL)) && enhancement_confers(obj, prop))
            return TRUE;
    return FALSE;
}

boolean
enhancement_elemental_contact(const struct obj *obj, const struct obj *launcher,
                              enum enhance_use use)
{
    return ((enhancement_active(obj) | socket_bits(obj)
             | enhancement_active(enhancement_launcher(obj, launcher, use))
             | socket_bits(enhancement_launcher(obj, launcher, use)))
            & (OEP_ELEMENTS | OEP_STONING)) != 0;
}

/* A native observable event can identify a single attributable source.
 * Redundant intrinsic, native, or enhanced sources cannot identify each other. */
void
enhancement_observe_worn(int prop)
{
    struct obj *obj, *source = 0;
    int i;
    long sources = u.uprops[prop].extrinsic;
    if (u.uprops[prop].intrinsic || u.uprops[prop].blocked
        || (prop == DISPLACED && u.mith_timers[MITH_VAUL]))
        return;
    for (obj = gi.invent; obj; obj = obj->nobj)
        if ((obj->owornmask & (W_ARMOR | W_RING | W_AMUL)) && enhancement_confers(obj, prop)) {
            if (source) return;
            source = obj;
        }
    if (!source || (sources & ~source->owornmask)
        || enhancement_native_property(source, prop)) return;
    for (i = 0; i < min(source->o_socket_capacity, 2); ++i) {
        const struct enhancement_entry *e = equipment_property(source->o_sockets[i].property);
        if (e && e->native_property == prop) source->o_sockets[i].known = 1;
    }
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
    mask = obj->owornmask & (W_ARMOR | W_RING | W_AMUL);
    for (i = 1; i <= LAST_PROP; ++i) {
        prop = i;
        if (enhancement_confers(obj, prop) && !enhancement_native_property(obj, prop)) {
            u.uprops[prop].extrinsic &= ~mask;
            if (mask) monstunseesu_prop(prop);
        }
    }
}

void
enhancement_worn_on(struct obj *obj, struct monst *wearer)
{
    int i, prop;
    long mask;
    boolean warning = Warning, searching = Searching, stealth = Stealth;
    if (!obj || wearer != &gy.youmonst)
        return;
    mask = obj->owornmask & (W_ARMOR | W_RING | W_AMUL);
    if (mask & W_ARMOR) obj->o_enh_known |= enhancement_active(obj) & OEP_CASTING;
    for (i = 1; i <= LAST_PROP; ++i) {
        prop = i;
        if (enhancement_confers(obj, prop)) u.uprops[prop].extrinsic |= mask;
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
    obj->owt = weight(obj);
    if (obj->where == OBJ_CONTAINED) container_weight(obj->ocontainer);
    if (obj->unpaid)
        enhancement_rebill(obj);
    if (carried(obj)) {
        enhancement_worn_on(obj, &gy.youmonst);
        if (obj->owornmask & W_ARMOR)
            find_ac();
        equipment_refresh();
        update_inventory();
    }
}

void
enhancement_clear(struct obj *obj)
{
    if (!obj)
        return;
    if (carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = obj->o_enh_known = 0;
    obj->o_stoning_remaining = 0;
    obj->o_stoning_turn = 0;
    obj->o_enh_quality = obj->o_enh_flags = 0;
    memset(obj->o_enh_values, 0, sizeof obj->o_enh_values);
    enhancement_changed(obj);
}

/* Identity-changing conversions discard generic state; constructors still
 * zero the whole object. Remove old worn effects before changing identity. */
void
enhancement_change_type(struct obj *obj, int otyp)
{
    if (obj->otyp == otyp) return;
    if (carried(obj)) enhancement_worn_off(obj, &gy.youmonst);
    obj->o_enh_props = obj->o_enh_known = 0;
    obj->o_stoning_remaining = 0;
    obj->o_stoning_turn = 0;
    obj->o_enh_quality = obj->o_enh_flags = 0;
    memset(obj->o_enh_values, 0, sizeof obj->o_enh_values);
    obj->otyp = otyp;
    socket_init(obj);
    enhancement_changed(obj);
}

void
enhancement_strip_for_artifact(struct obj *obj)
{
    if (obj && obj->oartifact) {
        /* Callers have already set the artifact identity. Remove the former
         * ordinary item's worn properties before its eligibility disappears. */
        if (carried(obj)) {
            struct obj previous = *obj;
            previous.oartifact = 0;
            enhancement_worn_off(&previous, &gy.youmonst);
        }
        socket_init(obj);
        enhancement_clear(obj);
    }
}

void
enhancement_normalize(struct obj *obj)
{
    if (!obj) return;
    if (carried(obj)) enhancement_worn_off(obj, &gy.youmonst);
    socket_normalize(obj);
    obj->o_enh_known &= OEP_ALL;
    enhancement_changed(obj);
}

boolean
enhancement_set(struct obj *obj, uint64 props,
                enum enhancement_quality quality, boolean known)
{
    if (!enhancement_property_allowed(obj, props)
        || quality < OQ_STANDARD || quality > OQ_EXCEPTIONAL)
        return FALSE;
    if (carried(obj))
        enhancement_worn_off(obj, &gy.youmonst);
    /* Retained identities preserve acquired values; only new identities roll. */
    {
        int i;
        for (i = 0; i < 8; ++i) {
            const struct enhancement_entry *e = &enhancement_catalog[24 + i];
            if (!(props & e->bit))
                obj->o_enh_values[i] = 0;
            else if (!(obj->o_enh_props & e->bit))
                obj->o_enh_values[i] = (uint8) d(e->dice, e->sides);
        }
    }
    if ((obj->o_enh_props & OEP_STONING) != (props & OEP_STONING)) {
        obj->o_stoning_remaining = 0;
        obj->o_stoning_turn = 0;
    }
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
    int i;
    for (i = 0; i < min(obj->o_socket_capacity, 2); ++i)
        if (obj->o_sockets[i].property) obj->o_sockets[i].known = 1;
    if (enhancement_eligible(obj)) {
        obj->o_enh_known = enhancement_allowed(obj);
        obj->o_enh_flags |= OEF_QUALITY_KNOWN;
    }
}

uint64
enhancement_visible_props(const struct obj *obj, boolean force_id)
{
    return enhancement_active(obj) & (force_id ? OEP_ALL
                                              : obj ? obj->o_enh_known : 0);
}

staticfn boolean
enhancement_knowledge_complete(const struct obj *obj)
{
    return enhancement_eligible(obj)
           && obj->o_enh_known == enhancement_allowed(obj);
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
    uint64 props = enhancement_active(obj) | socket_bits(obj);
    int bonus;

    launcher = enhancement_launcher(obj, launcher, use);
    bonus = enhancement_quality_bonus(launcher ? launcher : obj, use);
    if (use == ENHANCE_ARMOR || (obj && obj->oclass == ARMOR_CLASS))
        return 0;
    props |= enhancement_active(launcher) | socket_bits(launcher);
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
        : element == ACID_RES ? (hero ? Acid_resistance : resists_acid(target))
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
        uint64 bits = enhancement_active(sources[source]) | socket_bits(sources[source]);
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
                        element == FIRE_RES ? "fire" : element == COLD_RES ? "frost" : element == ACID_RES ? "acid" : "lightning");
            }
        }
    }
    return extra;
}

/* Visible elements, healing and petrification do not determine exact tiers.
 * Explicit identification remains the sole source of knowledge for these. */
void
enhancement_observe_hit(struct obj *obj UNUSED, struct obj *launcher UNUSED,
                        struct monst *target UNUSED, enum enhance_use use UNUSED)
{
}

/* Physical dice enter the caller's physical channel before its mitigation. */
int
enhancement_alignment_damage(const struct obj *obj, const struct obj *launcher,
                             const struct monst *target, enum enhance_use use)
{
    const struct obj *sources[2];
    int alignment, source, i, damage = 0;
    if (!obj || !target || use == ENHANCE_ARMOR) return 0;
    /* Native contact immunities apply to this physical component too. */
    if ((shadelike(target->data) && !shade_glare((struct obj *) obj))
        || (thick_skinned(target->data) && obj_material(obj) <= LEATHER))
        return 0;
    alignment = target == &gy.youmonst ? u.ualign.type
                                     : mon_aligntyp((struct monst *) target);
    if (alignment == A_NONE) return 0;
    sources[0] = obj;
    sources[1] = enhancement_launcher(obj, launcher, use);
    for (source = 0; source < 2; ++source) {
        uint64 bits = enhancement_active(sources[source]) | socket_bits(sources[source]);
        for (i = 0; i < SIZE(enhancement_catalog); ++i) {
            const struct enhancement_entry *e = &enhancement_catalog[i];
            if (!(bits & e->bit & OEP_ALIGNMENT)) continue;
            if (((e->bit & (OEP_ANARCHIC_I | OEP_ANARCHIC_II))
                 && alignment != A_CHAOTIC)
                || ((e->bit & (OEP_AXIOMATIC_I | OEP_AXIOMATIC_II))
                    && alignment != A_LAWFUL))
                damage += d(e->dice, e->sides);
        }
    }
    return damage;
}

/* Call exactly once, with the final channels before remaining-HP clamping. */
void
enhancement_vampiric(const struct obj *obj, const struct obj *launcher,
                      struct monst *attacker, const struct monst *target,
                      int physical, int total, enum enhance_use use)
{
    static const int percent[] = { 0, 10, 20, 25, 30 };
    uint64 bits;
    int i, tier = 0, basis, healing;
    if (!attacker || !target
        || (attacker != &gy.youmonst && DEADMONSTER(attacker))
        || nonliving(target->data) || use == ENHANCE_ARMOR)
        return;
    bits = enhancement_active(obj)
           | enhancement_active(enhancement_launcher(obj, launcher, use));
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (bits & enhancement_catalog[i].bit & OEP_VAMPIRIC)
            tier = max(tier, enhancement_catalog[i].tier);
    if (!tier) return;
    basis = tier < 3 ? physical : total;
    if (basis <= 0) return;
    healing = max(1, (int) ((long) basis * percent[tier] / 100L));
    if (attacker == &gy.youmonst) healup(healing, 0, FALSE, FALSE);
    else if (!DEADMONSTER(attacker)) healmon(attacker, healing, 0);
}

staticfn int
stoning_cooldown(const struct obj *obj)
{
    static const int turns[] = { 75, 50, 25, 10 };
    int i;
    uint64 bits = enhancement_active(obj);
    for (i = 3; i >= 0; --i)
        if (bits & (OEP_STONING_I << i)) return turns[i];
    return 0;
}

/* Native contact petrification, with the physical source owning recharge.
 * The return value is death, so combat callers never kill a statue twice. */
boolean
enhancement_stoning(struct obj *obj, struct obj *launcher,
                     struct monst *attacker, struct monst *target,
                     enum enhance_use use)
{
    struct obj *sources[2];
    int i, cooldown;
    if (!target || use == ENHANCE_ARMOR) return FALSE;
    sources[0] = obj;
    sources[1] = (struct obj *) enhancement_launcher(obj, launcher, use);
    for (i = 0; i < 2; ++i) {
        struct obj *source = sources[i];
        if (!source || source->o_stoning_remaining
            || !(cooldown = stoning_cooldown(source))) continue;
        if (target == &gy.youmonst) {
            struct permonst *before = gy.youmonst.data;
            boolean started;
            if (!attacker || Stoned || Stone_resistance) continue;
            started = do_stone_u(attacker);
            if (!started && before == gy.youmonst.data) continue;
            source->o_stoning_remaining = cooldown;
            source->o_stoning_turn = svm.moves;
        } else {
            if (DEADMONSTER(target)) return TRUE;
            if (resists_ston(target)) continue;
            /* Native consumption of a cure is also a petrification consequence.
             * Both cure damage and petrification can destroy thrown objects. */
            source->o_stoning_remaining = cooldown;
            source->o_stoning_turn = svm.moves;
            if (munstone(target, attacker == &gy.youmonst))
                return DEADMONSTER(target);
            minstapetrify(target, attacker == &gy.youmonst);
            return DEADMONSTER(target);
        }
    }
    return FALSE;
}

staticfn void
stoning_tick_chain(struct obj *chain)
{
    struct obj *obj;
    for (obj = chain; obj; obj = obj->nobj) {
        if (obj->o_stoning_remaining && obj->o_stoning_turn != svm.moves)
            --obj->o_stoning_remaining;
        if (obj->cobj) stoning_tick_chain(obj->cobj);
    }
}

/* Once per normal turn, before moves advances. No inactive or migrating
 * chains are visited and restore never applies elapsed-time catch-up. */
void
enhancement_tick(void)
{
    struct monst *mon;
    stoning_tick_chain(gi.invent);
    stoning_tick_chain(fobj);
    stoning_tick_chain(svl.level.buriedobjlist);
    for (mon = fmon; mon; mon = mon->nmon)
        stoning_tick_chain(mon->minvent);
}

long
enhancement_price_adjustment(const struct obj *obj)
{
    uint64 props = enhancement_active(obj);
    int i;
    long percent = 100L;
    if (!obj) return percent;
    for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
        const struct enhancement_entry *e = equipment_property(obj->o_sockets[i].property);
        if (e) percent += 50L << (e->tier - 1);
    }
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
    if (!enhancement_eligible(obj) && !socket_capacity(obj))
        return base;
    return max(1L, (base / 100L) * percent + (base % 100L) * percent / 100L);
}

/* Stable tier-first order; equal tiers retain catalog priority. */
staticfn void
enhancement_names(const struct obj *obj, boolean force_id, int *first, int *second)
{
    int i;
    uint64 bits = enhancement_visible_props(obj, force_id);
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
        Snprintf(buf, size, "%s", quality == OQ_FINE ? "Fine " : "Exceptional ");
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

staticfn boolean
enhancement_worn_recipient(const struct obj *obj)
{
    return obj && (obj->oclass == ARMOR_CLASS || obj->oclass == RING_CLASS
                   || obj->oclass == AMULET_CLASS);
}

staticfn const char *
enhancement_stat_name(int stat)
{
    switch (stat) {
    case ES_STR: return "STR";
    case ES_DEX: return "DEX";
    case ES_CON: return "CON";
    case ES_INT: return "INT";
    case ES_WIS: return "WIS";
    case ES_CHA: return "CHA";
    default: return "";
    }
}

staticfn const char *
enhancement_element_name(int element)
{
    return element == FIRE_RES ? "fire"
        : element == COLD_RES ? "cold" : element == ACID_RES ? "acid" : "shock";
}

staticfn const char *
enhancement_capability_name(int prop)
{
    switch (prop) {
    case FIRE_RES: return "fire resistance";
    case COLD_RES: return "cold resistance";
    case SHOCK_RES: return "shock resistance";
    case POISON_RES: return "poison resistance";
    case SEARCHING: return "Searching";
    case WARNING: return "Warning";
    case STEALTH: return "Stealth";
    case FAST: return "very fast movement";
    case REGENERATION: return "HP regeneration";
    case DISPLACED: return "Displacement";
    case SLOW_DIGESTION: return "slow digestion";
    case ANTIMAGIC: return "magic resistance";
    case REFLECTING: return "Reflection";
    case SLEEP_RES: return "Sleep Resistance";
    case SEE_INVIS: return "See Invisible";
    case JUMPING: return "Jumping";
    case MAGICAL_BREATHING: return "Magical Breathing";
    case TELEPAT: return "Telepathy";
    case TELEPORT_CONTROL: return "Teleport Control";
    case POLYMORPH_CONTROL: return "Polymorph Control";
    case FLYING: return "Flying";
    default: return "";
    }
}

void
enhancement_quality_impact(const struct obj *obj, char *buf, size_t size)
{
    int quality;
    if (!size) return;
    quality = obj && obj->o_enh_quality <= OQ_EXCEPTIONAL
                  ? obj->o_enh_quality : OQ_STANDARD;
    if (!obj || quality == OQ_STANDARD) {
        Snprintf(buf, size, "no additional quality bonus");
    } else if (obj->oclass == ARMOR_CLASS) {
        Snprintf(buf, size, "+%d AC while worn", quality);
    } else if (is_launcher(obj)) {
        Snprintf(buf, size, "+%d to hit when firing ammunition", quality);
    } else if (is_ammo(obj)) {
        Snprintf(buf, size, "+%d physical damage when fired", quality);
    } else {
        Snprintf(buf, size,
                 "+%d to hit and +%d physical damage in melee or when directly thrown",
                 quality, quality);
    }
}

void
enhancement_property_impact(const struct obj *obj, int property, int value,
                            char *buf, size_t size)
{
    const struct enhancement_entry *e = equipment_property(property);
    const char *statname, *capability;
    boolean worn;
    if (!size) return;
    *buf = '\0';
    if (!obj || !e) return;
    worn = enhancement_worn_recipient(obj);
    if (e->bit & OEP_DEFENSIVE) {
        if (e->bit == OEP_WARDING)
            Snprintf(buf, size, "prevents curses on equipped items while worn");
        else if (e->bit & OEP_CASTING)
            Snprintf(buf, size, "reduces this item's casting penalties by %d%% while worn",
                     e->tier == 2 ? 50 : 100);
        else if (e->bit & OEP_LIGHTNESS)
            Snprintf(buf, size, "reduces effective object weight by %d%%", e->tier * 30);
        else if (e->bit & OEP_THORNS)
            Snprintf(buf, size, "reflects %d%% of HP loss to adjacent attackers while worn",
                     e->tier == 1 ? 30 : 50);
        else
            Snprintf(buf, size, "reduces %s current-HP damage by %d%% while worn",
                     e->tier < 3 ? "physical" : "all", e->tier * 10);
        return;
    }
    if (e->bit & OEP_STONING) {
        if (!obj->o_stoning_remaining) Snprintf(buf, size, "Ready");
        else Snprintf(buf, size, "%u turns remaining", obj->o_stoning_remaining);
        return;
    }
    if (e->bit & OEP_VAMPIRIC) {
        Snprintf(buf, size, "heals %d%% of direct %s damage against living targets",
                 e->tier == 1 ? 10 : e->tier == 2 ? 20 : e->tier == 3 ? 25 : 30,
                 e->tier < 3 ? "physical" : "total");
        return;
    }
    if (e->bit & OEP_ALIGNMENT) {
        Snprintf(buf, size, "+%dd%d physical damage against %s or Neutral targets",
                 e->dice, e->sides,
                 e->bit & (OEP_ANARCHIC_I | OEP_ANARCHIC_II) ? "Lawful" : "Chaotic");
        return;
    }
    if (e->element == -1) {
        Snprintf(buf, size,
                 "+%dd%d fire, +%dd%d cold, and +%dd%d shock damage on a confirmed hit",
                 e->dice, e->sides, e->dice, e->sides, e->dice, e->sides);
        return;
    }
    if (e->element) {
        Snprintf(buf, size, "+%dd%d %s damage on a confirmed hit", e->dice,
                 e->sides, enhancement_element_name(e->element));
        return;
    }
    if (e->stat == ES_PROTECTION) {
        Snprintf(buf, size, "improves AC by %d while worn", value);
        return;
    }
    if (e->stat == ES_HP) {
        Snprintf(buf, size, "+%d maximum HP while worn", value);
        return;
    }
    if (e->stat == ES_MANA) {
        Snprintf(buf, size, "+%d maximum Pw while worn", value);
        return;
    }
    statname = enhancement_stat_name(e->stat);
    if (*statname) {
        Snprintf(buf, size, "+%d %s while %s", value, statname,
                 worn ? "worn" : "wielded");
        return;
    }
    if (property == EP_TRUEFLIGHT) {
        Snprintf(buf, size, "%s", obj && is_launcher(obj)
                 ? "+2 to hit when firing ammunition"
                 : "+2 to hit when directly thrown or fired");
        return;
    }
    capability = enhancement_capability_name(e->native_property);
    if (*capability)
        Snprintf(buf, size, "grants %s while worn", capability);
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
    int quality, presence, roll, slots, slot, tier, i, count, candidates[SIZE(enhancement_catalog)];
    uint64 props = 0;
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
    socket_init(obj);
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

/* Numeric effects are queried from actual active state, never written to
 * permanent attributes and never identified by status recalculation. */
int
equipment_bonus(const struct obj *obj, int stat)
{
    int i, total = 0;
    uint64 bits = enhancement_active(obj);
    if (!obj) return 0;
    for (i = 0; i < 8; ++i) {
        const struct enhancement_entry *e = &enhancement_catalog[24 + i];
        if (e->stat == stat && (bits & e->bit))
            total += obj->o_enh_values[i];
    }
    if (socket_capacity(obj))
        for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
            const struct enhancement_entry *e = equipment_property(obj->o_sockets[i].property);
            if (e && e->stat == stat) total += obj->o_sockets[i].value;
        }
    return total;
}

int
equipment_hero_bonus(int stat)
{
    struct obj *obj;
    int total = 0;
    for (obj = gi.invent; obj; obj = obj->nobj)
        if ((obj->owornmask & (W_ARMOR | W_RING | W_AMUL))
            || (obj->oclass == WEAPON_CLASS
                && (obj == uwep || (u.twoweap && obj == uswapwep))))
            total += equipment_bonus(obj, stat);
    return total;
}

void
equipment_refresh(void)
{
    int hp = equipment_hero_bonus(ES_HP), pw = equipment_hero_bonus(ES_MANA);
    int mh = Upolyd ? hp : 0;
    if (program_state.restoring) return;
    if (hp != u.equipment_hp || pw != u.equipment_pw || mh != u.equipment_mh
        || u.uhpmax < hp + 1 || u.uenmax < pw
        || (Upolyd && u.mhmax < mh + 1)) {
        u.uhpmax = max(hp + 1, u.uhpmax + hp - u.equipment_hp);
        u.uenmax = max(pw, u.uenmax + pw - u.equipment_pw);
        if (Upolyd) u.mhmax = max(mh + 1, u.mhmax + mh - u.equipment_mh);
        u.equipment_hp = hp; u.equipment_pw = pw; u.equipment_mh = mh;
        u.uhp = min(u.uhp, u.uhpmax);
        u.uen = min(u.uen, u.uenmax);
        if (Upolyd) u.mh = min(u.mh, u.mhmax);
    }
    disp.botl = TRUE;
}

int
doinspect(void)
{
    struct obj *obj, *selected = 0;
    winid win;
    menu_item *picked = 0;
    anything any;
    char buf[BUFSZ], label[100], impact[BUFSZ];
    int i, count = 0;
    win = create_nhwindow(NHW_MENU); start_menu(win, MENU_BEHAVE_STANDARD);
    for (obj = gi.invent; obj; obj = obj->nobj) {
        struct obj copy = *obj;
        any = cg.zeroany; any.a_obj = obj;
        ++gd.distantname;
        Snprintf(buf, sizeof buf, "%s", doname(&copy));
        --gd.distantname;
        add_menu(win, &nul_glyphinfo, &any, obj->invlet, 0, ATR_NONE, NO_COLOR, buf, MENU_ITEMFLAGS_NONE);
    }
    end_menu(win, "What do you want to inspect?");
    if (select_menu(win, PICK_ONE, &picked) > 0) selected = picked[0].item.a_obj;
    if (picked) free((genericptr_t) picked);
    destroy_nhwindow(win);
    for (obj = gi.invent; obj && obj != selected; obj = obj->nobj) ;
    if (!obj) return ECMD_OK;
    win = create_nhwindow(NHW_TEXT);
    {
        struct obj copy = *obj;
        ++gd.distantname; Snprintf(buf, sizeof buf, "%s", doname(&copy)); --gd.distantname;
        putstr(win, ATR_NONE, buf);
    }
    putstr(win, ATR_NONE, "");
    putstr(win, ATR_NONE, "Quality:");
    if (!(obj->o_enh_flags & OEF_QUALITY_KNOWN)) {
        putstr(win, ATR_NONE, "    unknown");
    } else {
        const char *quality = obj->o_enh_quality == OQ_EXCEPTIONAL ? "Exceptional"
            : obj->o_enh_quality == OQ_FINE ? "Fine" : "Standard";
        enhancement_quality_impact(obj, impact, sizeof impact);
        Snprintf(buf, sizeof buf, "    %s - %s", quality, impact);
        putstr(win, ATR_NONE, buf);
    }
    putstr(win, ATR_NONE, "");
    putstr(win, ATR_NONE, "Enhancements:");
    for (i = 0; i < SIZE(enhancement_catalog); ++i)
        if (enhancement_visible_props(obj, FALSE) & enhancement_catalog[i].bit) {
            if (i >= 24 && i < 32) Snprintf(label, sizeof label, "%s +%u",
                equipment_property_name(i < 32 ? i + 1 : EP_VAMPIRIC_I + i - 32), obj->o_enh_values[i - 24]);
            else Snprintf(label, sizeof label, "%s", equipment_property_name(i < 32 ? i + 1 : EP_VAMPIRIC_I + i - 32));
            enhancement_property_impact(obj, i < 32 ? i + 1 : EP_VAMPIRIC_I + i - 32,
                                         i >= 24 && i < 32 ? obj->o_enh_values[i - 24] : 0,
                                         impact, sizeof impact);
            Snprintf(buf, sizeof buf, "    %s - %s", label, impact);
            putstr(win, ATR_NONE, buf); ++count;
        }
    if (!count)
        putstr(win, ATR_NONE,
               enhancement_knowledge_complete(obj) ? "    none" : "    none known");
    putstr(win, ATR_NONE, "");
    Snprintf(buf, sizeof buf, "Sockets: %d/%u", socket_count(obj), obj->o_socket_capacity);
    putstr(win, ATR_NONE, buf);
    for (i = 0; i < min(obj->o_socket_capacity, 2); ++i) {
        const struct item_socket *socket = &obj->o_sockets[i];
        const struct enhancement_entry *entry = equipment_property(socket->property);
        socket_label(obj, i, label, sizeof label);
        if (entry && socket->known) {
            enhancement_property_impact(obj, socket->property, socket->value,
                                         impact, sizeof impact);
            Snprintf(buf, sizeof buf, "    Socket %d: %s - %s", i + 1,
                     label, impact);
        } else {
            Snprintf(buf, sizeof buf, "    Socket %d: %s", i + 1, label);
        }
        putstr(win, ATR_NONE, buf);
    }
    display_nhwindow(win, TRUE); destroy_nhwindow(win);
    return ECMD_OK;
}

void
equipment_mon_refresh(struct monst *mon)
{
    struct obj *obj;
    int hp = 0;
    if (!mon || mon == &gy.youmonst) return;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if (obj->owornmask & (W_ARMOR | W_AMUL | W_RING))
            hp += equipment_bonus(obj, ES_HP);
    if (hp != mon->equipment_hp) {
        mon->mhpmax = max(1, mon->mhpmax + hp - mon->equipment_hp);
        mon->mhp = min(mon->mhp, mon->mhpmax);
        mon->equipment_hp = hp;
    }
}

/* Defensive effects query actual equipment, independently of knowledge. */
boolean
enhancement_curse_protected(const struct obj *obj)
{
    const struct obj *armor, *chain;
    if (!obj || !obj->owornmask) return FALSE;
    if (carried(obj)) chain = gi.invent;
    else if (mcarried(obj)) chain = obj->ocarry->minvent;
    else return FALSE;
    /* Quivered objects and an inactive alternate weapon are not equipped. */
    if (!(obj->owornmask & ~(W_QUIVER | W_SWAPWEP))
        && !(carried(obj) && u.twoweap && (obj->owornmask & W_SWAPWEP)))
        return FALSE;
    for (armor = chain; armor; armor = armor->nobj)
        if ((armor->owornmask & W_ARMOR)
            && (enhancement_active(armor) & OEP_WARDING)) return TRUE;
    return FALSE;
}

int
enhancement_casting_penalty(const struct obj *obj, int penalty)
{
    uint64 bits = enhancement_active(obj);
    if (!obj || !(obj->owornmask & W_ARMOR) || penalty <= 0) return penalty;
    return bits & OEP_CASTING_IV ? 0 : bits & OEP_CASTING_II ? penalty / 2 : penalty;
}

int
enhancement_weight(const struct obj *obj, int normal)
{
    uint64 bits = enhancement_active(obj);
    int percent = bits & OEP_LIGHTNESS_III ? 10 : bits & OEP_LIGHTNESS_II ? 40
                  : bits & OEP_LIGHTNESS_I ? 70 : 100;
    return (int) ((uint64) normal * percent / 100);
}

/* Preserve the native aggregate mitigation, carrying its elemental share
 * proportionally into the new DR layer without changing the old HP total. */
int
enhancement_damage_component(int component, int before, int after)
{
    if (before <= 0 || after <= 0) return 0;
    return (int) ((uint64) min(max(0, component), before) * after / before);
}

/* One exact rational product per channel. Seven worn armor slots keep both
 * products and the damage multiplication safely within uint64. Mixed events
 * round their total reduction only once. */
int
enhancement_reduce(const struct monst *def, int damage, int physical)
{
    const struct obj *obj;
    uint64 denominator = 1, phys = 1, other = 1, pr, nr;
    if (damage <= 0 || physical == ENH_FATAL) return damage;
    physical = physical < 0 ? damage : min(physical, damage);
    for (obj = def == &gy.youmonst ? gi.invent : def->minvent;
         obj; obj = obj->nobj) {
        uint64 bits;
        int tier;
        if (!(obj->owornmask & W_ARMOR)) continue;
        bits = enhancement_active(obj);
        tier = bits & OEP_DR_IV ? 4 : bits & OEP_DR_III ? 3
               : bits & OEP_DR_II ? 2 : bits & OEP_DR_I ? 1 : 0;
        if (!tier) continue;
        denominator *= 10;
        phys *= 10 - tier;
        other *= tier >= 3 ? 10 - tier : 10;
    }
    pr = min(denominator - phys, denominator * 7 / 10);
    nr = min(denominator - other, denominator * 7 / 10);
    return damage - (int) (((uint64) physical * pr
                           + (uint64) (damage - physical) * nr) / denominator);
}

boolean
enhancement_physical_type(int type)
{
    /* Contact riders leave ordinary bodily damage physical. Numerical fire,
     * cold, electricity, acid, magic, psychic and life-energy damage do not. */
    switch (type) {
    case AD_PHYS: case AD_DRST: case AD_DRDX: case AD_DRCO:
    case AD_SLEE: case AD_PLYS: case AD_STUN: case AD_LEGS:
    case AD_WERE: case AD_STCK: case AD_WRAP: case AD_TLPT:
    case AD_SITM: case AD_SEDU: case AD_SGLD: case AD_DGST:
        return TRUE;
    default: return FALSE;
    }
}

/* Recursion guard controls only thorns. DR, native mitigation and native
 * death/lifesaving/kill-credit paths remain active for the reflected damage. */
static boolean reflecting_damage;

void
enhancement_reflect(struct monst *def, struct monst *attacker, int loss)
{
    const struct obj *obj;
    int percent = 0, reflected, dx, dy, ax, ay;
    if (reflecting_damage || loss <= 0 || !attacker || attacker == def
        || (attacker != &gy.youmonst && DEADMONSTER(attacker))) return;
    dx = def == &gy.youmonst ? u.ux : def->mx;
    dy = def == &gy.youmonst ? u.uy : def->my;
    ax = attacker == &gy.youmonst ? u.ux : attacker->mx;
    ay = attacker == &gy.youmonst ? u.uy : attacker->my;
    if (!isok(dx, dy) || !isok(ax, ay) || distmin(dx, dy, ax, ay) != 1) return;
    for (obj = def == &gy.youmonst ? gi.invent : def->minvent;
         obj; obj = obj->nobj) {
        uint64 bits;
        int p;
        if (!(obj->owornmask & W_ARMOR)) continue;
        bits = enhancement_active(obj);
        p = bits & OEP_THORNS_II ? 50 : bits & OEP_THORNS_I ? 30 : 0;
        percent = def == &gy.youmonst ? percent + p : max(percent, p);
    }
    reflected = (int) ((uint64) loss * percent / 100);
    if (!reflected) return;
    reflecting_damage = TRUE;
    if (attacker == &gy.youmonst) reflected = Maybe_Half_Phys(reflected);
    reflected = mith_physical_damage(attacker, NULL, AT_NONE, reflected);
    if (attacker == &gy.youmonst)
        mdamageu_damage(def, reflected, reflected);
    else {
        enhancement_mon_damage(attacker, def, reflected, reflected);
        if (DEADMONSTER(attacker)) {
            if (def == &gy.youmonst) killed(attacker);
            else monkilled(attacker, "", AD_PHYS);
        }
    }
    reflecting_damage = FALSE;
}

int
enhancement_mon_damage(struct monst *def, struct monst *attacker,
                       int damage, int physical)
{
    int before = max(0, def->mhp);
    damage = enhancement_reduce(def, damage, physical);
    def->mhp -= damage;
    if (physical != ENH_FATAL)
        enhancement_reflect(def, attacker, min(before, max(0, damage)));
    return def->mhp;
}
