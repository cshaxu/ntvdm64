#include "bx_ntvdm_search_transaction_v1.h"

#include <string.h>

static int bx_ntvdm_search_transaction_v1_direct_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    enum bx_ntvdm_search_plan_v1_form form, uint64_t output_address,
    const bx_ntvdm_search_query_v1 *query,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_host_namespace_entry_v1 *entries;
    uint32_t count = 0u;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) ||
        !bx_ntvdm_host_namespace_v1_valid(space) || query == 0 ||
        transaction == 0 || payload == 0 || query->drive_index >= 26u) return -1;
    entries = (bx_ntvdm_host_namespace_entry_v1 *)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES * sizeof(*entries));
    if (entries == 0) return -1;
    if (bx_ntvdm_host_namespace_v1_enumerate(space, query->drive_index,
            query->relative_directory, entries,
            BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES, &count) !=
        BX_NTVDM_HOST_NAMESPACE_V1_OK) {
        HeapFree(GetProcessHeap(), 0u, entries);
        return -1;
    }
    result = bx_ntvdm_search_plan_v1_first_entries(&value->plan, entries, count,
        boundary, cpu_before, owner_pdb, form, output_address, query,
        transaction, payload, payload_bytes);
    HeapFree(GetProcessHeap(), 0u, entries);
    return result;
}

static int merged_first(bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t output_address, const bx_ntvdm_search_query_v1 *query,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_host_namespace_entry_v1 *entries; uint32_t count = 0u, i, j;
    int result;
    if (!snapshot || !bx_ntvdm_profile_search_snapshot_v1_valid(snapshot) ||
        snapshot->drive_index != query->drive_index || query->relative_directory[0] != L'\0')
        return bx_ntvdm_search_transaction_v1_direct_first(value, space, boundary,
            cpu_before, owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH, output_address,
            query, transaction, payload, payload_bytes);
    entries = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES * sizeof(*entries));
    if (!entries || bx_ntvdm_host_namespace_v1_enumerate(space, query->drive_index,
        query->relative_directory, entries, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES,
        &count) != BX_NTVDM_HOST_NAMESPACE_V1_OK) { if (entries) HeapFree(GetProcessHeap(),0,entries); return -1; }
    for (i=0u;i<snapshot->entry_count;++i) { for (j=0u;j<count&&_wcsicmp(entries[j].dos_name,snapshot->entries[i].dos_name);++j) {} if (j<count) entries[j]=snapshot->entries[i]; else { if(count==BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES){HeapFree(GetProcessHeap(),0,entries);return -1;} entries[count++]=snapshot->entries[i]; } }
    for (i=1u;i<count;++i) { bx_ntvdm_host_namespace_entry_v1 item=entries[i]; for(j=i;j&&_wcsicmp(entries[j-1u].dos_name,item.dos_name)>0;--j) entries[j]=entries[j-1u]; entries[j]=item; }
    result=bx_ntvdm_search_plan_v1_first_entries(&value->plan,entries,count,boundary,cpu_before,owner_pdb,BX_NTVDM_SEARCH_PLAN_V1_PATH,output_address,query,transaction,payload,payload_bytes); HeapFree(GetProcessHeap(),0,entries); return result;
}

static int bx_ntvdm_search_transaction_path_no_more(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint64_t dta_address,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    if (payload_bytes == 0 || !bx_ntvdm_search_result_v1_prepare_path_no_more(
        boundary, cpu_before, dta_address, transaction, payload)) return -1;
    *payload_bytes = 8u;
    return 0;
}

void bx_ntvdm_search_transaction_v1_initialize(
    bx_ntvdm_search_transaction_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_SEARCH_TRANSACTION_V1_MAGIC;
    value->version = BX_NTVDM_SEARCH_TRANSACTION_V1_VERSION;
    bx_ntvdm_search_plan_v1_initialize(&value->plan);
}

void bx_ntvdm_search_transaction_v1_release(
    bx_ntvdm_search_transaction_v1 *value)
{
    if (value == 0) return;
    bx_ntvdm_search_plan_v1_release(&value->plan);
    memset(value, 0, sizeof(*value));
}

int bx_ntvdm_search_transaction_v1_valid(
    const bx_ntvdm_search_transaction_v1 *value)
{
    return value != 0 && value->magic == BX_NTVDM_SEARCH_TRANSACTION_V1_MAGIC &&
        value->version == BX_NTVDM_SEARCH_TRANSACTION_V1_VERSION &&
        bx_ntvdm_search_plan_v1_valid(&value->plan);
}

int bx_ntvdm_search_transaction_v1_path_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t path[128], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_query_v1 query;
    int result;
    if (!bx_ntvdm_search_request_v1_decode_first_path(path, attributes, &query)) {
        if (payload_bytes != 0) *payload_bytes = 0u;
        return -1;
    }
    result = bx_ntvdm_search_transaction_v1_direct_first(value, space, boundary,
        cpu_before, owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH, dta_address, &query,
        transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes) : result ==
        BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_path_first_merged(
    bx_ntvdm_search_transaction_v1 *value,const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,uint32_t owner_pdb,uint64_t dta_address,
    const uint8_t path[128],uint16_t attributes,bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51],uint32_t *payload_bytes)
{ bx_ntvdm_search_query_v1 query;int result;if(!bx_ntvdm_search_request_v1_decode_first_path(path,attributes,&query)){if(payload_bytes)*payload_bytes=0u;return -1;} result=merged_first(value,space,snapshot,boundary,cpu_before,owner_pdb,dta_address,&query,transaction,payload,payload_bytes);return result==BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH?bx_ntvdm_search_transaction_path_no_more(boundary,cpu_before,dta_address,transaction,payload,payload_bytes):result==BX_NTVDM_SEARCH_PLAN_V1_OK?1:-1; }

int bx_ntvdm_search_transaction_v1_path_first_entries(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    uint8_t admitted_drive_index,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t path[128], uint16_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_query_v1 query;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0 ||
        !bx_ntvdm_search_request_v1_decode_first_path(path, attributes, &query) ||
        query.drive_index != admitted_drive_index)
        return -1;
    result = bx_ntvdm_search_plan_v1_first_entries(&value->plan, entries, entry_count,
        boundary, cpu_before, owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH,
        dta_address, &query, transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes) : result ==
        BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_path_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t dta_address, const uint8_t dta[43],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_token_v1 token;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0) return -1;
    if (!bx_ntvdm_search_request_v1_decode_next_path(dta, &token))
        return bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes);
    result = bx_ntvdm_search_plan_v1_next(&value->plan, boundary, cpu_before,
        owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_PATH, dta_address, &token,
        transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_transaction_path_no_more(boundary, cpu_before,
            dta_address, transaction, payload, payload_bytes) : result ==
        BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_fcb_first(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t address, const uint8_t path[128], uint8_t extended_fcb,
    uint8_t attributes, bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51], uint32_t *payload_bytes)
{
    bx_ntvdm_search_query_v1 query;
    int result;
    if (!bx_ntvdm_search_request_v1_decode_first_fcb(path, extended_fcb,
            attributes, &query)) {
        if (payload_bytes != 0) *payload_bytes = 0u;
        return -1;
    }
    result = bx_ntvdm_search_transaction_v1_direct_first(value, space, boundary,
        cpu_before, owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_FCB, address, &query,
        transaction, payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_result_v1_prepare_fcb_no_more(boundary, cpu_before,
            address, transaction, payload) ? (*payload_bytes = 8u, 0) : -1 :
        result == BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_fcb_first_merged(
    bx_ntvdm_search_transaction_v1 *value,const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,uint32_t owner_pdb,uint64_t address,
    const uint8_t path[128],uint8_t extended_fcb,uint8_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction,uint8_t payload[51],uint32_t *payload_bytes)
{ bx_ntvdm_search_query_v1 query; bx_ntvdm_host_namespace_entry_v1 *entries; uint32_t count=0u,i,j; int result;
  if(!bx_ntvdm_search_request_v1_decode_first_fcb(path,extended_fcb,attributes,&query)){if(payload_bytes)*payload_bytes=0u;return -1;}
  if(!snapshot||!bx_ntvdm_profile_search_snapshot_v1_valid(snapshot)||snapshot->drive_index!=query.drive_index||query.relative_directory[0]!=L'\0') return bx_ntvdm_search_transaction_v1_fcb_first(value,space,boundary,cpu_before,owner_pdb,address,path,extended_fcb,attributes,transaction,payload,payload_bytes);
  entries=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES*sizeof(*entries)); if(!entries||bx_ntvdm_host_namespace_v1_enumerate(space,query.drive_index,query.relative_directory,entries,BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES,&count)!=BX_NTVDM_HOST_NAMESPACE_V1_OK){if(entries)HeapFree(GetProcessHeap(),0,entries);return -1;}
  for(i=0u;i<snapshot->entry_count;++i){for(j=0u;j<count&&_wcsicmp(entries[j].dos_name,snapshot->entries[i].dos_name);++j){}if(j<count)entries[j]=snapshot->entries[i];else{if(count==BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES){HeapFree(GetProcessHeap(),0,entries);return -1;}entries[count++]=snapshot->entries[i];}}
  for(i=1u;i<count;++i){bx_ntvdm_host_namespace_entry_v1 item=entries[i];for(j=i;j&&_wcsicmp(entries[j-1u].dos_name,item.dos_name)>0;--j)entries[j]=entries[j-1u];entries[j]=item;}
  result=bx_ntvdm_search_plan_v1_first_entries(&value->plan,entries,count,boundary,cpu_before,owner_pdb,BX_NTVDM_SEARCH_PLAN_V1_FCB,address,&query,transaction,payload,payload_bytes);HeapFree(GetProcessHeap(),0,entries);return result==BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH?bx_ntvdm_search_result_v1_prepare_fcb_no_more(boundary,cpu_before,address,transaction,payload)?(*payload_bytes=8u,0):-1:result==BX_NTVDM_SEARCH_PLAN_V1_OK?1:-1; }

int bx_ntvdm_search_transaction_v1_fcb_first_entries(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_count,
    uint8_t admitted_drive_index,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t searchbuf_address,
    const uint8_t path[128], uint8_t extended_fcb, uint8_t attributes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_query_v1 query;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0 ||
        !bx_ntvdm_search_request_v1_decode_first_fcb(path, extended_fcb,
            attributes, &query) || query.drive_index != admitted_drive_index) return -1;
    result = bx_ntvdm_search_plan_v1_first_entries(&value->plan, entries,
        entry_count, boundary, cpu_before, owner_pdb,
        BX_NTVDM_SEARCH_PLAN_V1_FCB, searchbuf_address, &query, transaction,
        payload, payload_bytes);
    /* demFindFirstFCB delegates a no-match to demClientError after
     * SearchFile.  The DOS-visible search error is ERROR_NO_MORE_FILES,
     * with the reserved FCB continuation words already clear.  Do not turn
     * this legitimate result into the package's unrelated AX=5 fallback. */
    return result == BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ?
        bx_ntvdm_search_result_v1_prepare_fcb_no_more(boundary, cpu_before,
            searchbuf_address, transaction, payload) ? (*payload_bytes = 8u, 0) : -1 :
        result == BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 : -1;
}

int bx_ntvdm_search_transaction_v1_fcb_next(
    bx_ntvdm_search_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t owner_pdb,
    uint64_t address, const uint8_t searchbuf[53],
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes)
{
    bx_ntvdm_search_token_v1 token;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_search_transaction_v1_valid(value) || payload_bytes == 0 ||
        searchbuf == 0) return -1;
    if (!bx_ntvdm_search_request_v1_decode_next_fcb(searchbuf, &token))
        return bx_ntvdm_search_result_v1_prepare_fcb_no_more(boundary,
            cpu_before, address, transaction, payload) ? (*payload_bytes = 8u, 0) : -1;
    result = bx_ntvdm_search_plan_v1_next(&value->plan, boundary, cpu_before,
        owner_pdb, BX_NTVDM_SEARCH_PLAN_V1_FCB, address, &token, transaction,
        payload, payload_bytes);
    return result == BX_NTVDM_SEARCH_PLAN_V1_OK ? 1 :
        bx_ntvdm_search_result_v1_prepare_fcb_no_more(boundary, cpu_before,
            address, transaction, payload) ? (*payload_bytes = 8u, 0) : -1;
}
