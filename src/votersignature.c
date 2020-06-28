/*******************************************************************************

Copyright 2020 The "Sam sabie CVK" Project Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

#include <openssl/bn.h>

int prepare_public_keys(int bits_prime,
    BIGNUM *modulo, BIGNUM *h,
    BIGNUM *g1, BIGNUM *g2, BIGNUM *k2,
    BN_CTX *ctx)
{
    int result = 0;

    // IMPORTANT: Setting this flag is necessary to guarantee properties
    // of a ring modulo `p` to be "safe" for our purposes, where `p` is the
    // found prime. There are high chances without these properties, that it
    // will be degradation of code-space and a higher probability of collisions
    // increasing interaction between issuing side and collecting side.
    //
    // The ring is safe when orders of roots of unity are as large as possible
    // and there are little chances to select a root of unity with a small
    // order, since the order is a size of the subset `[1; p-1]` to become
    // used as a code.
    //
    // With the set flag, the ring has almost half of roots of unity maximal
    // order and the other part has halved order except for the very last
    // element which is a root of unity order 2. It happens because
    // `(p - 1) / 2` is also prime and this is guaranteed by function, when
    // called with the flag.
    //
    // Any ring modulo `m` has `phi(phi(m))` primitive roots (a number which
    // exponentiations modulo `m` are equal all non-zero values of the ring, it
    // is obviously also root of unity), where `phi()` is Euler's totient
    // function (an amount of coprime numbers with the given number). For the
    // prime number `phi(p) = p - 1`. It is desirable that `p - 1` has as
    // minimal amount of factors as possible being that the `phi(p - 1)`
    // (i.e. the number of primitive roots) will be higher, OTOH any `p` but 2
    // is odd, hence `p - 1` is even, thus the largest amount of primitive roots
    // we have namely when `(p - 1) / 2` is prime. So almost half of the ring
    // consists from primitive roots. Moreover, because any root of unity root
    // in the ring can have only orders which are divisors of `p - 1`, the rest
    // part of the ring but one element are roots of unity of the order
    // `(p - 1) / 2` (half of any primitive root order).
    //
    // Proof:
    // The primitive roots has order `p - 1`. The very last element has order 2
    // (it is enough to show that `(p - 1)^2 = p^2 - 2 * p + 1 = 1 mod p`).
    // However, according Lagrange's theorem, the equation `x^2 - 1 = 0 mod p`
    // has no more than 2 incongruent solutions, hence there are no other number
    // `x`, that `x^2 = 1 mod p` but 1 and `p - 1`. Thus the rest roots of unity
    // shall have order `(p - 1) / 2`, as the only possible devisor of `p - 1`,
    // except 2 and `p - 1`.
    int safe_ring = 1;
    if(!BN_generate_prime_ex(modulo, bits_prime, safe_ring, NULL, NULL, NULL)) goto err;

    BIGNUM *upper_bound, *k1;
    BN_CTX_start(ctx);
    upper_bound = BN_CTX_get(ctx);
    k1 = BN_CTX_get(ctx);
    if(NULL == k1) goto err;

    if(NULL == BN_copy(upper_bound, modulo)) goto err;
    if(!BN_sub_word(upper_bound, 3)) goto err;

    if(!BN_rand_range(h, upper_bound)) goto err;
    if(!BN_add_word(h, 2)) goto err;

    do
    {
        if(!BN_priv_rand_range(k1, upper_bound)) goto err;
        if(!BN_add_word(k1, 2)) goto err;
        if(!BN_mod_exp(g1, h, k1, modulo, ctx)) goto err;
    }while(BN_is_one(g1));
    BN_zero(k1);

    do
    {
        if(!BN_rand_range(k2, upper_bound)) goto err;
        if(!BN_add_word(k2, 2)) goto err;
        if(!BN_mod_exp(g2, h, k2, modulo, ctx)) goto err;
    }while(BN_is_one(g2));

    result = 1;
err:
    BN_CTX_end(ctx);
    return result;
}

int make_voter_signature(BIGNUM *P, BIGNUM *Q,
    const BIGNUM *legal_identity,
    const BIGNUM *modulo, const BIGNUM *h,
    const BIGNUM *g1, const BIGNUM *g2, const BIGNUM *k2,
    BN_CTX *ctx)
{
    BIGNUM *upper_bound, *r, *t;
    int result = 0;

    BN_CTX_start(ctx);
    upper_bound = BN_CTX_get(ctx);
    r = BN_CTX_get(ctx);
    t = BN_CTX_get(ctx);
    if (NULL == t) goto err;

    if(NULL == BN_copy(upper_bound, modulo)) goto err;
    if(!BN_sub_word(upper_bound, 3)) goto err;

    if(!BN_priv_rand_range(r, upper_bound)) goto err;
    if(!BN_add_word(r, 2)) goto err;

    if(!BN_mod_exp(P, g2, r, modulo, ctx)) goto err;
    if(!BN_mod_exp(Q, h, r, modulo, ctx)) goto err;
    BN_zero(r);
    if(!BN_mod_exp(t, g1, legal_identity, modulo, ctx)) goto err;
    if(!BN_mod_mul(Q, Q, t, modulo, ctx)) goto err;
    result = 1;
err:
    BN_CTX_end(ctx);
    return result;
}

int is_equal_legal_identity(const BIGNUM *Pa, const BIGNUM *Qa,
    const BIGNUM *Pb, const BIGNUM *Qb,
    const BIGNUM *modulo, const BIGNUM *k2,
    BN_CTX *ctx)
{
    BIGNUM *tp, *tq, *tqk;
    int result = -1;

    BN_CTX_start(ctx);
    tp = BN_CTX_get(ctx);
    tq = BN_CTX_get(ctx);
    tqk = BN_CTX_get(ctx);
    if (NULL == tqk) goto err;

    if(!BN_mod_inverse(tp, Pb, modulo, ctx)) goto err;
    if(!BN_mod_mul(tp, tp, Pa, modulo, ctx)) goto err;
    if(!BN_mod_inverse(tq, Qb, modulo, ctx)) goto err;
    if(!BN_mod_mul(tq, tq, Qa, modulo, ctx)) goto err;
    if(!BN_mod_exp(tqk, tq, k2, modulo, ctx)) goto err;

    result = BN_cmp(tp, tqk) == 0;
err:
    BN_CTX_end(ctx);
    return result;
}
