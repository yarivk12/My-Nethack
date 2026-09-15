#include "hack.h"
#include <assert.h>
#include <stdio.h>

static struct obj replacement;
static int roll, percentile, tile_calls, deleted;
static char last_fruit[BUFSZ];
int rn2(int n) { assert(n > 0); return roll++ % n; }
int rnd(int n) { assert(n == 100); return percentile; }
int mith_tile_type(void) { ++tile_calls; return SYLLABLE_OF_STRENGTH__AESH; }
struct obj *mksobj_at(int typ, coordxy x, coordxy y, boolean init, boolean art) {
    assert(x == 10 && y == 10 && init && art);
    memset(&replacement, 0, sizeof replacement);
    replacement.otyp = (short) typ;
    replacement.oclass = objects[typ].oc_class;
    return &replacement;
}
void delobj(struct obj *o) { assert(o != &replacement); ++deleted; }
int fruitadd(char *name, struct fruit *fruit) {
    assert(!fruit); Strcpy(last_fruit, name); return 42;
}
int weight(struct obj *o) { (void) o; return 123; }
#include "step9c_shops.h"

int main(void) {
    int shop, i, sum, tiles, total = 0;
    struct obj stock;
    monst_globals_init(); objects_globals_init();
    check_donor_lists();
    for (shop = SHOPBASE; shop <= MAXRTYPE; ++shop) {
        const struct shclass *s = &shtypes[shop - SHOPBASE];
        sum = 0;
        for (i = 0; i < SIZE(s->iprobs); ++i) sum += s->iprobs[i].iprob;
        assert(sum == 100);
        total += s->prob;
        if (shop >= UNIQUESHOP) assert(s->prob == 0);
        if (shop >= SEAGARDEN) {
            tile_calls = tiles = 0;
            for (percentile = 1; percentile <= 100; ++percentile)
                if (get_shop_item(shop - SHOPBASE) == -SYLLABLE_OF_STRENGTH__AESH)
                    ++tiles;
            assert(tiles == (shop == SANDWALKER ? 5 : 1));
            assert(tile_calls == tiles);
        }
    }
    assert(total == 100);
    puts("PASS donor weighted equipment lists, stock sums, tile frequency and zero ordinary shop probability");
    for (shop = SEAGARDEN; shop <= NAIADSHOP; ++shop) {
        for (i = 0; i < 100; ++i) {
            memset(&stock, 0, sizeof stock);
            stock.otyp = DAGGER; stock.oclass = WEAPON_CLASS;
            roll = i; deleted = 0;
            mith_shop_stock(shop, &stock, 10, 10);
            if (shop == SEAGARDEN) {
                assert(deleted == 1 && replacement.obranch_material == SHELL);
                assert(replacement.opoisoned || (replacement.obranch_props & OBP_ACID));
            } else if (shop == SANDWALKER) {
                assert(deleted == 1 && (replacement.obranch_props & OBP_ACID));
                assert(replacement.otyp == CRYSTAL_SWORD
                       ? !replacement.obranch_material
                       : replacement.obranch_material == SILVER);
            } else assert(!deleted);
            memset(&stock, 0, sizeof stock);
            stock.otyp = PLATE_MAIL; stock.oclass = ARMOR_CLASS;
            deleted = 0;
            mith_shop_stock(shop, &stock, 10, 10);
            assert(deleted == (shop != SEAFOOD));
            if (shop == SEAGARDEN)
                assert(!is_metallic(&replacement));
        }
    }
    memset(&stock, 0, sizeof stock);
    stock.otyp = AMULET_OF_LIFE_SAVING; stock.oclass = AMULET_CLASS;
    mith_shop_stock(SEAFOOD, &stock, 10, 10);
    assert(stock.obranch_material == GOLD && stock.owt == 123);
    for (i = 0; i < 6; ++i) {
        stock.otyp = SLIME_MOLD; stock.oclass = FOOD_CLASS; roll = i;
        mith_shop_stock(SEAFOOD, &stock, 10, 10);
        assert(stock.spe == 42 && *last_fruit);
    }
    memset(&stock, 0, sizeof stock);
    stock.otyp = DAGGER; stock.oclass = WEAPON_CLASS; stock.oartifact = 1;
    deleted = 0; mith_shop_stock(SEAGARDEN, &stock, 10, 10);
    assert(!deleted && stock.oartifact == 1);
    roll = 71; mith_shop_stock(WEAPONSHOP, &stock, 10, 10);
    assert(roll == 71 && !deleted);
    puts("PASS shell/poison, silver/acid, clothing, golden amulets, food names, artifact and native shop guards");
    return 0;
}
