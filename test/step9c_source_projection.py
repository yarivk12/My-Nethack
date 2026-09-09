"""Remove only explicitly scoped 9C additions for older whole-file contracts.

This preserves every pre-existing shop table entry and native function byte.
New branch-only helper bodies have their own focused donor/runtime tests.
"""
import re

def replace_once(text, after, before=''):
    assert text.count(after) == 1, after
    return text.replace(after, before)

def remove_function(text, name):
    match = re.search(r'(?m)^staticfn \w+\n' + name + r'\([\s\S]*?^\}\n\n', text)
    assert match, name
    return text[:match.start()] + text[match.end():]

def project(path, text):
    if path == 'include/you.h':
        text = replace_once(text, '''    /* Step9C: independent persistent syllable effects and learned Words.
       uspare1 remains exclusively the Sheol frozen-feet timer. */
#define MITH_AESH 0
#define MITH_KRAU 1
#define MITH_HOON 2
#define MITH_UUR 3
#define MITH_NAEN 4
#define MITH_VAUL 5
#define MITH_FIRST 0x01U
#define MITH_DIVIDING 0x02U
#define MITH_NURTURING 0x04U
    int mith_syllables[6];
    int mith_timers[6];
    long mith_word_timeout[3];
    unsigned mith_words;
    unsigned mith_slabs;
''')
    elif path == 'include/artilist.h':
        text = replace_once(text, '''    /* Mithardir rewards; ordinary keys, no vanilla endgame gating. */
    A("The Second Key of Chaos", SKELETON_KEY,
      (SPFX_NOGEN | SPFX_RESTR), 0, 0,
      NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM,
      0, 0, 1500L, NO_COLOR, SECOND_KEY_OF_CHAOS),
    A("The Third Key of Chaos", SKELETON_KEY,
      (SPFX_NOGEN | SPFX_RESTR), 0, 0,
      NO_ATTK, NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM,
      0, 0, 1500L, NO_COLOR, THIRD_KEY_OF_CHAOS),

''')
    elif path == 'src/read.c':
        text = replace_once(text, '''staticfn int mith_read_tile(struct obj *);
staticfn int mith_learn_word(void);
static unsigned mith_study_id;
static int mith_study_delay;
''')
        text = replace_once(text, '''    if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS
        || is_mith_syllable(obj) || is_mith_slab(obj))''',
                            '    if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS)')
        text = replace_once(text, '''    if (is_mith_syllable(scroll) || is_mith_slab(scroll))
        return mith_read_tile(scroll);

''')
        text = replace_once(text, '''/* Keep an ID rather than a pointer that theft/destruction could invalidate.
   Interrupted study resumes in this process; restoring starts a new study. */
''')
        for name in ['mith_learn_word', 'mith_read_tile']:
            text = remove_function(text, name)
    elif path == 'src/mklev.c':
        text = replace_once(text, 'staticfn void mith_catacombs(void);\n')
        text = replace_once(text, '''                if (!In_mithardir_catacombs(&u.uz)
                    && !rn2(5) && IS_WALL(levl[xx][yy].typ)) {''',
            '''                if (!rn2(5) && IS_WALL(levl[xx][yy].typ)) {''')
        text = replace_once(text, '''        if (In_mithardir_catacombs(&u.uz)) {
            if (!rn2(2))
                (void) makemon(&mons[PM_ALABASTER_MUMMY], xx, yy + dy, NO_MM_FLAGS);
            else
                (void) mksobj_at(mith_tile_type(), xx, yy + dy, TRUE, FALSE);
        }
''')
        text = replace_once(text, '''/* dNetHack's river erodes walls while leaving existing room floors,
   stairs and the slab intact. The local extended depth requires positive
   denominators where the donor assumes a depth below 85/100/140. */
''')
        for name in ['mith_liquify', 'mith_river', 'mith_slabroom', 'mith_poolroom', 'mith_catacombs']:
            text = remove_function(text, name)
        text = replace_once(text, '''    } else if (In_mithardir(&u.uz) && u.uz.dlevel >= 8) {
        mith_catacombs();
''')
    elif path == 'src/shknam.c':
        for line in ['staticfn void mith_shop_stock(int, struct obj *, int, int);\n',
                     'staticfn void mith_init_services(struct monst *);\n',
                     '#define MITH_TILE_CLASS (MAXOCLASSES + 2)\n']:
            text = replace_once(text, line)
        start = text.index('/* Mithardir merchant names and weighted equipment lists, pinned dNetHack. */')
        end = text.index('const struct shclass shtypes[] = {', start)
        names = re.findall(r'(?m)^static const [^\n]+?\b(\w+)\[\] =', text[start:end])
        assert names == ['shkcrab', 'shkdeep', 'shkselkie', 'shknaiad',
                         'garden_armors', 'garden_weapons', 'sand_armors', 'sand_weapons', 'fancy_clothes']
        text = text[:start] + text[end:]
        start = text.index('    { "sea garden", NULL, ARMOR_CLASS, 0, D_SHOP,')
        end = text.index('    /* sentinel */', start)
        assert re.findall(r'^    \{ "([^"]+)"', text[start:end], re.M) == [
            'sea garden', 'fishery', "sand-walker's shop", 'spa']
        text = text[:start] + text[end:]
        text = replace_once(text, '''/* The four imported shops retain native billing and the Step 5 mimic cap.
   Their equipment substitutions and food names follow pinned shknam.c. */
''')
        for name in ['mith_shop_stock', 'mith_init_services']:
            text = remove_function(text, name)
        text = replace_once(text, '    struct obj *stock = 0;\n')
        text = replace_once(text, '            stock = mksobj_at(-atype, sx, sy, TRUE, TRUE);',
                            '            (void) mksobj_at(-atype, sx, sy, TRUE, TRUE);')
        text = replace_once(text, '            stock = mkobj_at(atype, sx, sy, TRUE);',
                            '            (void) mkobj_at(atype, sx, sy, TRUE);')
        text = replace_once(text, '        mith_shop_stock(SHOPBASE + (int) (shp - shtypes), stock, sx, sy);\n')
        text = replace_once(text, '    int species = PM_SHOPKEEPER;\n')
        text = replace_once(text, '''    switch (sroom->rtype) {
    case SEAGARDEN: species = PM_YURIAN; break;
    case SANDWALKER: species = PM_SELKIE; break;
    case NAIADSHOP: species = PM_OCEANID; break;
    }
''')
        text = replace_once(text, 'makemon(&mons[species], sx, sy, MM_ESHK)',
                            'makemon(&mons[PM_SHOPKEEPER], sx, sy, MM_ESHK)')
        text = replace_once(text, '    mith_init_services(shk);\n')
        text = replace_once(text, '''    /* Fishery keeps the donor's ordinary shopkeeper creation, then changes
       species while preserving the native shop extension. */
    if (sroom->rtype == SEAFOOD)
        (void) newcham(shk, &mons[PM_DEEP_ONE], NO_NC_FLAGS);
''')
        text = replace_once(text, '''        } else if (shp->iprobs[i].itype == MITH_TILE_CLASS) {
            if (obj->otyp >= SYLLABLE_OF_STRENGTH__AESH
                && obj->otyp <= SYLLABLE_OF_SPIRIT__VAUL)
                return TRUE;
''')
        text = replace_once(text, '''    return (shp->iprobs[i].itype == MITH_TILE_CLASS)
               ? -mith_tile_type() : shp->iprobs[i].itype;''',
                            '    return shp->iprobs[i].itype;')
    return text
