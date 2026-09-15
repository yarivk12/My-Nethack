/* NetHack 5.0	questpgr.c	$NHDT-Date: 1704043695 2023/12/31 17:28:15 $  $NHDT-Branch: keni-luabits2 $:$NHDT-Revision: 1.87 $ */
/*      Copyright 1991, M. Stephenson                             */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dlb.h"

/*  quest-specific pager routines. */

#define QTEXT_FILE "quest.lua"

#ifdef TTY_GRAPHICS
#include "wintty.h"
#endif

staticfn const char *intermed(void);
/* sometimes find_qarti(gi.invent), and gi.invent can be null */
staticfn struct obj *find_qarti(struct obj *) NO_NNARGS;
staticfn const char *neminame(void);
staticfn const char *guardname(void);
staticfn const char *homebase(void);
staticfn void qtext_pronoun(char, char);
staticfn void convert_arg(char);
staticfn void convert_line(char *,char *);
staticfn void deliver_by_pline(const char *);
staticfn void deliver_by_window(const char *, int);
staticfn boolean skip_pager(boolean);
staticfn boolean com_pager_core(const char *, const char *, boolean, char **);

short
quest_info(int typ)
{
    switch (typ) {
    case 0:
        return gu.urole.questarti;
    case MS_LEADER:
        return gu.urole.ldrnum;
    case MS_NEMESIS:
        return gu.urole.neminum;
    case MS_GUARDIAN:
        return gu.urole.guardnum;
    default:
        impossible("quest_info(%d)", typ);
    }
    return 0;
}

/* return your role leader's name */
const char *
ldrname(void)
{
    int i = gu.urole.ldrnum;

    Sprintf(gn.nambuf, "%s%s", type_is_pname(&mons[i]) ? "" : "the ",
            mons[i].pmnames[NEUTRAL]);
    return gn.nambuf;
}

/* return your intermediate target string */
staticfn const char *
intermed(void)
{
    return gu.urole.intermed;
}

boolean
is_quest_artifact(struct obj *otmp)
{
    return (boolean) (otmp->oartifact == gu.urole.questarti);
}

staticfn struct obj *
find_qarti(struct obj *ochain)
{
    struct obj *otmp, *qarti;

    for (otmp = ochain; otmp; otmp = otmp->nobj) {
        if (is_quest_artifact(otmp))
            return otmp;
        if (Has_contents(otmp) && (qarti = find_qarti(otmp->cobj)) != 0)
            return qarti;
    }
    return (struct obj *) 0;
}

/* check several object chains for the quest artifact to determine
   whether it is present on the current level */
struct obj *
find_quest_artifact(unsigned whichchains)
{
    struct monst *mtmp;
    struct obj *qarti = 0;

    if ((whichchains & (1 << OBJ_INVENT)) != 0)
        qarti = find_qarti(gi.invent);
    if (!qarti && (whichchains & (1 << OBJ_FLOOR)) != 0)
        qarti = find_qarti(fobj);
    if (!qarti && (whichchains & (1 << OBJ_MINVENT)) != 0)
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
            if (DEADMONSTER(mtmp))
                continue;
            if ((qarti = find_qarti(mtmp->minvent)) != 0)
                break;
        }
    if (!qarti && (whichchains & (1 << OBJ_MIGRATING)) != 0) {
        /* check migrating objects and minvent of migrating monsters */
        for (mtmp = gm.migrating_mons; mtmp; mtmp = mtmp->nmon) {
            if (DEADMONSTER(mtmp))
                continue;
            if ((qarti = find_qarti(mtmp->minvent)) != 0)
                break;
        }
        if (!qarti)
            qarti = find_qarti(gm.migrating_objs);
    }
    if (!qarti && (whichchains & (1 << OBJ_BURIED)) != 0)
        qarti = find_qarti(svl.level.buriedobjlist);

    return qarti;
}

/* return your role nemesis' name */
staticfn const char *
neminame(void)
{
    int i = gu.urole.neminum;

    Sprintf(gn.nambuf, "%s%s", type_is_pname(&mons[i]) ? "" : "the ",
            mons[i].pmnames[NEUTRAL]);
    return gn.nambuf;
}

staticfn const char *
guardname(void) /* return your role leader's guard monster name */
{
    int i = gu.urole.guardnum;

    return mons[i].pmnames[NEUTRAL];
}

staticfn const char *
homebase(void) /* return your role leader's location */
{
    return gu.urole.homebase;
}

/* returns 1 if nemesis death message mentions noxious fumes, otherwise 0;
   does not display the message */
int
stinky_nemesis(struct monst *mon)
{
    char *mesg = 0;
    int res = 0;

#if 0
    /* get the quest text for dying nemesis; don't assume that mon is
       hero's own role's nemesis (overkill since m_detach() and nemdead()
       both make that assumption--valid for normal play but not necessarily
       valid for wizard mode) */
    int r, mndx = monsndx(mon->data);
    for (r = 0; roles[r].name.m || roles[r].name.f; ++r)
        if (roles[r].neminum == mndx) {
            (void) com_pager_core(roles[r].filecode, "killed_nemesis",
                                  FALSE, &mesg);
            break;
        }
#else
    nhUse(mon);
    /* since nemdead() just gave the message for hero's nemesis even if 'mon'
       is some other role's nemesis (feasible in wizard mode), base any gas
       cloud on the text that was shown even if not appropriate for 'mon' */
    (void) com_pager_core(gu.urole.filecode, "killed_nemesis", FALSE, &mesg);
#endif

    /* this is somewhat fragile; it assumes that when both {noxious or
       poisonous or toxic} and {gas or fumes} are present, the latter
       refers to the former rather than to something unrelated; it does
       make sure that fumes occurs after noxious rather than before */
    if (mesg) {
        char *p;

        /* change newlines into spaces to cope with "...noxious\nfumes..." */
        (void) strNsubst(mesg, "\n", " ", 0);

        if (((p = strstri(mesg, "noxious")) != 0
             || (p = strstri(mesg, "poisonous")) != 0
             || (p = strstri(mesg, "toxic")) != 0)
            && (strstri(p, " gas") || strstri(p, " fumes")))
            res = 1;

        free((genericptr_t) mesg);
    }
    return res;
}

/* replace deity, leader, nemesis, or artifact name with pronoun;
   overwrites cvt_buf[] */
staticfn void
qtext_pronoun(
    char who,   /* 'd' => deity, 'l' => leader, 'n' => nemesis, 'o' => arti */
    char which) /* 'h'|'H'|'i'|'I'|'j'|'J' */
{
    const char *pnoun;
    int godgend;
    char lwhich = lowc(which); /* H,I,J -> h,i,j */

    /*
     * Invalid subject (not d,l,n,o) yields neuter, singular result.
     *
     * For %o, treat all artifacts as neuter; some have plural names,
     * which genders[] doesn't handle; cvt_buf[] already contains name.
     */
    if (who == 'o'
        && (strstri(gc.cvt_buf, "Eyes ")
            || strcmpi(gc.cvt_buf, makesingular(gc.cvt_buf)))) {
        pnoun = (lwhich == 'h') ? "they"
                : (lwhich == 'i') ? "them"
                : (lwhich == 'j') ? "their" : "?";
    } else {
        godgend = (who == 'd') ? svq.quest_status.godgend
            : (who == 'l') ? svq.quest_status.ldrgend
            : (who == 'n') ? svq.quest_status.nemgend
            : 2; /* default to neuter */
        pnoun = (lwhich == 'h') ? genders[godgend].he
                : (lwhich == 'i') ? genders[godgend].him
                : (lwhich == 'j') ? genders[godgend].his : "?";
    }
    Strcpy(gc.cvt_buf, pnoun);
    /* capitalize for H,I,J */
    if (lwhich != which)
        gc.cvt_buf[0] = highc(gc.cvt_buf[0]);
    return;
}

staticfn void
convert_arg(char c)
{
    const char *str;

    switch (c) {
    case 'p':
        str = svp.plname;
        break;
    case 'c':
        str = (flags.female && gu.urole.name.f) ? gu.urole.name.f
                                               : gu.urole.name.m;
        break;
    case 'r':
        str = rank_of(u.ulevel, Role_switch, flags.female);
        break;
    case 'R':
        str = rank_of(MIN_QUEST_LEVEL, Role_switch, flags.female);
        break;
    case 's':
        str = (flags.female) ? "sister" : "brother";
        break;
    case 'S':
        str = (flags.female) ? "daughter" : "son";
        break;
    case 'l':
        str = ldrname();
        break;
    case 'i':
        str = intermed();
        break;
    case 'O':
    case 'o':
        str = the(artiname(gu.urole.questarti));
        if (c == 'O') {
            /* shorten "the Foo of Bar" to "the Foo"
               (buffer returned by the() is modifiable) */
            char *p = strstri(str, " of ");

            if (p)
                *p = '\0';
        }
        break;
    case 'n':
        str = neminame();
        break;
    case 'g':
        str = guardname();
        break;
    case 'G':
        str = align_gtitle(u.ualignbase[A_ORIGINAL]);
        break;
    case 'H':
        str = homebase();
        break;
    case 'a':
        str = align_str(u.ualignbase[A_ORIGINAL]);
        break;
    case 'A':
        str = align_str(u.ualign.type);
        break;
    case 'd':
        str = align_gname(u.ualignbase[A_ORIGINAL]);
        break;
    case 'D':
        str = align_gname(A_LAWFUL);
        break;
    case 'C':
        str = "chaotic";
        break;
    case 'N':
        str = "neutral";
        break;
    case 'L':
        str = "lawful";
        break;
    case 'x':
        str = Blind ? "sense" : "see";
        break;
    case 'Z':
        str = svd.dungeons[0].dname;
        break;
    case '%':
        str = "%";
        break;
    default:
        str = "";
        break;
    }
    Strcpy(gc.cvt_buf, str);
}

staticfn void
convert_line(char *in_line, char *out_line)
{
    char *c, *cc;

    cc = out_line;
    for (c = in_line; *c; c++) {
        *cc = 0;
        switch (*c) {
        case '\r':
        case '\n':
            *(++cc) = 0;
            return;

        case '%':
            if (*(c + 1)) {
                convert_arg(*(++c));
                switch (*(++c)) {
                /* insert "a"/"an" prefix */
                case 'A':
                    Strcat(cc, An(gc.cvt_buf));
                    cc += strlen(cc);
                    continue; /* for */
                case 'a':
                    Strcat(cc, an(gc.cvt_buf));
                    cc += strlen(cc);
                    continue; /* for */

                /* capitalize */
                case 'C':
                    gc.cvt_buf[0] = highc(gc.cvt_buf[0]);
                    break;

                /* replace name with pronoun;
                   valid for %d, %l, %n, and %o */
                case 'h': /* he/she */
                case 'H': /* He/She */
                case 'i': /* him/her */
                case 'I':
                case 'j': /* his/her */
                case 'J':
                    if (strchr("dlno", lowc(*(c - 1))))
                        qtext_pronoun(*(c - 1), *c);
                    else
                        --c; /* default action */
                    break;

                /* pluralize */
                case 'P':
                    gc.cvt_buf[0] = highc(gc.cvt_buf[0]);
                    FALLTHROUGH;
                    /*FALLTHRU*/
                case 'p':
                    Strcpy(gc.cvt_buf, makeplural(gc.cvt_buf));
                    break;

                /* append possessive suffix */
                case 'S':
                    gc.cvt_buf[0] = highc(gc.cvt_buf[0]);
                    FALLTHROUGH;
                    /*FALLTHRU*/
                case 's':
                    Strcpy(gc.cvt_buf, s_suffix(gc.cvt_buf));
                    break;

                /* strip any "the" prefix */
                case 't':
                    if (!strncmpi(gc.cvt_buf, "the ", 4)) {
                        Strcat(cc, &gc.cvt_buf[4]);
                        cc += strlen(cc);
                        continue; /* for */
                    }
                    break;

                default:
                    --c; /* undo switch increment */
                    break;
                }
                Strcat(cc, gc.cvt_buf);
                cc += strlen(gc.cvt_buf);
                break;
            }
            FALLTHROUGH;
            /* FALLTHRU */
        default:
            *cc++ = *c;
            break;
        }
        if (cc > &out_line[BUFSZ - 1])
            panic("convert_line: overflow");
    }
    *cc = 0;
    return;
}

staticfn void
deliver_by_pline(const char *str)
{
    char in_line[BUFSZ], out_line[BUFSZ];
    const char *msgp = str, *msgend = eos((char *) str);

    while (msgp < msgend) {
        /* copynchars() will stop at newline if it finds one */
        copynchars(in_line, msgp, (int) sizeof in_line - 1);
        msgp += strlen(in_line) + 1;

        convert_line(in_line, out_line);
        pline("%s", out_line);
    }
}

staticfn void
deliver_by_window(const char *msg, int how)
{
    char in_line[BUFSZ], out_line[BUFSZ];
    const char *msgp = msg, *msgend = eos((char *) msg);
    winid datawin = create_nhwindow(how);

    while (msgp < msgend) {
        /* copynchars() will stop at newline if it finds one */
        copynchars(in_line, msgp, (int) sizeof in_line - 1);
        msgp += strlen(in_line) + 1;

        convert_line(in_line, out_line);
        putstr(datawin, 0, out_line);
    }

    display_nhwindow(datawin, TRUE);
    destroy_nhwindow(datawin);
}

staticfn boolean
skip_pager(boolean common UNUSED)
{
    /* WIZKIT: suppress plot feedback if starting with quest artifact */
    if (program_state.wizkit_wishing)
        return TRUE;
    return FALSE;
}

staticfn boolean
com_pager_core(
    const char *section,
    const char *msgid,
    boolean showerror,
    char **rawtext)
{
    static const char *const howtoput[] = {
        "pline", "window", "text", "menu", "default", NULL
    };
    static const int howtoput2i[] = { 1, 2, 2, 3, 0, 0 };
    int output;
    lua_State *L;
    char *text = NULL, *synopsis = NULL, *fallback_msgid = NULL;
    boolean res = FALSE;
    nhl_sandbox_info sbi = {NHL_SB_SAFE, 1*1024*1024, 0, 1*1024*1024};

    if (skip_pager(TRUE))
        return FALSE;

    L = nhl_init(&sbi);
    if (!L) {
        if (showerror)
            impossible("com_pager: nhl_init() failed");
        goto compagerdone;
    }

    if (!nhl_loadlua(L, QTEXT_FILE)) {
        if (showerror)
            impossible("com_pager: %s not found.", QTEXT_FILE);
        goto compagerdone;
    }

    lua_settop(L, 0);
    lua_getglobal(L, "questtext");
    if (!lua_istable(L, -1)) {
        if (showerror)
            impossible("com_pager: questtext in %s is not a lua table",
                       QTEXT_FILE);
        goto compagerdone;
    }

    lua_getfield(L, -1, section);
    if (!lua_istable(L, -1)) {
        if (showerror)
            impossible("com_pager: questtext[%s] in %s is not a lua table",
                       section, QTEXT_FILE);
        goto compagerdone;
    }

 tryagain:
    lua_getfield(L, -1, fallback_msgid ? fallback_msgid : msgid);
    if (!lua_istable(L, -1)) {
        if (!fallback_msgid) {
            /* Do we have questtxt[msg_fallbacks][<msgid>]? */
            lua_getfield(L, -3, "msg_fallbacks");
            if (lua_istable(L, -1)) {
                fallback_msgid = get_table_str_opt(L, msgid, NULL);
                lua_pop(L, 2);
                if (fallback_msgid)
                    goto tryagain;
            }
        }
        if (showerror) {
            if (!fallback_msgid)
                impossible(
                      "com_pager: questtext[%s][%s] in %s is not a lua table",
                           section, msgid, QTEXT_FILE);
            else
                impossible(
           "com_pager: questtext[%s][%s] and [][%s] in %s are not lua tables",
                           section, msgid, fallback_msgid, QTEXT_FILE);
        }
        goto compagerdone;
    }

    text = get_table_str_opt(L, "text", NULL);
    if (rawtext) {
        *rawtext = dupstr(text);
        res = TRUE;
        goto compagerdone;
    }
    synopsis = get_table_str_opt(L, "synopsis", NULL);
    output = howtoput2i[get_table_option(L, "output", "default", howtoput)];

    if (!text) {
        int nelems;

        lua_len(L, -1);
        nelems = (int) lua_tointeger(L, -1);
        lua_pop(L, 1);
        if (nelems < 2) {
            if (showerror)
                impossible(
              "com_pager: questtext[%s][%s] in %s is not an array of strings",
                           section, fallback_msgid ? fallback_msgid : msgid,
                           QTEXT_FILE);
            goto compagerdone;
        }
        nelems = rn2(nelems) + 1;
        lua_pushinteger(L, nelems);
        lua_gettable(L, -2);
        text = dupstr(luaL_checkstring(L, -1));
    }

    /* switch from by_pline to by_window if line has multiple segments or
       is unreasonably long (the latter ought to checked after formatting
       conversions rather than before...) */
    if (output == 0 && (strchr(text, '\n') || strlen(text) >= BUFSZ - 1)) {
        output = 2;

        /*
         * FIXME:  should update quest.lua to include proper synopsis line
         * for any item subject to having its delivery converted to by_window.
         */
        if (!synopsis) {
            char tmpbuf[BUFSZ];

            Sprintf(tmpbuf, "[%.*s]", BUFSZ - 1 - 2, text);
            /* change every newline character to a space */
            (void) strNsubst(tmpbuf, "\n", " ", 0);
            synopsis = dupstr(tmpbuf);
        }
    }

    if (output == 0 || output == 1)
        deliver_by_pline(text);
    else
        deliver_by_window(text, (output == 3) ? NHW_MENU : NHW_TEXT);

    if (synopsis) {
        char in_line[BUFSZ], out_line[BUFSZ];

#if 0   /* not yet -- brackets need to be removed from quest.lua */
        Sprintf(in_line, "[%.*s]",
                (int) (sizeof in_line - sizeof "[]"), synopsis);
#else
        Strcpy(in_line, synopsis);
#endif
        convert_line(in_line, out_line);
        /* bypass message delivery but be available for ^P recall */
        putmsghistory(out_line, FALSE);
    }
    res = TRUE;

 compagerdone:
    if (text)
        free((genericptr_t) text);
    if (synopsis)
        free((genericptr_t) synopsis);
    if (fallback_msgid)
        free((genericptr_t) fallback_msgid);
    nhl_done(L);
    return res;
}

void
com_pager(const char *msgid)
{
    (void) com_pager_core("common", msgid, TRUE, (char **) 0);
}

void
qt_pager(const char *msgid)
{
    if (!com_pager_core(gu.urole.filecode, msgid, FALSE, (char **) 0))
        (void) com_pager_core("common", msgid, TRUE, (char **) 0);
}

struct permonst *
qt_montype(void)
{
    int qpm;

    if (rn2(5)) {
        qpm = gu.urole.enemy1num;
        if (qpm != NON_PM && rn2(5) && !(svm.mvitals[qpm].mvflags & G_GENOD))
            return &mons[qpm];
        return mkclass(gu.urole.enemy1sym, 0);
    }
    qpm = gu.urole.enemy2num;
    if (qpm != NON_PM && rn2(5) && !(svm.mvitals[qpm].mvflags & G_GENOD))
        return &mons[qpm];
    return mkclass(gu.urole.enemy2sym, 0);
}

/* The pinned donor calls its entire Neutral dungeon "Outlands".  Keep that
 * definition explicit so Lost Cities and R'lyeh never inherit its rules. */
boolean
step10b_is_outlands_context(enum step10b_level_context context)
{
    return (boolean) (context >= STEP10B_CTX_GATE
                      && context <= STEP10B_CTX_SUM);
}

/* Dormant, controlled versions of the pinned Neutral selection logic.  The
 * caller supplies every random outcome; Step 10C will own branch identity and
 * activation.  `detail` is rn2(2) for outer branch 0 and rn2(4) for branch 3. */
int
step10b_neutral_montype(int outer, int chance, int difficulty, int detail)
{
    switch (outer) {
    case 0:
        return detail ? PM_HORSE : STEP10B_NEUTRAL_QUADRUPED;
    case 1:
        if (chance < 10 && mons[PM_ARGENACH_RILMANI].difficulty <= difficulty)
            return PM_ARGENACH_RILMANI;
        if (chance < 30 && mons[PM_CUPRILACH_RILMANI].difficulty <= difficulty)
            return PM_CUPRILACH_RILMANI;
        /* Preserve donor's duplicate Cuprilach result after testing
         * Ferrumach's strength; this is deliberately not corrected. */
        if (chance < 60 && mons[PM_FERRUMACH_RILMANI].difficulty <= difficulty)
            return PM_CUPRILACH_RILMANI;
        return PM_PLUMACH_RILMANI;
    case 2:
        if (chance < 5 && mons[PM_ARA_KAMEREL].difficulty <= difficulty)
            return PM_ARA_KAMEREL;
        if (chance < 15 && mons[PM_SHARAB_KAMEREL].difficulty <= difficulty)
            return PM_SHARAB_KAMEREL;
        return PM_AMM_KAMEREL;
    case 3:
        return detail < 3 ? STEP10B_NEUTRAL_QUADRUPED
                          : PM_SHATTERED_ZIGGURAT_CULTIST;
    case 4:
        return PM_PLAINS_CENTAUR;
    default:
        return NON_PM;
    }
}

/* Dormant until the Neutral/Lost Cities encounter work is authorized.  The
 * caller supplies the context, the already-bounded 1-in-5000 roll, and the
 * unique monster's current vital flags. */
int
step10b_center_candidate(int context, int roll, unsigned mvflags)
{
    return (context == STEP10B_CENTER_NEUTRAL
            || context == STEP10B_CENTER_LOST_CITIES)
               && roll == 0 && !(mvflags & G_GONE)
           ? PM_CENTER_OF_ALL : NON_PM;
}

/* Shared selector for the live Alhoon key birth path.  Artifact allocation is
 * kept in makemon.c so native mksobj/oname lifecycle remains authoritative. */
int
step10b_alhoon_key_choice(boolean second_exists, boolean third_exists)
{
    if (!second_exists)
        return STEP10B_KEY_SECOND;
    if (!third_exists)
        return STEP10B_KEY_THIRD;
    return STEP10B_KEY_ORDINARY;
}

int
step10b_sum_montype(int chance, int difficulty)
{
    if (chance < 5 && mons[PM_AURUMACH_RILMANI].difficulty <= difficulty)
        return PM_AURUMACH_RILMANI;
    if (chance < 15 && mons[PM_ARGENACH_RILMANI].difficulty <= difficulty)
        return PM_ARGENACH_RILMANI;
    if (chance < 35 && mons[PM_CUPRILACH_RILMANI].difficulty <= difficulty)
        return PM_CUPRILACH_RILMANI;
    if (chance < 65 && mons[PM_FERRUMACH_RILMANI].difficulty <= difficulty)
        return PM_CUPRILACH_RILMANI; /* exact pinned duplicate */
    return PM_PLUMACH_RILMANI;
}

/* Exact source formula: rnd(80 + level_difficulty()).  The strict `>`
 * cumulative tests make roll 80 an iron golem and rolls >=100 use a uniformly
 * selected fallback table entry rather than normalized percentages. */
int
step10b_neutral_squad(int level_difficulty, int roll, int fallback)
{
    static const int table[] = {
        PM_FERRUMACH_RILMANI, PM_IRON_GOLEM,
        PM_ARGENTUM_GOLEM, PM_CUPRILACH_RILMANI
    };

    if (level_difficulty < 0 || roll < 1 || roll > 80 + level_difficulty
        || fallback < 0 || fallback >= SIZE(table))
        return NON_PM;
    if (80 > roll)
        return table[0];
    if (95 > roll)
        return table[1];
    if (99 > roll)
        return table[2];
    if (100 > roll)
        return table[3];
    return table[fallback];
}

/* Complete pinned R'lyeh override, deliberately dormant until Step 10C owns
 * branch identity.  `d(1, 100)` precedes the 1-in-20 gate in the donor and
 * every group loop is inclusive.  Only genocide, not extinction, selects the
 * class fallback. */
staticfn int
step10b_rlyeh_emit(int pm, char mclass, coordxy x, coordxy y)
{
    struct permonst *ptr = (svm.mvitals[pm].mvflags & G_GENOD)
                              ? mkclass(mclass, G_NOHELL | G_HELL)
                              : &mons[pm];

    return ptr && makemon(ptr, x, y, NO_MM_FLAGS) ? 1 : 0;
}

int
step10b_rlyeh_create(coordxy x, coordxy y)
{
    int chance = d(1, 100), num, made = 0;

    if (rn2(20))
        return 0;
    if (chance < 2) {
        for (num = d(2, 3); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_HUNTING_HORROR, S_UMBER, x, y);
    } else if (chance < 6) {
        for (num = d(2, 4); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_BYAKHEE, S_UMBER, x, y);
    } else if (chance < 8) {
        made += step10b_rlyeh_emit(PM_SHOGGOTH, S_BLOB, x, y);
    } else if (chance < 10) {
        made += step10b_rlyeh_emit(PM_DEEPEST_ONE, S_HUMANOID, x, y);
        for (num = rnd(4); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_DEEPER_ONE, S_HUMANOID, x, y);
        for (num = rn1(2, 1); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_DEEP_ONE, S_HUMANOID, x, y);
    } else if (chance < 30) {
        for (num = rnd(3); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_MASTER_MIND_FLAYER, S_UMBER, x, y);
    } else if (chance < 50) {
        for (num = rn1(2, 2); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_MIND_FLAYER, S_UMBER, x, y);
    } else if (chance < 70) {
        for (num = rnd(6); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_DEEPER_ONE, S_HUMANOID, x, y);
    } else {
        for (num = rn1(4, 3); num >= 0; --num)
            made += step10b_rlyeh_emit(PM_DEEP_ONE, S_HUMANOID, x, y);
    }
    return made;
}

/* special levels can include a custom arrival message; display it */
void
deliver_splev_message(void)
{
    /* there's no provision for delivering via window instead of pline */
    if (gl.lev_message) {
        deliver_by_pline(gl.lev_message);

        free((genericptr_t) gl.lev_message);
        gl.lev_message = NULL;
    }
}

#undef QTEXT_FILE

/*questpgr.c*/
