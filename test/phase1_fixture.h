/* Historical fixtures describe complete initial enhancement configurations.
 * Step 17 mutations require explicit slot selection. Keep fixture replacement
 * separate from the production mutation API; new Step 17 tests use that API
 * directly. This builder validates every configuration and preserves retained
 * numeric values without drawing replacement dice. */
#ifndef PHASE1_FIXTURE_H
#define PHASE1_FIXTURE_H
static boolean
fixture_set_mask(struct obj *obj, struct enhancement_mask mask,
                 enum enhancement_quality quality, boolean known)
{
    struct obj old = *obj;
    struct enhancement_mask empty = {{0,0}};
    int id;
    if (!enhancement_mask_allowed(obj,mask) || quality > OQ_EXCEPTIONAL
        || quality < OQ_STANDARD || (obj->oclass==TOOL_CLASS && quality)) return FALSE;
    if (enhancement_slots_valid(obj)
        && mask.word[0]==obj->o_enh_props && mask.word[1]==obj->o_enh_props2)
        return enhancement_set_mask(obj,mask,quality,known);
    enhancement_clear(obj);
    if (!enhancement_set_mask(obj,empty,quality,known)) return FALSE;
    for (id=1;id<EP_COUNT;++id) if(enhancement_mask_has(mask,id)) {
        int value = id>=EP_STR_I && id<=EP_DEX_IV
                        && enhancement_mask_has(enhancement_actual(&old),id)
                    ? old.o_enh_values[id-EP_STR_I] : -1;
        if(!enhancement_slot_set(obj,enhancement_first_unused(obj),id,known,value)) return FALSE;
    }
    if ((old.o_enh_props&OEP_STONING)==(obj->o_enh_props&OEP_STONING)) {
        obj->o_stoning_remaining=old.o_stoning_remaining;
        obj->o_stoning_turn=old.o_stoning_turn;
    }
    if (utility_purification_interval(&old)==utility_purification_interval(obj)) {
        obj->o_purification_remaining=old.o_purification_remaining;
        obj->o_purification_sampled=old.o_purification_sampled;
    }
    return TRUE;
}
static boolean
fixture_set(struct obj *obj,uint64 props,enum enhancement_quality quality,boolean known)
{
    struct enhancement_mask mask={{0,0}}; mask.word[0]=props;
    return fixture_set_mask(obj,mask,quality,known);
}
#define enhancement_set fixture_set
#define enhancement_set_mask fixture_set_mask
#endif
