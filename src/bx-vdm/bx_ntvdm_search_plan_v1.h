#ifndef BX_NTVDM_SEARCH_PLAN_V1_H
#define BX_NTVDM_SEARCH_PLAN_V1_H

#include <stdint.h>

#include "bx_ntvdm_search_result_v1.h"

#define BX_NTVDM_SEARCH_PLAN_V1_MAGIC 0x42585350u
#define BX_NTVDM_SEARCH_PLAN_V1_VERSION 1u
#define BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY 127u

enum bx_ntvdm_search_plan_v1_form {
    BX_NTVDM_SEARCH_PLAN_V1_PATH = 1u,
    BX_NTVDM_SEARCH_PLAN_V1_FCB = 2u
};

enum bx_ntvdm_search_plan_v1_result {
    BX_NTVDM_SEARCH_PLAN_V1_OK = 1,
    BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH = 0,
    BX_NTVDM_SEARCH_PLAN_V1_REJECTED = -1,
    BX_NTVDM_SEARCH_PLAN_V1_CAPACITY = -2
};

/* This is adapter-local normalized input, not a guest ABI.  A later service
 * decoder must establish its values from source-defined guest buffers. */
typedef struct bx_ntvdm_search_query_v1 {
    uint8_t drive_index;
    uint8_t reserved[3];
    uint32_t attributes;
    wchar_t relative_directory[BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY + 1u];
    wchar_t pattern[13];
} bx_ntvdm_search_query_v1;

typedef struct bx_ntvdm_search_plan_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_search_sessions_v1 sessions;
} bx_ntvdm_search_plan_v1;

void bx_ntvdm_search_plan_v1_initialize(bx_ntvdm_search_plan_v1 *value);
void bx_ntvdm_search_plan_v1_release(bx_ntvdm_search_plan_v1 *value);
int bx_ntvdm_search_plan_v1_valid(const bx_ntvdm_search_plan_v1 *value);

/* Applies source-shaped pathname visibility and continuation handling to an
 * already admitted immutable entry snapshot.  It performs no host namespace
 * initialization, directory query, path conversion, or handle operation. */
int bx_ntvdm_search_plan_v1_first_entries(bx_ntvdm_search_plan_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_query_v1 *query,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);

/* Produces only a successful result plan.  NO_MATCH and REJECTED leave no
 * guest-write transaction; the source-specific error output is a later unit. */
int bx_ntvdm_search_plan_v1_first(bx_ntvdm_search_plan_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_query_v1 *query,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);

int bx_ntvdm_search_plan_v1_next(bx_ntvdm_search_plan_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_token_v1 *presented,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);

#endif
