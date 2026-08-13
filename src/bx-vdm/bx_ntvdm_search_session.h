#ifndef BX_NTVDM_SEARCH_SESSION_H
#define BX_NTVDM_SEARCH_SESSION_H

#include <stdint.h>

#include "bx_ntvdm_host_namespace.h"

/* Same-island search continuation state. It is never serialized to Bochs or
 * guest memory: guest-visible fields are copied into the source-defined DTA
 * or FCB slots by a later transaction service. */
#define BX_NTVDM_SEARCH_SESSION_V1_MAGIC 0x42585353u
#define BX_NTVDM_SEARCH_SESSION_V1_VERSION 1u
#define BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS 16u

typedef struct bx_ntvdm_search_token_v1 {
    uint32_t cookie;
    uint32_t id;
} bx_ntvdm_search_token_v1;

typedef struct bx_ntvdm_search_slot_v1 {
    bx_ntvdm_search_token_v1 token;
    uint32_t owner_pdb;
    uint32_t next_index;
    uint32_t entry_count;
    bx_ntvdm_host_namespace_entry_v1 *entries;
} bx_ntvdm_search_slot_v1;

typedef struct bx_ntvdm_search_sessions_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_search_slot_v1 slots[BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS];
} bx_ntvdm_search_sessions_v1;

void bx_ntvdm_search_sessions_v1_initialize(bx_ntvdm_search_sessions_v1 *value);
void bx_ntvdm_search_sessions_v1_release(bx_ntvdm_search_sessions_v1 *value);
int bx_ntvdm_search_sessions_v1_valid(const bx_ntvdm_search_sessions_v1 *value);

/* Takes an immutable private copy of an already projected, ordered result
 * sequence. A successful single-item search deliberately returns a zero token;
 * a continuation token is issued only when there is a next item. */
int bx_ntvdm_search_sessions_v1_begin(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t entry_count, bx_ntvdm_host_namespace_entry_v1 *first,
    bx_ntvdm_search_token_v1 *continuation);

/* Validates both opaque words and the PDB owner, then returns one item. On the
 * final item it releases the private snapshot and clears `continuation`. */
int bx_ntvdm_search_sessions_v1_next(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_search_token_v1 *presented,
    bx_ntvdm_host_namespace_entry_v1 *next,
    bx_ntvdm_search_token_v1 *continuation);
void bx_ntvdm_search_sessions_v1_cancel(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_search_token_v1 *presented);
/* Releases every private finite-search continuation owned by one terminating
 * PSP/PDB. This is same-island lifecycle state; the count never crosses an
 * adapter, guest, CLI or Bochs ABI. */
int bx_ntvdm_search_sessions_v1_release_pdb(bx_ntvdm_search_sessions_v1 *value,
    uint16_t owner_pdb, uint32_t *released_slots);

#endif
