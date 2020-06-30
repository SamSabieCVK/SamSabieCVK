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

#include <stdio.h>
#include <time.h>

#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "votersignature.h"

int main(int argc, char *argv[])
{
    int result = -1;
    unsigned long err_code;

    BN_CTX *ctx = BN_CTX_secure_new();
    if(NULL == ctx) goto err;

    if(!RAND_status()) goto err;

    BN_CTX_start(ctx);
    BIGNUM *modulo = BN_CTX_get(ctx);
    BIGNUM *h = BN_CTX_get(ctx);
    BIGNUM *g1 = BN_CTX_get(ctx);
    BIGNUM *g2 = BN_CTX_get(ctx);
    BIGNUM *k2 = BN_CTX_get(ctx);
    BIGNUM *temporary = BN_CTX_get(ctx);
    if(NULL == k2) goto err;

    const int N = 1000;
    BIGNUM **Ps, **Qs;
    Ps = (BIGNUM **)malloc(N * sizeof(BIGNUM *));
    Qs = (BIGNUM **)malloc(N * sizeof(BIGNUM *));
    if(NULL == Qs) goto err;
    for(int i = 0; i < N; ++i)
    {
        Ps[i] = BN_secure_new();
        Qs[i] = BN_secure_new();
    }
    if(NULL == Qs[N-1]) goto err;

    int bits_prime = 512;
    if(!prepare_public_keys(bits_prime, modulo, h, g1, g2, k2, ctx)) goto err;

    clock_t tic0 = clock();
    for(int i = 0; i < N; ++i)
    {
        if(!BN_set_word(temporary, i % (N - 3))) goto err;
        if(!make_voter_signature(Ps[i], Qs[i], temporary, modulo, h, g1, g2, ctx)) goto err;
    }
    clock_t tic1 = clock();
    printf("t create signatures: %f\n", (double)(tic1-tic0)/CLOCKS_PER_SEC);

    int equal_pairs = 0;
    clock_t tic2 = clock();
    for(int i = 0; i < N; ++i)
    {
        for(int j = i + 1; j < N; ++j)
        {
            int cmp_result;
            if(0 > (cmp_result = is_equal_legal_identity(Ps[i], Qs[i], Ps[j], Qs[j], modulo, k2, ctx))) goto err;
            equal_pairs += cmp_result;
        }
    }
    clock_t tic3 = clock();
    printf("t compare signatures: %f\n", (double)(tic3-tic2)/CLOCKS_PER_SEC);
    printf("equal pairs: %d\n", equal_pairs);

    result = 0;
err:
    while (0 != (err_code = ERR_peek_error()))
    {
        fprintf(stderr, "Detailed error: %s\n",
                ERR_reason_error_string(err_code));
        ERR_get_error(); // remove error from error stack
    }
    if(NULL != Qs)
    {
        for(int i = 0; i < N; ++i)
        {
            BN_free(Qs[i]);
            Qs[i] = NULL;
        }
    }
    free(Qs);
    Qs = NULL;
    if(NULL != Ps)
    {
        for(int i = 0; i < N; ++i)
        {
            BN_free(Ps[i]);
            Ps[i] = NULL;
        }
    }
    free(Ps);
    Ps = NULL;
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    return result;
}
