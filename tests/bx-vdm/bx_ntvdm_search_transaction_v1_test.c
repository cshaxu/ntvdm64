#include <string.h>

#include "bx_ntvdm_search_transaction_v1.h"

static void put_u32(uint8_t *out, uint32_t value)
{
    out[0] = (uint8_t)value; out[1] = (uint8_t)(value >> 8);
    out[2] = (uint8_t)(value >> 16); out[3] = (uint8_t)(value >> 24);
}

int main(void)
{
    bx_ntvdm_search_transaction_v1 value;
    bx_ntvdm_host_namespace_entry_v1 entries[2] = {0}, first;
    bx_ntvdm_search_token_v1 token;
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7c00u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 tx;
    uint8_t dta[43] = {0}, fcb[53] = {0}, payload[51], path[128] = {0};
    uint32_t payload_bytes = 0u;
    wcscpy(entries[0].dos_name, L"ALPHA.TXT");
    wcscpy(entries[1].dos_name, L"ZETA.TXT");
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_search_transaction_v1_initialize(&value);
    entries[0].attributes = 0x20u;
    entries[0].byte_count = 7u;
    entries[0].dos_time = 1u;
    entries[0].dos_date = 2u;
    entries[1].attributes = 0x20u;
    memcpy(path, "C:\\*.TXT", 8u);
    if (bx_ntvdm_search_transaction_v1_path_first_entries(&value, entries, 2u,
        2u, &event, &cpu, 0x1111u, 0x900u, path, 0u, &tx, payload,
        &payload_bytes) != 1 || payload_bytes != 30u ||
        memcmp(payload + 17u, "ALPHA.TXT", 9u) != 0) return 1;
    memset(path, 0, sizeof(path));
    memcpy(path, "C:\\*.COM", 8u);
    if (bx_ntvdm_search_transaction_v1_path_first_entries(&value, entries, 2u,
        2u, &event, &cpu, 0x1111u, 0x900u, path, 0u, &tx, payload,
        &payload_bytes) != 0 || payload_bytes != 8u ||
        tx.result.cpu_delta.gpr16_values[0] != 0x12u) return 1;
    bx_ntvdm_search_transaction_v1_release(&value);
    bx_ntvdm_search_transaction_v1_initialize(&value);
    if (!bx_ntvdm_search_sessions_v1_begin(&value.plan.sessions, 0x1111u,
        entries, 2u, &first, &token)) return 1;
    put_u32(dta, token.cookie); put_u32(dta + 4u, token.id);
    if (bx_ntvdm_search_transaction_v1_path_next(&value, &event, &cpu, 0x1111u,
        0x900u, dta, &tx, payload, &payload_bytes) != 1 || payload_bytes != 30u ||
        memcmp(payload + 17u, "ZETA.TXT", 8u) != 0 || payload[0] != 0u) return 2;
    memset(dta, 0, sizeof(dta));
    if (bx_ntvdm_search_transaction_v1_path_next(&value, &event, &cpu, 0x900u,
        0x900u, dta, &tx, payload, &payload_bytes) != 0 || payload_bytes != 8u ||
        tx.result.cpu_delta.gpr16_values[0] != 0x12u ||
        tx.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 3;
    bx_ntvdm_search_transaction_v1_release(&value);
    bx_ntvdm_search_transaction_v1_initialize(&value);
    if (!bx_ntvdm_search_sessions_v1_begin(&value.plan.sessions, 0x2222u,
        entries, 2u, &first, &token)) return 4;
    put_u32(fcb + 32u, token.cookie); put_u32(fcb + 36u, token.id);
    if (bx_ntvdm_search_transaction_v1_fcb_next(&value, &event, &cpu, 0x2222u,
        0xa00u, fcb, &tx, payload, &payload_bytes) != 1 || payload_bytes != 51u ||
        memcmp(payload, "ZETA    TXT", 11u) != 0) return 5;
    memset(fcb, 0, sizeof(fcb));
    if (bx_ntvdm_search_transaction_v1_fcb_next(&value, &event, &cpu, 0x2222u,
        0xa00u, fcb, &tx, payload, &payload_bytes) != -1 || payload_bytes != 0u)
        return 6;
    memcpy(path, "C:REL\\*.TXT", 11u);
    if (bx_ntvdm_search_transaction_v1_path_first(&value, 0, &event, &cpu,
        0x2222u, 0x900u, path, 0u, &tx, payload, &payload_bytes) != -1) return 7;
    bx_ntvdm_search_transaction_v1_release(&value);
    return 0;
}
