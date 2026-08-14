#include "bx_ntvdm_spckbd_init_service.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x4567u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t payload[1];
    const uint8_t offset[2] = { 0x56u, 0x34u };

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xbeefu; cpu.cs = 0x2000u; cpu.ds = 0x1000u; cpu.esi = 0x0020u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x5fu }, 3u);
    if (!bx_ntvdm_spckbd_stream_state_v1_prepare(
            BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO, &event, &cpu, &window, &action) ||
        action.disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action.range_count != 1u || action.total_bytes != 2u ||
        action.ranges[0].address != 0x20042u || action.ranges[0].length != 2u ||
        action.cpu_result.resume_rip != 0x456au) return 1;
    if (!bx_ntvdm_spckbd_stream_state_v1_complete(
            BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO, &event, &cpu, &action, offset,
            sizeof(offset), &transaction, payload) ||
        transaction.writes.write_count != 1u ||
        transaction.writes.writes[0].guest_physical_address != 0x13456u ||
        transaction.writes.writes[0].byte_count != 1u || payload[0] != 2u ||
        transaction.result.resume_rip != 0x456au ||
        transaction.result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 2;
    if (bx_ntvdm_spckbd_stream_state_v1_prepare(0u, &event, &cpu, &window,
            &action)) return 3;
    if (bx_ntvdm_spckbd_stream_state_v1_complete(
            0u, &event, &cpu, &action, offset,
            sizeof(offset), &transaction, payload)) return 4;
    cpu.cs = 0xffffu; cpu.esi = 0xffefu;
    if (bx_ntvdm_spckbd_stream_state_v1_prepare(
            BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO, &event, &cpu, &window, &action))
        return 5;
    cpu.cs = 0xfffcu; cpu.ds = 0xfffcu; cpu.esi = 0u;
    if (!bx_ntvdm_spckbd_stream_state_v1_prepare(
            BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO, &event, &cpu, &window, &action))
        return 6;
    payload[0] = 0xa5u;
    if (bx_ntvdm_spckbd_stream_state_v1_complete(
            BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO, &event, &cpu, &action,
            (const uint8_t[]){ 0xffu, 0xffu }, 2u, &transaction, payload) ||
        payload[0] != 0xa5u) return 7;
    puts("bx-ntvdm SPCKBD stream state: checked K.SYS offset-to-byte transaction verified");
    return 0;
}
