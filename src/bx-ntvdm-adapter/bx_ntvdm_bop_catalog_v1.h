#ifndef BX_NTVDM_BOP_CATALOG_V1_H
#define BX_NTVDM_BOP_CATALOG_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"

#define BX_NTVDM_BOP_CATALOG_V1_MAGIC 0x42584243u
#define BX_NTVDM_BOP_CATALOG_V1_VERSION 1u

enum bx_ntvdm_bop_catalog_v1_source_class {
    BX_NTVDM_BOP_CATALOG_V1_SOURCE_UNCATALOGUED = 0u,
    BX_NTVDM_BOP_CATALOG_V1_SOURCE_SOFTPC_BIOS = 1u,
    BX_NTVDM_BOP_CATALOG_V1_SOURCE_OPENNT_TOP_LEVEL = 2u,
    /* A selector can have both a named OpenNT top-level definition and a
     * named SoftPC BIOS definition.  The three BOP bytes alone do not choose
     * an owner, so the catalogue must preserve that ambiguity. */
    BX_NTVDM_BOP_CATALOG_V1_SOURCE_OVERLAPPING = 3u
};

enum bx_ntvdm_bop_catalog_v1_family {
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE = 0u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_DOS = 1u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_XMS = 2u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_DPMI = 3u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_COMMAND = 4u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_DEBUGGER = 5u,
    BX_NTVDM_BOP_CATALOG_V1_FAMILY_REDIR = 6u
};

enum bx_ntvdm_bop_catalog_v1_service_state {
    BX_NTVDM_BOP_CATALOG_V1_SERVICE_NOT_APPLICABLE = 0u,
    BX_NTVDM_BOP_CATALOG_V1_SERVICE_PRESENT = 1u,
    BX_NTVDM_BOP_CATALOG_V1_SERVICE_TRUNCATED = 2u,
    /* A family BOP supplied its fourth byte, but that byte has no matching
     * definition in the pinned OpenNT service-header inventory. */
    BX_NTVDM_BOP_CATALOG_V1_SERVICE_UNDEFINED = 3u
};

/* Identity only: this record does not contain a dispatch target, result,
 * pointer, guest address, or mutable CPU state. */
typedef struct bx_ntvdm_bop_catalog_v1_identity {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t is_bop, source_class, family, service_state;
    uint8_t selector, service;
    uint16_t reserved0;
} bx_ntvdm_bop_catalog_v1_identity;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_bop_catalog_v1_initialize(
    bx_ntvdm_bop_catalog_v1_identity *identity);
int bx_ntvdm_bop_catalog_v1_valid(
    const bx_ntvdm_bop_catalog_v1_identity *identity);
int bx_ntvdm_bop_catalog_v1_observe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_catalog_v1_identity *identity);
#ifdef __cplusplus
}
#endif

#endif
