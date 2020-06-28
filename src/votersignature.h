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

#ifndef VOTERSIGNATURE_H
#define VOTERSIGNATURE_H

int prepare_public_keys(int bits_prime,
    BIGNUM *modulo, BIGNUM *h,
    BIGNUM *g1, BIGNUM *g2, BIGNUM *k2,
    BN_CTX *ctx);

int make_voter_signature(BIGNUM *P, BIGNUM *Q,
    const BIGNUM *legal_identity,
    const BIGNUM *modulo, const BIGNUM *h,
    const BIGNUM *g1, const BIGNUM *g2, const BIGNUM *k2,
    BN_CTX *ctx);

int is_equal_legal_identity(const BIGNUM *Pa, const BIGNUM *Qa,
    const BIGNUM *Pb, const BIGNUM *Qb,
    const BIGNUM *modulo, const BIGNUM *k2,
    BN_CTX *ctx);

#endif
