/* Exercise production name parsing independently of the mapping helper. */
static void
step19_wish_tests(void)
{
    static const struct {
        const char *color;
        int scales, mail;
    } cases[] = {
        { "gray", GRAY_DRAGON_SCALES, GRAY_DRAGON_SCALE_MAIL },
        { "silver", SILVER_DRAGON_SCALES, SILVER_DRAGON_SCALE_MAIL },
        { "red", RED_DRAGON_SCALES, RED_DRAGON_SCALE_MAIL },
        { "white", WHITE_DRAGON_SCALES, WHITE_DRAGON_SCALE_MAIL },
        { "orange", ORANGE_DRAGON_SCALES, ORANGE_DRAGON_SCALE_MAIL },
        { "black", BLACK_DRAGON_SCALES, BLACK_DRAGON_SCALE_MAIL },
        { "blue", BLUE_DRAGON_SCALES, BLUE_DRAGON_SCALE_MAIL },
        { "green", GREEN_DRAGON_SCALES, GREEN_DRAGON_SCALE_MAIL },
        { "gold", GOLD_DRAGON_SCALES, GOLD_DRAGON_SCALE_MAIL },
        { "yellow", YELLOW_DRAGON_SCALES, YELLOW_DRAGON_SCALE_MAIL },
        { "glowing", GLOWING_DRAGON_SCALES, GLOWING_DRAGON_SCALE_MAIL },
        { "chromatic", CHROMATIC_DRAGON_SCALES, CHROMATIC_DRAGON_SCALE_MAIL },
        { "shimmering", SHIMMERING_DRAGON_SCALES, SHIMMERING_DRAGON_SCALE_MAIL },
        { "deep", DEEP_DRAGON_SCALES, DEEP_DRAGON_SCALE_MAIL },
        { "razor", RAZOR_DRAGON_SCALES, RAZOR_DRAGON_SCALE_MAIL },
        { "filth", FILTH_DRAGON_SCALES, FILTH_DRAGON_SCALE_MAIL },
        { "shadow", SHADOW_DRAGON_SCALES, SHADOW_DRAGON_SCALE_MAIL },
        { "celestial", CELESTIAL_DRAGON_SCALES, CELESTIAL_DRAGON_SCALE_MAIL }
    };
    boolean saved_wizard = wizard;
    int saved_luck = u.uluck;
    int mode, i, mail, expected;
    struct obj *o;
    char wish[BUFSZ];

    u.uluck = 0;
    for (mode = 0; mode < 2; ++mode) {
        wizard = mode ? TRUE : FALSE;
        for (i = 0; i < SIZE(cases); ++i)
            for (mail = 0; mail < 2; ++mail) {
                /* Ordinary wishes may randomly reduce magnitudes above 1. */
                Sprintf(wish, "blessed -%d %s dragon %s", wizard ? 2 : 1,
                        cases[i].color,
                        mail ? "scale mail" : "scales");
                expected = mail ? cases[i].mail : cases[i].scales;
                o = readobjnam(wish, (struct obj *) 0);
                assert(o != 0);
                if (!wizard && cases[i].scales == CHROMATIC_DRAGON_SCALES) {
                    /* Preserve the existing ordinary-wish fallback range. */
                    assert(o->otyp >= (mail ? GRAY_DRAGON_SCALE_MAIL
                                          : GRAY_DRAGON_SCALES));
                    assert(o->otyp < (mail ? YELLOW_DRAGON_SCALE_MAIL
                                         : YELLOW_DRAGON_SCALES));
                } else {
                    if (o->otyp != expected)
                        printf("FAIL dragon wish: wizard=%d %s %s: expected %d, got %d\n",
                               mode, cases[i].color, mail ? "mail" : "scales",
                               expected, o->otyp);
                    assert(o->otyp == expected);
                }
                assert(o->blessed && !o->cursed
                       && o->spe == (wizard ? -2 : -1));
                obfree(o, (struct obj *) 0);
            }
    }
    wizard = saved_wizard;
    u.uluck = saved_luck;
    puts("PASS Step 19 dragon wish parser: 72 exact-object/restriction and BUC/enchantment cases");
}
