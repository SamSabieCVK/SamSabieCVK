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

/**
 * @brief Generates public keys for voter's signature
 *
 * All `BIGNUM`'s should be proper allocated beforehand,
 * as well as entropy source for DRBG proper initialised.
 *
 * @param bits_prime number of bits in generated prime
 * @param modulo [out], generated safe prime
 * @param h [out] some root of unity in the ring
 * @param g1 [out] public key to generate voter's signature
 * @param g2 [out] public key to generate voter's signature
 * @param k2 [out] public key to compare legal identity
 * @param ctx context to allocate intermediate results
 *
 * @return 1 on success, 0 on failure
 */

int prepare_public_keys(int bits_prime,
    BIGNUM *modulo, BIGNUM *h,
    BIGNUM *g1, BIGNUM *g2, BIGNUM *k2,
    BN_CTX *ctx);

/**
 * @brief Makes voter's signature from legal identity
 *
 * All `BIGNUM`'s should be proper allocated beforehand,
 * as well as entropy source for DRBG proper initialised.
 *
 * @param P [out] the first part of voter's signature
 * @param Q [out] the second part of voter's signature
 * @param modulo prime number defining ring
 * @param h some root of unity in the ring
 * @param g1 public key 1
 * @param g2 public key 2
 * @param ctx context to allocate intermediate results
 *
 * @return 1 on success, 0 on failure
 */

int make_voter_signature(BIGNUM *P, BIGNUM *Q,
    const BIGNUM *legal_identity,
    const BIGNUM *modulo, const BIGNUM *h,
    const BIGNUM *g1, const BIGNUM *g2,
    BN_CTX *ctx);

/**
 * @brief Tests two voter's signature for the same legal identity
 *
 * @param Pa the first part of Alice's signature
 * @param Qa the second part of Alice's signature
 * @param Pb the first part of Bob's signature
 * @param Qb the second part of Bob's signature
 * @param modulo prime number defining ring
 * @param k2 public key to make comparison
 * @param ctx context to allocate intermediate results
 *
 * @return 1 if equal, 0 if not equal, -1 on failure
 */

int is_equal_legal_identity(const BIGNUM *Pa, const BIGNUM *Qa,
    const BIGNUM *Pb, const BIGNUM *Qb,
    const BIGNUM *modulo, const BIGNUM *k2,
    BN_CTX *ctx);

#endif
