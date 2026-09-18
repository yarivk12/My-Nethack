/* Shared equipment enhancement engine and opt-in natural acquisition. */
#ifndef ENHANCE_H
#define ENHANCE_H

enum enhancement_quality { OQ_STANDARD = 0, OQ_FINE = 1, OQ_EXCEPTIONAL = 2 };
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
#define OEP_ALL         0x01ffff7fU
#define OEP_ELEMENTS   0x00007f07U
#define OEP_WORN        0x01ff8070U
#define OEF_QUALITY_KNOWN 0x01U

boolean enhancement_eligible(const struct obj *);
boolean enhancement_property_allowed(const struct obj *, uint32);
void enhancement_normalize(struct obj *);
boolean enhancement_set(struct obj *, uint32, enum enhancement_quality, boolean);
void enhancement_identify(struct obj *);
uint32 enhancement_visible_props(const struct obj *, boolean);
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
    uint32 bit;
    int tier, native_property, element, dice, sides;
    const char *prefix, *suffix;
};
extern const struct enhancement_entry enhancement_catalog[24];
struct enhancement_band { int gate, standard, fine, presence, two, tier[4]; };
const struct enhancement_band *enhancement_depth_band(int);
#endif
