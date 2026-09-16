/* YarivNetHack custom-room coordinator. NetHack license; see dat/license. */
#ifndef CUSTOMROOM_H
#define CUSTOMROOM_H

/* Saved IDs: never renumber or recycle. Zero is always vanilla. */
enum custom_feature_id {
    CUSTOM_NONE = 0,
    CUSTOM_GIANT_COURT = 1, CUSTOM_REAL_ZOO = 2, CUSTOM_DRAGON_LAIR = 3,
    CUSTOM_WIZARD_STUDY = 4, CUSTOM_STOREROOM = 5,
    CUSTOM_HONEYCOMB = 6, CUSTOM_DRAGON_HALL = 7
};
enum custom_backend { CUSTOM_CLASSIC = 1, CUSTOM_LUA = 2 };
enum custom_progress {
    CUSTOM_IDLE, CUSTOM_SELECTED, CUSTOM_CLEAN_FAILURE,
    CUSTOM_PENDING_FILL, CUSTOM_COMPLETE, CUSTOM_PARTIAL_ERROR
};
#define CUSTOM_DEFAULT_BP 300
#define CUSTOM_OUTCOMES 10000
#define CUSTOM_MAX_ATTEMPTS 3
struct custom_descriptor {
    unsigned id;
    const char *name;
    int backend, roomtype;
    const char *target;
    int probability; /* zero inherits CUSTOM_DEFAULT_BP */
    int min_dl, max_dl, min_diff, max_diff; /* zero: no bound */
    boolean pre_medusa;
};
struct custom_context {
    boolean ordinary_dod;
    int dlevel, logical_depth, difficulty, medusa_depth;
};
struct custom_generation {
    struct custom_context context;
    unsigned selected;
    int progress, attempts, emissions;
    boolean rolled;
    int vanilla_attempt, vanilla_placements;
};
extern const struct custom_descriptor custom_registry[];
extern const int custom_registry_count;
extern struct custom_generation custom_generation;
extern const char *custom_validate(const struct custom_descriptor *, int);
extern boolean custom_eligible(const struct custom_descriptor *,
                               const struct custom_context *);
extern int custom_select(const struct custom_descriptor *, int,
                         const struct custom_context *, int);
extern void custom_reset(void);
extern void custom_begin(const struct custom_context *);
extern struct mkroom *custom_classic_room(unsigned, int);
extern void custom_diagnostics(void);
#endif
