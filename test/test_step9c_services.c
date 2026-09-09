#include "hack.h"
#include <assert.h>
#include <stdio.h>

struct you u;
struct instance_globals_i gi;
struct display_hints disp;
static int charisma, rolls[16], index;
static char answer;
static long cash, transferred;
schar acurr(int a) { assert(a == A_CHA); return (schar) charisma; }
int rn2(int n) { assert(n > 0 && index < 16); return rolls[index++] % n; }
char yn_function(const char *p, const char *opts, char def, boolean hist) {
    (void) p; (void) opts; (void) def; (void) hist; return answer;
}
long money_cnt(struct obj *o) { (void) o; return cash; }
long money2mon(struct monst *m, long n) {
    (void) m; assert(n > 0 && cash >= n); cash -= n; transferred += n;
    return n;
}
void verbalize(const char *fmt, ...) { (void) fmt; }
static long check_credit(long n, struct monst *m) {
    long used = min(n, ESHK(m)->credit);
    ESHK(m)->credit -= used; return n - used;
}
#include "step9c_services.h"

int main(void) {
    struct monst merchant = { 0 };
    struct mextra extra = { 0 };
    struct eshk shop = { 0 };
    const int lower[] = { 25, 50, 200, 50 };
    const int upper[] = { 750, 250, 1500, -1 };
    int b, bonus, t, i, amount, expected, cases = 0;
    merchant.mextra = &extra; extra.eshk = &shop;
    for (charisma = 3; charisma <= 25; ++charisma)
        for (b = 3; b <= 25; b += 2)
            for (bonus = -5; bonus <= 10; bonus += 3)
                for (t = -4; t <= 4; t += 2) {
                    ABASE(A_CHA) = (schar) b;
                    ABON(A_CHA) = (schar) bonus;
                    ATEMP(A_CHA) = (schar) t;
                    for (i = 0; i < 4; ++i)
                        for (amount = 0; amount < 100000; amount += 1997) {
                            expected = amount;
                            donor_price(&expected, lower[i], upper[i]);
                            assert(mith_service_price(amount, lower[i], upper[i]) == expected);
                            ++cases;
                        }
                }
    printf("PASS %d actual donor/native charisma price comparisons\n", cases);
    for (i = 0; i < 16; ++i) rolls[i] = 0;
    merchant.mspare1 = 0x123L; shop.shoptype = SEAGARDEN; index = 0;
    mith_init_services(&merchant);
    assert(mith_shk_services(&merchant) == (MITH_SHK_BASIC | MITH_SHK_PREMIUM | MITH_SHK_UNCURSE));
    assert((merchant.mspare1 & 0x3ffL) == 0x123L);
    shop.shoptype = SANDWALKER; index = 0;
    mith_init_services(&merchant);
    assert(mith_shk_services(&merchant) == (MITH_SHK_BASIC | MITH_SHK_PREMIUM
           | MITH_SHK_UNCURSE | MITH_SHK_PROOF | MITH_SHK_ENCHANT));
    for (i = 0; i < 16; ++i) rolls[i] = 1;
    index = 0; mith_init_services(&merchant);
    assert(mith_shk_services(&merchant) == (MITH_SHK_BASIC | MITH_SHK_APPRAISE | MITH_SHK_COAT));
    index = 0; rolls[1] = 0; mith_init_services(&merchant);
    assert(mith_shk_services(&merchant) & MITH_SHK_PREMIUM);
    assert(!(mith_shk_services(&merchant) & MITH_SHK_BASIC));
    shop.shoptype = WEAPONSHOP; index = 0;
    mith_init_services(&merchant); assert(index == 0);
    puts("PASS service branches, unrelated shop guard and preserved low saved bits");
    cash = 100; shop.credit = 400; transferred = 0; answer = 'n';
    assert(!mith_service_pay(&merchant, 500));
    assert(cash == 100 && shop.credit == 400 && !transferred);
    answer = 'y';
    assert(!mith_service_pay(&merchant, 501));
    assert(cash == 100 && shop.credit == 400 && !transferred);
    assert(mith_service_pay(&merchant, 500));
    assert(!cash && !shop.credit && transferred == 100);
    shop.credit = 600; transferred = 0;
    assert(mith_service_pay(&merchant, 500));
    assert(shop.credit == 100 && !cash && !transferred);
    puts("PASS decline, insufficient funds, exact credit/cash payment and credit-only payment");
    return 0;
}
