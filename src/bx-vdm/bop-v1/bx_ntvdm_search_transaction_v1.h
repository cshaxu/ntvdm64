#ifndef BX_NTVDM_SEARCH_TRANSACTION_V1_H
#define BX_NTVDM_SEARCH_TRANSACTION_V1_H

#include "bx_ntvdm_search_request_v1.h"
#include "bx_ntvdm_profile_search_snapshot_v1.h"

#define BX_NTVDM_SEARCH_TRANSACTION_V1_MAGIC 0x42585354u
#define BX_NTVDM_SEARCH_TRANSACTION_V1_VERSION 1u

typedef struct bx_ntvdm_search_transaction_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_search_plan_v1 plan;
} bx_ntvdm_search_transaction_v1;

void bx_ntvdm_search_transaction_v1_initialize(
    bx_ntvdm_search_transaction_v1 *value);
void bx_ntvdm_search_transaction_v1_release(
    bx_ntvdm_search_transaction_v1 *value);
int bx_ntvdm_search_transaction_v1_valid(
    const bx_ntvdm_search_transaction_v1 *value);

/* Returns 1 with a success plan, 0 with a prepared pathname no-more plan, or
 * -1 with no plan.  FCB no-match/error remains -1 until its source gate is
 * closed; it must not be silently projected as pathname semantics. */
int bx_ntvdm_search_transaction_v1_path_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_search_transaction_v1_path_first_merged(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
/* Pathname-only form for a caller-owned immutable snapshot.  This does not
 * inspect a host namespace and must not be used for the FCB form. */
int bx_ntvdm_search_transaction_v1_path_first_entries(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    uint8_t admitted_drive_index,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_search_transaction_v1_path_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address,
    const uint8_t dta[BX_NTVDM_SEARCH_REQUEST_V1_DTA_BYTES],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_search_transaction_v1_fcb_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t searchbuf_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES],
    uint8_t extended_fcb, uint8_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_search_transaction_v1_fcb_first_merged(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t searchbuf_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES],
    uint8_t extended_fcb, uint8_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
/* FCB form for the declared immutable profile snapshot.  Like the pathname
 * entries form, this deliberately has no host-directory dependency. */
int bx_ntvdm_search_transaction_v1_fcb_first_entries(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    uint8_t admitted_drive_index,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t searchbuf_address,
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES],
    uint8_t extended_fcb, uint8_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_search_transaction_v1_fcb_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t searchbuf_address,
    const uint8_t searchbuf[BX_NTVDM_SEARCH_REQUEST_V1_FCB_BYTES],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);

#endif
