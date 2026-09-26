/* Included by the native enhancement regression harness. */
extern boolean step16c_settle_bill(struct monst *, long *);
extern int step16c_removal_weight(struct obj *, struct obj *, long);

static void
step16c_property(struct obj *obj, int id)
{
    assert(enhancement_set_mask(obj, enhancement_mask_property(id),
                                OQ_STANDARD, FALSE));
}

static void
step16c_storage_tests(void)
{
    const int types[] = { GOLD_PIECE, DIAMOND, WORTHLESS_RED_GLASS,
        SPE_MAGIC_MISSILE, SCR_IDENTIFY, WAN_NOTHING, POT_WATER,
        PLATE_MAIL, ARROW, LONG_SWORD, BOW, PICK_AXE, UNICORN_HORN,
        GRAPPLING_HOOK, RIN_ADORNMENT, AMULET_OF_LIFE_SAVING };
    const int props[] = { EP_TREASURE_I, EP_TREASURE_II, EP_ARCANE_II,
        EP_ARCANE_III, EP_ARMOR_STORAGE_II, EP_ARMOR_STORAGE_III,
        EP_WEAPON_STORAGE_II, EP_WEAPON_STORAGE_III };
    const int numerator[] = { 3, 3, 7, 2, 3, 3, 3, 3 };
    const int denominator[] = { 5, 10, 10, 5, 5, 10, 5, 10 };
    struct obj *bag = item(SACK), *inner = item(SACK), *content, *part;
    int p, t, native, expected, cls, base = objects[SACK].oc_weight;
    boolean qualifies;

    for (p = 0; p < SIZE(props); ++p) {
        step16c_property(bag, props[p]);
        for (t = 0; t < SIZE(types); ++t) {
            content = item(types[t]);
            content->quan = 3;
            content->owt = weight(content);
            native = weight(content);
            cls = content->oclass;
            qualifies = p < 2 ? cls == COIN_CLASS || cls == GEM_CLASS
                        : p < 4 ? cls == SPBOOK_CLASS || cls == SCROLL_CLASS
                                  || cls == WAND_CLASS || cls == POTION_CLASS
                        : p < 6 ? cls == ARMOR_CLASS : cls == WEAPON_CLASS;
            expected = qualifies ? native * numerator[p] / denominator[p]
                                 : native;
            add_to_container(bag, content);
            container_weight(bag);
            assert(weight(bag) == base + expected);
            assert(bag->owt == (unsigned) weight(bag));
            assert(!bag->o_enh_known2);
            obj_extract_self(content);
            assert(bag->owt == (unsigned) base);
            obfree(content, NULL);
        }
    }
    /* A one-unit contribution can vanish, but the container base cannot. */
    content = item(DIAMOND);
    step16c_property(bag, EP_TREASURE_II);
    add_to_container(bag, content);
    assert(weight(bag) == base);
    obj_extract_self(content);

    /* Outer storage never reaches through the inner TOOL_CLASS object. */
    content->quan = 17;
    content->owt = weight(content);
    add_to_container(inner, content);
    add_to_container(bag, inner);
    container_weight(bag);
    assert(weight(bag) == base + weight(inner));
    step16c_property(inner, EP_TREASURE_I);
    assert(weight(bag) == 2 * base + weight(content) * 3 / 5);
    assert(bag->owt == (unsigned) weight(bag));
    content->quan = 23;
    content->owt = weight(content);
    container_weight(inner);
    assert(bag->owt == (unsigned) weight(bag));
    expected = weight(bag);
    content->quan -= 7;
    expected -= weight(bag);
    content->quan += 7;
    assert(step16c_removal_weight(inner, content, 7) == expected);
    content->quan = 5;
    container_weight(content);
    part = splitobj(content, 1);
    assert(bag->owt == (unsigned) weight(bag));
    assert(weight(inner) == base + 2);
    assert(merged(&content, &part));
    assert(weight(inner) == base + 3);
    assert(bag->owt == (unsigned) weight(bag));
    content->quan = 23;
    container_weight(content);
    obj_extract_self(inner);
    obfree(bag, NULL);

    bag = item(BAG_OF_HOLDING);
    step16c_property(bag, EP_TREASURE_II);
    obj_extract_self(content);
    add_to_container(bag, content);
    native = weight(content) * 3 / 10;
    base = objects[BAG_OF_HOLDING].oc_weight;
    assert(weight(bag) == base + (native + 1) / 2);
    /* Pickup's capacity calculation must agree with whole-stack rounding,
       including the remainder of a partly removed direct stack. */
    expected = weight(bag);
    content->quan -= 7;
    expected -= weight(bag);
    content->quan += 7;
    assert(step16c_removal_weight(bag, content, 7) == expected);
    bless(bag);
    assert(weight(bag) == base + (native + 3) / 4);
    curse(bag);
    assert(weight(bag) == base + 2 * native);
    uncurse(bag);
    assert(weight(bag) == base + (native + 1) / 2);
    obfree(inner, NULL);
    obfree(bag, NULL);
    puts("PASS Step 16C canonical Storage class matrix, stack floors, zero, nested composition, BOH order and caches");
}

static void
step16c_water_tests(void)
{
    struct obj *bag = item(SACK), *scroll = item(SCR_IDENTIFY);
    int i, leaks = 0, saved_luck = u.uluck, saved_moreluck = u.moreluck;
    u.uluck = u.moreluck = 0;
    step16c_property(bag, EP_WATERTIGHT);
    add_to_container(bag, scroll);
    for (i = 0; i < 100; ++i) {
        water_damage(bag, NULL, TRUE);
        assert(scroll->otyp == SCR_IDENTIFY);
    }
    bless(bag);
    water_damage(bag, NULL, TRUE);
    assert(scroll->otyp == SCR_IDENTIFY);
    curse(bag);
    for (i = 0; i < 600; ++i) {
        scroll->otyp = SCR_IDENTIFY;
        water_damage(bag, NULL, TRUE);
        if (scroll->otyp == SCR_BLANK_PAPER)
            ++leaks;
    }
    /* Native contents damage has its own luck check after the 1-in-3 leak. */
    assert(leaks > 30 && leaks < 260);
    assert(!bag->o_enh_known2);
    obfree(bag, NULL);
    bag = item(OILSKIN_SACK);
    assert(!enhancement_set_mask(bag, enhancement_mask_property(EP_WATERTIGHT),
                                  OQ_STANDARD, FALSE));
    step16c_property(bag, EP_TREASURE_I);
    obfree(bag, NULL);
    u.uluck = saved_luck; u.moreluck = saved_moreluck;
    puts("PASS Step 16C Watertight native uncursed/blessed protection, cursed leakage and no affix identification");
}

static void
step16c_commerce_tests(void)
{
    struct monst shk = { 0 }, *old = fmon;
    struct obj *source = item(PICK_AXE), *bag = item(SACK), *goods, *gold;
    struct eshk *eshk;
    long quote, price, expected, oldcredit;
    int tier, n;

    shk.data = &mons[PM_SHOPKEEPER];
    shk.mhp = 100; shk.mpeaceful = shk.isshk = 1;
    shk.mx = 20; shk.my = 10;
    neweshk(&shk); eshk = ESHK(&shk);
    eshk->bill_p = eshk->bill;
    eshk->shoplevel = u.uz; eshk->shoproom = ROOMOFFSET;
    fmon = &shk;
    svr.rooms[0].rtype = SHOPBASE; svr.rooms[0].resident = &shk;
    levl[20][10].roomno = ROOMOFFSET;
    u.ushops[0] = ROOMOFFSET; u.ushops[1] = 0;
    addinv(source);

    for (tier = 0; tier < 2; ++tier) {
        step16c_property(source, tier ? EP_COMMERCE_III : EP_COMMERCE_I);
        for (n = 1; n < 12; ++n) {
            goods = item(ARROW); goods->quan = 3; addinv(goods);
            eshk->billct = 1;
            eshk->bill[0].bo_id = goods->o_id;
            eshk->bill[0].bquan = goods->quan;
            eshk->bill[0].price = n;
            goods->unpaid = 1;
            price = n * goods->quan;
            expected = max(1L, tier ? price / 2 : price * 3 / 4);
            eshk->credit = expected; eshk->debit = 19; eshk->loan = 7;
            assert(unpaid_cost(goods, COST_NOCONTENTS) == price);
            assert(step16c_settle_bill(&shk, &quote));
            assert(quote == expected && eshk->credit == 0);
            assert(!goods->unpaid && !eshk->billct);
            assert(eshk->debit == 19 && eshk->loan == 7);
            assert(!source->o_enh_known2);
            freeinv(goods); obfree(goods, NULL);
        }
    }
    /* Same original bill, source deactivated and activated at settlement. */
    goods = item(ARROW); addinv(goods);
    eshk->billct = 1; eshk->bill[0].bo_id = goods->o_id;
    eshk->bill[0].bquan = 1; eshk->bill[0].price = 101;
    goods->unpaid = 1; eshk->credit = 50;
    freeinv(source); add_to_container(bag, source); addinv(bag);
    assert(!step16c_settle_bill(&shk, &quote));
    assert(quote == 101 && eshk->billct == 1 && goods->unpaid);
    obj_extract_self(source); addinv(source);
    assert(step16c_settle_bill(&shk, &quote));
    assert(quote == 50 && !eshk->billct && !goods->unpaid);
    freeinv(goods); obfree(goods, NULL);

    /* An unpaid active source may buy itself, with full debt discharge. */
    eshk->billct = 1; eshk->bill[0].bo_id = source->o_id;
    eshk->bill[0].bquan = 1; eshk->bill[0].price = 100;
    eshk->credit = 50; source->unpaid = 1;
    assert(step16c_settle_bill(&shk, &quote));
    assert(quote == 50 && !source->unpaid && !eshk->billct);

    /* Strongest source only; the ordinary mixed credit/gold path transfers
       exactly the remaining payable amount to the shopkeeper. */
    step16c_property(bag, EP_COMMERCE_I);
    gold = item(GOLD_PIECE); gold->quan = 200; gold->owt = weight(gold);
    assert(money_cnt(gi.invent) == 0);
    addinv(gold);
    goods = item(ARROW); addinv(goods);
    eshk->billct = 1; eshk->bill[0].bo_id = goods->o_id;
    eshk->bill[0].bquan = 1; eshk->bill[0].price = 100;
    goods->unpaid = 1; eshk->credit = 10;
    assert(step16c_settle_bill(&shk, &quote));
    assert(quote == 50 && !eshk->credit && !eshk->billct);
    assert(money_cnt(gi.invent) == 160 && money_cnt(shk.minvent) == 40);
    freeinv(gold); obfree(gold, NULL);
    gold = shk.minvent; obj_extract_self(gold); obfree(gold, NULL);
    freeinv(goods); obfree(goods, NULL);

    /* Native container purchase settles two entries separately: 3/2 + 3/2. */
    goods = item(ARROW); add_to_container(bag, goods);
    eshk->billct = 2;
    eshk->bill[0].bo_id = bag->o_id;
    eshk->bill[1].bo_id = goods->o_id;
    eshk->bill[0].bquan = eshk->bill[1].bquan = 1;
    eshk->bill[0].price = eshk->bill[1].price = 3;
    bag->unpaid = goods->unpaid = 1;
    eshk->credit = oldcredit = 2;
    assert(step16c_settle_bill(&shk, &quote));
    assert(quote == oldcredit && !eshk->credit && !eshk->billct);
    assert(!bag->unpaid && !goods->unpaid);
    freeinv(bag); obfree(bag, NULL);
    freeinv(source); obfree(source, NULL);
    u.ushops[0] = 0; svr.rooms[0].resident = NULL; svr.rooms[0].rtype = OROOM;
    fmon = old; dealloc_mextra(&shk);
    puts("PASS Step 16C native Commerce settlement floors, full debt discharge, credit, activation, self-purchase and container quotes");
}

static void
step16c_shop_storage_tests(void)
{
    step16c_storage_tests();
    step16c_water_tests();
    step16c_commerce_tests();
}
