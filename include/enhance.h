/* Generic equipment enhancement contract.  Acquisition belongs to Step 14+. */
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
#define OEP_CUMBERSOME 0x00000080U
#define OEP_ALL        0x000000ffU
#define OEP_ELEMENTS (OEP_FIRE | OEP_COLD | OEP_SHOCK)
#define OEP_WORN (OEP_WARNING | OEP_SEARCHING | OEP_STEALTH)
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
#endif
