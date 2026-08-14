#include "bx_ntvdm_bop_ingress_v1.h"

#include <string.h>

static void bx_ntvdm_bop_ingress_v1_set(bx_ntvdm_bop_ingress_v1 *ingress,
    uint32_t route, uint16_t family, uint8_t selector, uint8_t service,
    uint32_t has_service)
{
    bx_ntvdm_bop_ingress_v1_clear(ingress);
    ingress->route = route;
    ingress->family = family;
    ingress->selector = selector;
    ingress->service = service;
    ingress->has_service = has_service;
}

void bx_ntvdm_bop_ingress_v1_clear(bx_ntvdm_bop_ingress_v1 *ingress)
{
    if (ingress == 0) return;
    memset(ingress, 0, sizeof(*ingress));
    ingress->magic = BX_NTVDM_BOP_INGRESS_V1_MAGIC;
    ingress->abi_version = BX_NTVDM_BOP_INGRESS_V1_VERSION;
    ingress->struct_bytes = (uint32_t)sizeof(*ingress);
    ingress->route = BX_NTVDM_BOP_ROUTE_NOT_BOP;
}

int bx_ntvdm_bop_ingress_v1_valid(const bx_ntvdm_bop_ingress_v1 *ingress)
{
    return ingress != 0 && ingress->magic == BX_NTVDM_BOP_INGRESS_V1_MAGIC &&
        ingress->abi_version == BX_NTVDM_BOP_INGRESS_V1_VERSION &&
        ingress->struct_bytes == sizeof(*ingress) &&
        ingress->route <= BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE &&
        ingress->family <= BX_NTVDM_BOP_FAMILY_TOP_LEVEL &&
        ingress->has_service <= 1u;
}

static void bx_ntvdm_bop_ingress_v1_classify_service(
    bx_ntvdm_bop_ingress_v1 *ingress, uint8_t selector, uint8_t service,
    uint16_t family, uint8_t callable_count, uint8_t sentinel)
{
    if (service < callable_count)
        bx_ntvdm_bop_ingress_v1_set(ingress,
            BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, family, selector, service, 1u);
    else if (service == sentinel)
        bx_ntvdm_bop_ingress_v1_set(ingress,
            BX_NTVDM_BOP_ROUTE_NOT_CALLABLE_SENTINEL, family, selector,
            service, 1u);
    else
        bx_ntvdm_bop_ingress_v1_set(ingress,
            BX_NTVDM_BOP_ROUTE_UNKNOWN_SERVICE, family, selector, service, 1u);
}

int bx_ntvdm_bop_ingress_v1_classify(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress)
{
    uint8_t selector;
    uint8_t service;

    if (ingress == 0 || !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_bop_ingress_v1_clear(ingress);
    if (window->valid_bytes < 2u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u) return 1;
    if (window->valid_bytes < 3u) {
        bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_INCOMPLETE,
            BX_NTVDM_BOP_FAMILY_NONE, 0u, 0u, 0u);
        return 1;
    }
    selector = window->bytes[2];
    if (selector == 0x50u || selector == 0x52u || selector == 0x53u ||
        selector == 0x54u || selector == 0x56u || selector == 0x57u) {
        if (window->valid_bytes < 4u) {
            bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_INCOMPLETE,
                BX_NTVDM_BOP_FAMILY_NONE, selector, 0u, 0u);
            return 1;
        }
        service = window->bytes[3];
        switch (selector) {
        case 0x50u: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_DEM, 73u, 73u); break;
        case 0x52u: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_XMS, 12u, 0xffu); break;
        case 0x53u: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_DPMI, 25u, 0xffu); break;
        case 0x54u: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_COMMAND, 17u, 17u); break;
        case 0x56u: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_DEBUGGER, 16u, 0xffu); break;
        default: bx_ntvdm_bop_ingress_v1_classify_service(ingress, selector, service, BX_NTVDM_BOP_FAMILY_REDIR, 50u, 0xffu); break;
        }
        return 1;
    }
    switch (selector) {
    case 0x51u: bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_WOW, selector, 0u, 0u); break;
    case 0x59u: case 0x5bu: case 0x5fu:
        bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE, BX_NTVDM_BOP_FAMILY_TOP_LEVEL, selector, 0u, 0u); break;
    case 0x5au: case 0x5cu: case 0x5du: case 0x5eu: case 0xfdu: case 0xfeu:
        bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL, selector, 0u, 0u); break;
    default:
        bx_ntvdm_bop_ingress_v1_set(ingress, BX_NTVDM_BOP_ROUTE_UNKNOWN_SELECTOR, BX_NTVDM_BOP_FAMILY_NONE, selector, 0u, 0u); break;
    }
    return 1;
}

int bx_ntvdm_bop_ingress_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (ingress == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u) {
        bx_ntvdm_bop_ingress_v1_clear(ingress);
        return bx_ntvdm_cpu_result_v2_valid(result);
    }
    return bx_ntvdm_bop_ingress_v1_classify(window, ingress) &&
        bx_ntvdm_cpu_result_v2_valid(result);
}
