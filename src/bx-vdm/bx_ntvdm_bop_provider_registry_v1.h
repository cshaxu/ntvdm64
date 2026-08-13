#ifndef BX_NTVDM_BOP_PROVIDER_REGISTRY_V1_H
#define BX_NTVDM_BOP_PROVIDER_REGISTRY_V1_H

#include <stdint.h>

#include "bx_ntvdm_bop_ingress_v1.h"

#define BX_NTVDM_BOP_PROVIDER_SELECTION_V1_MAGIC 0x42585031u
#define BX_NTVDM_BOP_PROVIDER_SELECTION_V1_VERSION 1u

typedef enum bx_ntvdm_bop_provider_family_v1 {
    BX_NTVDM_BOP_PROVIDER_NONE = 0u,
    BX_NTVDM_BOP_PROVIDER_DEM = 1u,
    BX_NTVDM_BOP_PROVIDER_WOW = 2u,
    BX_NTVDM_BOP_PROVIDER_XMS = 3u,
    BX_NTVDM_BOP_PROVIDER_DPMI = 4u,
    BX_NTVDM_BOP_PROVIDER_COMMAND = 5u,
    BX_NTVDM_BOP_PROVIDER_DEBUGGER = 6u,
    BX_NTVDM_BOP_PROVIDER_REDIR = 7u,
    BX_NTVDM_BOP_PROVIDER_TOP_LEVEL = 8u
} bx_ntvdm_bop_provider_family_v1;

typedef enum bx_ntvdm_bop_provider_precedence_v1 {
    BX_NTVDM_BOP_PROVIDER_PRECEDENCE_NONE = 0u,
    BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT = 1u,
    BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_WITH_CLI_CAPABILITY = 2u,
    BX_NTVDM_BOP_PROVIDER_PRECEDENCE_SOURCE_DERIVED_AFTER_BLOCKER = 3u,
    BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_FAILURE_OR_DEFERRED = 4u
} bx_ntvdm_bop_provider_precedence_v1;

typedef enum bx_ntvdm_bop_provider_disposition_v1 {
    BX_NTVDM_BOP_PROVIDER_NOT_APPLICABLE = 0u,
    BX_NTVDM_BOP_PROVIDER_DEFERRED = 1u,
    BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE = 2u
} bx_ntvdm_bop_provider_disposition_v1;

/* Selection is an adapter-local value record. It deliberately does not hold
 * a callback or module pointer: provider linking/ABI is decided per plane. */
typedef struct bx_ntvdm_bop_provider_selection_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t disposition;
    uint32_t provider_family;
    uint32_t precedence;
    uint32_t reserved0;
} bx_ntvdm_bop_provider_selection_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_bop_provider_selection_v1_clear(
    bx_ntvdm_bop_provider_selection_v1 *selection);
int bx_ntvdm_bop_provider_selection_v1_valid(
    const bx_ntvdm_bop_provider_selection_v1 *selection);
int bx_ntvdm_bop_provider_registry_v1_select(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection);

#ifdef __cplusplus
}
#endif

#endif
