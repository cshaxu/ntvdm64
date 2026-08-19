#include "bx_ntvdm_bop_catalog_v1.h"

static uint32_t bx_ntvdm_bop_catalog_v1_source_class(uint8_t selector)
{
    switch (selector) {
    case 0x50u: case 0x51u: case 0x52u: case 0x53u: case 0x54u:
    case 0x56u: case 0x57u: case 0x59u: case 0x5au: case 0x5bu:
    case 0x5cu: case 0x5du: case 0x5eu: case 0x5fu: case 0xfdu:
    case 0xfeu:
        /* The historical SoftPC BIOS[] table explicitly has rows 00..FF;
         * named OpenNT top-level selectors therefore retain both provenances.
         * This is classification only, never table installation or dispatch. */
        return BX_NTVDM_BOP_CATALOG_V1_SOURCE_OVERLAPPING;
    default:
        return BX_NTVDM_BOP_CATALOG_V1_SOURCE_SOFTPC_BIOS;
    }
}

static uint32_t bx_ntvdm_bop_catalog_v1_family(uint8_t selector)
{
    switch (selector) {
    case 0x50u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_DOS;
    case 0x52u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_XMS;
    case 0x53u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_DPMI;
    case 0x54u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_COMMAND;
    case 0x56u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_DEBUGGER;
    case 0x57u: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_REDIR;
    default: return BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE;
    }
}

/* The pinned inventory has no holes in any family.  These maxima are source
 * definition bounds only: they do not select a handler or authorize one. */
static int bx_ntvdm_bop_catalog_v1_service_defined(uint32_t family,
    uint8_t service)
{
    switch (family) {
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_DOS: return service <= 0x49u;
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_XMS: return service <= 0x0bu;
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_DPMI: return service <= 0x18u;
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_COMMAND: return service <= 0x11u;
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_DEBUGGER: return service <= 0x0fu;
    case BX_NTVDM_BOP_CATALOG_V1_FAMILY_REDIR: return service <= 0x31u;
    default: return 0;
    }
}

void bx_ntvdm_bop_catalog_v1_initialize(
    bx_ntvdm_bop_catalog_v1_identity *identity)
{
    if (identity == 0) return;
    identity->magic = BX_NTVDM_BOP_CATALOG_V1_MAGIC;
    identity->abi_version = BX_NTVDM_BOP_CATALOG_V1_VERSION;
    identity->struct_bytes = sizeof(*identity);
    identity->is_bop = 0u;
    identity->source_class = BX_NTVDM_BOP_CATALOG_V1_SOURCE_UNCATALOGUED;
    identity->family = BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE;
    identity->service_state = BX_NTVDM_BOP_CATALOG_V1_SERVICE_NOT_APPLICABLE;
    identity->selector = 0u;
    identity->service = 0u;
    identity->reserved0 = 0u;
}

int bx_ntvdm_bop_catalog_v1_valid(
    const bx_ntvdm_bop_catalog_v1_identity *identity)
{
    return identity != 0 && identity->magic == BX_NTVDM_BOP_CATALOG_V1_MAGIC &&
        identity->abi_version == BX_NTVDM_BOP_CATALOG_V1_VERSION &&
        identity->struct_bytes == sizeof(*identity) && identity->reserved0 == 0u &&
        identity->is_bop <= 1u &&
        identity->source_class <= BX_NTVDM_BOP_CATALOG_V1_SOURCE_OVERLAPPING &&
        identity->family <= BX_NTVDM_BOP_CATALOG_V1_FAMILY_REDIR &&
        identity->service_state <= BX_NTVDM_BOP_CATALOG_V1_SERVICE_UNDEFINED;
}

int bx_ntvdm_bop_catalog_v1_observe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_catalog_v1_identity *identity)
{
    uint8_t selector;
    uint32_t family;
    if (event == 0 || cpu_before == 0 || window == 0 || identity == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u)
        return 0;
    bx_ntvdm_bop_catalog_v1_initialize(identity);
    if (window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u) return 1;
    selector = window->bytes[2];
    family = bx_ntvdm_bop_catalog_v1_family(selector);
    identity->is_bop = 1u;
    identity->selector = selector;
    identity->source_class = bx_ntvdm_bop_catalog_v1_source_class(selector);
    identity->family = family;
    if (family != BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE) {
        if (window->valid_bytes < 4u) {
            identity->service_state = BX_NTVDM_BOP_CATALOG_V1_SERVICE_TRUNCATED;
        } else {
            identity->service_state = BX_NTVDM_BOP_CATALOG_V1_SERVICE_PRESENT;
            identity->service = window->bytes[3];
            if (!bx_ntvdm_bop_catalog_v1_service_defined(family,
                    identity->service))
                identity->service_state =
                    BX_NTVDM_BOP_CATALOG_V1_SERVICE_UNDEFINED;
        }
    }
    return bx_ntvdm_bop_catalog_v1_valid(identity);
}
