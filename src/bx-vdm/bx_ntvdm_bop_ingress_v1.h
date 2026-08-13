#ifndef BX_NTVDM_BOP_INGRESS_V1_H
#define BX_NTVDM_BOP_INGRESS_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

#define BX_NTVDM_BOP_INGRESS_V1_MAGIC 0x42584931u
#define BX_NTVDM_BOP_INGRESS_V1_VERSION 1u

typedef enum bx_ntvdm_bop_family_v1 {
    BX_NTVDM_BOP_FAMILY_NONE = 0u,
    BX_NTVDM_BOP_FAMILY_DEM = 1u,
    BX_NTVDM_BOP_FAMILY_WOW = 2u,
    BX_NTVDM_BOP_FAMILY_XMS = 3u,
    BX_NTVDM_BOP_FAMILY_DPMI = 4u,
    BX_NTVDM_BOP_FAMILY_COMMAND = 5u,
    BX_NTVDM_BOP_FAMILY_DEBUGGER = 6u,
    BX_NTVDM_BOP_FAMILY_REDIR = 7u,
    BX_NTVDM_BOP_FAMILY_TOP_LEVEL = 8u
} bx_ntvdm_bop_family_v1;

typedef enum bx_ntvdm_bop_route_v1 {
    BX_NTVDM_BOP_ROUTE_NOT_BOP = 0u,
    BX_NTVDM_BOP_ROUTE_INCOMPLETE = 1u,
    BX_NTVDM_BOP_ROUTE_UNKNOWN_SELECTOR = 2u,
    BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED = 3u,
    BX_NTVDM_BOP_ROUTE_NOT_CALLABLE_SENTINEL = 4u,
    BX_NTVDM_BOP_ROUTE_UNKNOWN_SERVICE = 5u,
    BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE = 6u
} bx_ntvdm_bop_route_v1;

/* Fixed-width, copied routing fact. It carries neither a guest pointer nor a
 * CPU result. Later provider planes must choose their own source-proven
 * failure/resume semantics. */
typedef struct bx_ntvdm_bop_ingress_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t route;
    uint16_t family;
    uint8_t selector;
    uint8_t service;
    uint32_t has_service;
} bx_ntvdm_bop_ingress_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_bop_ingress_v1_clear(bx_ntvdm_bop_ingress_v1 *ingress);
int bx_ntvdm_bop_ingress_v1_valid(const bx_ntvdm_bop_ingress_v1 *ingress);
int bx_ntvdm_bop_ingress_v1_classify(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress);
/* The common entrypoint has no provider in S2. It validates a #UD boundary,
 * records the route, and returns a typed pass-through result. Future provider
 * planes may replace that result only through their separately admitted ABI. */
int bx_ntvdm_bop_ingress_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
