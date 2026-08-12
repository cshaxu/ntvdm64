#ifndef BX_NTVDM_SEARCH_RESULT_V1_H
#define BX_NTVDM_SEARCH_RESULT_V1_H

#include <stdint.h>

#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_search_session.h"

/* Builds only successful source-shaped outputs from an already selected
 * projected entry. It owns neither enumeration nor search continuation. */
int bx_ntvdm_search_result_v1_prepare_path(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    const bx_ntvdm_host_namespace_entry_v1 *entry,
    const bx_ntvdm_search_token_v1 *token,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[30]);

/* The source-defined FCB semantic prefix ends at byte 51. This writes only
 * bytes 1..51 and deliberately preserves byte 52 pending its trace gate. */
int bx_ntvdm_search_result_v1_prepare_fcb(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t searchbuf_address,
    const bx_ntvdm_host_namespace_entry_v1 *entry,
    const bx_ntvdm_search_token_v1 *token,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51]);

/* Path FindFirst/FindNext source paths clear both continuation words and
 * return ERROR_NO_MORE_FILES. FCB error behavior remains intentionally out
 * of this API because its next-path source observation is unresolved. */
int bx_ntvdm_search_result_v1_prepare_path_no_more(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[8]);

#endif
