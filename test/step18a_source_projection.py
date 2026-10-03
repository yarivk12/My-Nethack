"""Project exact Step 18A production deltas out of Step 17 historical checks."""

HUNKS = {
    'src/apply.c': [
        (
            '''static const struct forge_recipe forge_recipes[] = {
    { KATANA, { { LONG_SWORD, 1 }, { LONG_SWORD, 1 } } },
    { TWO_HANDED_SWORD, { { LONG_SWORD, 1 }, { BROADSWORD, 1 } } },
    { TSURUGI, { { KATANA, 1 }, { TWO_HANDED_SWORD, 1 } } },
    { BATTLE_AXE, { { AXE, 1 }, { BROADSWORD, 1 } } },
    { DWARVISH_MATTOCK, { { PICK_AXE, 1 }, { DWARVISH_SHORT_SWORD, 1 } } },
    { TRIDENT, { { SCIMITAR, 1 }, { SPEAR, 1 } } },
    { ATHAME, { { DAGGER, 1 }, { STILETTO, 1 } } },
    { RUNESWORD, { { BROADSWORD, 1 }, { DAGGER, 1 } } },
    { CHAIN_MAIL, { { RING_MAIL, 1 }, { RING_MAIL, 1 } } },
    { SPLINT_MAIL, { { SCALE_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { PLATE_MAIL, { { SPLINT_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { ELVEN_SHIELD, { { ELVEN_DAGGER, 1 }, { SMALL_SHIELD, 1 } } }
};''',
            '''static const struct forge_recipe forge_recipes[] = {
    { KATANA, { { LONG_SWORD, 1 }, { LONG_SWORD, 1 } } },
    { TWO_HANDED_SWORD, { { LONG_SWORD, 1 }, { BROADSWORD, 1 } } },
    { TSURUGI, { { KATANA, 1 }, { TWO_HANDED_SWORD, 1 } } },
    { BATTLE_AXE, { { AXE, 1 }, { AXE, 1 } } },
    { DWARVISH_MATTOCK, { { PICK_AXE, 1 }, { DWARVISH_SHORT_SWORD, 1 } } },
    { TRIDENT, { { SCIMITAR, 1 }, { SPEAR, 1 } } },
    { ATHAME, { { DAGGER, 1 }, { STILETTO, 1 } } },
    { RUNESWORD, { { BROADSWORD, 1 }, { DAGGER, 1 } } },
    { DAGGER, { { KNIFE, 1 }, { KNIFE, 1 } } },
    { SHORT_SWORD, { { DAGGER, 1 }, { DAGGER, 1 } } },
    { LONG_SWORD, { { SHORT_SWORD, 1 }, { SHORT_SWORD, 1 } } },
    { HALBERD, { { SPEAR, 1 }, { AXE, 1 } } },
    { MORNING_STAR, { { MACE, 1 }, { MACE, 1 } } },
    { CHAIN_MAIL, { { RING_MAIL, 1 }, { RING_MAIL, 1 } } },
    { STUDDED_LEATHER_ARMOR, { { LEATHER_ARMOR, 1 }, { LEATHER_ARMOR, 1 } } },
    { SCALE_MAIL, { { STUDDED_LEATHER_ARMOR, 1 }, { RING_MAIL, 1 } } },
    { SPLINT_MAIL, { { SCALE_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { BANDED_MAIL, { { RING_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { PLATE_MAIL, { { SPLINT_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { PLATE_MAIL, { { BANDED_MAIL, 1 }, { CHAIN_MAIL, 1 } } },
    { LARGE_SHIELD, { { SMALL_SHIELD, 1 }, { SMALL_SHIELD, 1 } } },
    { SHIELD_OF_REFLECTION, { { LARGE_SHIELD, 1 }, { AMULET_OF_REFLECTION, 1 } } },
    { ELVEN_SHIELD, { { ELVEN_DAGGER, 1 }, { SMALL_SHIELD, 1 } } }
};'''
        ),
        (
            '''/* Step 15C: transient values only; quantities do not weight the reductions. */
struct forge_state {
    boolean present, proof;
    int quality, spe, buc, eroded, eroded2;
    struct enhancement_mask props;
    uint8 values[8];
    uint8 order[EP_COUNT];
    int order_count;
};

staticfn void
forge_gather(struct forge_state *state, struct obj *obj)
{
    int i, buc = bcsign(obj);

    if (!state->present) {
        state->quality = obj->o_enh_quality;
        state->spe = obj->spe;
        state->buc = buc;
        state->eroded = obj->oeroded;
        state->eroded2 = obj->oeroded2;
        state->present = TRUE;
    } else {
        state->quality = max(state->quality, obj->o_enh_quality);
        state->spe = max(state->spe, obj->spe);
        state->buc = max(state->buc, buc);
        state->eroded = min(state->eroded, (int) obj->oeroded);
        state->eroded2 = min(state->eroded2, (int) obj->oeroded2);
    }
    for (i = 0; i < ENHANCEMENT_MAX_SLOTS; ++i) {
        int id = obj->o_affixes[i].property;
        if (id && !enhancement_mask_has(state->props, id)) {
            state->order[state->order_count++] = (uint8) id;
            enhancement_mask_add(&state->props, id);
        }
    }
    for (i = 0; i < 8; ++i)
        if (obj->o_enh_props & enhancement_catalog[24 + i].bit)
            state->values[i] = max(state->values[i], obj->o_enh_values[i]);
    state->proof |= obj->oerodeproof;
}''',
            '''/* Step 15C: transient values only; quantities do not weight the reductions. */
struct forge_state {
    boolean present, proof, spe_present, eroded_present, eroded2_present;
    int quality, spe, buc, eroded, eroded2;
    struct enhancement_mask props;
    uint8 values[8];
    uint8 order[EP_COUNT];
    int order_count;
};

staticfn boolean
forge_spe_supported(const struct obj *obj)
{
    return obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS
        || is_weptool(obj)
        || ((obj->oclass == RING_CLASS || obj->oclass == TOOL_CLASS)
            && objects[obj->otyp].oc_charged)
        || obj->oclass == WAND_CLASS;
}

staticfn void
forge_gather(struct forge_state *state, struct obj *obj)
{
    int i, buc = bcsign(obj);

    if (!state->present) {
        state->buc = buc;
        state->present = TRUE;
    } else {
        state->buc = max(state->buc, buc);
    }
    if (enhancement_eligible(obj)) {
        state->quality = max(state->quality, (int) obj->o_enh_quality);
        for (i = 0; i < ENHANCEMENT_MAX_SLOTS; ++i) {
            int id = obj->o_affixes[i].property;
            if (id && !enhancement_mask_has(state->props, id)) {
                state->order[state->order_count++] = (uint8) id;
                enhancement_mask_add(&state->props, id);
            }
        }
        for (i = 0; i < 8; ++i)
            if (obj->o_enh_props & enhancement_catalog[24 + i].bit)
                state->values[i] = max(state->values[i], obj->o_enh_values[i]);
    }
    if (forge_spe_supported(obj)) {
        state->spe = state->spe_present ? max(state->spe, (int) obj->spe)
                                       : obj->spe;
        state->spe_present = TRUE;
    }
    if (erosion_matters(obj)) {
        state->eroded = state->eroded_present
                            ? min(state->eroded, (int) obj->oeroded)
                            : obj->oeroded;
        state->eroded_present = TRUE;
        state->eroded2 = state->eroded2_present
                             ? min(state->eroded2, (int) obj->oeroded2)
                             : obj->oeroded2;
        state->eroded2_present = TRUE;
        state->proof |= obj->oerodeproof;
    }
}'''
        ),
        (
            '''    if (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS
        || is_weptool(obj)
        || ((obj->oclass == RING_CLASS || obj->oclass == TOOL_CLASS)
            && objects[obj->otyp].oc_charged)
        || obj->oclass == WAND_CLASS) {''',
            '''    if (forge_spe_supported(obj) && state->spe_present) {'''
        ),
        (
            '''        if (is_flammable(obj) || is_rustprone(obj) || is_crackable(obj))
            obj->oeroded = state->eroded;
        if (is_corrodeable(obj) || is_rottable(obj))
            obj->oeroded2 = state->eroded2;''',
            '''        if (state->eroded_present
            && (is_flammable(obj) || is_rustprone(obj) || is_crackable(obj)))
            obj->oeroded = state->eroded;
        if (state->eroded2_present && (is_corrodeable(obj) || is_rottable(obj)))
            obj->oeroded2 = state->eroded2;'''
        )
    ]
}


def project(path, text):
    for before, after in HUNKS.get(path, []):
        if after in text:
            assert text.count(after) == 1, (path, 'ambiguous Step 18A hunk')
            text = text.replace(after, before, 1)
    return text
