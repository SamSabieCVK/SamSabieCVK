# Welcome to the Sam sabie CVK

Sam sabie CVK (translated as DIY election board) provides protocol and infrastructure to find the result of an election fairly and independently.

## Overview

Here is presented a script how shall a voter to behave using Sam sabie CVK.

![Overview diagram](./doc/image/overview.svg)

1) Make a choice of your candidate/party, mark appropriately.
2) Place your passport below a row with your choice; make a photo of your pass last page together with the ballot.
3) Cast your vote.
4) Encrypt your photo with dedicated application of Sam sabie CVK project. The resulting digital ballot is a picture consists of area of the paper ballot with your choice and a digital signature presented as coloured grid. The coloured grid is dedicated to be recognized by a machine, however, it has human recognizable form. The signature consists of a code for passport validity (country, age, date of expiration), a voter signature and a signature to protect the digital ballot integrity.
5) Send the digital ballot to the alternative election board from Sam sabie CVK project.
6) The alternative election board verifies that one voter cast only one vote by comparing all pairs of ballots with a special procedure. Duplications are marked as invalid.
7) The alternative election board publishes results.

The alternative board cannot compare real identity, it has the procedure to compare the voter signatures of two ballots only. Neither the alternative board nor the third party cannot decrypt the ballot and obtain personal information provided by pass (8). Even knowing the personal data from the pass, it is impossible to know the ballot, because every time the signature will be different (9).

## Voter signature

The voter signature is essential part of proposed scheme. With term `voter signature` here and further will be referred only such part of all signature which encrypts legal identity of the voter. The other parts of the whole signature are named differently. The legal identity is based on the personal information from the photo of pass, namely the first and second name, the date of birth, the serial number of pass and the other data like personal number if presented. The legal identity is different for any new election, this can be done by hashing information from legal identity and the election specific information like a memo phrase or a public key.

There are 3 main requirements on encryption for the voter signature:
1) It is not possible from the voter signature to restore either any piece information from the pass or the legal identity as the whole.
2) It is not possible by knowing either any piece information from the pass or the legal identity as the whole to recreate the same voter signature and thus to find the corresponding digital ballot. The algorithm yields every time a different voter signature when called with the same legal identity.
3) For the two voter signatures, a fast algorithm exists which can verify without decryption that these signatures have either the same or the different legal identity. In both outcome of the verification no additional information can be guessed about the legal identities.
The possibility and speed in this paragraph are understood as usual in the asymmetrical cryptography, i.e. fast and possible means an algorithm with polynomial time exists, when slow and impossible means only an exponential time algorithm exists. And the parameters of algorithm are such, that brute-forcing or other exponential algorithm will take essential time to finish, e.g. average duration of human life or even more. The typical and widely used in cryptography example of the asymmetric pair of function is discrete exponentiation `O(n)` and discrete logarithm `O(exp(n))`, where `n` number of bits in key.

The protocol for the voter signature is based on the socialist millionaire problem an introduction to it can be found on [wiki](https://en.wikipedia.org/wiki/Socialist_millionaire_problem) and the algorithm was published in Fabrice Boudot, Berry Schoenmakers, Jacques Traoré (2001). "A Fair and Efficient Solution to the Socialist Millionaires' Problem". Discrete Applied Mathematics. 111 (1). pp. 23--36.

Let say we have an alternative election board (AEB) based on this project and voters we call by cryptographic tradition as Alice and Bob. Then the protocol is following:

1) AEB selects long prime number `p` and random non-unitary element `h` of the ring modulo `p`. The `h` is a root of unity (as any element in the ring with prime modulo), however it is necessary that `h` is the root of unity of the high order. The later can be guaranteed if `p` is such prime that `(p - 1) / 2` is also prime.
2) AEB generates two random numbers `k1` and `k2`.
3) AEB calculates `g1 = h ^ k1` and `g2 = h ^ k2`.
4) AEB makes public `p`, `h`, `g1`, `g2`.

5) Voter Alice has unique legal identity `x`, which evil Eve could know from external source.
6) Alice generates once random number `r` and calculates `P = g2^r` and `Q = h^r * g1^x`, she don't store `r`.
7) Alice sends ballots containing the voter signature `P` and `Q` to AEB.

8) AEB checks that no accepted ballot has the same `P` and `Q`, if any numbers match AEB asks Alice to repeat steps 5-7. The later should happen rare if `h` is high order root of unity and there are many possible `r`.
9) AEB, before counting votes, tests all pairs of ballots: it calculates `(Q_a / Q_b)^k2 == P_a / P_b`  if true both ballots marked as invalid.

| Alice                                  | AEB                                |
|----------------------------------------|------------------------------------|
| legal identity `x`                     | prime `p`, random `h`              |
|                                        | generate random `k1`, `k2`         |
|                                        | calculate `g1 = h^k1`, `g2 = h^k2` |
|                                        | test `g1 != 1`, `g2 != 1`          |
|                                        | publish `p`, `h`, `g1`, `g2`, `k2` |
| generate random `r`                    |                                    |
| calculate `P = g2^r`, `Q = h^r * g1^x` |                                    |
| insecure send `P`, `Q`                 |                                    |
|                                        | test `P != P_i`, `Q != Q_i`        |
|                                        | calculate `c = (Q_i / Q)^k2`       |
|                                        | test `P_i / P == c`                |

How does verification work? Let us have Alice and Bob. Alice has legal identity `x` and generates random number `r` to calculate `P_a` and `Q_a` according rules. And Bob has identity `y`, generates `s` and calculates `P_b` and `Q_b`. Because we have checked that `P_a != P_b`, we are sure that `r != s`. Now the AEB calculates `(Q_a / Q_b)^k2) == (h^(r + k1 * x) / h^(s + k1 * y))^k2 == h^(k2 * (r - s) + k1 * k2 * (x - y))` and `P_a / P_b = h^(k2 * r) / h^(k2 * s) == h^(k2 * (r - s))`. These two numbers can be equal if and only if `x - y == 0`, i.e. the Bob isn't Bob, but Alice imposing Bob.
