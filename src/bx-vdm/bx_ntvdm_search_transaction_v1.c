#include "bx_ntvdm_search_transaction_v1.h"

#include <string.h>

static int bx_ntvdm_search_transaction_path_no_more(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    if (payload_bytes == 0 || !bx_ntvdm_search_result_v1_prepare_path_no_more(
        boundary, cpu_before, dta_address, transaction, payload)) return -1;
    *payload_bytes = 8u;
    return 0;
}

void bx_ntvdm_search_transaction_v1_initialize(
    bx_ntvdm_search_transaction_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_SEARCH_TRANSACTION_V1_MAGIC;
    value->version = BX_NTVDM_SEARCH_TRANSACTION_V1_VERSION;
    bx_ntvdm_search_plan_v1_initialize(&value->plan);
}

void bx_ntvdm_search_transaction_v1_release(
    bx_ntvdm_search_transaction_v1 *value)
{
    if (value == 0) return;
    bx_ntvdm_search_plan_v1_release(&value->plan);
    memset(value, 0, sizeof(*value));
}

int bx_ntvdm_search_transaction_v1_valid(
    const bx_ntvdm_search_transaction_v1 *value)
{
    return value != 0 && value->magic == BX_NTVDM_SEARCH_TRANSACTION_V1_MAGIC &&
        value->version == BX_NTVDM_SEARCH_TRANSACTION_V1_VERSION &&
        bx_ntvdm_search_plan_v1_valid(&value->plan);
}

int bx_ntvdm_search_transaction_v1_path_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t path[128], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    (void)value; (void)space; (void)boundary; (void)cpu_before; (void)owner_pdb;
    (void)dta_address; (void)path; (void)attributes; (void)transaction; (void)payload;
    if (payload_bytes != 0) *payload_bytes = 0u;
    /* The ambient host-directory backend is deliberately not in the runtime
     * closure. T97's provider reaches only path_first_entries. */
    return -1;
}

int bx_ntvdm_search_transaction_v1_path_first_entries(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    uint8_t admitted_drive_index,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t path[128], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_query_v1 query;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0 ||
        !bx_ntvdm_search_request_v1_decode_first_path(path, attributes, &query) ||
        query.drive_index != admitted_drive_index || query.relative_directory[0] != L'\0')
        return -1;
    result = bx_ntvdm_search_plan_v1_first_entries(&value->plan, entries, entry_count,
        boundary, cpu_before, owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH,
        dta_address, &query, transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes) : result ==
        BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_path_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t dta[43],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_token_v1 token;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0) return -1;
    if (!bx_ntvdm_search_request_v1_decode_next_path(dta, &token))
        return bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes);
    result = bx_ntvdm_search_plan_v1_next(&value->plan, boundary, cpu_before,
        owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH, dta_address, &token,
        transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes) : result ==
        BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_fcb_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t address, const uint8_t path[128], uint8_t extended_fcb,
    uint8_t attributes, bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51], uint32_t *payload_bytes)
{
    (void)value; (void)space; (void)boundary; (void)cpu_before; (void)owner_pdb;
    (void)address; (void)path; (void)extended_fcb; (void)attributes; (void)transaction;
    (void)payload;
    if (payload_bytes != 0) *payload_bytes = 0u;
    return -1;
}

int bx_ntvdm_search_transaction_v1_fcb_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t address, const uint8_t searchbuf[53],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_token_v1 token;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0 ||
        !bx_ntvdm_search_request_v1_decode_next_fcb(searchbuf, &token)) return -1;
    return bx_ntvdm_search_plan_v1_next(&value->plan, boundary, cpu_before,
        owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_FCB, address, &token, transaction,
        payload, payload_bytes) == BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}
