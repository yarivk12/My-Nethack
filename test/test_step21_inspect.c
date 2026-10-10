/* Included after the native inspection harness, exercising the real command. */
static void
step21_inspect_tests(void)
{
    struct obj *obj;
    long saved_moves = svm.moves;
    inspect_window_setup();
    obj = inspect_item(CRYSTAL_BALL);
    artifact_exists(obj, "The Orb of Fate", TRUE, ONAME_WISH);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    discover_artifact(obj->oartifact);
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: 37 turns remaining"));
    assert(obj->age == svm.moves + 37);
    svm.moves += 37;
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: Ready"));
    ++svm.moves;
    inspect_run(obj);
    assert(strstr(inspect_screen, "Cooldown: Ready"));
    inspect_discard(obj);
    obj = inspect_item(UNIVERSAL_KEY);
    artifact_exists(obj, "The Silver Key", TRUE, ONAME_WISH);
    discover_artifact(obj->oartifact);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    inspect_discard(obj);
    obj = inspect_item(BRASS_LANTERN);
    obj->known = 1;
    obj->age = svm.moves + 37;
    inspect_run(obj);
    assert(!strstr(inspect_screen, "Cooldown:"));
    inspect_discard(obj);
    svm.moves = saved_moves;
    step15b_window_setup();
    puts("PASS Step 21 invocation cooldown inspection, identity, expiry and fuel exclusions");
}
