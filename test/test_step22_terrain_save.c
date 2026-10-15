static void
step22_terrain_save_tests(void)
{
    const int types[4] = { POOL, MOAT, BOG, PUDDLE };
    struct obj *source, *o;
    unsigned id;
    int i, pass, x[4], y, flags[4];
    boolean shopdamage;
    long now = svm.moves;
    NHFILE *file;
    assert(!gi.invent);
    source = inspect_item(DAGGER); source->oartifact = ART_STING;
    ex_test_grant(source, EX_TRANSMUTE, EX_MANUAL, EX_HERO, 0);
    id = source->o_id; y = u.uy + 2;
    for (i = 0; i < 4; ++i) {
        x[i] = u.ux + i + 1;
        assert(!m_at(x[i], y) && !t_at(x[i], y));
        levl[x[i]][y].typ = types[i];
        assert(ex_transmute(source, x[i], y));
        assert(levl[x[i]][y].typ == ICE);
        flags[i] = levl[x[i]][y].icedpool;
        assert(!spot_time_left(x[i], y, MELT_ICE_AWAY));
    }
    freeinv(source); place_object(source, u.ux, u.uy);
    file = get_freeing_nhfile();
    file_mode(file, WRITING | FREEING, open("step22-terrain.tmp",
              O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, _S_IREAD | _S_IWRITE));
    savelev(file, ledger_no(&u.uz)); close_nhfile(file);
    file = get_freeing_nhfile();
    file_mode(file, READING, open("step22-terrain.tmp", O_RDONLY | O_BINARY));
    getlev(file, 0, ledger_no(&u.uz)); close_nhfile(file);
    source = NULL;
    for (o = fobj; o; o = o->nobj) if (o->o_id == id) source = o;
    assert(source && source->exotic.charges[1] == 6);
    obj_extract_self(source); source = addinv(source);
    for (i = 0; i < 4; ++i) {
        assert(levl[x[i]][y].typ == ICE && levl[x[i]][y].icedpool == flags[i]);
        shopdamage = FALSE;
        (void) zap_over_floor(x[i], y, AD_FIRE - 1, &shopdamage, TRUE, 0);
        assert(levl[x[i]][y].typ == types[i]);
    }
    for (pass = 0; pass < 500; ++pass) for (i = 0; i < 4; ++i) {
        svm.moves += 10;
        assert(ex_transmute(source, x[i], y));
        assert(levl[x[i]][y].typ == ICE && levl[x[i]][y].icedpool == flags[i]);
        assert(!spot_time_left(x[i], y, MELT_ICE_AWAY));
        svm.moves += 10;
        assert(ex_transmute(source, x[i], y));
        assert(levl[x[i]][y].typ == types[i]);
    }
    for (i = 0; i < 4; ++i) levl[x[i]][y].typ = ROOM;
    freeinv(source); obfree(source, NULL); svm.moves = now;
    puts("PASS Step22 T7 native savelev/getlev all four water provenances and external fire melt; 2000 freeze/thaw stress cycles preserve exact terrain, charges and no automatic melt timer");
}
