/* My-NetHack modification notice, added 2026-10-15.
 * Local project integration/modifications; original authorship is retained.
 * Recorded project change dates (Git author dates): 2026-10-15.
 * See doc/license-compliance.md and doc/my-nethack-modifications.json.
 * Existing copyright, attribution, license and warranty notices still apply.
 */
/* Independent defensive events and their native lifecycle. */
static struct ex_melee_context *ex_incoming;
static struct monst *ex_area_caster;
static int ex_area_level;
void ex_area_spell_begin(struct monst *caster, int level)
{
    ex_area_caster = caster; ex_area_level = level;
}
void ex_area_spell_end(void) { ex_area_caster = NULL; }
int ex_area_spell_square(int x, int y)
{
    return ex_area_caster && u_at(x,y) && ex_absorb(ex_area_caster,TRUE,ex_area_level);
}
static struct ex_guardian_window *
ex_window(unsigned source, long until, int create)
{
    unsigned i;
    struct ex_guardian_window *empty = NULL;
    for (i = 0; i < EX_MAX_SOURCES; ++i) {
        struct ex_guardian_window *w = &u.ex_guardians[i];
        if (w->source == source && w->until == until) return w;
        if (!empty && (!w->source || w->until <= svm.moves)) empty = w;
    }
    if (!create || !empty) return NULL;
    empty->source = source; empty->until = until; empty->count = 0;
    return empty;
}

void
ex_guardian_detach(struct monst *m)
{
    struct ex_guardian_window *w;
    if (!m || !m->ex_guardian_until) return;
    w = ex_window(m->ex_guardian_source,m->ex_guardian_until,0);
    if (w && w->count) --w->count;
    m->ex_guardian_until = 0; m->ex_guardian_source = 0;
}
void
ex_guardian_clone(struct monst *m)
{
    struct ex_guardian_window *w;
    if (!m || !m->ex_guardian_until) return;
    w = ex_window(m->ex_guardian_source,m->ex_guardian_until,0);
    if (w) ++w->count;
}

static int ex_low_hp(void)
{
    int hp = Upolyd ? u.mh : u.uhp, maximum = Upolyd ? u.mhmax : u.uhpmax;
    return hp > 0 && maximum > 0 && (long) hp * 4 <= maximum;
}

static struct monst *ex_defender_by_id(unsigned id)
{
    struct monst *m;
    for (m = fmon; m; m = m->nmon)
        if (m->m_id == id && !DEADMONSTER(m)) return m;
    return NULL;
}

int
ex_guardian_expire(struct monst *m)
{
    if (!m || !m->ex_guardian_until || svm.moves < m->ex_guardian_until)
        return 0;
    /* Migration overloads mx/my. Death and loot need a real map square. */
    if ((m->mstate & (MON_MIGRATING | MON_LIMBO)) || !isok(m->mx, m->my)
        || (m != u.usteed && m_at(m->mx, m->my) != m))
        return 0;
    /* Unlink before native death, which may lifesave, revert, or recurse.
     * A lifesaved survivor is permanently an ordinary pet. */
    ex_guardian_detach(m);
    if (DEADMONSTER(m)) return 1;
    mondied(m);
    return DEADMONSTER(m) ? 1 : 0;
}

void ex_monster_arrived(struct monst *m) { (void) ex_guardian_expire(m); }

static void ex_guardian_group(struct obj *source)
{
    struct permonst *pm;
    struct monst *m;
    coord cc;
    int n, i, placed = 0;
    long deadline;
    if (source->exotic.guardian_until > svm.moves) return;
    n = rnd(3); deadline = svm.moves + 30;
    for (i = 0; i < n; ++i) {
        pm = rndmonst_adj(0,0);
        if (!pm || (pm->geno & G_UNIQ) || is_rider(pm)
            || !enexto(&cc,u.ux,u.uy,pm)) continue;
        m = makemon(pm,cc.x,cc.y,MM_EDOG | NO_MINVENT | MM_NOMSG);
        if (!m) continue;
        initedog(m,FALSE);
        m->ex_guardian_source = source->o_id;
        m->ex_guardian_until = deadline;
        ++placed;
    }
    if (placed) {
        struct ex_guardian_window *w = ex_window(source->o_id,deadline,1);
        if (w) w->count = placed;
        source->exotic.guardian_until = deadline;
    }
}

void
ex_melee_begin(struct ex_melee_context *e, struct monst *attacker)
{
    memset(e,0,sizeof *e);
    e->attacker = attacker->m_id;
    e->hostile = !ex_secondary() && !attacker->mpeaceful && !attacker->mtame
                 && m_next2u(attacker);
    e->before_hp = Upolyd ? u.mh : u.uhp;
    e->before_poly = !!Upolyd;
    ex_snapshot(&e->sources,&gy.youmonst);
    ex_incoming = e;
    if (!ex_low_hp()) u.ex_teleport_disarmed = FALSE;
}

void
ex_melee_damage(struct monst *attacker, int damage)
{
    if (ex_incoming && !ex_secondary() && attacker
        && attacker->m_id == ex_incoming->attacker && damage > 0)
        ex_incoming->damage += damage;
}

void
ex_melee_end(struct ex_melee_context *e)
{
    struct obj *source, *weapon;
    struct monst *attacker;
    unsigned i;
    int original_low, mortality, hp = Upolyd ? u.mh : u.uhp;
    ex_incoming = NULL;
    if (!e->hostile) return;
    if (!!Upolyd == e->before_poly && hp < e->before_hp)
        e->damage = max(e->damage,e->before_hp - hp);
    original_low = e->damage > 0 && ex_low_hp();
    if (!ex_low_hp()) u.ex_teleport_disarmed = FALSE;
    mortality = u.umortality;
    for (i = 0; i < e->sources.count; ++i) {
        if (gm.multi < 0 || (Upolyd ? u.mh : u.uhp) < 1
            || u.umortality != mortality) break;
        attacker = ex_defender_by_id(e->attacker);
        if (!attacker || !m_next2u(attacker)) break;
        source = ex_source(&e->sources,i,EX_COUNTER,&gy.youmonst);
        if (!source) continue;
        if (source == uwep || (u.twoweap && source == uswapwep)) weapon = source;
        else if (source->exotic.counter_weapon == EX_WEAPON_MAIN) weapon = uwep;
        else if (source->exotic.counter_weapon == EX_WEAPON_OFF && u.twoweap) weapon = uswapwep;
        else continue;
        if (!weapon || (weapon->oclass != WEAPON_CLASS && !is_weptool(weapon))
            || is_launcher(weapon)
            || is_ammo(weapon) || is_missile(weapon)) continue;
        if (!ex_roll(EX_COUNTER)) continue;
        ex_secondary_begin();
        (void) ex_counterattack(attacker,weapon);
        ex_secondary_end();
    }
    if (!original_low || !ex_low_hp()) return;
    for (i = 0; i < e->sources.count && ex_low_hp(); ++i) {
        source = ex_source(&e->sources,i,EX_GUARDIANS,&gy.youmonst);
        if (source) ex_guardian_group(source);
    }
    for (i = 0; i < e->sources.count && ex_low_hp()
                && !u.ex_teleport_disarmed; ++i) {
        source = ex_source(&e->sources,i,EX_TELEPORT,&gy.youmonst);
        if (source && ex_reactive_teleport()) {
            u.ex_teleport_disarmed = TRUE;
            break;
        }
    }
}

int
ex_absorb(struct monst *caster, int listed, int level)
{
    struct ex_sources sources;
    unsigned i;
    int energy;
    if (!caster || caster->mpeaceful || caster->mtame || ex_secondary()) return 0;
    ex_snapshot(&sources,&gy.youmonst);
    for (i = 0; i < sources.count; ++i) {
        if (!ex_source(&sources,i,EX_ABSORB,&gy.youmonst) || !ex_roll(EX_ABSORB)) continue;
        energy = listed ? max(1,level) * rnd(6) : d(6,6);
        u.uen = min(u.uenmax,u.uen + energy);
        disp.botl = TRUE;
        You("absorb the spell's energy.");
        return 1;
    }
    return 0;
}

void
ex_world_turn(void)
{
    struct monst *m, *next;
    if (!ex_low_hp()) u.ex_teleport_disarmed = FALSE;
    for (m = fmon; m; m = next) {
        next = m->nmon;
        (void) ex_guardian_expire(m);
    }
}

void ex_health_changed(void)
{
    if (!ex_low_hp()) u.ex_teleport_disarmed = FALSE;
}

void
ex_inspect(struct obj *o, int win)
{
    static const char *const names[EX_COUNT] = {
        "Disarming (20%)", "Enhanced Steal (20%)", "Polymorphic Strike (20%)",
        "Anti-Regeneration (20%, 5 turns)", "Frenzy (20%, 1-3 ticks)",
        "Executioner (20% ordinary, 10% eligible unique)", "Elemental Chain (30%, 3 hops)",
        "Counterstrike (33%)", "Reactive Teleportation", "Emergency Summoning (30 turns)",
        "Spell Absorption (20%)", "Universal Trap Disarming", "Wall Consumption",
        "Corpse Preservation", "Consumable Conservation (30/20/10%)",
        "Environmental Transmutation", "Anti-Theft Ward (100%)", "Ability Mute (25%, 2d3 turns)"
    };
    static const char *const modes[] = {
        "inactive", "directly carried", "carried, including containers",
        "wielded", "worn in the assigned slot", "manual activation", "invoked"
    };
    int p, k, count;
    long remaining, charge;
    char buf[BUFSZ];
    ex_preserve_inspect(o,win);
    if (!o->oartifact || !o->known || !o->dknown
        || undiscovered_artifact(o->oartifact)) return;
    for (p = 0; p < EX_COUNT; ++p) {
        if (!(o->exotic.grants & (1UL << p))) continue;
        Snprintf(buf,sizeof buf,"%s: %s (%s).",names[p],
                 modes[min(o->exotic.activation[p],EX_INVOKED)],
                 ex_active(o,p,&gy.youmonst) ? "active" : "inactive");
        putstr(win,ATR_NONE,buf);
        if (p == EX_TELEPORT)
            putstr(win,ATR_NONE,u.ex_teleport_disarmed
                   ? "Disarmed: rearm when active HP rises above 25%."
                   : "Armed: rearm after success when active HP rises above 25%.");
        if (p == EX_GUARDIANS) {
            struct ex_guardian_window *w = ex_window(o->o_id,o->exotic.guardian_until,0);
            count = w && w->until > svm.moves ? w->count : 0;
            remaining = max(0L,o->exotic.guardian_until - svm.moves);
            Snprintf(buf,sizeof buf,"%s: %d linked guardians, %ld turns remaining.",
                     !remaining ? "Ready" : count ? "Active" : "Recovering",count,remaining);
            putstr(win,ATR_NONE,buf);
        }
        if (p == EX_DIG || p == EX_TRANSMUTE) {
            struct obj copy = *o; /* inspection cannot mutate saved charge state */
            k = p == EX_DIG ? 0 : 1; ex_resources(&copy);
            charge = k ? 10 : 100;
            remaining = copy.exotic.charges[k] == (k ? 10 : 5) ? 0
                : charge - (svm.moves-copy.exotic.charge_turn[k]);
            Snprintf(buf,sizeof buf,"Charges: %u/%d, next recharge in %ld turns.",
                     copy.exotic.charges[k],k ? 10 : 5,remaining);
            putstr(win,ATR_NONE,buf);
        }
    }
}

int
ex_invoke(struct obj *o, int native)
{
    int props[2] = {EX_DIG,EX_TRANSMUTE}, selected = -1, n = 0, p, result;
    anything any;
    winid win;
    menu_item *items = NULL;
    for (p = 0; p < 2; ++p) if (ex_active(o,props[p],&gy.youmonst)) {selected = props[p]; ++n;}
    if (!n) return -1;
    if (n > 1 || native) {
        win = create_nhwindow(NHW_MENU); start_menu(win,MENU_BEHAVE_STANDARD);
        for (p = 0; p < 2; ++p) {
            if (!ex_active(o,props[p],&gy.youmonst)) continue;
            any = cg.zeroany; any.a_int = props[p]+1;
            add_menu(win,&nul_glyphinfo,&any,0,0,ATR_NONE,NO_COLOR,
                     p ? "Environmental Transmutation" : "Wall Consumption",MENU_ITEMFLAGS_NONE);
        }
        if (native) {
            any = cg.zeroany; any.a_int = EX_COUNT+1;
            add_menu(win,&nul_glyphinfo,&any,0,0,ATR_NONE,NO_COLOR,
                     "Existing invocation",MENU_ITEMFLAGS_NONE);
        }
        end_menu(win,"Invoke which ability?");
        result = select_menu(win,PICK_ONE,&items); destroy_nhwindow(win);
        if (result < 1) {if (items) free(items); return 0;}
        selected = items[0].item.a_int-1; free(items);
        if (selected == EX_COUNT) return -1;
    }
    return ex_utility_invoke(o,selected);
}
int ex_apply(struct obj *o) { return ex_preserve_apply(o); }
