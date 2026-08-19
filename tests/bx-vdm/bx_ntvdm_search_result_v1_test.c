#include <string.h>

#include "bop-v1/bx_ntvdm_search_result_v1.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7c00u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_host_namespace_entry_v1 entry = {0};
    bx_ntvdm_search_token_v1 token = { 0x11223344u, 0x55667788u };
    bx_ntvdm_multi_write_transaction_v1 tx;
    uint8_t payload[51];
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    wcscpy(entry.dos_name, L"ALPHA.TXT");
    entry.attributes = 0x20u; entry.dos_time = 0x1234u;
    entry.dos_date = 0x5678u; entry.byte_count = 0x01020304u;
    if (!bx_ntvdm_search_result_v1_prepare_path(&event, &cpu, 0x900u,
        &entry, &token, &tx, payload) || tx.writes.write_count != 2u ||
        tx.writes.writes[0].guest_physical_address != 0x900u ||
        tx.writes.writes[1].guest_physical_address != 0x915u ||
        tx.writes.payload_bytes != 30u || payload[0] != 0x44u ||
        payload[4] != 0x88u || payload[8] != 0x20u ||
        memcmp(payload + 17u, "ALPHA.TXT", 9u) != 0 ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(&tx, 0x100000u, 30u)) return 1;
    if (!bx_ntvdm_search_result_v1_prepare_fcb(&event, &cpu, 0xa00u,
        &entry, &token, &tx, payload) || tx.writes.write_count != 2u ||
        tx.writes.writes[0].guest_physical_address != 0xa01u ||
        tx.writes.writes[0].byte_count != 19u ||
        tx.writes.writes[1].guest_physical_address != 0xa14u ||
        tx.writes.writes[1].byte_count != 32u || tx.writes.payload_bytes != 51u ||
        memcmp(payload, "ALPHA   TXT", 11u) != 0 || payload[30] != 0x20u ||
        payload[31] != 0x44u || payload[35] != 0x88u ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(&tx, 0x100000u, 51u)) return 2;
    if (!bx_ntvdm_search_result_v1_prepare_path_no_more(&event, &cpu, 0x900u,
        &tx, payload) || tx.writes.write_count != 1u ||
        tx.writes.writes[0].guest_physical_address != 0x900u ||
        tx.writes.writes[0].byte_count != 8u || tx.writes.payload_bytes != 8u ||
        tx.result.cpu_delta.gpr16_write_mask != 1u ||
        tx.result.cpu_delta.gpr16_values[0] != 0x12u ||
        tx.result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        tx.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(&tx, 0x100000u, 8u)) return 3;
    return 0;
}
