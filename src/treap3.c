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

#include <stdlib.h>
#include <openssl/bn.h>
#include "treap3.h"

Node *node(Signatures record, unsigned long int prior)
{
    Node *result=(Node *)malloc(sizeof(Node));
    result->record = record;
    result->prior = prior;
    result->l = result->m = result->r = NULL;
    return result;
}

/**
 * @brief Splits treap into 3 sub-treaps with mapped identity less, equal
 * and greater than given one
 *
 * @param t pointer to (sub-)treap
 * @param record which mapped identity is used to split
 * @param l [out] left sub-treap with lesser elements
 * @param m [out] middle sub-treap with equal elements
 * @param r [out] right sub-treap with greater elements
 */

void split (Node *t, Signatures record, Node **l, Node **m, Node **r)
{
    if(!t)
    {
        *l = *m = *r = NULL;
    }
    else if(BN_cmp(record.mlid, t->record.mlid) < 0)
    {
        split (t->l, record, l, m, &(t->l));
        *r = t;
    }
    else if(BN_cmp(record.mlid, t->record.mlid) == 0)
    {
        *l = t->l;
        *r = t->r;
        t->l = NULL;
        t->r = NULL;
        *m = t;
    }
    else
    {
        split (t->r, record, &(t->r), m, r);
        *l = t;
    }
}

void insert (Node **t, Node *it)
{
    if(!(*t))
    {
        *t = it;
    }
    else if(it->prior > (*t)->prior)
    {
        split (*t, it->record, &(it->l), &(it->m), &(it->r));
        *t = it;
    }
    else if(BN_cmp(it->record.mlid, (*t)->record.mlid) == 0)
    {
        Node *p = *t;
        while(p->m && it->prior < p->prior)
        {
            p = p->m;
        }
        it->m = p->m, p->m = it;
    }
    else
    {
        insert (BN_cmp(it->record.mlid, (*t)->record.mlid) < 0 ? &((*t)->l) : &((*t)->r), it);
    }
}

void free_treap(Node **t)
{
    if(*t)
    {
        free_treap(&((*t)->l));
        free_treap(&((*t)->m));
        free_treap(&((*t)->r));
        free(*t);
        *t = NULL;
    }
}
