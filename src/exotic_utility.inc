/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Step 22 manual terrain utilities and safe native untrapping. */
int
ex_theft_ward(void)
{
    struct ex_sources sources;
    unsigned i;
    ex_snapshot(&sources, &gy.youmonst);
    for (i = 0; i < sources.count; ++i)
        if (ex_source(&sources, i, EX_WARD, &gy.youmonst)) {
            Your("artifact ward prevents the theft.");
            return 1;
        }
    return 0;
}

int
ex_untrap_attempt(void)
{
    struct ex_sources sources;
    unsigned i;
    int chance = max(20, 100 - 5 * ((max(1, depth(&u.uz)) - 1) / 5));
    ex_snapshot(&sources, &gy.youmonst);
    for (i = 0; i < sources.count; ++i)
        if (ex_source(&sources, i, EX_UNTRAP, &gy.youmonst)
            && rn2(100) < chance)
            return 1;
    return 0;
}

int
ex_untrap_floor(struct trap *trap)
{
    int x, y, holding;
    struct monst *mon;
    if (!ex_has(EX_UNTRAP))
        return -1;
    if (!trap || !trap->tseen || !isok(trap->tx, trap->ty)
        || distmin(u.ux, u.uy, trap->tx, trap->ty) > 1
        || !can_reach_floor(FALSE) || ex_protected_tile(trap->tx, trap->ty)
        || trap->ttyp == MAGIC_PORTAL || trap->ttyp == VIBRATING_SQUARE
        /* Castle shafts are the native mandatory connection to Gehennom:
         * fall_through redirects them through find_hell(). */
        || (Is_stronghold(&u.uz) && is_hole(trap->ttyp))
        || (is_hole(trap->ttyp) && trap->dst.dnum >= 0
            && trap->dst.dnum != u.uz.dnum)
        || (Sokoban && (is_pit(trap->ttyp) || is_hole(trap->ttyp)))) {
        You("cannot disarm that protected or unreachable trap.");
        return 0;
    }
    x = trap->tx;
    y = trap->ty;
    holding = u_at(x, y) && u.utrap
        && ((u.utraptype == TT_BEARTRAP && trap->ttyp == BEAR_TRAP)
            || (u.utraptype == TT_WEB && trap->ttyp == WEB)
            || (u.utraptype == TT_PIT && is_pit(trap->ttyp)));
    if (u.utrap && !holding) {
        You("cannot reach another trap while restrained.");
        return 0;
    }
    if (!ex_untrap_attempt()) {
        Your("artifact fails to disarm the trap safely.");
        return 1;
    }
    /* Only the actual holding hazard releases restraint. Native cleanup
     * restores blocked levitation/flight; deltrap clears conjoined pits. */
    if (holding)
        reset_utrap(TRUE);
    mon = m_at(x, y);
    if (mon)
        mon->mtrapped = 0;
    switch (trap->ttyp) {
    case BEAR_TRAP:
        cnv_trap_obj(BEARTRAP, 1, trap, FALSE);
        break;
    case LANDMINE:
        cnv_trap_obj(LAND_MINE, 1, trap, FALSE);
        break;
    case ARROW_TRAP:
    case DART_TRAP:
        cnv_trap_obj(trap->ttyp == ARROW_TRAP ? ARROW : DART,
                     50 - rnl(50), trap, FALSE);
        break;
    default:
        deltrap(trap);
        break;
    }
    if (holding)
        gv.vision_full_recalc = 1;
    newsym(x, y);
    You("disarm the trap.");
    return 1;
}

int
ex_transmute(struct obj *obj, int x, int y)
{
    int typ, cost;
    boolean shopdamage = FALSE;
    struct obj *item;
    if (!obj || !ex_active(obj, EX_TRANSMUTE, &gy.youmonst))
        return 0;
    if (!isok(x, y) || distmin(u.ux, u.uy, x, y) > 5
        || !cansee(x, y) || !clear_path(u.ux, u.uy, x, y)
        || ex_protected_tile(x, y)) {
        You("cannot transmute that protected or unseen terrain.");
        return 0;
    }
    typ = levl[x][y].typ;
    if (typ != POOL && typ != MOAT && typ != BOG && typ != PUDDLE
        && typ != ICE && typ != LAVAPOOL) {
        You("cannot transmute that terrain.");
        return 0;
    }
    for (item = svl.level.objects[x][y]; item; item = item->nexthere)
        if (ex_protected_object(item)) {
            You("cannot disturb the protected object there.");
            return 0;
        }
    cost = typ == LAVAPOOL ? 2 : 1;
    if (!ex_spend(obj, 1, cost)) {
        Your("artifact lacks sufficient transmutation charges.");
        return 0;
    }
    if (typ == ICE) {
        /* This native route owns exact icedpool provenance, timers,
         * corpse aging, traps, boulders, drowning and occupant handling. */
        melt_ice(x, y, (const char *) 0);
    } else {
        /* Native freezing sets the exact existing ICED_POOL/MOAT/BOG/
         * PUDDLE flags serialized by Sfo_rm. No extra provenance format
         * is needed, and external fire uses the same melt_ice route. */
        (void) zap_over_floor(x, y, AD_COLD - 1, &shopdamage,
                             TRUE, 0);
        spot_stop_timers(x, y, MELT_ICE_AWAY);
        if (t_at(x, y))
            trap_ice_effects(x, y, FALSE);
    }
    recalc_block_point(x, y);
    gv.vision_full_recalc = 1;
    newsym(x, y);
    return 1;
}

int
ex_utility_invoke(struct obj *obj, int property)
{
    coord cc;
    if (property != EX_DIG && property != EX_TRANSMUTE)
        return -1;
    if (!ex_active(obj, property, &gy.youmonst))
        return 0;
    ex_resources(obj);
    if (property == EX_DIG) {
        if (!obj->exotic.charges[0]) {
            Your("artifact has no digging charges.");
            return 0;
        }
        if (!getdir((const char *) 0))
            return 0;
        if (!ex_spend(obj, 0, 1))
            return 0;
        zap_dig();
        return 1;
    }
    cc.x = u.ux;
    cc.y = u.uy;
    if (getpos(&cc, TRUE, "the terrain to transmute") < 0)
        return 0;
    return ex_transmute(obj, cc.x, cc.y);
}
