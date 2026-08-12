#include "bx_ntvdm_search_plan_v1.h"

#include <string.h>
#include <wctype.h>

static int bx_ntvdm_search_plan_wildcard(const wchar_t *pattern,
    const wchar_t *name)
{
    while (*pattern != L'\0') {
        wchar_t current = towupper((wint_t)*pattern++);
        if (current == L'*') {
            while (*pattern == L'*') ++pattern;
            if (*pattern == L'\0') return 1;
            while (*name != L'\0') {
                if (bx_ntvdm_search_plan_wildcard(pattern, name++)) return 1;
            }
            return bx_ntvdm_search_plan_wildcard(pattern, name);
        }
        if (*name == L'\0' || (current != L'?' &&
            current != towupper((wint_t)*name))) return 0;
        ++name;
    }
    return *name == L'\0';
}

static int bx_ntvdm_search_plan_valid_component(const wchar_t *text,
    int allow_wildcards)
{
    size_t index, base = 0u, extension = 0u;
    int dot = 0;
    if (text == 0 || text[0] == L'\0') return 0;
    for (index = 0u; text[index] != L'\0'; ++index) {
        wchar_t value = towupper((wint_t)text[index]);
        int allowed = (value >= L'A' && value <= L'Z') ||
            (value >= L'0' && value <= L'9') || value == L'$' ||
            value == L'%' || value == L'\'' || value == L'-' ||
            value == L'_' || value == L'@' || value == L'~' ||
            value == L'!' || value == L'#' || value == L'(' ||
            value == L')' || value == L'{' || value == L'}' || value == L'^';
        if (value == L'.') { if (dot) return 0; dot = 1; continue; }
        if (allow_wildcards && (value == L'*' || value == L'?')) allowed = 1;
        if (!allowed || value > 0x7fu) return 0;
        if (dot) { if (++extension > 3u) return 0; }
        else { if (++base > 8u) return 0; }
    }
    return base != 0u && (!dot || extension != 0u || allow_wildcards);
}

static int bx_ntvdm_search_plan_query_valid(const bx_ntvdm_search_query_v1 *query)
{
    const wchar_t *component;
    if (query == 0 || query->drive_index >= 26u ||
        wcsnlen(query->relative_directory,
            BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY + 1u) >
            BX_NTVDM_SEARCH_PLAN_V1_MAX_RELATIVE_DIRECTORY ||
        !bx_ntvdm_search_plan_valid_component(query->pattern, 1)) return 0;
    if ((query->attributes & 0x08u) != 0u) return 0; /* volume labels await trace */
    component = query->relative_directory;
    while (*component != L'\0') {
        const wchar_t *end = wcschr(component, L'\\');
        wchar_t name[13];
        size_t length = end == 0 ? wcslen(component) : (size_t)(end - component);
        if (length == 0u || length >= sizeof(name) / sizeof(name[0])) return 0;
        memcpy(name, component, length * sizeof(*name)); name[length] = L'\0';
        if (!bx_ntvdm_search_plan_valid_component(name, 0)) return 0;
        component = end == 0 ? component + length : end + 1u;
    }
    return 1;
}

static int bx_ntvdm_search_plan_visible(const bx_ntvdm_search_query_v1 *query,
    const bx_ntvdm_host_namespace_entry_v1 *entry)
{
    uint32_t special = entry->attributes & 0x16u;
    return (special & ~query->attributes) == 0u &&
        bx_ntvdm_search_plan_wildcard(query->pattern, entry->dos_name);
}

static int bx_ntvdm_search_plan_output(enum bx_ntvdm_search_plan_v1_form form,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t address,
    const bx_ntvdm_host_namespace_entry_v1 *entry,
    const bx_ntvdm_search_token_v1 *token,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    if (payload_bytes == 0) return 0;
    if (form == BX_NTVDM_SEARCH_PLAN_V1_PATH) {
        *payload_bytes = 30u;
        return bx_ntvdm_search_result_v1_prepare_path(boundary, cpu_before,
            address, entry, token, transaction, payload);
    }
    if (form == BX_NTVDM_SEARCH_PLAN_V1_FCB) {
        *payload_bytes = 51u;
        return bx_ntvdm_search_result_v1_prepare_fcb(boundary, cpu_before,
            address, entry, token, transaction, payload);
    }
    return 0;
}

void bx_ntvdm_search_plan_v1_initialize(bx_ntvdm_search_plan_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_SEARCH_PLAN_V1_MAGIC;
    value->version = BX_NTVDM_SEARCH_PLAN_V1_VERSION;
    bx_ntvdm_search_sessions_v1_initialize(&value->sessions);
}

void bx_ntvdm_search_plan_v1_release(bx_ntvdm_search_plan_v1 *value)
{
    if (value == 0) return;
    bx_ntvdm_search_sessions_v1_release(&value->sessions);
    memset(value, 0, sizeof(*value));
}

int bx_ntvdm_search_plan_v1_valid(const bx_ntvdm_search_plan_v1 *value)
{
    return value != 0 && value->magic == BX_NTVDM_SEARCH_PLAN_V1_MAGIC &&
        value->version == BX_NTVDM_SEARCH_PLAN_V1_VERSION &&
        bx_ntvdm_search_sessions_v1_valid(&value->sessions);
}

int bx_ntvdm_search_plan_v1_first_entries(bx_ntvdm_search_plan_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_query_v1 *query,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_host_namespace_entry_v1 *selected_entries, first;
    bx_ntvdm_search_token_v1 continuation;
    uint32_t selected = 0u, index;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_plan_v1_valid(value) ||
        (entries == 0 && entry_count != 0u) ||
        entry_count > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES || transaction == 0 ||
        payload == 0 || !bx_ntvdm_search_plan_query_valid(query))
        return BX_NTVDM_SEARCH_PLAN_V1_REJECTED;
    if (entry_count == 0u) return BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH;
    selected_entries = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        entry_count * sizeof(*selected_entries));
    if (selected_entries == 0) return BX_NTVDM_SEARCH_PLAN_V1_REJECTED;
    for (index = 0u; index < entry_count; ++index) {
        if (bx_ntvdm_search_plan_visible(query, &entries[index]))
            selected_entries[selected++] = entries[index];
    }
    if (selected == 0u) { HeapFree(GetProcessHeap(), 0u, selected_entries); return BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH; }
    if (!bx_ntvdm_search_sessions_v1_begin(&value->sessions,
        owner_pdb, selected_entries, selected, &first, &continuation) ||
        !bx_ntvdm_search_plan_output(form, boundary, cpu_before, output_address,
          &first, &continuation, transaction, payload, payload_bytes)) {
        bx_ntvdm_search_sessions_v1_cancel(&value->sessions, owner_pdb,
            &continuation);
        HeapFree(GetProcessHeap(), 0u, selected_entries); return BX_NTVDM_SEARCH_PLAN_V1_REJECTED;
    }
    HeapFree(GetProcessHeap(), 0u, selected_entries);
    return BX_NTVDM_SEARCH_PLAN_V1_OK;
}

int bx_ntvdm_search_plan_v1_next(bx_ntvdm_search_plan_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_token_v1 *presented,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_host_namespace_entry_v1 next;
    bx_ntvdm_search_token_v1 continuation;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_plan_v1_valid(value) || transaction == 0 || payload == 0 ||
        !bx_ntvdm_search_sessions_v1_next(&value->sessions, owner_pdb, presented,
            &next, &continuation)) return BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH;
    return bx_ntvdm_search_plan_output(form, boundary, cpu_before, output_address,
        &next, &continuation, transaction, payload, payload_bytes) ?
        BX_NTVDM_SEARCH_PLAN_V1_OK : BX_NTVDM_SEARCH_PLAN_V1_REJECTED;
}
