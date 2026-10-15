/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Step 22 artifact property engine. NetHack license, see license. */
#ifndef EXOTIC_H
#define EXOTIC_H

enum ex_property {
    EX_DISARM, EX_STEAL, EX_POLYMORPH, EX_ANTIREGEN, EX_FRENZY,
    EX_EXECUTION, EX_CHAIN, EX_COUNTER, EX_TELEPORT, EX_GUARDIANS,
    EX_ABSORB, EX_UNTRAP, EX_DIG, EX_PRESERVE, EX_CONSERVE,
    EX_TRANSMUTE, EX_WARD, EX_MUTE, EX_COUNT
};
enum ex_activation {
    EX_INACTIVE, EX_DIRECT, EX_CARRIED, EX_WIELDED, EX_WORN,
    EX_MANUAL, EX_INVOKED
};
#define EX_HERO 1
#define EX_MONSTER 2
#define EX_PET 4
#define EX_STRIKING_SOURCE 1
#define EX_WEAPON_MAIN 1
#define EX_WEAPON_OFF 2
#define EX_MAX_SOURCES 128

/* Inline POD state is copied by native object/monster/save machinery. No
 * production catalogue binds any grant in the engine-only milestone. */
struct ex_object_state {
    unsigned long grants, striking;
    unsigned char activation[EX_COUNT], actors[EX_COUNT];
    long slots[EX_COUNT];
    unsigned char counter_weapon;
    unsigned char chain_native;
    unsigned char invoked;
    unsigned char charges[2], initialized;
    long charge_turn[2], guardian_until;
    unsigned preservation_count;
    unsigned char preserved, multiplier, frozen;
    long age_turn, age_units;
    long decay_units;
};
struct ex_sources {
    unsigned ids[EX_MAX_SOURCES];
    unsigned long grants[EX_MAX_SOURCES];
    unsigned count;
};
struct ex_melee_context {
    struct ex_sources sources;
    unsigned attacker;
    int before_hp, before_poly, hostile, damage;
};
struct ex_guardian_window {
    unsigned source, count;
    long until;
};
struct obj;
struct monst;
struct permonst;
struct trap;

/* Native entrypoints deliberately share source enumeration and event state. */
void ex_snapshot(struct ex_sources *, struct monst *);
struct obj *ex_source(const struct ex_sources *, unsigned, int, struct monst *);
int ex_active(const struct obj *, int, const struct monst *);
int ex_has(int);
int ex_roll(int);
int ex_protected_monster(const struct monst *);
int ex_protected_object(const struct obj *);
int ex_protected_tile(int, int);
void ex_resources(struct obj *);
int ex_spend(struct obj *, int, int);
void ex_inspect(struct obj *, int);
int ex_invoke(struct obj *, int);
int ex_apply(struct obj *);
void ex_world_turn(void);
void ex_monster_arrived(struct monst *);
int ex_guardian_expire(struct monst *);
int ex_absorb(struct monst *, int, int);
int ex_secondary(void);
void ex_secondary_begin(void);
void ex_secondary_end(void);
int ex_utility_invoke(struct obj *, int);
int ex_untrap_attempt(void);
int ex_untrap_floor(struct trap *);
int ex_theft_ward(void);
int ex_transmute(struct obj *, int, int);
long ex_corpse_age(struct obj *);
void ex_corpse_transition(struct obj *, int, int);
int ex_preserve(struct obj *, struct obj *);
int ex_preserve_apply(struct obj *);
int ex_conserve(struct obj *);
void ex_consumption_begin(struct obj *);
void ex_consumption_end(void);
int ex_consumption_retain(struct obj *);
void ex_preserve_inspect(struct obj *, int);
void ex_conserved_use_fee(struct obj *);
void ex_melee_begin(struct ex_melee_context *, struct monst *);
void ex_melee_end(struct ex_melee_context *);
void ex_melee_damage(struct monst *, int);
int ex_reactive_teleport(void);
void ex_guardian_detach(struct monst *);
void ex_guardian_clone(struct monst *);
void ex_health_changed(void);
void ex_area_spell_begin(struct monst *, int);
void ex_area_spell_end(void);
int ex_area_spell_square(int, int);

#ifdef STEP15_TEST
void ex_test_grant(struct obj *, int, int, int, long);
void ex_test_clear(struct obj *);
#endif
#endif
