#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-vdm/bx_ntvdm_startup_machine_interrupt_v1.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *,
    struct bx_ntvdm_generic_ud_outcome_v1 *) { return 0; }

static void event_for(struct bx_ntvdm_generic_ud_event_v1 *event,
    unsigned char selector, unsigned long long rip)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = rip;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = selector;
}

int main()
{
    bx_ntvdm_minimal_machine_c machine;
    bx_ntvdm_generic_ud_event_v1 event;
    bx_ntvdm_cpu_result_v2 result;
    Bit8u frame[4] = {0x34u, 0x12u, 0x70u, 0u};
    Bit8u value = 0u;
    if (machine.initialize(0x200000, 0x200000) != BX_NTVDM_MINIMAL_MACHINE_OK)
        return 1;
    event_for(&event, 0x02u, 0x200u);
    if (!bx_ntvdm_startup_machine_interrupt_v1_dispatch(&event, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x203u ||
        !bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
        return 2;
    if (!bx_mem.copy_to_ordinary_ram(0xb79u, 4u, frame)) return 3;
    event_for(&event, 0x06u, 0x300u);
    event.ss = 0x70u;
    event.esp = 0x479u;
    if (!bx_ntvdm_startup_machine_interrupt_v1_dispatch(&event, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x303u ||
        !bx_mem.copy_from_ordinary_ram(0xb79u, 1u, &value) || value != 0x35u ||
        !bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
        return 4;
    event_for(&event, 0x02u, 0x400u);
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_startup_machine_interrupt_v1_dispatch(&event, &result)) return 5;
    return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 6;
}