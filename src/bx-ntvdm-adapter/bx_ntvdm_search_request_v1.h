#ifndef BX_NTVDM_SEARCH_REQUEST_V1_H
#define BX_NTVDM_SEARCH_REQUEST_V1_H

#include <stdint.h>

#include "bx_ntvdm_search_plan_v1.h"

#define BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES 128u
#define BX_NTVDM_SEARCH_REQUEST_V1_DTA_BYTES 43u
#define BX_NTVDM_SEARCH_REQUEST_V1_FCB_BYTES 53u

/* Decode source-staged bytes into adapter-local input.  These functions own
 * no guest address, CPU, selector, BOP, namespace or session state. */
int bx_ntvdm_search_request_v1_decode_first_path(
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES],
    uint16_t attributes, bx_ntvdm_search_query_v1 *query);
int bx_ntvdm_search_request_v1_decode_first_fcb(
    const uint8_t path[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES],
    uint8_t extended_fcb, uint8_t attributes, bx_ntvdm_search_query_v1 *query);
int bx_ntvdm_search_request_v1_decode_next_path(
    const uint8_t dta[BX_NTVDM_SEARCH_REQUEST_V1_DTA_BYTES],
    bx_ntvdm_search_token_v1 *token);
int bx_ntvdm_search_request_v1_decode_next_fcb(
    const uint8_t searchbuf[BX_NTVDM_SEARCH_REQUEST_V1_FCB_BYTES],
    bx_ntvdm_search_token_v1 *token);

#endif
