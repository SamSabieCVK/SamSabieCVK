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

    BIGNUM *xa = BN_CTX_get(ctx);
    BIGNUM *Pa = BN_CTX_get(ctx);
    BIGNUM *Qa = BN_CTX_get(ctx);
    BIGNUM *xb = BN_CTX_get(ctx);
    BIGNUM *Pb = BN_CTX_get(ctx);
    BIGNUM *Qb = BN_CTX_get(ctx);
    BIGNUM *xc = BN_CTX_get(ctx);
    BIGNUM *Pc = BN_CTX_get(ctx);
    BIGNUM *Qc = BN_CTX_get(ctx);

    clock_t tic2 = clock();
    int bits_prime = 512;
    if(!prepare_public_keys(bits_prime, modulo, h, g1, g2, k2, ctx)) goto err;
    clock_t tic3 = clock();
    printf("p:  %s\n", BN_bn2hex(modulo));
    printf("h:  %s\n", BN_bn2hex(h));
    printf("g1: %s\n", BN_bn2hex(g1));
    printf("g2: %s\n", BN_bn2hex(g2));
    printf("k2: %s\n", BN_bn2hex(k2));

    printf("t: %f\n", (double)(tic3-tic2)/CLOCKS_PER_SEC);

    printf("\n");

    clock_t tic4 = clock();
    if(!BN_set_word(xa, 1732567)) goto err;
    if(!make_voter_signature(Pa, Qa, xa, modulo, h, g1, g2, k2, ctx)) goto err;
    if(!BN_set_word(xb, 1732566)) goto err;
    if(!make_voter_signature(Pb, Qb, xb, modulo, h, g1, g2, k2, ctx)) goto err;
    if(!BN_set_word(xc, 1732567)) goto err;
    if(!make_voter_signature(Pc, Qc, xc, modulo, h, g1, g2, k2, ctx)) goto err;
    clock_t tic5 = clock();

    printf("Pa: %s\n", BN_bn2hex(Pa));
    printf("Qa: %s\n", BN_bn2hex(Qa));
    printf("Pb: %s\n", BN_bn2hex(Pb));
    printf("Qb: %s\n", BN_bn2hex(Qb));
    printf("Pc: %s\n", BN_bn2hex(Pc));
    printf("Qc: %s\n", BN_bn2hex(Qc));
    printf("t: %f\n", (double)(tic5-tic4)/CLOCKS_PER_SEC);

    printf("\n");
    clock_t tic6 = clock();
    int cmp_ab, cmp_ac, cmp_bc;
    if(0 > (cmp_ab = is_equal_legal_identity(Pa, Qa, Pb, Qb, k2, modulo, ctx))) goto err;
    if(0 > (cmp_ac = is_equal_legal_identity(Pa, Qa, Pc, Qc, k2, modulo, ctx))) goto err;//);
    if(0 > (cmp_bc = is_equal_legal_identity(Pb, Qb, Pc, Qc, k2, modulo, ctx))) goto err;
    clock_t tic7 = clock();
    printf("ab %d\n", cmp_ab);
    printf("ac %d\n", cmp_ac);
    printf("bc %d\n", cmp_bc);
    printf("t: %f\n", (double)(tic7-tic6)/CLOCKS_PER_SEC);

    result = 0;
err:
    while (0 != (err_code = ERR_peek_error()))
    {
        fprintf(stderr, "Detailed error: %s\n",
                ERR_reason_error_string(err_code));
        ERR_get_error(); // remove error from error stack
    }
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    return result;
}
