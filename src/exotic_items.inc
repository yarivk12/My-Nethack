/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Step 22 corpse aging and committed-consumption transactions.
 * This file is included by exotic.c. Native NetHack license applies. */
#define EX_AGE_SCALE 120L

/* 'age' remains native bookkeeping for resurrection and ice transitions.
 * Food freshness alone reads the independent fractional effective clock. */
long
ex_corpse_age(struct obj *body)
{
    long elapsed;
    if (!body || body->otyp != CORPSE)
        return 0L;
    if (!body->exotic.preserved)
        return max(0L, svm.moves - peek_at_iced_corpse_age(body));
    elapsed = max(0L, svm.moves - body->exotic.age_turn);
    if (!(body->exotic.frozen & 2))
        body->exotic.age_units += elapsed
            * (EX_AGE_SCALE / (body->exotic.multiplier
                              * ((body->exotic.frozen & 1) ? 2 : 1)));
    body->exotic.age_turn = svm.moves;
    return body->exotic.age_units / EX_AGE_SCALE;
}

void
ex_corpse_transition(struct obj *body, int ice, int frozen)
{
    if (body && body->otyp == CORPSE && body->exotic.preserved) {
        (void) ex_corpse_age(body);
        body->exotic.frozen = (ice ? 1 : 0) | (frozen ? 2 : 0);
    }
}

static int
ex_preservable(struct obj *body)
{
    long units;
    if (!body || body->otyp != CORPSE || !ismnum(body->corpsenm)
        || body->exotic.preserved
        || body->orotten || body->corpsenm == PM_LIZARD
        || body->corpsenm == PM_LICHEN || is_rider(&mons[body->corpsenm]))
        return FALSE;
    units = body->on_ice
        ? max(0L, svm.moves - body->age) * (EX_AGE_SCALE / 2L)
        : ex_corpse_age(body) * EX_AGE_SCALE;
    return units < (body->cursed ? 20L
                    : body->blessed ? 60L : 40L) * EX_AGE_SCALE;
}

static int
ex_corpse_reachable(struct obj *body)
{
    int x, y;
    if (body->where == OBJ_INVENT)
        return TRUE;
    x = body->ox;
    y = body->oy;
    return body->where == OBJ_FLOOR && can_reach_floor(FALSE)
        && isok(x, y) && distmin(u.ux, u.uy, x, y) <= 1
        && ACCESSIBLE(levl[x][y].typ) && !closed_door(x, y)
        && !sobj_at(BOULDER, x, y)
        && !(x != u.ux && y != u.uy
             && (IS_OBSTRUCTED(levl[x][u.uy].typ) || closed_door(x, u.uy))
             && (IS_OBSTRUCTED(levl[u.ux][y].typ) || closed_door(u.ux, y)))
        && clear_path(u.ux, u.uy, x, y);
}

int
ex_preserve(struct obj *source, struct obj *body)
{
    long age, remaining;
    int multiplier;
    if (!ex_active(source, EX_PRESERVE, &gy.youmonst)
        || !ex_preservable(body) || !ex_corpse_reachable(body))
        return 0;
    age = ex_corpse_age(body);
    if (body->quan > 1L)
        body = splitobj(body, 1L);
    multiplier = source->exotic.preservation_count < 20 ? 10
        : source->exotic.preservation_count < 60 ? 20 : 30;
    body->exotic.preserved = 1;
    body->exotic.multiplier = multiplier;
    body->exotic.age_units = body->on_ice
        ? max(0L, svm.moves - body->age) * (EX_AGE_SCALE / 2L)
        : age * EX_AGE_SCALE;
    body->exotic.age_turn = svm.moves;
    body->exotic.frozen = ((body->where == OBJ_FLOOR
                            || body->where == OBJ_BURIED)
                           && is_ice(body->ox, body->oy)) ? 1 : 0;
    /* Only decay is stretched. Revival and zombification retain their
       native deadlines, species behavior, and icebox treatment. */
    remaining = stop_timer(ROT_CORPSE, obj_to_any(body));
    if (remaining) {
        body->exotic.decay_units = body->exotic.age_units
            + remaining * EX_AGE_SCALE / (body->on_ice ? 2L : 1L);
        (void) start_timer(remaining * multiplier, TIMER_OBJECT, ROT_CORPSE,
                           obj_to_any(body));
    }
    ++source->exotic.preservation_count;
    pline("%s is now Preserved (%dx slower aging).", The(xname(body)),
          multiplier);
    update_inventory();
    return 1;
}

int
ex_preserve_apply(struct obj *source)
{
    struct obj *body;
    menu_item *pick = 0;
    anything any;
    winid win;
    int x, y, n;
    if (!ex_active(source, EX_PRESERVE, &gy.youmonst))
        return -1;
    win = create_nhwindow(NHW_MENU);
    start_menu(win, MENU_BEHAVE_STANDARD);
    for (body = gi.invent; body; body = body->nobj)
        if (ex_preservable(body)) {
            any = cg.zeroany;
            any.a_obj = body;
            add_menu(win, &nul_glyphinfo, &any, 0, 0, ATR_NONE, NO_COLOR,
                     doname(body), MENU_ITEMFLAGS_NONE);
        }
    for (x = u.ux - 1; x <= u.ux + 1; ++x)
        for (y = u.uy - 1; y <= u.uy + 1; ++y)
            if (isok(x, y))
                for (body = svl.level.objects[x][y]; body;
                     body = body->nexthere)
                    if (ex_preservable(body) && ex_corpse_reachable(body)) {
                        char buf[BUFSZ];
                        any = cg.zeroany;
                        any.a_obj = body;
                        Sprintf(buf, "%s (floor %d,%d)", doname(body), x, y);
                        add_menu(win, &nul_glyphinfo, &any, 0, 0, ATR_NONE,
                                 NO_COLOR, buf, MENU_ITEMFLAGS_NONE);
                    }
    end_menu(win, "Preserve which fresh corpse?");
    n = select_menu(win, PICK_ONE, &pick);
    destroy_nhwindow(win);
    body = n > 0 ? pick[0].item.a_obj : (struct obj *) 0;
    if (pick)
        free((genericptr_t) pick);
    return body ? ex_preserve(source, body) : 0;
}

void
ex_preserve_inspect(struct obj *obj, int win)
{
    char buf[BUFSZ];
    if (obj->otyp == CORPSE && obj->exotic.preserved) {
        struct obj copy = *obj;
        Sprintf(buf, "Preserved: %dx slower aging (effective age %ld turns).",
                obj->exotic.multiplier, ex_corpse_age(&copy));
        putstr(win, 0, buf);
    }
    if (obj->known && obj->dknown && obj->oartifact
        && !undiscovered_artifact(obj->oartifact)
        && (obj->exotic.grants & (1UL << EX_PRESERVE))) {
        unsigned n = obj->exotic.preservation_count;
        Sprintf(buf, "Preservation rank %s: %u successes, %dx slower aging%s.",
                n < 20 ? "I" : n < 60 ? "II" : "III", n,
                n < 20 ? 10 : n < 60 ? 20 : 30,
                n < 20 ? " (rank II at 20)"
                : n < 60 ? " (rank III at 60)" : " (maximum rank)");
        putstr(win, 0, buf);
    }
}

/* The command opens a transaction, but only native useup commits it.
 * An object id rather than a pointer excludes unrelated use during effects.
 * The captured source set also excludes grants gained by the consumed item. */
static unsigned ex_consuming_id, ex_conserved_id;
static struct ex_sources ex_consuming_sources;

void
ex_consumption_begin(struct obj *obj)
{
    ex_consuming_id = obj->o_id;
    ex_conserved_id = 0;
    ex_snapshot(&ex_consuming_sources, &gy.youmonst);
}

void
ex_consumption_end(void)
{
    struct obj *obj;
    /* Retain native in-use protection through late prompts and secondary
       effects (fire scrolls, for example). One retained unit cannot be
       destroyed by the very effect whose original input is already gone. */
    if (ex_conserved_id)
        for (obj = gi.invent; obj; obj = obj->nobj)
            if (obj->o_id == ex_conserved_id) {
                obj->in_use = FALSE;
                break;
            }
    ex_consuming_id = 0;
    ex_conserved_id = 0;
}

static int
ex_conserve_sources(struct obj *obj, struct ex_sources *sources)
{
    unsigned i;
    int price = objects[obj->otyp].oc_cost;
    int chance = price <= 100 ? 30 : price <= 200 ? 20 : 10;
    if (obj->oclass != POTION_CLASS && obj->oclass != SCROLL_CLASS)
        return FALSE;
    for (i = 0; i < sources->count; ++i)
        if (ex_source(sources, i, EX_CONSERVE, &gy.youmonst)
            && rn2(100) < chance)
            return TRUE;
    return FALSE;
}

int
ex_conserve(struct obj *obj)
{
    struct ex_sources sources;
    ex_snapshot(&sources, &gy.youmonst);
    return ex_conserve_sources(obj, &sources);
}

int
ex_consumption_retain(struct obj *obj)
{
    if (!ex_consuming_id || obj->o_id != ex_consuming_id)
        return FALSE;
    ex_consuming_id = 0; /* one commitment, even on a failed roll */
    if (!ex_conserve_sources(obj, &ex_consuming_sources))
        return FALSE;
    ex_conserved_use_fee(obj);
    obj->in_use = TRUE;
    ex_conserved_id = obj->o_id;
    pline("%s is conserved.", The(xname(obj)));
    update_inventory();
    return TRUE;
}
