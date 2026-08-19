#include <stdio.h>
#include <string.h>

#include "bop-v1/bx_ntvdm_dem_dpb_service.h"

int main(void)
{
    uint8_t inventory[26] = { 2u, 0u, 3u };
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD] = { 0u };
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x7588u
    };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_multi_write_transaction_v1 transaction;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.es = 0x100u;
    cpu.ebp = 0x200u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x50u, 0x46u }, 4u);
    if (!bx_ntvdm_dem_dpb_service_v1_prepare(inventory, &event, &cpu, &window,
            &transaction, payload) || transaction.writes.write_count != 4u ||
        transaction.writes.payload_bytes != 12u ||
        transaction.writes.writes[0].guest_physical_address != 0x1200u ||
        transaction.writes.writes[1].guest_physical_address != 0x121bu ||
        transaction.writes.writes[2].guest_physical_address != 0x1223u ||
        transaction.writes.writes[3].guest_physical_address != 0x123eu ||
        payload[0] != 0u || payload[1] != 0u || payload[2] != 0x21u ||
        payload[3] != 0x02u || payload[4] != 0u || payload[5] != 0x01u ||
        payload[6] != 2u || payload[7] != 2u || payload[8] != 0xffu ||
        payload[9] != 0xffu || payload[10] != 0xffu || payload[11] != 0xffu ||
        transaction.result.cpu_delta.gpr16_write_mask != (1u << 6) ||
        transaction.result.cpu_delta.gpr16_values[6] != 0x246u ||
        transaction.result.resume_rip != 0x758cu) return 1;
    cpu.ebp = 0xffddu;
    if (bx_ntvdm_dem_dpb_service_v1_prepare(inventory, &event, &cpu, &window,
            &transaction, payload)) return 2;
    memset(inventory, 0, sizeof(inventory));
    cpu.ebp = 0x200u;
    if (!bx_ntvdm_dem_dpb_service_v1_prepare(inventory, &event, &cpu, &window,
            &transaction, payload) || transaction.writes.write_count != 0u ||
        transaction.writes.payload_bytes != 0u ||
        transaction.result.cpu_delta.gpr16_values[6] != 0x200u) return 3;
    puts("bx-ntvdm DEM DPB service: source-written fields and BP contract verified");
    return 0;
}
