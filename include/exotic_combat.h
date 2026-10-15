/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
#ifndef EXOTIC_COMBAT_H
#define EXOTIC_COMBAT_H
#define EX_INNATE_TELEPORT 0xf001
#define EX_INNATE_SUMMON 0xf002
#define EX_INNATE_TRANSFORM 0xf003
#define EX_INNATE_SHRIEK 0xf004
#define EX_INNATE_GROUP 0xf005
#define EX_INNATE_WEB 0xf006
#define EX_INNATE_WIND 0xf007
#define EX_INNATE_STENCH 0xf008
#define EX_INNATE_DARKEN 0xf009
#define EX_INNATE_PSYCHIC 0xf00a
#define EX_MAX_ABILITIES 64
struct ex_hit_context {
    struct ex_sources sources;
    unsigned target, weapon;
    int x, y, oldhp, enabled, death_processed, actual_damage, defer_primary;
    struct attack attack;
    struct obj strike;
    int has_strike, twohits, thrown, clumsy, native_bonus, native_skill, two_weapon, main_hand;
    long contact_mask;
    unsigned chains;
    struct { int element, base; } chain[EX_MAX_SOURCES];
};
void ex_hit_begin(struct ex_hit_context *, struct monst *, struct obj *, const struct attack *);
int ex_hit_frenzy(struct ex_hit_context *, struct monst *, int);
void ex_hit_finish(struct ex_hit_context *, struct monst *, int);
int ex_muted(struct monst *, int, int);
int ex_innate_muted(struct monst *, int);
int ex_ability_count(struct monst *, unsigned short *, int);
void ex_apply_mute(struct monst *);
int ex_poly_candidate(struct monst *);
void ex_steal(struct monst *);
struct permonst *ex_contact_form(struct monst *);
int ex_contact_muted(struct monst *, int, int);
void ex_contact_end(struct monst *);
int ex_contact_cancelled(struct monst *);
int ex_contact_level(struct monst *);
int ex_contact_sight(struct monst *);
int ex_counterattack(struct monst *, struct obj *);
int ex_defer_element(struct obj *, struct monst *, int, int);
void ex_cancel_element(void);
int ex_resolve_element(struct ex_hit_context *, struct monst *);
int ex_native_physical_roll(const struct ex_hit_context *, struct monst *);
int ex_weapon_dam_bonus(struct obj *, int);
int ex_dmgval(struct obj *, struct monst *, int);
#ifdef STEP15_TEST
extern int ex_test_poly_attempts, ex_test_poly_changes;
#endif
#endif
