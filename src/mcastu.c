/* NetHack 5.0	mcastu.c	$NHDT-Date: 1781973053 2026/06/20 16:30:53 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.122 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2011. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

#define MCASTU_ENUM
enum mcast_spells {
    #include "mcastu.h"
};
#undef MCASTU_ENUM

struct _mcast_data {
    int level;
    int flags;
};

#define MCASTU_INIT
static struct _mcast_data mcast_data[] = {
    #include "mcastu.h"
};
#undef MCASTU_INIT

/* spell lists for specific monster casters */
/* the spells in the list should be in ascending level order */
static int mon_cleric_spells[] = {
    MCAST_OPEN_WOUNDS, MCAST_CURE_SELF, MCAST_CONFUSE_YOU, MCAST_PARALYZE,
    MCAST_BLIND_YOU, MCAST_INSECTS, MCAST_CURSE_ITEMS, MCAST_LIGHTNING,
    MCAST_FIRE_PILLAR, MCAST_GEYSER
};
static int mon_wizard_spells[] = {
    MCAST_PSI_BOLT, MCAST_CURE_SELF, MCAST_HASTE_SELF, MCAST_STUN_YOU,
    MCAST_DISAPPEAR, MCAST_WEAKEN_YOU, MCAST_DESTRY_ARMR, MCAST_CURSE_ITEMS,
    MCAST_AGGRAVATION, MCAST_SUMMON_MONS, MCAST_CLONE_WIZ, MCAST_DEATH_TOUCH
};

staticfn void cursetxt(struct monst *, boolean);
staticfn int choose_monster_spell(struct monst *, int);
staticfn int m_cure_self(struct monst *, int);
staticfn void mcast_death_touch(struct monst *);
staticfn void mcast_clone_wiz(struct monst *);
staticfn void mcast_summon_mons(struct monst *);
staticfn void mcast_destroy_armor(void);
staticfn void mcast_weaken_you(struct monst *, int);
staticfn void mcast_disappear(struct monst *);
staticfn void mcast_stun_you(int);
staticfn int mcast_geyser(int);
staticfn int mcast_fire_pillar(struct monst *, int);
staticfn int mcast_lightning(struct monst *, int);
staticfn int mcast_psi_bolt(int);
staticfn int mcast_open_wounds(int);
staticfn void mcast_insects(struct monst *);
staticfn void mcast_blind_you(void);
staticfn int mcast_paralyze(struct monst *);
staticfn void mcast_confuse_you(struct monst *);
staticfn void mcast_spell(struct monst *, int, int);
staticfn boolean is_undirected_spell(int);
staticfn boolean spell_would_be_useless(struct monst *, int);

/* feedback when frustrated monster couldn't cast a spell */
staticfn void
cursetxt(struct monst *mtmp, boolean undirected)
{
    if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
        const char *point_msg; /* spellcasting monsters are impolite */

        if (undirected)
            point_msg = "all around, then curses";
        else if ((Invis && !perceives(mtmp->data)
                  && (mtmp->mux != u.ux || mtmp->muy != u.uy))
                 || is_obj_mappear(&gy.youmonst, STRANGE_OBJECT)
                 || u.uundetected)
            point_msg = "and curses in your general direction";
        else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
            point_msg = "and curses at your displaced image";
        else
            point_msg = "at you, then curses";

        pline_mon(mtmp, "%s points %s.", Monnam(mtmp), point_msg);
    } else if ((!(svm.moves % 4) || !rn2(4))) {
        if (!Deaf)
            Norep("You hear a mumbled curse.");   /* Deaf-aware */
    }
}

/* choose a spell for monster to cast */
staticfn int
mith_elder_spell(void)
{
    static const int spells[] = {
        MCAST_DISAPPEAR, MCAST_CONFUSE_YOU, MCAST_BLIND_YOU,
        MCAST_MITH_SLEEP, MCAST_MITH_CURE_FAR, MCAST_MITH_CURE_CLOSE,
        MCAST_AGGRAVATION, MCAST_MITH_CURE_FAR
    };

    /* choose_magic_special first tests its unrelated 50% favored list. */
    (void) rn2(2);
    return spells[rnd(8) - 1];
}

staticfn int
mith_spell_damage(struct monst *mtmp, struct attack *mattk)
{
    int die = mattk->damd ? mattk->damd : 6;

    if (mith_mon_syllable(mtmp) == MITH_KRAU)
        die = die * 3 / 2;
    /* Pinned spell.h MAX_BONUS_DICE is 10; the later cap of 15 in
       donor castmu never raises that limit. */
    return d(min(10, mtmp->m_lev / 3 + 1) + mattk->damn, die);
}

int
step10b_spell_cooldown(const struct permonst *ptr, int normal)
{
    return ptr == &mons[PM_AURUMACH_RILMANI] || ptr == &mons[PM_KUKER]
               || ptr == &mons[PM_WITCH_S_FAMILIAR]
               || (ptr >= &mons[PM_APPRENTICE_WITCH]
                   && ptr <= &mons[PM_HMNYW_PHARAOH])
               ? 0 : normal;
}

int
step10b_mon_spell_fumble_threshold(enum step10b_level_context context,
                                   int native_threshold)
{
    switch (context) {
    case STEP10B_CTX_GATE:       return native_threshold + 2;
    case STEP10B_CTX_OUTLANDS_1: return native_threshold + 4;
    case STEP10B_CTX_OUTLANDS_2: return native_threshold + 6;
    case STEP10B_CTX_OUTLANDS_3: return native_threshold + 8;
    case STEP10B_CTX_OUTLANDS_4: return native_threshold + 10;
    case STEP10B_CTX_SUM:        return native_threshold - 1;
    default:                     return native_threshold;
    }
}

boolean
step10b_mon_spell_always_fumbles(enum step10b_level_context context)
{
    return (boolean) (context == STEP10B_CTX_SPIRE);
}

int
step10b_species_spell(const struct permonst *ptr, int favored, int detail)
{
    static const int cuprilach[] = {
        MCAST_RILMANI_DRAIN_LIFE, MCAST_RILMANI_ACID_BLAST,
        MCAST_RILMANI_SOLID_FOG, MCAST_DISAPPEAR,
        MCAST_RILMANI_POISON_GAS, MCAST_RILMANI_MAKE_VISIBLE
    };
    static const int argenach[] = {
        MCAST_RILMANI_ICE_STORM, MCAST_RILMANI_SOLID_FOG,
        MCAST_DISAPPEAR, MCAST_RILMANI_MAKE_VISIBLE
    };
    static const int aurumach[] = {
        MCAST_RILMANI_ICE_STORM, MCAST_RILMANI_ACID_RAIN,
        MCAST_RILMANI_SOLID_FOG, MCAST_DISAPPEAR,
        MCAST_RILMANI_POISON_GAS, MCAST_RILMANI_MAKE_VISIBLE,
        MCAST_RILMANI_PRISMATIC_SPRAY
    };
    static const int kuker[] = {
        MCAST_CONFUSE_YOU, MCAST_RILMANI_MAKE_VISIBLE,
        MCAST_KUKER_EVIL_EYE, MCAST_CURSE_ITEMS,
        MCAST_KUKER_PROTECTION, MCAST_PUNISHMENT
    };

    if (ptr == &mons[PM_AMM_KAMEREL]
        || ptr == &mons[PM_HUDOR_KAMEREL]
        || ptr == &mons[PM_ARA_KAMEREL])
        return MCAST_OPEN_WOUNDS;
    if (ptr == &mons[PM_SHARAB_KAMEREL])
        return MCAST_PSI_BOLT;
    if (ptr == &mons[PM_PLUMACH_RILMANI])
        return MCAST_RILMANI_SOLID_FOG;
    if (ptr == &mons[PM_FERRUMACH_RILMANI])
        return favored ? MCAST_RILMANI_HAIL_FLURY
                       : MCAST_RILMANI_SOLID_FOG;
    if (ptr == &mons[PM_CUPRILACH_RILMANI])
        return detail >= 0 && detail < SIZE(cuprilach) ? cuprilach[detail] : -1;
    if (ptr == &mons[PM_ARGENACH_RILMANI])
        return favored ? MCAST_RILMANI_SILVER_RAYS
                       : detail >= 0 && detail < SIZE(argenach)
                             ? argenach[detail] : -1;
    if (ptr == &mons[PM_AURUMACH_RILMANI]
        || ptr == &mons[PM_CENTER_OF_ALL])
        return favored ? MCAST_RILMANI_GOLDEN_WAVE
                       : detail >= 0 && detail < SIZE(aurumach)
                             ? aurumach[detail] : -1;
    if (ptr == &mons[PM_KUKER])
        return detail >= 0 && detail < SIZE(kuker) ? kuker[detail] : -1;
    if (ptr == &mons[PM_STAR_SPAWN])
        return MCAST_PSI_BOLT;
    if (ptr == &mons[PM_WITCH_S_FAMILIAR])
        return MCAST_OPEN_WOUNDS;
    return -1;
}

staticfn int
step10b_choose_species_spell(const struct permonst *ptr)
{
    if (ptr == &mons[PM_FERRUMACH_RILMANI])
        return step10b_species_spell(ptr, rn2(4), 0);
    if (ptr == &mons[PM_CUPRILACH_RILMANI])
        return step10b_species_spell(ptr, 0, rn2(6));
    if (ptr == &mons[PM_ARGENACH_RILMANI]) {
        int favored = rn2(4);

        return step10b_species_spell(ptr, favored, favored ? 0 : rn2(4));
    }
    if (ptr == &mons[PM_AURUMACH_RILMANI]
        || ptr == &mons[PM_CENTER_OF_ALL]) {
        int favored = rn2(4);

        return step10b_species_spell(ptr, favored, favored ? 0 : rn2(7));
    }
    if (ptr == &mons[PM_KUKER])
        return step10b_species_spell(ptr, 0, rn2(6));
    return step10b_species_spell(ptr, 0, 0);
}

staticfn void
mith_mass_cure(struct monst *mtmp, boolean far, coordxy tx, coordxy ty)
{
    struct monst *cmon;
    int n = min(10, mtmp->m_lev / 3 + 1);
    coordxy x = far && (tx || ty) ? tx : mtmp->mx,
            y = far && (tx || ty) ? ty : mtmp->my;

    for (cmon = fmon; cmon; cmon = cmon->nmon) {
        if ((!far || cmon != mtmp) && !DEADMONSTER(cmon)
            && cmon->mhp < cmon->mhpmax
            && cmon->mpeaceful == mtmp->mpeaceful
            && dist2(x, y, cmon->mx, cmon->my) <= 10) {
            cmon->mhp += d(n, 8);
            if (cmon->mhp > cmon->mhpmax)
                cmon->mhp = cmon->mhpmax;
            if (canseemon(cmon))
                pline_mon(cmon, "%s looks better.", Monnam(cmon));
        }
    }
    if (mtmp->mtame && dist2(x, y, u.ux, u.uy) <= 10
        && (Upolyd ? u.mh < u.mhmax : u.uhp < u.uhpmax)) {
        healup(d(n, 8), 0, FALSE, FALSE);
        You_feel("better.");
    }
}

staticfn boolean
mith_mm_useless(struct monst *caster, struct monst *target, int spell)
{
    struct monst *other;

    switch (spell) {
    case MCAST_CLONE_WIZ:
        return TRUE;
    case MCAST_SUMMON_MONS:
        return caster->mpeaceful || caster->mtame;
    case MCAST_CURE_SELF:
        return caster->mhp == caster->mhpmax;
    case MCAST_HASTE_SELF:
        return caster->permspeed == MFAST;
    case MCAST_DISAPPEAR:
        return caster->minvis || caster->invis_blkd
               || (caster->mtame && !See_invisible);
    case MCAST_BLIND_YOU:
        return !haseyes(target->data) || target->mblinded;
    case MCAST_MITH_SLEEP:
        return !linedup(target->mx, target->my, caster->mx, caster->my, 0);
    case MCAST_MITH_CURE_CLOSE:
    case MCAST_MITH_CURE_FAR:
        if (caster->mhp < caster->mhpmax
            || (caster->mtame && (Upolyd ? u.mh < u.mhmax : u.uhp < u.uhpmax)))
            return FALSE;
        for (other = fmon; other; other = other->nmon)
            if (!DEADMONSTER(other) && other->mpeaceful == caster->mpeaceful
                && other->mhp < other->mhpmax)
                return FALSE;
        return TRUE;
    default:
        return FALSE;
    }
}

staticfn int
mith_mm_choose_spell(struct monst *caster, struct monst *target)
{
    int i, value, special;
    int maxlev = mcast_data[mon_wizard_spells[SIZE(mon_wizard_spells)-1]].level;

    if (caster->data == &mons[PM_ALABASTER_ELF_ELDER])
        return mith_elder_spell();
    special = step10b_choose_species_spell(caster->data);
    if (special >= 0)
        return special;
    /* Mummies retain the native general wizard list. Its usefulness checks
     * must inspect the actual monster target, not the hero's properties. */
    value = rn2(caster->m_lev);
    if (value > maxlev && rn2(maxlev))
        value = rn2(maxlev);
    for (i = SIZE(mon_wizard_spells) - 1; i >= 0; --i)
        if (mcast_data[mon_wizard_spells[i]].level <= value
            && !mith_mm_useless(caster, target, mon_wizard_spells[i]))
            return mon_wizard_spells[i];
    return MCAST_PSI_BOLT;
}

staticfn void
mith_mm_curse(struct monst *target)
{
    struct obj *obj;
    int count = 0, tries, index;
    boolean resists = resist(target, 0, 0, FALSE) != 0;

    if (MON_WEP(target) && MON_WEP(target)->oartifact == ART_MAGICBANE && rn2(20))
        return;
    for (obj = target->minvent; obj; obj = obj->nobj)
        if (obj->oclass != COIN_CLASS)
            ++count;
    if (!count)
        return;
    if (resists)
        shieldeff(target->mx, target->my);
    for (tries = rnd(6 / (resists + 1)); tries > 0; --tries) {
        index = rnd(count);
        for (obj = target->minvent; obj; obj = obj->nobj)
            if (obj->oclass != COIN_CLASS && !--index)
                break;
        if (!obj || obj->cursed
            || (obj->oartifact && spec_ability(obj, SPFX_INTEL) && rn2(10) < 8))
            continue;
        if (obj->blessed)
            unbless(obj);
        else
            curse(obj);
    }
}

/* Native combat has no general castmm. Keep the new entry point confined to
 * imported elders and syllable mummies, with no new native pet spellcasting. */
int
mith_castmm(struct monst *caster, struct monst *target, struct attack *attack)
{
    int spell = 0, tries, damage, result = M_ATTK_HIT;
    int syllable = mith_mon_syllable(caster);
    boolean lake = caster->data >= &mons[PM_FLASHING_LAKE]
                   && caster->data <= &mons[PM_SPARKLING_LAKE];
    struct monst *other;
    struct obj *armor;

    /* Native combat has no elemental castmm path.  The four lakes use their
       declared AT_MAGC attack against monsters with the same level-scaled
       dice and full resistance handling used against the hero. */
    if (lake) {
        boolean resisted;

        if (!caster->m_lev || caster->mcan || DEADMONSTER(caster)
            || DEADMONSTER(target) || helpless(caster)
            || (attack->adtyp != AD_FIRE && attack->adtyp != AD_COLD
                && attack->adtyp != AD_ELEC && attack->adtyp != AD_MAGM))
            return M_ATTK_MISS;
        if (rn2(caster->m_lev * 10) < (caster->mconf ? 100 : 20))
            return M_ATTK_MISS;
        damage = mith_spell_damage(caster, attack);
        resisted = attack->adtyp == AD_FIRE ? resists_fire(target)
                   : attack->adtyp == AD_COLD ? resists_cold(target)
                   : attack->adtyp == AD_ELEC ? resists_elec(target)
                                              : resists_magm(target);
        if (resisted) {
            shieldeff(target->mx, target->my);
            damage = 0;
        }
        if (canseemon(caster))
            pline_mon(caster, "%s casts an elemental spell!", Monnam(caster));
        if (damage > 0 && enhancement_mon_damage(target, caster, damage, 0) <= 0) {
            monkilled(target, "", (int) attack->adtyp);
            if (DEADMONSTER(target))
                result |= M_ATTK_DEF_DIED;
        }
        if (DEADMONSTER(caster)) result |= M_ATTK_AGR_DIED;
        return result;
    }

    if ((caster->data != &mons[PM_ALABASTER_ELF_ELDER]
         && caster->data != &mons[PM_OGRE_MAGE]
         && caster->data != &mons[PM_PLUMACH_RILMANI]
         && caster->data != &mons[PM_FERRUMACH_RILMANI]
         && caster->data != &mons[PM_CUPRILACH_RILMANI]
         && caster->data != &mons[PM_ARGENACH_RILMANI]
         && caster->data != &mons[PM_AURUMACH_RILMANI]
         && caster->data != &mons[PM_AMM_KAMEREL]
         && caster->data != &mons[PM_HUDOR_KAMEREL]
         && caster->data != &mons[PM_SHARAB_KAMEREL]
         && caster->data != &mons[PM_ARA_KAMEREL]
         && caster->data != &mons[PM_KUKER]
         && caster->data != &mons[PM_STAR_SPAWN]
         && caster->data != &mons[PM_WITCH_S_FAMILIAR]
         && !step10b_is_witch(caster->data) && syllable < 0)
        || (attack->adtyp != AD_SPEL && attack->adtyp != AD_CLRC
            && attack->adtyp != AD_PSON) || !caster->m_lev
        || DEADMONSTER(caster) || DEADMONSTER(target) || helpless(caster))
        return M_ATTK_MISS;
    for (tries = 0; tries < 40; ++tries) {
        spell = mith_mm_choose_spell(caster, target);
        if (!mith_mm_useless(caster, target, spell))
            break;
    }
    if (tries == 40 || caster->mcan
        || (caster->mspec_used && syllable != MITH_NAEN)
        || step10b_witch_needs_familiar(caster))
        return M_ATTK_MISS;
    caster->mspec_used = syllable == MITH_NAEN ? 0
                        : caster->m_lev < 8 ? 10 - caster->m_lev : 2;
    caster->mspec_used = step10b_spell_cooldown(caster->data,
                                                caster->mspec_used);
    if (syllable >= 0
        ? rn2(caster->m_lev * 2) < ((syllable == MITH_NAEN ? 0 : 2)
                                  + (caster->mconf ? 8 : 0))
        : rn2(caster->m_lev * 10) < (caster->mconf ? 100 : 20))
        return M_ATTK_MISS;
    if (canseemon(caster))
        pline_mon(caster, "%s casts a spell!", Monnam(caster));
    /* Donor castmm rolls its basic spell dice even for a support spell. */
    damage = mith_spell_damage(caster, attack);
    switch (spell) {
    case MCAST_PSI_BOLT:
        if (canseemon(target))
            pline_mon(target, "A psychic bolt strikes %s!", mon_nam(target));
        if (resists_magm(target) || resist(target, 0, 0, FALSE))
            damage = (damage + 1) / 2;
        break;
    case MCAST_CURE_SELF:
        (void) m_cure_self(caster, 0);
        damage = 0;
        break;
    case MCAST_HASTE_SELF:
        mon_adjust_speed(caster, 1, (struct obj *) 0);
        damage = 0;
        break;
    case MCAST_STUN_YOU:
        if (resists_magm(target) || resist(target, 0, 0, FALSE))
            shieldeff(target->mx, target->my);
        else {
            target->mstun = 1;
            if (canseemon(target))
                pline_mon(target, "%s reels!", Monnam(target));
        }
        damage = 0;
        break;
    case MCAST_WEAKEN_YOU:
        if (resists_magm(target) || resist(target, 0, 0, FALSE)) {
            shieldeff(target->mx, target->my);
            damage = 0;
        } else {
            damage = rnd(4) * 5;
            target->mhpmax = max(1, target->mhpmax - damage);
            if (canseemon(target))
                pline_mon(target, "%s suddenly seems weaker!", Monnam(target));
        }
        break;
    case MCAST_DESTRY_ARMR:
        if (resists_magm(target) || resist(target, 0, 0, FALSE)) {
            shieldeff(target->mx, target->my);
        } else if ((armor = some_armor(target)) != 0
                   && objects[armor->otyp].oc_oprop != DISINT_RES
                   && armor->otyp != CHROMATIC_DRAGON_SCALES
                   && armor->otyp != CHROMATIC_DRAGON_SCALE_MAIL
                   && !is_quest_artifact(armor) && !obj_resists(armor, 0, 90)) {
            if (canseemon(target))
                pline_mon(target, "%s armor crumbles!", s_suffix(mon_nam(target)));
            m_useupall(target, armor);
        }
        damage = 0;
        break;
    case MCAST_CURSE_ITEMS:
        mith_mm_curse(target);
        damage = 0;
        break;
    case MCAST_KUKER_EVIL_EYE:
        target->mconf = 1;
        damage = 0;
        break;
    case MCAST_KUKER_PROTECTION:
        caster->mconf = caster->mstun = 0;
        caster->mhp = min(caster->mhpmax, caster->mhp + d(2, 6));
        damage = 0;
        break;
    case MCAST_SUMMON_MONS: {
        /* Native nasty() reads data and remembered target from its caster.
         * Supply a target proxy without changing the real hero memory. */
        struct monst proxy = *caster;

        proxy.mux = target->mx;
        proxy.muy = target->my;
        (void) nasty(&proxy);
        damage = 0;
        break;
    }
    case MCAST_DEATH_TOUCH:
        damage = 0;
        if (!nonliving(target->data) && !is_demon(target->data)
            && !step20_celestial(target->data)
            && (!(resists_magm(target) || resist(target, 0, 0, FALSE))
                || rn2(caster->m_lev) > 12))
            damage = target->mhp;
        break;
    case MCAST_DISAPPEAR:
        mcast_disappear(caster);
        damage = 0;
        break;
    case MCAST_CONFUSE_YOU:
        if (resists_magm(target) || resist(target, 0, 0, FALSE)) {
            shieldeff(target->mx, target->my);
        } else {
            target->mconf = 1;
            if (canseemon(target))
                pline_mon(target, "%s seems confused!", Monnam(target));
        }
        damage = 0;
        break;
    case MCAST_BLIND_YOU:
        if (!resists_blnd(target)) {
            target->mblinded = 127;
            target->mcansee = 0;
            if (canseemon(target))
                pline_mon(target, "Scales cover %s eyes!", s_suffix(mon_nam(target)));
        }
        damage = 0;
        break;
    case MCAST_MITH_SLEEP:
        if (linedup(target->mx, target->my, caster->mx, caster->my, 0)) {
            gb.buzzer = caster;
            dobuzz(BZ_M_SPELL(BZ_OFS_AD(AD_SLEE)), min(10, caster->m_lev / 3 + 1),
                   caster->mx, caster->my, sgn(gt.tbx), sgn(gt.tby),
                   FALSE, FALSE, FALSE);
            gb.buzzer = 0;
        }
        damage = 0;
        break;
    case MCAST_MITH_CURE_CLOSE:
    case MCAST_MITH_CURE_FAR:
        mith_mass_cure(caster, spell == MCAST_MITH_CURE_FAR, target->mx, target->my);
        damage = 0;
        break;
    case MCAST_AGGRAVATION:
        for (other = fmon; other; other = other->nmon)
            if (other != target && !DEADMONSTER(other)
                && other->mpeaceful != target->mpeaceful) {
                other->msleeping = 0;
                if (!other->mcanmove && !rn2(5)) {
                    other->mfrozen = 0;
                    other->mcanmove = 1;
                }
            }
        if (canseemon(target))
            pline_mon(target, "%s draws attention!", Monnam(target));
        damage = 0;
        break;
    case MCAST_RILMANI_DRAIN_LIFE:
        if (resists_drli(target))
            damage = 0;
        else
            damage = max(damage, target->m_lev + 1);
        break;
    case MCAST_RILMANI_ACID_BLAST:
        damage = min(60, damage);
        if (resists_acid(target))
            damage = 0;
        break;
    case MCAST_RILMANI_SOLID_FOG:
        target->mblinded = max(target->mblinded, 8);
        target->mcansee = 0;
        mon_adjust_speed(target, -1, (struct obj *) 0);
        if (caster->data == &mons[PM_PLUMACH_RILMANI])
            caster->mcan = 1;
        damage = 0;
        break;
    case MCAST_RILMANI_POISON_GAS:
        (void) create_gas_cloud(target->mx, target->my, rnd(3), rnd(3) + 1);
        damage = 0;
        break;
    case MCAST_RILMANI_MAKE_VISIBLE:
        target->minvis = target->perminvis = 0;
        newsym(target->mx, target->my);
        damage = 0;
        break;
    case MCAST_RILMANI_HAIL_FLURY:
    case MCAST_RILMANI_ICE_STORM:
        if (resists_cold(target))
            damage = (damage + 1) / 2;
        break;
    case MCAST_RILMANI_SILVER_RAYS:
        damage = d(2, 20);
        if (resists_fire(target) && resists_cold(target)
            && resists_elec(target) && resists_acid(target))
            damage = max(1, damage / 2);
        break;
    case MCAST_RILMANI_GOLDEN_WAVE:
        damage = d(2, 12);
        if (resists_fire(target) && resists_cold(target)
            && resists_elec(target) && resists_acid(target))
            damage = max(1, damage / 2);
        break;
    case MCAST_RILMANI_ACID_RAIN:
        damage = resists_acid(target) ? 0 : d(8, 6);
        break;
    case MCAST_RILMANI_PRISMATIC_SPRAY:
        switch (rn2(6)) {
        case 0: if (resists_fire(target)) damage = 0; break;
        case 1: if (resists_cold(target)) damage = 0; break;
        case 2: if (resists_elec(target)) damage = 0; break;
        case 3: if (resists_acid(target)) damage = 0; break;
        case 4: if (resists_poison(target)) damage = 0; break;
        default: if (resists_magm(target)) damage = 0; break;
        }
        break;
    }
    if (damage > 0 && !DEADMONSTER(target)) {
        enhancement_mon_damage(target, caster, damage, 0);
        if (target->mhp <= 0)
            monkilled(target, "", AD_SPEL);
    }
    if (DEADMONSTER(target))
        result |= M_ATTK_DEF_DIED;
    if (DEADMONSTER(caster))
        result |= M_ATTK_AGR_DIED;
    return result;
}

staticfn int
choose_monster_spell(struct monst *mtmp, int adtyp)
{
    int *list = NULL;
    int i, spellval, special, len = 0;
    int maxlev;

    if (mtmp->data == &mons[PM_ALABASTER_ELF_ELDER] && adtyp == AD_SPEL)
        return mith_elder_spell();
    special = (adtyp == AD_CLRC || adtyp == AD_SPEL || adtyp == AD_PSON)
                  ? step10b_choose_species_spell(mtmp->data) : -1;
    if (special >= 0)
        return special;

    if (adtyp == AD_PUNI) {
        static const int punisher_spells[] = {
            MCAST_OPEN_WOUNDS, MCAST_PSI_BOLT, MCAST_HASTE_SELF,
            MCAST_PARALYZE, MCAST_GEYSER, MCAST_FIRE_PILLAR,
            MCAST_SUMMON_MONS, MCAST_AGGRAVATION, MCAST_DEATH_TOUCH,
            MCAST_PUNISHMENT
        };
        int selected;
        do {
            selected = punisher_spells[rn2(SIZE(punisher_spells))];
        } while (selected == MCAST_SUMMON_MONS && rn2(3));
        return selected;
    }
    /* which spell list to use? */
    if (adtyp == AD_SPEL) {
        list = mon_wizard_spells;
        len = SIZE(mon_wizard_spells);
    } else if (adtyp == AD_CLRC) {
        list = mon_cleric_spells;
        len = SIZE(mon_cleric_spells);
    }

    if (!list || len < 1)
        return MCAST_PSI_BOLT;

    /* max spell level in this monster spell list */
    maxlev = mcast_data[list[len - 1]].level;

    /* which level spell to cast? */
    spellval = rn2(mtmp->m_lev);
    if (spellval > maxlev && rn2(maxlev))
        spellval = rn2(maxlev);

    /* find the highest spell in the list we could cast */
    for (i = len-1; i >= 0; i--)
        if (mcast_data[list[i]].level <= spellval
            && !spell_would_be_useless(mtmp, list[i]))
            return list[i];

    /* or return the first spell in the list */
    return list[0];
}

/* return values:
 * 1: successful spell
 * 0: unsuccessful spell
 */
int
castmu(
    struct monst *mtmp,   /* caster */
    struct attack *mattk, /* caster's current attack */
    boolean thinks_it_foundyou,    /* might be mistaken if displaced */
    boolean foundyou)              /* knows hero's precise location */
{
    int dmg, ml = mtmp->m_lev;
    int ret;
    int spellnum = 0;

    /* Three cases:
     * -- monster is attacking you.  Search for a useful spell.
     * -- monster thinks it's attacking you.  Search for a useful spell,
     *    without checking for undirected.  If the spell found is directed,
     *    it fails with cursetxt() and loss of mspec_used.
     * -- monster isn't trying to attack.  Select a spell once.  Don't keep
     *    searching; if that spell is not useful (or if it's directed),
     *    return and do something else.
     * Since most spells are directed, this means that a monster that isn't
     * attacking casts spells only a small portion of the time that an
     * attacking monster does.
     */
    if ((mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC
         || mattk->adtyp == AD_PSON
         || mattk->adtyp == AD_PUNI) && ml) {
        int cnt = 40;

        do {
            spellnum = choose_monster_spell(mtmp, mattk->adtyp);
            /* not trying to attack?  don't allow directed spells */
            if (!thinks_it_foundyou) {
                if (!is_undirected_spell(spellnum)
                    || spell_would_be_useless(mtmp, spellnum)) {
                    if (foundyou)
                        impossible(
                       "spellcasting monster found you and doesn't know it?");
                    return M_ATTK_MISS;
                }
                break;
            }
        } while (--cnt > 0
                 && spell_would_be_useless(mtmp, spellnum));
        if (cnt == 0)
            return M_ATTK_MISS;
    }

    /* monster unable to cast spells? */
    if (mtmp->mcan || (mtmp->mspec_used
                       && mith_mon_syllable(mtmp) != MITH_NAEN) || !ml
        || step10b_witch_needs_familiar(mtmp)
        || m_seenres(mtmp, cvt_adtyp_to_mseenres(mattk->adtyp))) {
        cursetxt(mtmp, is_undirected_spell(spellnum));
        return M_ATTK_MISS;
    }

    debugpline3("castmu:%s,lvl:%i,spell:%i", noit_Monnam(mtmp), ml, spellnum);

    if (mattk->adtyp == AD_SPEL || mattk->adtyp == AD_CLRC
         || mattk->adtyp == AD_PSON
         || mattk->adtyp == AD_PUNI) {
        /* monst->m_lev is unsigned (uchar), monst->mspec_used is int */
        mtmp->mspec_used = (int) ((mtmp->m_lev < 8) ? (10 - mtmp->m_lev) : 2);
        if (mith_mon_syllable(mtmp) == MITH_NAEN)
            mtmp->mspec_used = 0;
        mtmp->mspec_used = step10b_spell_cooldown(mtmp->data,
                                                  mtmp->mspec_used);
    }

    /* Monster can cast spells, but is casting a directed spell at the
     * wrong place?  If so, give a message, and return.
     * Do this *after* penalizing mspec_used.
     *
     * FIXME?
     *  Shouldn't wall of lava have a case similar to wall of water?
     *  And should cold damage hit water or lava instead of missing
     *  even when the caster has targeted the wrong spot?  Likewise
     *  for fire mis-aimed at ice.
     */
    if (!foundyou && thinks_it_foundyou
        && !is_undirected_spell(spellnum)) {
        pline_mon(mtmp, "%s casts a spell at %s!",
                 canseemon(mtmp) ? Monnam(mtmp) : "Something",
                 is_waterwall(mtmp->mux, mtmp->muy) ? "empty water"
                                                    : "thin air");
        return M_ATTK_MISS;
    }

    nomul(0);
    {
        enum step10b_level_context context = step10c_level_context(&u.uz);
        int native_threshold = mith_mon_syllable(mtmp) >= 0
                               ? ((mith_mon_syllable(mtmp) == MITH_NAEN
                                   ? 0 : 2) + (mtmp->mconf ? 8 : 0))
                               : (mtmp->mconf ? 100 : 20);
        int roll = 0;
        /* Step 10C-D supplies the real production level identity. */
        boolean fumbled = step10b_mon_spell_always_fumbles(context);

        if (!fumbled) {
            roll = rn2(ml * (mith_mon_syllable(mtmp) >= 0 ? 2 : 10));
            fumbled = roll < step10b_mon_spell_fumble_threshold(
                                  context, native_threshold);
        }

        if (fumbled) { /* fumbled attack */
            Soundeffect(se_air_crackles, 60);
            if (canseemon(mtmp) && !Deaf) {
                set_msg_xy(mtmp->mx, mtmp->my);
                pline_The("air crackles around %s.", mon_nam(mtmp));
            }
            return M_ATTK_MISS;
        }
    }
    if (canspotmon(mtmp) || !is_undirected_spell(spellnum)) {
        pline_mon(mtmp, "%s casts a spell%s!",
                 canspotmon(mtmp) ? Monnam(mtmp) : "Something",
                 is_undirected_spell(spellnum) ? ""
                 : (Invis && !perceives(mtmp->data)
                    && !u_at(mtmp->mux, mtmp->muy))
                   ? " at a spot near you"
                   : (Displaced && !u_at(mtmp->mux, mtmp->muy))
                     ? " at your displaced image"
                     : " at you");
    }

    /*
     * As these are spells, the damage is related to the level
     * of the monster casting the spell.
     */
    if (!foundyou) {
        dmg = 0;
        if (mattk->adtyp != AD_SPEL && mattk->adtyp != AD_CLRC
            && mattk->adtyp != AD_PSON
            && mattk->adtyp != AD_PUNI) {
            impossible(
              "%s casting non-hand-to-hand version of hand-to-hand spell %d?",
                       Monnam(mtmp), mattk->adtyp);
            return M_ATTK_MISS;
        }
    } else if (mith_mon_syllable(mtmp) >= 0
               || mtmp->data == &mons[PM_ALABASTER_ELF_ELDER]
               || mtmp->data == &mons[PM_OGRE_MAGE]
               || mtmp->data == &mons[PM_PLUMACH_RILMANI]
               || mtmp->data == &mons[PM_FERRUMACH_RILMANI]
               || mtmp->data == &mons[PM_CUPRILACH_RILMANI]
               || mtmp->data == &mons[PM_ARGENACH_RILMANI]
               || mtmp->data == &mons[PM_AURUMACH_RILMANI]
               || mtmp->data == &mons[PM_AMM_KAMEREL]
               || mtmp->data == &mons[PM_HUDOR_KAMEREL]
               || mtmp->data == &mons[PM_SHARAB_KAMEREL]
               || mtmp->data == &mons[PM_ARA_KAMEREL]) {
        dmg = mith_spell_damage(mtmp, mattk);
    } else if (mattk->damd)
        dmg = d((int) ((ml / 2) + mattk->damn), (int) mattk->damd);
    else
        dmg = d((int) ((ml / 2) + 1), 6);
    if (Half_spell_damage)
        dmg = (dmg + 1) / 2;
    if (u.mith_timers[MITH_VAUL])
        dmg = (dmg + 1) / 2;

    ret = M_ATTK_HIT;
    /*
     * FIXME: none of these hit the steed when hero is riding, nor do
     *  they inflict damage on carried items.
     */
    switch (mattk->adtyp) {
    case AD_FIRE:
        pline("You're enveloped in flames.");
        if (Fire_resistance) {
            shieldeff(u.ux, u.uy);
            pline("But you resist the effects.");
            monstseesu(M_SEEN_FIRE);
            dmg = 0;
        } else {
            monstunseesu(M_SEEN_FIRE);
        }
        burn_away_slime();
        /* burn up flammable items on the floor, melt ice terrain */
        mon_spell_hits_spot(mtmp, AD_FIRE, u.ux, u.uy);
        break;
    case AD_COLD:
        pline("You're covered in frost.");
        if (Cold_resistance) {
            shieldeff(u.ux, u.uy);
            pline("But you resist the effects.");
            monstseesu(M_SEEN_COLD);
            dmg = 0;
        } else {
            monstunseesu(M_SEEN_COLD);
        }
        /* freeze water or lava terrain */
        /* FIXME: mon_spell_hits_spot() uses zap_over_floor(); unlike with
         * fire, it does not target susceptible floor items with cold */
        mon_spell_hits_spot(mtmp, AD_COLD, u.ux, u.uy);
        break;
    case AD_MAGM:
        You("are hit by a shower of missiles!");
        if (Antimagic) {
            shieldeff(u.ux, u.uy);
            pline_The("missiles bounce off!");
            monstseesu(M_SEEN_MAGR);
            dmg = 0;
        } else {
            dmg = d((int) mtmp->m_lev / 2 + 1, 6);
            if (u.mith_timers[MITH_VAUL])
                dmg = (dmg + 1) / 2;
            monstunseesu(M_SEEN_MAGR);
        }
        /* shower of magic missiles scuffs an engraving */
        mon_spell_hits_spot(mtmp, AD_MAGM, u.ux, u.uy);
        break;
    case AD_PUNI: /* Sheol Punisher */
    case AD_PSON: /* Star Spawn pinned psionic spell */
    case AD_SPEL: /* wizard spell */
    case AD_CLRC: /* clerical spell */
        mcast_spell(mtmp, dmg, spellnum);
        dmg = 0; /* done by the spell casting functions */
        break;
    } /* switch */
    if (dmg)
        mdamageu_damage(mtmp, dmg, 0);
    if (DEADMONSTER(mtmp)) ret |= M_ATTK_AGR_DIED;
    return ret;
}

staticfn int
m_cure_self(struct monst *mtmp, int dmg)
{
    if (mtmp->mhp < mtmp->mhpmax) {
        if (canseemon(mtmp))
            pline_mon(mtmp, "%s looks better.", Monnam(mtmp));
        /* note: player healing does 6d4; this used to do 1d8 */
        healmon(mtmp, d(3, 6), 0);
        dmg = 0;
    }
    return dmg;
}

/* unlike the finger of death spell which behaves like a wand of death,
   this monster spell only attacks the hero */
void
touch_of_death(struct monst *mtmp)
{
    char kbuf[BUFSZ];
    int dmg = 50 + d(8, 6);
    int drain = dmg / 2;

    /* if we get here, we know that hero isn't magic resistant and isn't
       poly'd into an undead or demon */
    You_feel("drained...");
    (void) death_inflicted_by(kbuf, "the touch of death", mtmp);

    if (Upolyd) {
        u.mh = 0;
        rehumanize(); /* fatal iff Unchanging */
    } else if (drain >= u.uhpmax) {
        svk.killer.format = KILLED_BY;
        Strcpy(svk.killer.name, kbuf);
        done(DIED);
    } else {
        /* HP manipulation similar to poisoned(attrib.c) */
        int olduhp = u.uhp,
            uhpmin = minuhpmax(3),
            newuhpmax = u.uhpmax - drain;

        setuhpmax(max(newuhpmax, uhpmin), FALSE);
        dmg = adjuhploss(dmg, olduhp); /* reduce pending damage if uhp has
                                        * already been reduced due to drop
                                        * in uhpmax */
        losehp_damage(dmg, kbuf, KILLED_BY, 0, mtmp);
    }
    svk.killer.name[0] = '\0'; /* not killed if we get here... */
}

/* give a reason for death by some monster spells */
char *
death_inflicted_by(
    char *outbuf,            /* assumed big enough; pm_names are short */
    const char *deathreason, /* cause of death */
    struct monst *mtmp)      /* monster who caused it */
{
    Strcpy(outbuf, deathreason);
    if (mtmp) {
        struct permonst *mptr = mtmp->data,
            *champtr = (ismnum(mtmp->cham)) ? &mons[mtmp->cham] : mptr;
        const char *realnm = pmname(champtr, Mgender(mtmp)),
            *fakenm = pmname(mptr, Mgender(mtmp));

        /* greatly simplified extract from done_in_by(), primarily for
           reason for death due to 'touch of death' spell; if mtmp is
           shape changed, it won't be a vampshifter or mimic since they
           can't cast spells */
        if (!type_is_pname(champtr) && !the_unique_pm(mptr))
            realnm = an(realnm);
        Sprintf(eos(outbuf), " inflicted by %s%s",
                the_unique_pm(mptr) ? "the " : "", realnm);
        if (champtr != mptr)
            Sprintf(eos(outbuf), " imitating %s", an(fakenm));
    }
    return outbuf;
}

/*
 * Monster wizard and cleric spellcasting functions.
 */

staticfn void
mcast_death_touch(struct monst *mtmp)
{
    pline("Oh no, %s's using the touch of death!", mhe(mtmp));
    if (nonliving(gy.youmonst.data) || is_demon(gy.youmonst.data)
            || step20_celestial(gy.youmonst.data)) {
        You("seem no deader than before.");
    } else if (!Antimagic && rn2(mtmp->m_lev) > 12) {
        if (Hallucination) {
            You("have an out of body experience.");
        } else {
            touch_of_death(mtmp);
        }
        monstunseesu(M_SEEN_MAGR);
    } else {
        if (Antimagic) {
            shieldeff(u.ux, u.uy);
            monstseesu(M_SEEN_MAGR);
        }
        pline("Lucky for you, it didn't work!");
    }
}

staticfn void
mcast_clone_wiz(struct monst *mtmp)
{
    if (mtmp->iswiz && svc.context.no_of_wizards == 1) {
        pline("Double Trouble...");
        clonewiz();
    } else
        impossible("bad wizard cloning?");
}

staticfn void
mcast_summon_mons(struct monst *mtmp)
{
    int count = nasty(mtmp);

    if (!count) {
        ; /* nothing was created? */
    } else if (mtmp->iswiz) {
        SetVoice(mtmp, 0, 80, 0);
        verbalize("Destroy the thief, my pet%s!", plur(count));
    } else {
        boolean one = (count == 1);
        const char *mappear = one ? "A monster appears"
                                  : "Monsters appear";

        /* messages not quite right if plural monsters created but
           only a single monster is seen */
        if (Invis && !perceives(mtmp->data)
            && (mtmp->mux != u.ux || mtmp->muy != u.uy))
            pline("%s %s a spot near you!", mappear,
                  one ? "at" : "around");
        else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy))
            pline("%s %s your displaced image!", mappear,
                  one ? "by" : "around");
        else
            pline("%s from nowhere!", mappear);
    }
}

staticfn void
mcast_destroy_armor(void)
{
    if (Antimagic) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        pline("A field of force surrounds you!");
    } else if (!destroy_arm()) {
        Your("skin itches.");
    } else {
        /* monsters only realize you aren't magic-protected if armor is
           actually destroyed */
        monstunseesu(M_SEEN_MAGR);
    }
}

staticfn void
mcast_weaken_you(struct monst *mtmp, int dmg)
{
    if (Antimagic) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        You_feel("momentarily weakened.");
    } else {
        char kbuf[BUFSZ];

        You("suddenly feel weaker!");
        dmg = mtmp->m_lev - 6;
        if (dmg < 1) /* paranoia since only chosen when m_lev is high */
            dmg = 1;
        if (Half_spell_damage)
            dmg = (dmg + 1) / 2;
        if (u.mith_timers[MITH_VAUL])
            dmg = (dmg + 1) / 2;
        losestr(rnd(dmg),
                death_inflicted_by(kbuf, "strength loss", mtmp),
                KILLED_BY);
        svk.killer.name[0] = '\0'; /* not killed if we get here... */
        monstunseesu(M_SEEN_MAGR);
    }
}

staticfn void
mcast_disappear(struct monst *mtmp)
{
    if (!mtmp->minvis && !mtmp->invis_blkd) {
        if (canseemon(mtmp))
            pline_mon(mtmp, "%s suddenly %s!", Monnam(mtmp),
                      !See_invisible ? "disappears" : "becomes transparent");
        mon_set_minvis(mtmp, FALSE);
        if (cansee(mtmp->mx, mtmp->my) && !canspotmon(mtmp))
            map_invisible(mtmp->mx, mtmp->my);
    } else
        impossible("no reason for monster to cast disappear spell?");
}

staticfn void
mcast_stun_you(int dmg)
{
    if (Antimagic || Free_action) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        if (!Stunned)
            You_feel("momentarily disoriented.");
        make_stunned(1L, FALSE);
    } else {
        You(Stunned ? "struggle to keep your balance." : "reel...");
        dmg = d(ACURR(A_DEX) < 12 ? 6 : 4, 4);
        if (Half_spell_damage)
            dmg = (dmg + 1) / 2;
        if (u.mith_timers[MITH_VAUL])
            dmg = (dmg + 1) / 2;
        make_stunned((HStun & TIMEOUT) + (long) dmg, FALSE);
        monstunseesu(M_SEEN_MAGR);
    }
}

staticfn int
mcast_geyser(int dmg)
{
    /* this is physical damage (force not heat),
     * not magical damage or fire damage
     */
    pline("A sudden geyser slams into you from nowhere!");
    dmg = d(8, 6);
    if (Half_physical_damage)
        dmg = (dmg + 1) / 2;
    if (u.mith_timers[MITH_VAUL])
        dmg = (dmg + 1) / 2;
#if 0   /* since inventory items aren't affected, don't include this */
        /* make floor items wet */
    water_damage_chain(level.objects[u.ux][u.uy], TRUE);
#endif
    return dmg;
}

staticfn int
mcast_fire_pillar(struct monst *mtmp, int dmg)
{
    int orig_dmg;

    pline("A pillar of fire strikes all around you!");
    orig_dmg = dmg = d(8, 6);
    if (Fire_resistance) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_FIRE);
        dmg = 0;
    } else {
        monstunseesu(M_SEEN_FIRE);
    }
    if (Half_spell_damage)
        dmg = (dmg + 1) / 2;
    if (u.mith_timers[MITH_VAUL])
        dmg = (dmg + 1) / 2;
    burn_away_slime();
    (void) burnarmor(&gy.youmonst);
    /* item destruction dmg */
    (void) destroy_items(&gy.youmonst, AD_FIRE, orig_dmg);
    ignite_items(gi.invent);
    /* burn up flammable items on the floor, melt ice terrain */
    mon_spell_hits_spot(mtmp, AD_FIRE, u.ux, u.uy);
    return dmg;
}

staticfn int
mcast_lightning(struct monst *mtmp, int dmg)
{
    int orig_dmg;
    boolean reflects;

    Soundeffect(se_bolt_of_lightning, 80);
    pline("A bolt of lightning strikes down at you from above!");
    reflects = ureflects("It bounces off your %s%s.", "");
    orig_dmg = dmg = d(8, 6);
    if (reflects || Shock_resistance) {
        shieldeff(u.ux, u.uy);
        dmg = 0;
        if (reflects) {
            monstseesu(M_SEEN_REFL);
            return dmg;
        }
        monstunseesu(M_SEEN_REFL);
        monstseesu(M_SEEN_ELEC);
    } else {
        monstunseesu(M_SEEN_ELEC | M_SEEN_REFL);
    }
    if (Half_spell_damage)
        dmg = (dmg + 1) / 2;
    if (u.mith_timers[MITH_VAUL])
        dmg = (dmg + 1) / 2;
    (void) destroy_items(&gy.youmonst, AD_ELEC, orig_dmg);
    /* lightning might destroy iron bars if hero is on such a spot;
       reflection protects terrain here [execution won't get here due
       to 'if (reflects) break' above] but hero resistance doesn't;
       do this before maybe blinding the hero via flashburn() */
    mon_spell_hits_spot(mtmp, AD_ELEC, u.ux, u.uy);
    /* blind hero; no effect if already blind */
    (void) flashburn((long) rnd(100), TRUE);
    return dmg;
}

staticfn int
mcast_psi_bolt(int dmg)
{
    /* prior to 3.4.0 Antimagic was setting the damage to 1--this
       made the spell virtually harmless to players with magic res. */
    if (Antimagic) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        dmg = (dmg + 1) / 2;
    } else {
        monstunseesu(M_SEEN_MAGR);
    }
    if (dmg <= 5)
        You("get a slight %sache.", body_part(HEAD));
    else if (dmg <= 10)
        Your("brain is on fire!");
    else if (dmg <= 20)
        Your("%s suddenly aches painfully!", body_part(HEAD));
    else
        Your("%s suddenly aches very painfully!", body_part(HEAD));
    return dmg;
}

staticfn int
mcast_open_wounds(int dmg)
{
    if (Antimagic) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        dmg = (dmg + 1) / 2;
    } else {
        monstunseesu(M_SEEN_MAGR);
    }
    if (dmg <= 5)
        Your("skin itches badly for a moment.");
    else if (dmg <= 10)
        pline("Wounds appear on your body!");
    else if (dmg <= 20)
        pline("Severe wounds appear on your body!");
    else
        Your("body is covered with painful wounds!");
    return dmg;
}

staticfn void
mcast_insects(struct monst *mtmp)
{
    /* Try for insects, and if there are none
       left, go for (sticks to) snakes.  -3. */
    struct permonst *pm = mkclass(S_ANT, 0);
    struct monst *mtmp2 = (struct monst *) 0;
    char whatbuf[QBUFSZ], let = (pm ? S_ANT : S_SNAKE);
    boolean success = FALSE, seecaster;
    int i, quan, oldseen, newseen;
    coord bypos;
    const char *fmt, *what;

    oldseen = monster_census(TRUE);
    quan = (mtmp->m_lev < 2) ? 1 : rnd((int) mtmp->m_lev / 2);
    if (quan < 3)
        quan = 3;
    for (i = 0; i <= quan; i++) {
        if (!enexto(&bypos, mtmp->mux, mtmp->muy, mtmp->data))
            return;
        if ((pm = mkclass(let, 0)) != 0
            && (mtmp2 = makemon(pm, bypos.x, bypos.y, MM_ANGRY | MM_NOMSG))
            != 0) {
            success = TRUE;
            mtmp2->msleeping = mtmp2->mpeaceful = mtmp2->mtame = 0;
            set_malign(mtmp2);
        }
    }
    newseen = monster_census(TRUE);

    /* not canspotmon() which includes unseen things sensed via warning */
    seecaster = canseemon(mtmp) || tp_sensemon(mtmp) || Detect_monsters;
    what = (let == S_SNAKE) ? "snakes" : "insects";
    if (Hallucination)
        what = makeplural(bogusmon(whatbuf, (char *) 0));

    fmt = 0;
    if (!seecaster) {
        if (newseen <= oldseen || Unaware) {
            /* unseen caster fails or summons unseen critters,
               or unconscious hero ("You dream that you hear...") */
            You_hear("someone summoning %s.", what);
        } else {
            char *arg;

            if (what != whatbuf)
                what = strcpy(whatbuf, what);
            /* unseen caster summoned seen critter(s) */
            arg = (newseen == oldseen + 1) ? an(makesingular(what))
                                           : whatbuf;
            if (!Deaf) {
                Soundeffect(se_someone_summoning, 100);
                You_hear("someone summoning something, and %s %s.", arg,
                         vtense(arg, "appear"));
            } else {
                pline("%s %s.", upstart(arg), vtense(arg, "appear"));
            }
        }

        /* seen caster, possibly producing unseen--or just one--critters;
           hero is told what the caster is doing and doesn't necessarily
           observe complete accuracy of that caster's results (in other
           words, no need to fuss with visibility or singularization;
           player is told what's happening even if hero is unconscious) */
    } else if (!success) {
        fmt = "%s casts at a clump of sticks, but nothing happens.%s";
        what = "";
    } else if (let == S_SNAKE) {
        fmt = "%s transforms a clump of sticks into %s!";
    } else if (Invis && !perceives(mtmp->data)
               && (mtmp->mux != u.ux || mtmp->muy != u.uy)) {
        fmt = "%s summons %s around a spot near you!";
    } else if (Displaced && (mtmp->mux != u.ux || mtmp->muy != u.uy)) {
        fmt = "%s summons %s around your displaced image!";
    } else {
        fmt = "%s summons %s!";
    }
    if (fmt) {
        DISABLE_WARNING_FORMAT_NONLITERAL;
        pline_mon(mtmp, fmt, Monnam(mtmp), what);
        RESTORE_WARNING_FORMAT_NONLITERAL;
    }
}

staticfn void
mcast_blind_you(void)
{
    /* note: resists_blnd() doesn't apply here */
    if (!Blinded) {
        int num_eyes = eyecount(gy.youmonst.data);
        long duration = Half_spell_damage ? 100L : 200L;

        pline("Scales cover your %s!", (num_eyes == 1)
                                       ? body_part(EYE)
                                       : makeplural(body_part(EYE)));
        if (u.mith_timers[MITH_VAUL])
            duration = (duration + 1L) / 2L;
        make_blinded(duration, FALSE);
        if (!Blind)
            Your1(vision_clears);
    } else
        impossible("no reason for monster to cast blindness spell?");
}

staticfn int
mcast_paralyze(struct monst *mtmp)
{
    int dmg = 0;

    if (Antimagic || Free_action) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        if (gm.multi >= 0)
            You("stiffen briefly.");
        dmg = 1; /* to produce nomul(-1), not actual damage */
    } else {
        if (gm.multi >= 0)
            You("are frozen in place!");
        dmg = 4 + (int) mtmp->m_lev;
        if (Half_spell_damage)
            dmg = (dmg + 1) / 2;
        if (u.mith_timers[MITH_VAUL])
            dmg = (dmg + 1) / 2;
        monstunseesu(M_SEEN_MAGR);
    }
    nomul(-dmg);
    gm.multi_reason = "paralyzed by a monster";
    gn.nomovemsg = 0;
    return dmg;
}

staticfn void
mcast_confuse_you(struct monst *mtmp)
{
    if (Antimagic) {
        shieldeff(u.ux, u.uy);
        monstseesu(M_SEEN_MAGR);
        You_feel("momentarily dizzy.");
    } else {
        boolean oldprop = !!Confusion;
        int dmg = (int) mtmp->m_lev;

        if (Half_spell_damage)
            dmg = (dmg + 1) / 2;
        if (u.mith_timers[MITH_VAUL])
            dmg = (dmg + 1) / 2;
        make_confused(HConfusion + dmg, TRUE);
        if (Hallucination)
            You_feel("%s!", oldprop ? "trippier" : "trippy");
        else
            You_feel("%sconfused!", oldprop ? "more " : "");
        monstunseesu(M_SEEN_MAGR);
    }
}

/*
   If dmg is zero, then the monster is not casting at you.
   If the monster is intentionally not casting at you, we have previously
   called spell_would_be_useless() and spellnum should always be a valid
   undirected spell.
   If you modify either of these, be sure to change is_undirected_spell()
   and spell_would_be_useless().
 */
staticfn void
mcast_spell(struct monst *mtmp, int dmg, int spellnum)
{
    if (dmg < 0) {
        impossible("monster cast spell (%d) with negative dmg (%d)?",
                   spellnum, dmg);
        return;
    }
    if (dmg == 0 && !is_undirected_spell(spellnum)) {
        impossible("cast directed wizard spell (%d) with dmg=0?", spellnum);
        return;
    }

    switch (spellnum) {
    case MCAST_PUNISHMENT:
        punish((struct obj *) 0);
        dmg = 0;
        break;
    case MCAST_DEATH_TOUCH:
        mcast_death_touch(mtmp);
        dmg = 0;
        break;
    case MCAST_CLONE_WIZ:
        mcast_clone_wiz(mtmp);
        dmg = 0;
        break;
    case MCAST_SUMMON_MONS:
        mcast_summon_mons(mtmp);
        dmg = 0;
        break;
    case MCAST_AGGRAVATION:
        You_feel("that monsters are aware of your presence.");
        aggravate();
        dmg = 0;
        break;
    case MCAST_CURSE_ITEMS:
        You_feel("as if you need some help.");
        rndcurse();
        dmg = 0;
        break;
    case MCAST_KUKER_EVIL_EYE:
        You_feel("your luck running out.");
        change_luck(-1);
        dmg = 0;
        break;
    case MCAST_KUKER_PROTECTION:
        mtmp->mconf = mtmp->mstun = 0;
        mtmp->mhp = min(mtmp->mhpmax, mtmp->mhp + d(2, 6));
        if (canseemon(mtmp))
            pline("A shimmering shield surrounds %s!", mon_nam(mtmp));
        dmg = 0;
        break;
    case MCAST_DESTRY_ARMR:
        mcast_destroy_armor();
        dmg = 0;
        break;
    case MCAST_WEAKEN_YOU: /* drain strength */
        mcast_weaken_you(mtmp, dmg);
        dmg = 0;
        break;
    case MCAST_DISAPPEAR: /* makes self invisible */
        mcast_disappear(mtmp);
        dmg = 0;
        break;
    case MCAST_MITH_SLEEP:
        if (lined_up(mtmp)) {
            gb.buzzer = mtmp;
            buzz(BZ_M_SPELL(BZ_OFS_AD(AD_SLEE)), min(10, mtmp->m_lev / 3 + 1),
                 mtmp->mx, mtmp->my, sgn(gt.tbx), sgn(gt.tby));
            gb.buzzer = 0;
        }
        stop_occupation();
        dmg = 0;
        break;
    case MCAST_MITH_CURE_CLOSE:
    case MCAST_MITH_CURE_FAR:
        mith_mass_cure(mtmp, spellnum == MCAST_MITH_CURE_FAR,
                       mtmp->mux, mtmp->muy);
        dmg = 0;
        break;
    case MCAST_STUN_YOU:
        mcast_stun_you(dmg);
        dmg = 0;
        break;
    case MCAST_HASTE_SELF:
        mon_adjust_speed(mtmp, 1, (struct obj *) 0);
        dmg = 0;
        break;
    case MCAST_CURE_SELF:
        dmg = m_cure_self(mtmp, dmg);
        break;
    case MCAST_PSI_BOLT:
        dmg = mcast_psi_bolt(dmg);
        break;
    case MCAST_GEYSER:
        dmg = mcast_geyser(dmg);
        break;
    case MCAST_FIRE_PILLAR:
        dmg = mcast_fire_pillar(mtmp, dmg);
        break;
    case MCAST_LIGHTNING:
        dmg = mcast_lightning(mtmp, dmg);
        break;
    case MCAST_INSECTS:
        mcast_insects(mtmp);
        dmg = 0;
        break;
    case MCAST_BLIND_YOU:
        mcast_blind_you();
        dmg = 0;
        break;
    case MCAST_PARALYZE:
        dmg = mcast_paralyze(mtmp);
        break;
    case MCAST_CONFUSE_YOU:
        mcast_confuse_you(mtmp);
        dmg = 0;
        break;
    case MCAST_OPEN_WOUNDS:
        dmg = mcast_open_wounds(dmg);
        break;
    case MCAST_RILMANI_DRAIN_LIFE:
        if (Drain_resistance) {
            shieldeff(u.ux, u.uy);
        } else {
            Your("body deteriorates!");
            losexp("life drainage");
        }
        dmg = 0;
        break;
    case MCAST_RILMANI_ACID_BLAST:
    case MCAST_RILMANI_ACID_RAIN:
        if (Acid_resistance) {
            shieldeff(u.ux, u.uy);
            dmg = 0;
        } else {
            dmg = min(60, dmg);
            erode_armor(&gy.youmonst, TRUE);
        }
        break;
    case MCAST_RILMANI_SOLID_FOG:
        You("are engulfed by solid fog!");
        make_blinded(Blinded + 8L, FALSE);
        gy.youmonst.movement -= NORMAL_SPEED / 2;
        if (mtmp->data == &mons[PM_PLUMACH_RILMANI])
            mtmp->mcan = 1;
        dmg = 0;
        break;
    case MCAST_RILMANI_POISON_GAS:
        (void) create_gas_cloud(u.ux, u.uy, rnd(3), rnd(3) + 1);
        dmg = 0;
        break;
    case MCAST_RILMANI_MAKE_VISIBLE:
        HInvis &= ~INTRINSIC;
        You_feel("paranoid.");
        dmg = 0;
        break;
    case MCAST_RILMANI_HAIL_FLURY:
    case MCAST_RILMANI_ICE_STORM:
        if (Cold_resistance) {
            shieldeff(u.ux, u.uy);
            dmg = (dmg + 1) / 2;
        }
        break;
    case MCAST_RILMANI_SILVER_RAYS:
        pline("Silver rays strike you!");
        dmg = d(2, 20);
        break;
    case MCAST_RILMANI_GOLDEN_WAVE:
        pline("A wave of golden light strikes you!");
        dmg = d(2, 12);
        break;
    case MCAST_RILMANI_PRISMATIC_SPRAY:
        pline("Prismatic light bursts around you!");
        switch (rn2(6)) {
        case 0: if (Fire_resistance) dmg = 0; break;
        case 1: if (Cold_resistance) dmg = 0; break;
        case 2: if (Shock_resistance) dmg = 0; break;
        case 3: if (Acid_resistance) dmg = 0; break;
        case 4: if (Poison_resistance) dmg = 0; break;
        default: if (Antimagic) dmg = 0; break;
        }
        break;
    default:
        impossible("mcastu: invalid magic spell (%d)", spellnum);
        dmg = 0;
        break;
    }

    if (dmg)
        mdamageu_damage(mtmp, dmg, spellnum == MCAST_GEYSER ? dmg : 0);
}

staticfn boolean
is_undirected_spell(int spellnum)
{
    if ((mcast_data[spellnum].flags & MCF_INDIRECT) != 0)
        return TRUE;
    return FALSE;
}

/* Some spells are useless under some circumstances. */
staticfn boolean
spell_would_be_useless(struct monst *mtmp, int spellnum)
{
    /* Some spells don't require the player to really be there and can be cast
     * by the monster when you're invisible, yet still shouldn't be cast when
     * the monster doesn't even think you're there.
     * This check isn't quite right because it always uses your real position.
     * We really want something like "if the monster could see mux, muy".
     */

    /* spell is only cast by hostile monsters */
    if ((mcast_data[spellnum].flags & MCF_HOSTILE) != 0) {
        if (mtmp->mpeaceful)
            return TRUE;
    }

    /* spell needs the monster to see hero */
    if ((mcast_data[spellnum].flags & MCF_SIGHT) != 0) {
        boolean mcouldseeu = couldsee(mtmp->mx, mtmp->my);

        if (!mcouldseeu)
            return TRUE;
    }

    switch (spellnum) {
    case MCAST_DEATH_TOUCH:
        if ((Antimagic || Hallucination) && !rn2(2))
            return TRUE;
        break;
    case MCAST_MITH_SLEEP:
        if (Sleepy || !lined_up(mtmp))
            return TRUE;
        break;
    case MCAST_GEYSER:
        if (!rn2(5))
            return TRUE;
        break;
    case MCAST_CLONE_WIZ:
        /* only the Wizard is allowed to clone himself */
        if (!mtmp->iswiz || svc.context.no_of_wizards > 1)
            return TRUE;
        break;
    case MCAST_AGGRAVATION:
        /* aggravation (global wakeup) when everyone is already active */
        /* if nothing needs to be awakened then this spell is useless
           but caster might not realize that [chance to pick it then
           must be very small otherwise caller's many retry attempts
           will eventually end up picking it too often] */
        if (!has_aggravatables(mtmp))
            return rn2(100) ? TRUE : FALSE;
        break;
    case MCAST_HASTE_SELF:
        /* haste self when already fast */
        if (mtmp->permspeed == MFAST)
            return TRUE;
        break;
    case MCAST_DISAPPEAR:
        /* invisibility when already invisible */
        if (mtmp->minvis || mtmp->invis_blkd)
            return TRUE;
        /* peaceful monster won't cast invisibility if you can't see
           invisible,
           same as when monsters drink potions of invisibility.  This doesn't
           really make a lot of sense, but lets the player avoid hitting
           peaceful monsters by mistake */
        if (mtmp->mpeaceful && !See_invisible)
            return TRUE;
        break;
    case MCAST_CURE_SELF:
        /* healing when already healed */
        if (mtmp->mhp == mtmp->mhpmax)
            return TRUE;
        break;
    case MCAST_BLIND_YOU:
        if (Blinded)
            return TRUE;
        break;
    default:
        break;
    }
    return FALSE;
}

/* monster uses spell (ranged) */
int
buzzmu(struct monst *mtmp, struct attack *mattk)
{
    /* don't print constant stream of curse messages for 'normal'
       spellcasting monsters at range */
    if (!BZ_VALID_ADTYP(mattk->adtyp))
        return M_ATTK_MISS;

    if (mtmp->mcan || m_seenres(mtmp, cvt_adtyp_to_mseenres(mattk->adtyp))) {
        cursetxt(mtmp, FALSE);
        return M_ATTK_MISS;
    }
    if (lined_up(mtmp) && rn2(3)) {
        nomul(0);
        if (canseemon(mtmp))
            pline_mon(mtmp, "%s zaps you with a %s!", Monnam(mtmp),
                  flash_str(BZ_OFS_AD(mattk->adtyp), FALSE));
        gb.buzzer = mtmp;
        buzz(BZ_M_SPELL(BZ_OFS_AD(mattk->adtyp)), (int) mattk->damn,
             mtmp->mx, mtmp->my, sgn(gt.tbx), sgn(gt.tby));
        gb.buzzer = 0;
        return M_ATTK_HIT;
    }
    return M_ATTK_MISS;
}

/*mcastu.c*/
