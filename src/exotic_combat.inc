/* Step 22 primary melee and canonical per-monster capability suppression. */
#include "exotic_combat.h"
static unsigned ex_contact_id;
static struct permonst *ex_contact_data;
static unsigned short ex_contact_ability;
static int ex_contact_suppressed, ex_contact_pending;
static int ex_contact_mcan, ex_contact_mlevel, ex_contact_cansee;
static int ex_combat_chance(int percent) { return rn2(100) < percent; }
#ifdef STEP15_TEST
int ex_test_poly_attempts, ex_test_poly_changes;
#endif
static struct {
    unsigned target, weapon;
    int enabled, pending, base, roll;
    struct obj source;
} ex_element_pending;

void ex_cancel_element(void) { ex_element_pending.enabled = ex_element_pending.pending = 0; }

int
ex_defer_element(struct obj *weapon, struct monst *mon, int base, int roll)
{
    if (!ex_element_pending.enabled || ex_element_pending.target != mon->m_id
        || ex_element_pending.weapon != weapon->o_id || ex_secondary()) return 0;
    ex_element_pending.source = *weapon;
    ex_element_pending.source.oextra = NULL;
    ex_element_pending.base = base;
    ex_element_pending.roll = roll;
    ex_element_pending.pending = 1;
    return 1;
}

int
ex_resolve_element(struct ex_hit_context *ctx, struct monst *mon)
{
    struct obj *weapon;
    int damage, oldhp, base, roll;
    if (!ex_element_pending.pending || ex_element_pending.target != mon->m_id) return 0;
    base = ex_element_pending.base; roll = ex_element_pending.roll;
    ex_cancel_element();
    if (DEADMONSTER(mon) || !mon->mx) return 0;
    for (weapon = gi.invent; weapon && weapon->o_id != ex_element_pending.weapon;
         weapon = weapon->nobj) ;
    if (!weapon) weapon = &ex_element_pending.source;
    damage = base;
    (void) artifact_hit_fatal(&gy.youmonst, mon, weapon, &damage, roll, NULL, NULL);
    damage = max(0, damage - base);
    oldhp = mon->mhp;
    enhancement_mon_damage(mon, &gy.youmonst, damage, 0);
    ctx->actual_damage += max(0, oldhp - mon->mhp);
    return damage;
}

static unsigned short
ex_attack_identity(int delivery, int effect)
{
    if (effect == AD_SPEL || effect == AD_CLRC || effect == AD_PSON)
        delivery = AT_MAGC;
    return (unsigned short) (1 + delivery * 256 + effect);
}

int
ex_innate_muted(struct monst *mon, int ability)
{
    if (!mon || mon == &gy.youmonst)
        return 0;
    if (mon->ex_mute_species != monsndx(mon->data)) {
        mon->ex_mute_until = 0;
        mon->ex_mute_ability = 0;
    }
    return mon->ex_mute_until > svm.moves
           && mon->ex_mute_ability == ability;
}

int
ex_muted(struct monst *mon, int delivery, int effect)
{
    return ex_innate_muted(mon, ex_attack_identity(delivery, effect));
}

int
ex_ability_count(struct monst *mon, unsigned short *out, int cap)
{
    unsigned short catalogue[EX_MAX_ABILITIES], id;
    int i, j, count = 0;
    struct permonst *pm = mon->data;
    for (i = 0; i < NATTK; ++i) {
        const struct attack *a = &pm->mattk[i];
        struct attack configured;
        /* Match getmattk's native global configuration before identifying
         * capabilities. Disabled seduction has a real drain replacement. */
        if (!SYSOPT_SEDUCE) {
            extern const struct attack c_sa_no[NATTK];
            if (pm->mattk[0].adtyp == AD_SSEX) a = &c_sa_no[i];
            else if (a->adtyp == AD_SSEX) {
                configured = *a; configured.adtyp = AD_DRLI; a = &configured;
            }
        }
        if (a->aatyp == AT_BKGT) {
            catalogue[count++] = ex_attack_identity(AT_TUCH, AD_EACD);
            catalogue[count++] = ex_attack_identity(AT_GAZE, AD_CONF);
            continue;
        }
        if (a->aatyp == AT_BKG2) {
            catalogue[count++] = ex_attack_identity(AT_TENT, AD_DRST);
            catalogue[count++] = ex_attack_identity(AT_BUTT, AD_STUN);
            catalogue[count++] = ex_attack_identity(AT_GAZE, AD_CONF);
            catalogue[count++] = ex_attack_identity(AT_CLAW, AD_SEDU);
            continue;
        }
        if (a->adtyp == AD_PHYS || a->aatyp == AT_BOOM)
            continue; /* death-trigger effects are never muted */
        id = ex_attack_identity(a->aatyp == AT_WDGZ ? AT_GAZE : a->aatyp, a->adtyp);
        for (j = 0; j < count && catalogue[j] != id; ++j)
            ;
        if (j == count)
            catalogue[count++] = id;
        /* Consecutive native disease/hunger attacks can become stun after
         * an earlier hit. That concrete rider is a separate capability. */
        if (i > 0 && (a->adtyp == AD_DISE || a->adtyp == AD_PEST || a->adtyp == AD_FAMN)
            && a->adtyp == pm->mattk[i - 1].adtyp)
            catalogue[count++] = ex_attack_identity(a->aatyp, AD_STUN);
    }
    if (can_teleport(pm) || is_covetous(pm) || is_unicorn(pm))
        catalogue[count++] = EX_INNATE_TELEPORT;
    if (is_demon(pm) || is_were(pm) || pm == &mons[PM_WITCH]
        || pm == &mons[PM_COVEN_LEADER]) catalogue[count++] = EX_INNATE_SUMMON;
    if (ismnum(mon->cham) || is_shapeshifter(pm) || is_were(pm)
        || pm == &mons[PM_COURE_ELADRIN] || pm == &mons[PM_MOTE_OF_LIGHT]
        || pm == &mons[PM_NOVIERE_ELADRIN] || pm == &mons[PM_WATER_DOLPHIN]
        || pm == &mons[PM_BRALANI_ELADRIN] || pm == &mons[PM_SINGING_SAND]
        || pm == &mons[PM_SELKIE] || pm == &mons[PM_SEAL])
        catalogue[count++] = EX_INNATE_TRANSFORM;
    if (pm->msound == MS_SHRIEK || pm == &mons[PM_ERINYS])
        catalogue[count++] = EX_INNATE_SHRIEK;
    if (pm->mflags3 & M3_GROUPATTACK) catalogue[count++] = EX_INNATE_GROUP;
    if (pm == &mons[PM_DRIDER] || pm == &mons[PM_PRIESTESS_OF_GHAUNADAUR])
        catalogue[count++] = EX_INNATE_WEB;
    if (pm == &mons[PM_FIRST_WRAITHWORM]) catalogue[count++] = EX_INNATE_WIND;
    if (pm == &mons[PM_HEZROU] || pm == &mons[PM_STEAM_VORTEX]
        || pm == &mons[PM_FOG_CLOUD])
        catalogue[count++] = EX_INNATE_STENCH;
    if (is_shadow(pm)) catalogue[count++] = EX_INNATE_DARKEN;
    if (is_mind_flayer(pm) || pm == &mons[PM_GREAT_CTHULHU])
        catalogue[count++] = EX_INNATE_PSYCHIC;
    if (mith_mon_syllable(mon) >= 0) {
        int syllable = mith_mon_syllable(mon);
        if (syllable == MITH_KRAU || syllable == MITH_NAEN
            || ((syllable == MITH_HOON || syllable == MITH_VAUL) && (mon->m_id & 1)))
            catalogue[count++] = ex_attack_identity(AT_MAGC, AD_SPEL);
    }
    for (i = 0; i < count; ++i)
        for (j = i + 1; j < count; )
            if (catalogue[i] == catalogue[j]) catalogue[j] = catalogue[--count];
            else ++j;
    for (i = 0; i < count && i < cap; ++i) out[i] = catalogue[i];
    return count;
}

void
ex_apply_mute(struct monst *mon)
{
    unsigned short abilities[EX_MAX_ABILITIES];
    int count;
    long until;
    if (!mon || DEADMONSTER(mon) || ex_protected_monster(mon)) return;
    if (!ex_innate_muted(mon, mon->ex_mute_ability)) {
        count = ex_ability_count(mon, abilities, SIZE(abilities));
        if (!count) return;
        mon->ex_mute_ability = abilities[rn2(count)];
        mon->ex_mute_species = monsndx(mon->data);
        mon->ex_mute_until = 0;
    }
    until = svm.moves + d(2, 3);
    mon->ex_mute_until = max(mon->ex_mute_until, until);
}

struct permonst *
ex_contact_form(struct monst *mon)
{
    return ex_contact_pending && ex_contact_id == mon->m_id
               ? ex_contact_data : mon->data;
}

void
ex_contact_end(struct monst *mon)
{
    if (ex_contact_pending && ex_contact_id == mon->m_id)
        ex_contact_pending = 0;
}

int ex_contact_cancelled(struct monst *mon)
{
    return ex_contact_pending && ex_contact_id == mon->m_id ? ex_contact_mcan : mon->mcan;
}
int ex_contact_level(struct monst *mon)
{
    return ex_contact_pending && ex_contact_id == mon->m_id ? ex_contact_mlevel : mon->m_lev;
}
int ex_contact_sight(struct monst *mon)
{
    return ex_contact_pending && ex_contact_id == mon->m_id ? ex_contact_cansee : mon->mcansee;
}

int
ex_contact_muted(struct monst *mon, int delivery, int effect)
{
    int result;
    if (ex_contact_pending && ex_contact_id == mon->m_id) {
        result = ex_contact_suppressed
                 && ex_contact_ability == ex_attack_identity(delivery, effect);
        ex_contact_pending = 0;
        return result;
    }
    return ex_muted(mon, delivery, effect);
}

int
ex_poly_candidate(struct monst *mon)
{
    int i, total = 0, selected = NON_PM, weight;
    const struct permonst *old = mon->data, *pm;
    if (ex_protected_monster(mon) || (old->geno & G_UNIQ)) return NON_PM;
    /* Weighted reservoir over a finite legal native form pool. */
    for (i = LOW_PM; i < SPECIAL_PM; ++i) {
        pm = &mons[i];
        weight = pm->geno & G_FREQ;
        if (pm == old || !weight || !polyok(pm) || is_placeholder(pm)
            || (pm->geno & (G_UNIQ | G_NOGEN))
            || (svm.mvitals[i].mvflags & (G_GENOD | G_EXTINCT))
            || pm->mlevel > old->mlevel || pm->difficulty > old->difficulty
            || ((pm->geno & G_HELL) && !In_hell(&u.uz))
            || ((pm->geno & G_NOHELL) && In_hell(&u.uz))) continue;
        total += weight;
        if (rn2(total) < weight) selected = i;
    }
    return selected;
}

static int
ex_loot_eligible(struct obj *obj)
{
    struct obj *nested;
    if (ex_protected_object(obj) || (obj->cursed && obj->owornmask)) return 0;
    for (nested = obj->cobj; nested; nested = nested->nobj)
        if (!ex_loot_eligible(nested)) return 0;
    return 1;
}

void
ex_steal(struct monst *mon)
{
    struct obj *obj, *chosen = 0;
    int n = 0, burden, nomerge;
    long worn;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if (ex_loot_eligible(obj) && !rn2(++n)) chosen = obj;
    if (!chosen) return;
    burden = calc_capacity((int) chosen->owt);
    worn = chosen->owornmask;
    nomerge = chosen->nomerge;
    chosen->nomerge = 1; /* The selected entire stack keeps its own identity. */
    extract_from_minvent(mon, chosen, TRUE, FALSE);
    if (burden > near_capacity()) {
        if (!flooreffects(chosen, u.ux, u.uy, "fall")) {
            place_object(chosen, u.ux, u.uy);
            stackobj(chosen);
            chosen->nomerge = nomerge;
            newsym(u.ux, u.uy);
        }
    } else {
        chosen = hold_another_object(chosen, "You snatch but drop %s.",
                                     doname(chosen), "You steal: ");
        if (chosen) chosen->nomerge = nomerge;
        if (chosen && chosen->where == OBJ_INVENT && !uarmg
            && chosen->otyp == CORPSE && !Stone_resistance
            && touch_petrifies(&mons[chosen->corpsenm]))
            instapetrify(corpse_xname(chosen, "stolen", CXN_ARTICLE));
    }
    if (worn & W_WEP) possibly_unwield(mon, FALSE);
    if ((worn & W_ARMG) && !DEADMONSTER(mon)) mselftouch(mon, (const char *) 0, TRUE);
}

void
ex_hit_begin(struct ex_hit_context *ctx, struct monst *mon, struct obj *weapon,
             const struct attack *attack)
{
    unsigned i;
    struct obj *source;
    int elements[4], n, element, base;
    boolean oldapplies;
    memset(ctx, 0, sizeof *ctx);
    ctx->target = mon->m_id;
    ctx->weapon = weapon ? weapon->o_id : 0;
    if (weapon) {
        ctx->strike = *weapon;
        ctx->strike.oextra = NULL;
        ctx->has_strike = 1;
    }
    ctx->twohits = gt.twohits;
    ctx->two_weapon = u.twoweap && weapon && (weapon == uwep || weapon == uswapwep);
    ctx->main_hand = weapon && weapon == uwep && !u.twoweap;
    ctx->thrown = HMON_MELEE;
    ctx->native_bonus = 1;
    ctx->native_skill = weapon != NULL;
    ctx->x = mon->mx; ctx->y = mon->my; ctx->oldhp = mon->mhp;
    ctx->attack = *attack;
    ex_contact_id = mon->m_id;
    ex_contact_data = mon->data;
    ex_contact_mcan = mon->mcan;
    ex_contact_mlevel = mon->m_lev;
    ex_contact_cansee = mon->mcansee;
    ex_contact_ability = mon->ex_mute_ability;
    ex_contact_suppressed = mon->ex_mute_until > svm.moves
        && mon->ex_mute_species == monsndx(mon->data);
    ex_contact_pending = 1;
    ctx->enabled = !ex_secondary();
    ex_cancel_element();
    if (!ctx->enabled) return;
    ex_snapshot(&ctx->sources, &gy.youmonst);
    for (i = 0; i < ctx->sources.count; ++i) {
        source = ex_source(&ctx->sources, i, EX_FRENZY, &gy.youmonst);
        if (source && (!(source->exotic.striking & (1UL << EX_FRENZY))
                       || source->o_id == ctx->weapon)) {
            ctx->defer_primary = 1;
            break;
        }
    }
    if (ctx->defer_primary && weapon && weapon->oartifact
        && (attacks(AD_FIRE, weapon) || attacks(AD_COLD, weapon)
            || attacks(AD_ELEC, weapon) || attacks(AD_ACID, weapon))) {
        ex_element_pending.enabled = 1;
        ex_element_pending.target = mon->m_id;
        ex_element_pending.weapon = weapon->o_id;
    }
    for (i = 0; i < ctx->sources.count; ++i) {
        source = ex_source(&ctx->sources, i, EX_CHAIN, &gy.youmonst);
        /* Only the actual intrinsic native artifact resolver is eligible.
         * Forge elements and unrelated carried elemental artifacts are not. */
        if (!source || !source->oartifact
            || (source != weapon && !source->exotic.chain_native)) continue;
        n = 0;
        if (attacks(AD_FIRE, source)) elements[n++] = AD_FIRE;
        if (attacks(AD_COLD, source)) elements[n++] = AD_COLD;
        if (attacks(AD_ELEC, source)) elements[n++] = AD_ELEC;
        if (attacks(AD_ACID, source)) elements[n++] = AD_ACID;
        if (!n || !ex_combat_chance(30)) continue;
        element = elements[rn2(n)];
        oldapplies = gs.spec_dbon_applies;
        base = spec_dbon(source, mon, max(1, dmgval(source, mon)));
        gs.spec_dbon_applies = oldapplies;
        if (base > 0) {
            ctx->chain[ctx->chains].element = element;
            ctx->chain[ctx->chains++].base = base;
        }
    }
}

static int
ex_fresh_physical(struct ex_hit_context *ctx, struct monst *mon)
{
    return ex_native_physical_roll(ctx, mon);
}

int
ex_hit_frenzy(struct ex_hit_context *ctx, struct monst *mon, int physical)
{
    unsigned i;
    int tick, total = 0, dmg;
    if (!ctx->enabled || physical <= 0 || DEADMONSTER(mon)) return 0;
    if (ctx->has_strike && (ctx->strike.otyp == ACID_VENOM
        || ctx->strike.otyp == BLINDING_VENOM || ctx->strike.otyp == CREAM_PIE))
        return 0;
    for (i = 0; i < ctx->sources.count; ++i) {
        struct obj *source = ex_source(&ctx->sources, i, EX_FRENZY, &gy.youmonst);
        if (!source || ((source->exotic.striking & (1UL << EX_FRENZY))
                        && source->o_id != ctx->weapon) || !ex_combat_chance(20)) continue;
        for (tick = 0; tick < 3; ++tick) {
            dmg = ex_fresh_physical(ctx, mon);
            dmg = enhancement_reduce(mon, dmg, ENH_PHYSICAL);
            total += min(mon->mhp, dmg);
            mon->mhp -= dmg;
            if (DEADMONSTER(mon)) {
                ctx->death_processed = 1;
                killed(mon);
            }
            if (DEADMONSTER(mon) || !mon->mx || tick == 2 || !ex_combat_chance(45)) break;
        }
        if (DEADMONSTER(mon) || !mon->mx) break;
    }
    ctx->actual_damage += total;
    return total;
}

static int
ex_chain_open(int x, int y)
{
    return isok(x, y) && !IS_STWALL(levl[x][y].typ)
           && !(IS_DOOR(levl[x][y].typ)
                && (levl[x][y].doormask & (D_CLOSED | D_LOCKED)));
}

static void
ex_run_chains(struct ex_hit_context *ctx)
{
    unsigned c, seen[4];
    int hop, x, y, nx, ny, dx, dy, n, dmg, el;
    struct monst *m, *chosen;
    for (c = 0; c < ctx->chains; ++c) {
        x = ctx->x; y = ctx->y; seen[0] = ctx->target;
        el = ctx->chain[c].element;
        for (hop = 1; hop <= 3; ++hop) {
            int numerator = hop == 1 ? 3 : hop == 2 ? 9 : 27;
            int denominator = hop == 1 ? 4 : hop == 2 ? 16 : 64;
            chosen = 0; n = 0;
            for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy <= 1; ++dy) {
                int k;
                nx = x + dx; ny = y + dy;
                if ((!dx && !dy) || !ex_chain_open(nx, ny)
                    || (dx && dy && (!ex_chain_open(x + dx, y)
                                    || !ex_chain_open(x, y + dy)))) continue;
                m = m_at(nx, ny);
                if (!m || DEADMONSTER(m) || m->mpeaceful || m->mtame) continue;
                for (k = 0; k < hop && seen[k] != m->m_id; ++k) ;
                if (k != hop) continue;
                if (!rn2(++n)) chosen = m;
            }
            if (!chosen) break;
            x = chosen->mx; y = chosen->my; seen[hop] = chosen->m_id;
            if (defended(chosen, el) || (el == AD_FIRE && resists_fire(chosen))
                || (el == AD_COLD && resists_cold(chosen))
                || (el == AD_ELEC && resists_elec(chosen))
                || (el == AD_ACID && resists_acid(chosen))) break;
            dmg = max(1, ctx->chain[c].base * numerator / denominator);
            ex_secondary_begin();
            enhancement_mon_damage(chosen, &gy.youmonst, dmg, 0);
            if (DEADMONSTER(chosen)) killed(chosen);
            ex_secondary_end();
        }
    }
}

void
ex_hit_finish(struct ex_hit_context *ctx, struct monst *mon, int actual_damage)
{
    unsigned i;
    int phase, prop, candidate, chance;
    static const int order[] = { EX_EXECUTION, EX_STEAL, EX_DISARM,
                                EX_POLYMORPH, EX_MUTE, EX_ANTIREGEN };
    struct obj *source, *wep;
    ex_cancel_element();
    if (!ctx->enabled) return;
    for (phase = 0; phase < SIZE(order); ++phase) {
        prop = order[phase];
        for (i = 0; i < ctx->sources.count && !DEADMONSTER(mon) && mon->mx; ++i) {
            source = ex_source(&ctx->sources, i, prop, &gy.youmonst);
            if (!source || ((source->exotic.striking & (1UL << prop))
                            && source->o_id != ctx->weapon)) continue;
            chance = prop == EX_MUTE ? 25 : 20;
            if (prop == EX_EXECUTION) {
                int unique = !!(mon->data->geno & G_UNIQ);
                if (actual_damage <= 0 || ex_protected_monster(mon)
                    || 100L * mon->mhp > (unique ? 15L : 25L) * mon->mhpmax) continue;
                chance = unique ? 10 : 20;
            }
            if (prop == EX_DISARM && (!(wep = MON_WEP(mon)) || wep->cursed
                                     || ex_protected_object(wep))) continue;
            if (prop == EX_POLYMORPH
                && (ex_protected_monster(mon) || (mon->data->geno & G_UNIQ))) continue;
            if (!ex_combat_chance(chance)) continue;
            switch (prop) {
            case EX_EXECUTION:
                mon->mhp = 0; /* F4: fatal, never send through DR again */
                killed(mon);
                break;
            case EX_STEAL: ex_steal(mon); break;
            case EX_DISARM:
                wep = MON_WEP(mon);
                if (wep && !wep->cursed && !ex_protected_object(wep)) mdrop_obj(mon, wep, TRUE);
                break;
            case EX_POLYMORPH:
#ifdef STEP15_TEST
                ++ex_test_poly_attempts;
#endif
                if (!resists_magm(mon) && !resist(mon, SPBOOK_CLASS, 0, NOTELL)
                    && (candidate = ex_poly_candidate(mon)) != NON_PM) {
                    int transformed = newcham(mon, &mons[candidate], NC_SHOW_MSG);
#ifdef STEP15_TEST
                    if (transformed) ++ex_test_poly_changes;
#else
                    nhUse(transformed);
#endif
                }
                break;
            case EX_MUTE: ex_apply_mute(mon); break;
            case EX_ANTIREGEN: mon->ex_regen_until = svm.moves + 5; break;
            }
        }
    }
    ex_run_chains(ctx);
}
