/* Shared equipment enhancement engine and opt-in natural acquisition. */
#ifndef ENHANCE_H
#define ENHANCE_H

enum enhancement_quality { OQ_STANDARD = 0, OQ_FINE = 1, OQ_EXCEPTIONAL = 2 };
/* Persistent identity and mask coordinate are deliberately independent.
 * Word 0 constants below are retained for combat-specific queries. */
struct enhancement_mask { uint64 word[2]; };
struct enhancement_mask enhancement_mask_property(int);
struct enhancement_mask enhancement_mask_union(struct enhancement_mask, struct enhancement_mask);
struct enhancement_mask enhancement_actual(const struct obj *);
struct enhancement_mask enhancement_known(const struct obj *);
boolean enhancement_mask_has(struct enhancement_mask, int);
void enhancement_mask_add(struct enhancement_mask *, int);
void enhancement_mask_remove(struct enhancement_mask *, int);
int enhancement_mask_count(struct enhancement_mask);
boolean enhancement_mask_allowed(const struct obj *, struct enhancement_mask);
boolean enhancement_set_mask(struct obj *, struct enhancement_mask, enum enhancement_quality, boolean);
void essence_normalize(struct obj *);
long essence_wish_quantity(long);
boolean essence_loot_context(void);
struct obj *essence_random(void);
struct obj *ordinary_loot_at(char, coordxy, coordxy, boolean);
int enhancement_capacity(const struct obj *);
int enhancement_slot_count(const struct obj *);
int enhancement_first_unused(const struct obj *);
boolean enhancement_slots_valid(const struct obj *);
boolean enhancement_slot_set(struct obj *, int, int, boolean, int);
void enhancement_slot_history(struct obj *, int);
int enhancement_forge_candidates(const struct obj *, int, int, boolean, int *);
boolean enhancement_forge_known(const struct obj *);
void enhancement_finalize_stack(struct obj *);
boolean enhancement_has(const struct obj *, int);
void enhancement_learn(struct obj *, int);
enum enhance_use {
    ENHANCE_MELEE = 0, ENHANCE_THROWN, ENHANCE_LAUNCHER,
    ENHANCE_AMMO, ENHANCE_ARMOR
};
#define OEP_FIRE       0x00000001U
#define OEP_COLD       0x00000002U
#define OEP_SHOCK      0x00000004U
#define OEP_TRUEFLIGHT 0x00000008U
#define OEP_WARNING    0x00000010U
#define OEP_SEARCHING  0x00000020U
#define OEP_STEALTH    0x00000040U
/* 0x80 is retired; never reuse it for another property. Epoch 8. */
#define OEP_FIRE_II     0x00000100U
#define OEP_COLD_II     0x00000200U
#define OEP_SHOCK_II    0x00000400U
#define OEP_FIRE_III    0x00000800U
#define OEP_COLD_III    0x00001000U
#define OEP_SHOCK_III   0x00002000U
#define OEP_PRIMORDIAL  0x00004000U
#define OEP_FIRE_RES    0x00008000U
#define OEP_COLD_RES    0x00010000U
#define OEP_SHOCK_RES   0x00020000U
#define OEP_POISON_RES  0x00040000U
#define OEP_SPEED       0x00080000U
#define OEP_REGEN       0x00100000U
#define OEP_DISPLACED   0x00200000U
#define OEP_SLOW_DIGEST 0x00400000U
#define OEP_MAGIC_RES   0x00800000U
#define OEP_REFLECTION  0x01000000U
#define OEP_STR_I       (1ULL << 25)
#define OEP_STR_II      (1ULL << 26)
#define OEP_STR_III     (1ULL << 27)
#define OEP_STR_IV      (1ULL << 28)
#define OEP_DEX_I       (1ULL << 29)
#define OEP_DEX_II      (1ULL << 30)
#define OEP_DEX_III     (1ULL << 31)
#define OEP_DEX_IV      (1ULL << 32)
#define OEP_ATTRIBUTES  (0xffULL << 25)
#define OEP_VAMPIRIC_I (1ULL << 33)
#define OEP_VAMPIRIC_II (1ULL << 34)
#define OEP_VAMPIRIC_III (1ULL << 35)
#define OEP_VAMPIRIC_IV (1ULL << 36)
#define OEP_ACID_I (1ULL << 37)
#define OEP_ACID_II (1ULL << 38)
#define OEP_ACID_III (1ULL << 39)
#define OEP_ANARCHIC_I (1ULL << 40)
#define OEP_ANARCHIC_II (1ULL << 41)
#define OEP_AXIOMATIC_I (1ULL << 42)
#define OEP_AXIOMATIC_II (1ULL << 43)
#define OEP_STONING_I (1ULL << 44)
#define OEP_STONING_II (1ULL << 45)
#define OEP_STONING_III (1ULL << 46)
#define OEP_STONING_IV (1ULL << 47)
#define OEP_VAMPIRIC (0xfULL << 33)
#define OEP_ACID (0x7ULL << 37)
#define OEP_ALIGNMENT (0xfULL << 40)
#define OEP_STONING (0xfULL << 44)
#define OEP_OFFENSIVE (OEP_VAMPIRIC | OEP_ACID | OEP_ALIGNMENT | OEP_STONING)
#define OEP_WARDING (1ULL << 48)
#define OEP_CASTING_II (1ULL << 49)
#define OEP_CASTING_IV (1ULL << 50)
#define OEP_LIGHTNESS_I (1ULL << 51)
#define OEP_LIGHTNESS_II (1ULL << 52)
#define OEP_LIGHTNESS_III (1ULL << 53)
#define OEP_THORNS_I (1ULL << 54)
#define OEP_THORNS_II (1ULL << 55)
#define OEP_DR_I (1ULL << 56)
#define OEP_DR_II (1ULL << 57)
#define OEP_DR_III (1ULL << 58)
#define OEP_DR_IV (1ULL << 59)
#define OEP_CASTING (3ULL << 49)
#define OEP_LIGHTNESS (7ULL << 51)
#define OEP_THORNS (3ULL << 54)
#define OEP_DR (15ULL << 56)
#define OEP_DEFENSIVE (OEP_WARDING | OEP_CASTING | OEP_LIGHTNESS | OEP_THORNS | OEP_DR)
#define OEP_ALL         (0x01ffff7fULL | OEP_ATTRIBUTES | OEP_OFFENSIVE | OEP_DEFENSIVE)
#define OEP_ELEMENTS   (0x00007f07ULL | OEP_ACID)
#define OEP_WORN        0x01ff8070U
#define OEF_QUALITY_KNOWN 0x01U
#define OEF_AFFIXES_KNOWN 0x02U /* includes knowledge of absence */

boolean enhancement_eligible(const struct obj *);
boolean enhancement_property_allowed(const struct obj *, uint64);
void enhancement_normalize(struct obj *);
boolean enhancement_set(struct obj *, uint64, enum enhancement_quality, boolean);
void enhancement_identify(struct obj *);
struct enhancement_mask enhancement_visible_mask(const struct obj *, boolean);
uint64 enhancement_visible_word0(const struct obj *, boolean); /* combat adapter */
int enhancement_quality_bonus(const struct obj *, enum enhance_use);
int enhancement_hit_bonus(const struct obj *, const struct obj *,
                          const struct monst *, enum enhance_use);
int enhancement_damage_bonus(const struct obj *, const struct obj *,
                             const struct monst *, enum enhance_use);
/* Returns extra elemental damage, not total damage.  Confirmed hits only. */
int enhancement_weapon_effects(struct obj *, const struct obj *, struct monst *,
                               int, enum enhance_use);
void enhancement_observe_hit(struct obj *, struct obj *, struct monst *, enum enhance_use);
void enhancement_observe_attack(struct obj *, struct obj *, enum enhance_use);
void enhancement_worn_on(struct obj *, struct monst *);
void enhancement_worn_off(struct obj *, struct monst *);
void enhancement_strip_for_artifact(struct obj *);
void enhancement_clear(struct obj *);
void enhancement_change_type(struct obj *, int);
long enhancement_price_adjustment(const struct obj *); /* percentage */
long enhancement_price(const struct obj *, long);
void enhancement_rebill(struct obj *); /* existing shop codec and pricing */
void enhancement_prefix(const struct obj *, boolean, char *, size_t);
void enhancement_suffix(const struct obj *, boolean, char *, size_t);
void enhancement_quality_impact(const struct obj *, char *, size_t);
void enhancement_property_impact(const struct obj *, int, int, char *, size_t);
boolean enhancement_confers(const struct obj *, int);
boolean enhancement_mon_confers(const struct monst *, int);
boolean enhancement_elemental_contact(const struct obj *, const struct obj *, enum enhance_use);
boolean enhancement_native_property(const struct obj *, int);
void enhancement_observe_worn(int);
enum enhancement_context { ENH_CONTEXT_NONE, ENH_CONTEXT_FLOOR,
    ENH_CONTEXT_MONSTER, ENH_CONTEXT_SHOP };
enum enhancement_context enhancement_context_set(enum enhancement_context);
void enhancement_created(struct obj *);
void enhancement_generate(struct obj *, int);
struct obj *enhancement_mkobj(int, boolean);
struct obj *enhancement_mkobj_at(char, coordxy, coordxy, boolean);
struct obj *enhancement_mksobj_at(int, coordxy, coordxy, boolean, boolean);
struct monst *enhancement_makemon(struct permonst *, coordxy, coordxy, mmflags_nht);
struct enhancement_entry {
    uint64 bit;
    int tier, native_property, element, dice, sides;
    const char *prefix, *suffix;
    int stat; /* zero: no static value; ES_* otherwise */
    uint64 bit2; /* word 1 coordinate; zero for earlier properties */
};
extern const struct enhancement_entry enhancement_catalog[82];
struct enhancement_band { int gate, standard, fine, presence, two, tier[4]; };
const struct enhancement_band *enhancement_depth_band(int);
/* IDs 1..32 retain their original catalog indices; Step 16A IDs follow
 * the existing socket-only IDs, preserving all earlier persisted identities. */
/* Persistent IDs are declared in affix.h. */
enum equipment_stat { ES_NONE, ES_STR, ES_DEX, ES_CON, ES_INT, ES_WIS,
                      ES_CHA, ES_HP, ES_MANA, ES_PROTECTION };
const struct enhancement_entry *equipment_property(int);
const char *equipment_property_name(int);
int socket_capacity(const struct obj *);
void socket_init(struct obj *);
void socket_normalize(struct obj *);
int socket_count(const struct obj *);
int socket_gem_tier(int);
int socket_candidates(const struct obj *, int, int, int *);
void socket_label(const struct obj *, int, char *, size_t);
int equipment_bonus(const struct obj *, int);
int equipment_hero_bonus(int);
void equipment_refresh(void);
void equipment_mon_refresh(struct monst *);
int enhancement_alignment_damage(const struct obj *, const struct obj *, const struct monst *, enum enhance_use);
void enhancement_vampiric(const struct obj *, const struct obj *, struct monst *, const struct monst *, int, int, enum enhance_use);
boolean enhancement_stoning(struct obj *, struct obj *, struct monst *, struct monst *, enum enhance_use);
void enhancement_tick(void);
/* Damage classification sentinels: all physical, or a native instant death. */
#define ENH_PHYSICAL (-1)
#define ENH_FATAL (-2)
boolean artifact_hit_fatal(struct monst *, struct monst *, struct obj *, int *, int, boolean *, int *);
int enhancement_damage_component(int, int, int);
int enhancement_reduce(const struct monst *, int, int);
int enhancement_mon_damage(struct monst *, struct monst *, int, int);
void enhancement_reflect(struct monst *, struct monst *, int);
boolean enhancement_physical_type(int);
void explode_oil_by(struct obj *, coordxy, coordxy, struct monst *);
void explode_by(coordxy, coordxy, int, int, char, int, struct monst *);
void potionhit_by(struct monst *, struct obj *, int, struct monst *);
void losehp_physical(int, const char *, schar);
void losehp_damage(int, const char *, schar, int, struct monst *);
void mdamageu_damage(struct monst *, int, int);
boolean enhancement_curse_protected(const struct obj *);
boolean armor_spell_penalty(const struct obj *);
int enhancement_casting_penalty(const struct obj *, int);
int enhancement_weight(const struct obj *, int);
boolean utility_active(int);
void utility_turn_tick(void);
void utility_turn_cancel(void);
void utility_forget_object(struct obj *);
void utility_purification_restore(struct obj *);
boolean enhancement_equipped_target(const struct obj *);
boolean utility_erosion_protected(const struct obj *, int);
boolean utility_curse_protected(const struct obj *);
void utility_discernment_refresh(void);
int utility_purification_interval(const struct obj *);
void utility_turn_snapshot(void);
void utility_turn_end(void);
int utility_excavating_effort(struct obj *, int, int, int);
int doinspect(void);
#endif
