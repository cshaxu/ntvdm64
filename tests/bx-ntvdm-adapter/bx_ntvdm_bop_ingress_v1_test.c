#include "bx_ntvdm_bop_ingress_v1.h"

static int classify(uint8_t selector, uint8_t service, uint32_t bytes,
    uint32_t route, uint16_t family)
{
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    uint8_t instruction[4] = { 0xc4u, 0xc4u, selector, service };
    bx_ntvdm_instruction_window_v1_capture(&window, instruction, bytes);
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_ingress_v1_valid(&ingress) && ingress.route == route &&
        ingress.family == family;
}

int main(void)
{
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    uint8_t not_bop[4] = { 0x90u, 0u, 0u, 0u };
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0u
    };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_cpu_result_v2 result;
    struct family_case {
        uint8_t selector, count, sentinel;
        uint16_t family;
    } families[] = {
        { 0x50u, 73u, 73u, BX_NTVDM_BOP_FAMILY_DEM },
        { 0x52u, 12u, 0xffu, BX_NTVDM_BOP_FAMILY_XMS },
        { 0x53u, 25u, 0xffu, BX_NTVDM_BOP_FAMILY_DPMI },
        { 0x54u, 17u, 17u, BX_NTVDM_BOP_FAMILY_COMMAND },
        { 0x56u, 16u, 0xffu, BX_NTVDM_BOP_FAMILY_DEBUGGER },
        { 0x57u, 49u, 0xffu, BX_NTVDM_BOP_FAMILY_REDIR }
    };
    uint32_t index, service;
    int failed = 0;
    failed |= !classify(0x50u, 0x11u, 4u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_DEM);
    failed |= !classify(0x54u, 17u, 4u,
        BX_NTVDM_BOP_ROUTE_NOT_CALLABLE_SENTINEL, BX_NTVDM_BOP_FAMILY_COMMAND);
    failed |= !classify(0x52u, 12u, 4u,
        BX_NTVDM_BOP_ROUTE_UNKNOWN_SERVICE, BX_NTVDM_BOP_FAMILY_XMS);
    failed |= !classify(0x56u, 15u, 4u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_DEBUGGER);
    failed |= !classify(0x59u, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x50u, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_INCOMPLETE, BX_NTVDM_BOP_FAMILY_NONE);
    failed |= !classify(0x58u, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_UNKNOWN_SELECTOR, BX_NTVDM_BOP_FAMILY_NONE);
    for (index = 0u; index < sizeof(families) / sizeof(families[0]); ++index) {
        for (service = 0u; service < families[index].count; ++service)
            failed |= !classify(families[index].selector, (uint8_t)service, 4u,
                BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, families[index].family);
        if (families[index].sentinel != 0xffu)
            failed |= !classify(families[index].selector, families[index].sentinel, 4u,
                BX_NTVDM_BOP_ROUTE_NOT_CALLABLE_SENTINEL, families[index].family);
        failed |= !classify(families[index].selector, families[index].count + 1u, 4u,
            BX_NTVDM_BOP_ROUTE_UNKNOWN_SERVICE, families[index].family);
    }
    failed |= !classify(0x51u, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_WOW);
    failed |= !classify(0x5au, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x5bu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x5cu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x5du, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x5eu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0x5fu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0xfdu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    failed |= !classify(0xfeu, 0u, 3u,
        BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED, BX_NTVDM_BOP_FAMILY_TOP_LEVEL);
    bx_ntvdm_instruction_window_v1_capture(&window, not_bop, 4u);
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_NOT_BOP;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    {
        uint8_t instruction[4] = { 0xc4u, 0xc4u, 0x50u, 0x11u };
        bx_ntvdm_instruction_window_v1_capture(&window, instruction, 4u);
    }
    failed |= !bx_ntvdm_bop_ingress_v1_dispatch(&event, &state, &window,
        &ingress, &result) || ingress.route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        result.resume_rip != 0u;
    event.vector = 13u;
    failed |= !bx_ntvdm_bop_ingress_v1_dispatch(&event, &state, &window,
        &ingress, &result) || ingress.route != BX_NTVDM_BOP_ROUTE_NOT_BOP ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH;
    return failed ? 1 : 0;
}
