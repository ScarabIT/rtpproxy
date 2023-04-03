/*
 * Copyright (c) 2004-2006 Maxim Sobolev <sobomax@FreeBSD.org>
 * Copyright (c) 2006-2014 Sippy Software, Inc., http://www.sippysoft.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "rtpp_debug.h"
#include "rtpp_types.h"
#include "rtpp_hash_table.h"
#include "rtpp_hash_table_fin.h"
#include "rtpp_pearson.h"
#include "rtpp_refcnt.h"
#include "rtpp_mallocs.h"

enum rtpp_hte_types {rtpp_hte_naive_t = 0, rtpp_hte_refcnt_t};

#define	RTPP_HT_LEN	256

struct rtpp_hash_table_entry {
    struct rtpp_hash_table_entry *prev;
    struct rtpp_hash_table_entry *next;
    void *sptr;
    union {
        char *ch;
        uint64_t u64;
        uint32_t u32;
        uint16_t u16;
    } key;
    uint8_t hash;
    enum rtpp_hte_types hte_type;
    char chstor[0];
};

struct rtpp_hash_table_priv
{
    struct rtpp_hash_table pub;
    struct rtpp_pearson rp;
    struct rtpp_hash_table_entry *hash_table[RTPP_HT_LEN];
    pthread_mutex_t hash_table_lock;
    int hte_num;
    enum rtpp_ht_key_types key_type;
    int flags;
};

static struct rtpp_hash_table_entry * hash_table_append_refcnt(struct rtpp_hash_table *,
const void *, struct rtpp_refcnt *, struct rtpp_ht_opstats *);
static void hash_table_remove(struct rtpp_hash_table *self, const void *key, struct rtpp_hash_table_entry * sp);
static struct rtpp_refcnt * hash_table_remove_by_key(struct rtpp_hash_table *self,
  const void *key, struct rtpp_ht_opstats *);
static struct rtpp_refcnt * hash_table_find(struct rtpp_hash_table *self, const void *key);
static void hash_table_foreach(struct rtpp_hash_table *self, rtpp_hash_table_match_t,
  void *, struct rtpp_ht_opstats *);
static void hash_table_foreach_key(struct rtpp_hash_table *, const void *,
  rtpp_hash_table_match_t, void *);
static void hash_table_dtor(struct rtpp_hash_table_priv *);
static int hash_table_get_length(struct rtpp_hash_table *self);
static int hash_table_purge(struct rtpp_hash_table *self);

static const struct rtpp_hash_table_smethods _rtpp_hash_table_smethods = {
    .append_refcnt = &hash_table_append_refcnt,
    .remove = &hash_table_remove,
    .remove_by_key = &hash_table_remove_by_key,
    .find = &hash_table_find,
    .foreach = &hash_table_foreach,
    .foreach_key = &hash_table_foreach_key,
    .get_length = &hash_table_get_length,
    .purge = &hash_table_purge,
};
const struct rtpp_hash_table_smethods * const rtpp_hash_table_smethods = &_rtpp_hash_table_smethods;

struct rtpp_hash_table *
rtpp_hash_table_ctor(enum rtpp_ht_key_types key_type, int flags)
{
    struct rtpp_hash_table *pub;
    struct rtpp_hash_table_priv *pvt;

    pvt = rtpp_rzmalloc(sizeof(struct rtpp_hash_table_priv), PVT_RCOFFS(pvt));
    if (pvt == NULL) {
        goto e0;
    }
    if (pthread_mutex_init(&pvt->hash_table_lock, NULL) != 0)
        goto e1;
    pvt->key_type = key_type;
    pvt->flags = flags;
    pub = &(pvt->pub);
#if defined(RTPP_DEBUG)
    pub->smethods = rtpp_hash_table_smethods;
#endif
    rtpp_pearson_shuffle(&pvt->rp);
    CALL_SMETHOD(pvt->pub.rcnt, attach, (rtpp_refcnt_dtor_t)&hash_table_dtor,
      pvt);
    return (pub);
e1:
    RTPP_OBJ_DECREF(&(pvt->pub));
    free(pvt);
e0:
    return (NULL);
}

static void
hash_table_dtor(struct rtpp_hash_table_priv *pvt)
{
    struct rtpp_hash_table_entry *sp, *sp_next;
    int i;

    rtpp_hash_table_fin(&(pvt->pub));
    for (i = 0; i < RTPP_HT_LEN; i++) {
        sp = pvt->hash_table[i];
        if (sp == NULL)
            continue;
        do {
            sp_next = sp->next;
            if (sp->hte_type == rtpp_hte_refcnt_t) {
                RC_DECREF((struct rtpp_refcnt *)sp->sptr);
            }
            free(sp);
            sp = sp_next;
            pvt->hte_num -= 1;
        } while (sp != NULL);
    }
    pthread_mutex_destroy(&pvt->hash_table_lock);
    RTPP_DBG_ASSERT(pvt->hte_num == 0);

    free(pvt);
}

static inline uint8_t
rtpp_ht_hashkey(struct rtpp_hash_table_priv *pvt, const void *key)
{

    switch (pvt->key_type) {
    case rtpp_ht_key_str_t:
        return rtpp_pearson_hash8(&pvt->rp, key, NULL);

    case rtpp_ht_key_u16_t:
        return rtpp_pearson_hash8b(&pvt->rp, key, sizeof(uint16_t));

    case rtpp_ht_key_u32_t:
        return rtpp_pearson_hash8b(&pvt->rp, key, sizeof(uint32_t));

    case rtpp_ht_key_u64_t:
        return rtpp_pearson_hash8b(&pvt->rp, key, sizeof(uint64_t));

    default:
	abort();
    }
}

static inline int
rtpp_ht_cmpkey(struct rtpp_hash_table_priv *pvt,
  struct rtpp_hash_table_entry *sp, const void *key)
{
    switch (pvt->key_type) {
    case rtpp_ht_key_str_t:
        return (strcmp(sp->key.ch, key) == 0);

    case rtpp_ht_key_u16_t:
        return (sp->key.u16 == *(const uint16_t *)key);

    case rtpp_ht_key_u32_t:
        return (sp->key.u32 == *(const uint32_t *)key);

    case rtpp_ht_key_u64_t:
        return (sp->key.u64 == *(const uint64_t *)key);

    default:
	abort();
    }
}

static inline int
rtpp_ht_cmpkey2(struct rtpp_hash_table_priv *pvt,
  struct rtpp_hash_table_entry *sp1, struct rtpp_hash_table_entry *sp2)
{
    switch (pvt->key_type) {
    case rtpp_ht_key_str_t:
        return (strcmp(sp1->key.ch, sp2->key.ch) == 0);

    case rtpp_ht_key_u16_t:
        return (sp1->key.u16 == sp2->key.u16);

    case rtpp_ht_key_u32_t:
        return (sp1->key.u32 == sp2->key.u32);

    case rtpp_ht_key_u64_t:
        return (sp1->key.u64 == sp2->key.u64);

    default:
        abort();
    }
}

static struct rtpp_hash_table_entry *
hash_table_append_raw(struct rtpp_hash_table *self, const void *key,
  void *sptr, enum rtpp_hte_types htype, struct rtpp_ht_opstats *hosp)
{
    int malen, klen;
    struct rtpp_hash_table_entry *sp, *tsp, *tsp1;
    struct rtpp_hash_table_priv *pvt;

    PUB2PVT(self, pvt);
    if (pvt->key_type == rtpp_ht_key_str_t) {
        klen = strlen(key);
        malen = sizeof(struct rtpp_hash_table_entry) + klen + 1;
    } else {
        malen = sizeof(struct rtpp_hash_table_entry);
    }
    sp = rtpp_zmalloc(malen);
    if (sp == NULL) {
        return (NULL);
    }
    sp->sptr = sptr;
    sp->hte_type = htype;

    sp->hash = rtpp_ht_hashkey(pvt, key);

    switch (pvt->key_type) {
    case rtpp_ht_key_str_t:
        sp->key.ch = &sp->chstor[0];
        memcpy(sp->key.ch, key, klen);
        break;

    case rtpp_ht_key_u16_t:
        sp->key.u16 = *(const uint16_t *)key;
        break;

    case rtpp_ht_key_u32_t:
        sp->key.u32 = *(const uint32_t *)key;
        break;

    case rtpp_ht_key_u64_t:
        sp->key.u64 = *(const uint64_t *)key;
        break;
    }

    pthread_mutex_lock(&pvt->hash_table_lock);
    tsp = pvt->hash_table[sp->hash];
    if (tsp == NULL) {
       	pvt->hash_table[sp->hash] = sp;
    } else {
        for (tsp1 = tsp; tsp1 != NULL; tsp1 = tsp1->next) {
            tsp = tsp1;
            if ((pvt->flags & RTPP_HT_NODUPS) == 0) {
                continue;
            }
            if (rtpp_ht_cmpkey2(pvt, sp, tsp) == 0) {
                continue;
            }
            /* Duplicate detected, reject / abort */
            if ((pvt->flags & RTPP_HT_DUP_ABRT) != 0) {
                abort();
            }
            pthread_mutex_unlock(&pvt->hash_table_lock);
            free(sp);
            return (NULL);
        }
        tsp->next = sp;
        sp->prev = tsp;
    }
    if (hosp != NULL && pvt->hte_num == 0)
        hosp->first = 1;
    pvt->hte_num += 1;
    pthread_mutex_unlock(&pvt->hash_table_lock);
    return (sp);
}

static struct rtpp_hash_table_entry *
hash_table_append_refcnt(struct rtpp_hash_table *self, const void *key,
  struct rtpp_refcnt *rptr, struct rtpp_ht_opstats *hosp)
{
    static struct rtpp_hash_table_entry *rval;

    RC_INCREF(rptr);
    rval = hash_table_append_raw(self, key, rptr, rtpp_hte_refcnt_t, hosp);
    if (rval == NULL) {
        RC_DECREF(rptr);
        return (NULL);
    }
    return (rval);
}

static inline void
hash_table_remove_locked(struct rtpp_hash_table_priv *pvt,
  struct rtpp_hash_table_entry *sp, uint8_t hash, struct rtpp_ht_opstats *hosp)
{

    if (sp->prev != NULL) {
        sp->prev->next = sp->next;
        if (sp->next != NULL) {
            sp->next->prev = sp->prev;
        }
    } else {
        /* Make sure we are removing the right session */
        RTPP_DBG_ASSERT(pvt->hash_table[hash] == sp);
        pvt->hash_table[hash] = sp->next;
        if (sp->next != NULL) {
            sp->next->prev = NULL;
        }
    }
    pvt->hte_num -= 1;
    if (hosp != NULL && pvt->hte_num == 0)
        hosp->last = 1;
}

static void
hash_table_remove(struct rtpp_hash_table *self, const void *key,
  struct rtpp_hash_table_entry * sp)
{
    uint8_t hash;
    struct rtpp_hash_table_priv *pvt;

    PUB2PVT(self, pvt);
    hash = rtpp_ht_hashkey(pvt, key);
    pthread_mutex_lock(&pvt->hash_table_lock);
    hash_table_remove_locked(pvt, sp, hash, NULL);
    pthread_mutex_unlock(&pvt->hash_table_lock);
    if (sp->hte_type == rtpp_hte_refcnt_t) {
        RC_DECREF((struct rtpp_refcnt *)sp->sptr);
    }
    free(sp);
}

static struct rtpp_refcnt *
hash_table_remove_by_key(struct rtpp_hash_table *self, const void *key,
  struct rtpp_ht_opstats *hosp)
{
    uint8_t hash;
    struct rtpp_hash_table_entry *sp;
    struct rtpp_hash_table_priv *pvt;
    struct rtpp_refcnt *rptr;

    PUB2PVT(self, pvt);
    hash = rtpp_ht_hashkey(pvt, key);
    pthread_mutex_lock(&pvt->hash_table_lock);
    for (sp = pvt->hash_table[hash]; sp != NULL; sp = sp->next) {
        if (rtpp_ht_cmpkey(pvt, sp, key)) {
            break;
        }
    }
    if (sp == NULL) {
        pthread_mutex_unlock(&pvt->hash_table_lock);
        return (NULL);
    }
    hash_table_remove_locked(pvt, sp, hash, hosp);
    pthread_mutex_unlock(&pvt->hash_table_lock);
    if (sp->hte_type == rtpp_hte_refcnt_t) {
        RC_DECREF((struct rtpp_refcnt *)sp->sptr);
    }
    rptr = sp->sptr;
    free(sp);
    return (rptr);
}

static struct rtpp_refcnt *
hash_table_find(struct rtpp_hash_table *self, const void *key)
{
    struct rtpp_refcnt *rptr;
    struct rtpp_hash_table_priv *pvt;
    struct rtpp_hash_table_entry *sp;
    uint8_t hash;

    PUB2PVT(self, pvt);
    hash = rtpp_ht_hashkey(pvt, key);
    pthread_mutex_lock(&pvt->hash_table_lock);
    for (sp = pvt->hash_table[hash]; sp != NULL; sp = sp->next) {
        if (rtpp_ht_cmpkey(pvt, sp, key)) {
            break;
        }
    }
    if (sp != NULL) {
        RTPP_DBG_ASSERT(sp->hte_type == rtpp_hte_refcnt_t);
        rptr = (struct rtpp_refcnt *)sp->sptr;
        RC_INCREF(rptr);
    } else {
        rptr = NULL;
    }
    pthread_mutex_unlock(&pvt->hash_table_lock);
    return (rptr);
}

#define VDTE_MVAL(m) (((m) & ~(RTPP_HT_MATCH_BRK | RTPP_HT_MATCH_DEL)) == 0)

static void
hash_table_foreach(struct rtpp_hash_table *self,
  rtpp_hash_table_match_t hte_ematch, void *marg, struct rtpp_ht_opstats *hosp)
{
    struct rtpp_hash_table_entry *sp, *sp_next;
    struct rtpp_hash_table_priv *pvt;
    struct rtpp_refcnt *rptr;
    int i, mval;

    PUB2PVT(self, pvt);
    pthread_mutex_lock(&pvt->hash_table_lock);
    if (pvt->hte_num == 0) {
        pthread_mutex_unlock(&pvt->hash_table_lock);
        return;
    }
    for (i = 0; i < RTPP_HT_LEN; i++) {
        for (sp = pvt->hash_table[i]; sp != NULL; sp = sp_next) {
            RTPP_DBG_ASSERT(sp->hte_type == rtpp_hte_refcnt_t);
            rptr = (struct rtpp_refcnt *)sp->sptr;
            sp_next = sp->next;
            mval = hte_ematch(CALL_SMETHOD(rptr, getdata), marg);
            RTPP_DBG_ASSERT(VDTE_MVAL(mval));
            if (mval & RTPP_HT_MATCH_DEL) {
                hash_table_remove_locked(pvt, sp, sp->hash, hosp);
                RC_DECREF(rptr);
                free(sp);
            }
            if (mval & RTPP_HT_MATCH_BRK) {
                break;
            }
        }
    }
    pthread_mutex_unlock(&pvt->hash_table_lock);
}

static void
hash_table_foreach_key(struct rtpp_hash_table *self, const void *key,
  rtpp_hash_table_match_t hte_ematch, void *marg)
{
    struct rtpp_hash_table_entry *sp, *sp_next;
    struct rtpp_hash_table_priv *pvt;
    struct rtpp_refcnt *rptr;
    int mval;
    uint8_t hash;

    PUB2PVT(self, pvt);
    hash = rtpp_ht_hashkey(pvt, key);
    pthread_mutex_lock(&pvt->hash_table_lock);
    if (pvt->hte_num == 0 || pvt->hash_table[hash] == NULL) {
        pthread_mutex_unlock(&pvt->hash_table_lock);
        return;
    }
    for (sp = pvt->hash_table[hash]; sp != NULL; sp = sp_next) {
        sp_next = sp->next;
        if (!rtpp_ht_cmpkey(pvt, sp, key)) {
            continue;
        }
        RTPP_DBG_ASSERT(sp->hte_type == rtpp_hte_refcnt_t);
        rptr = (struct rtpp_refcnt *)sp->sptr;
        mval = hte_ematch(CALL_SMETHOD(rptr, getdata), marg);
        RTPP_DBG_ASSERT(VDTE_MVAL(mval));
        if (mval & RTPP_HT_MATCH_DEL) {
            hash_table_remove_locked(pvt, sp, sp->hash, NULL);
            RC_DECREF(rptr);
            free(sp);
        }
        if (mval & RTPP_HT_MATCH_BRK) {
            break;
        }
    }
    pthread_mutex_unlock(&pvt->hash_table_lock);
}

static int
hash_table_get_length(struct rtpp_hash_table *self)
{
    struct rtpp_hash_table_priv *pvt;
    int rval;

    PUB2PVT(self, pvt);
    pthread_mutex_lock(&pvt->hash_table_lock);
    rval = pvt->hte_num;
    pthread_mutex_unlock(&pvt->hash_table_lock);

    return (rval);
}

static int
hash_table_purge_f(void *dp, void *ap)
{
    int *npurgedp;

    npurgedp = (int *)ap;
    *npurgedp += 1;
    return (RTPP_HT_MATCH_DEL);
}

static int
hash_table_purge(struct rtpp_hash_table *self)
{
    int npurged;

    npurged = 0;
    CALL_SMETHOD(self, foreach, hash_table_purge_f, &npurged, NULL);
    return (npurged);
}
