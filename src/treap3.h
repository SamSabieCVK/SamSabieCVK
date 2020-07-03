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

#ifndef TREAP3_H
#define TREAP3_H

#include "votersignature.h"

/**
 * @brief Structure used as node for the treap with repetitions
 */

typedef struct Node
{
    Signatures record; /**< different parts of voter's signatures */
    unsigned long int prior; /**< random priority used in the treap structure */
    struct Node *l, *m, *r; /**< pointers to the left, middle and right sub-tree */
} Node;

/**
 * @brief initializes node
 *
 * @param record voter's signatures and mapped identity
 * @param prior priority of the node
 *
 * @return the newly allocated node
 */

Node *node(Signatures record, unsigned long int prior);

/**
 * @brief Inserts node into the treap
 *
 * @param t [out] pointer to resulting treap
 * @param it pointer to node to insert
 */

void insert (Node **t, Node *it);

/**
 * @brief Destructs treap and frees memory
 *
 * @param t [out] pointer to the treap
 */

void free_treap(Node **t);

#endif
