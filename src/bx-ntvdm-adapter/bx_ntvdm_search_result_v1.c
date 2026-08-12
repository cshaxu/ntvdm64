#include "bx_ntvdm_search_result_v1.h"

#include <string.h>
#include <wctype.h>

static void bx_ntvdm_search_result_u16(uint8_t *out, uint16_t value)
{ out[0] = (uint8_t)value; out[1] = (uint8_t)(value >> 8); }
static void bx_ntvdm_search_result_u32(uint8_t *out, uint32_t value)
{ bx_ntvdm_search_result_u16(out, (uint16_t)value); bx_ntvdm_search_result_u16(out + 2u, (uint16_t)(value >> 16)); }

static int bx_ntvdm_search_result_name(const wchar_t *source, uint8_t name[8],
    uint8_t ext[3], uint8_t path_name[13])
{
    size_t index = 0u, base = 0u, extension = 0u;
    int dot = 0;
    if (source == 0 || source[0] == L'\0') return 0;
    memset(name, ' ', 8u); memset(ext, ' ', 3u); memset(path_name, 0, 13u);
    while (source[index] != L'\0') {
        wchar_t value = towupper((wint_t)source[index]);
        if (index >= 12u || value > 0x7fu) return 0;
        path_name[index] = (uint8_t)value;
        if (value == L'.') { if (dot || base == 0u) return 0; dot = 1; }
        else if (!dot) { if (base >= 8u) return 0; name[base++] = (uint8_t)value; }
        else { if (extension >= 3u) return 0; ext[extension++] = (uint8_t)value; }
        ++index;
    }
    return base != 0u && (!dot || extension != 0u);
}

static int bx_ntvdm_search_result_finish(bx_ntvdm_multi_write_transaction_v1 *tx,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before)
{
    return boundary != 0 && cpu_before != 0 && tx != 0 &&
        boundary->fault_rip <= UINT64_MAX - 4u &&
        bx_ntvdm_cpu_result_v2_resume(&tx->result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(&tx->result, 0);
}

int bx_ntvdm_search_result_v1_prepare_path(const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    const bx_ntvdm_host_namespace_entry_v1 *entry, const bx_ntvdm_search_token_v1 *token,
    bx_ntvdm_multi_write_transaction_v1 *tx, uint8_t payload[30])
{
    uint8_t name[8], ext[3], path_name[13];
    if (entry == 0 || token == 0 || tx == 0 || payload == 0 ||
        dta_address > UINT64_MAX - 43u ||
        !bx_ntvdm_search_result_name(entry->dos_name, name, ext, path_name)) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(tx, boundary, cpu_before);
    memset(payload, 0, 30u);
    bx_ntvdm_search_result_u32(payload, token->cookie);
    bx_ntvdm_search_result_u32(payload + 4u, token->id);
    payload[8u] = (uint8_t)entry->attributes;
    bx_ntvdm_search_result_u16(payload + 9u, entry->dos_time);
    bx_ntvdm_search_result_u16(payload + 11u, entry->dos_date);
    bx_ntvdm_search_result_u32(payload + 13u, (uint32_t)entry->byte_count);
    memcpy(payload + 17u, path_name, sizeof(path_name));
    if (!bx_ntvdm_multi_write_v1_add(&tx->writes, dta_address, 8u, 0u) ||
        !bx_ntvdm_multi_write_v1_add(&tx->writes, dta_address + 21u, 22u, 8u) ||
        !bx_ntvdm_search_result_finish(tx, boundary, cpu_before)) return 0;
    return 1;
}

int bx_ntvdm_search_result_v1_prepare_fcb(const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t address,
    const bx_ntvdm_host_namespace_entry_v1 *entry, const bx_ntvdm_search_token_v1 *token,
    bx_ntvdm_multi_write_transaction_v1 *tx, uint8_t payload[51])
{
    uint8_t name[8], ext[3], path_name[13];
    if (entry == 0 || token == 0 || tx == 0 || payload == 0 ||
        address > UINT64_MAX - 53u ||
        !bx_ntvdm_search_result_name(entry->dos_name, name, ext, path_name)) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(tx, boundary, cpu_before);
    memset(payload, 0, 51u);
    memcpy(payload, name, 8u); memcpy(payload + 8u, ext, 3u);
    bx_ntvdm_search_result_u32(payload + 15u, (uint32_t)entry->byte_count);
    memcpy(payload + 19u, name, 8u); memcpy(payload + 27u, ext, 3u);
    payload[30u] = (uint8_t)entry->attributes;
    bx_ntvdm_search_result_u32(payload + 31u, token->cookie);
    bx_ntvdm_search_result_u32(payload + 35u, token->id);
    bx_ntvdm_search_result_u16(payload + 41u, entry->dos_time);
    bx_ntvdm_search_result_u16(payload + 43u, entry->dos_date);
    bx_ntvdm_search_result_u32(payload + 47u, (uint32_t)entry->byte_count);
    if (!bx_ntvdm_multi_write_v1_add(&tx->writes, address + 1u, 19u, 0u) ||
        !bx_ntvdm_multi_write_v1_add(&tx->writes, address + 20u, 32u, 19u) ||
        !bx_ntvdm_search_result_finish(tx, boundary, cpu_before)) return 0;
    return 1;
}

int bx_ntvdm_search_result_v1_prepare_path_no_more(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    bx_ntvdm_multi_write_transaction_v1 *tx, uint8_t payload[8])
{
    /* Win32 ERROR_NO_MORE_FILES is the DOS-visible 0x12 used by the original
     * pathname FindFirst/FindNext path after it clears both DTA words. */
    if (tx == 0 || payload == 0 || dta_address > UINT64_MAX - 43u) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(tx, boundary, cpu_before);
    memset(payload, 0, 8u);
    if (!bx_ntvdm_multi_write_v1_add(&tx->writes, dta_address, 8u, 0u) ||
        !bx_ntvdm_search_result_finish(tx, boundary, cpu_before) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&tx->result.cpu_delta, 0u, 0x12u) ||
        !bx_ntvdm_cpu_result_v2_set_cf(&tx->result, 1)) return 0;
    return 1;
}
