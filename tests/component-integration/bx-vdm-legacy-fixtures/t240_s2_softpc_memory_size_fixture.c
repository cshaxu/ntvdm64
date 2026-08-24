#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/softpc_memory_size_v2_generic_ud_bridge.h"
#include "bx_ntvdm_cpu_state_abi.h"

static uint8_t bda[2] = { 0x7fu, 0x02u };
static int readable = 1;

int bx_ntvdm_mantle_checked_ram_read_v1(uint64_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    if (!readable || address != 0x413u || byte_count != sizeof(bda)) return 0;
    memcpy(bytes, bda, sizeof(bda));
    return 1;
}

static void initialize(struct bx_ntvdm_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = 0x7000u;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x12u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    initialize(&event);
    if (!bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(&event) ||
        !bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x7003u || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0x027fu || outcome.gpr16_values[1] != 0u)
        return 1;
    bda[0] = 0x34u; bda[1] = 0x12u;
    if (!bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(&event, &outcome) ||
        outcome.gpr16_values[0] != 0x1234u) return 2;
    readable = 0;
    if (bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(&event, &outcome))
        return 3;
    readable = 1;
    event.window[2] = 0x15u;
    if (bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(&event) ||
        bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(&event, &outcome))
        return 4;
    event.window[2] = 0x12u;
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(&event)) return 5;
    puts("T240 S2 source-shaped SoftPC memory_size reads mutable BDA RAM");
    return 0;
}
