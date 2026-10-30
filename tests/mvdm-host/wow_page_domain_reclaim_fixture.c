/* Test-only failed decommit boundary. Compile the production page-domain
 * translation unit unchanged except for this one unavailable-operation leaf.
 * Successful frees still execute original nt_mem; no production fault hook. */
#include <windows.h>
#include <stdio.h>

extern LONG VdmFreeVirtualMemory(ULONG address);
static ULONG failed_address;
static unsigned failure_calls;

void __cdecl wow_fixture_fail_window_free(ULONG address)
{
    failed_address = address;
    failure_calls = 0;
}

unsigned __cdecl wow_fixture_window_free_failures(void)
{
    return failure_calls;
}

static LONG wow_fixture_free_memory(ULONG address)
{
    if (failed_address && address == failed_address) {
        ++failure_calls;
        return (LONG)0xc0000001u;
    }
    return VdmFreeVirtualMemory(address);
}

#define VdmFreeVirtualMemory wow_fixture_free_memory
#include "ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"
#undef VdmFreeVirtualMemory

/* Metadata-only index collision test: never dereference these synthetic guest
 * addresses. Real backing retirement/free failures are exercised separately. */
int __cdecl wow_fixture_window_index_collision(void)
{
    mvdm_softpc_wow_window_borrow *records[3] = {0};
    ULONG addresses[3] = {0}, address;
    unsigned count = 0, i, bucket = window_bucket(0xf0000000u);
    int passed = 0;
    for (address = 0xf0000000u; address < 0xff000000u && count < 3; address += 4096u) {
        if (window_bucket(address) != bucket || window_borrow_find(address)) continue;
        records[count] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*records[count]));
        if (!records[count]) goto done;
        records[count]->backing = addresses[count] = address;
        window_index_insert(records[count]);
        ++count;
    }
    if (count != 3) goto done;
    for (i = 0; i < 3; ++i)
        if (window_borrow_find(addresses[i]) != records[i]) goto done;
    dispose_window_borrow(records[1]); records[1] = NULL;
    if (window_borrow_find(addresses[1]) ||
            window_borrow_find(addresses[0]) != records[0] ||
            window_borrow_find(addresses[2]) != records[2]) goto done;
    records[1] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*records[1]));
    if (!records[1]) goto done;
    records[1]->backing = addresses[1];
    window_index_insert(records[1]);
    if (window_borrow_find(addresses[1]) != records[1]) goto done;
    passed = 1;
done:
    for (i = 0; i < 3; ++i) if (records[i]) dispose_window_borrow(records[i]);
    for (i = 0; i < count; ++i) if (window_borrow_find(addresses[i])) passed = 0;
    if (passed) fprintf(stderr, "WINDOW_INDEX_COLLISION exact=3 middle_remove=1 address_reuse=1 empty=1\n");
    return passed;
}

int __cdecl wow_fixture_class_index_fault(void)
{
    ULONG values[48] = {0}, first = 0, second = 0, third = 0, reuse = 0, i, j, k;
    mvdm_softpc_wow_class_allocation *held;
    for (i = 0; i < 48; ++i) {
        values[i] = mvdm_softpc_wow_page_domain_publish_named_class(0x123, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IndexFixture", 13);
        if (!values[i] || !page_domain_class_allocation(values[i])) return 0;
    }
    for (i = 0; i < 48 && !third; ++i) for (j = i + 1; j < 48 && !third; ++j)
        for (k = j + 1; k < 48; ++k)
            if ((((values[i] - domain.client_delta) >> 12u) ^ ((values[i] - domain.client_delta) >> 22u)) % 16u == (((values[j] - domain.client_delta) >> 12u) ^ ((values[j] - domain.client_delta) >> 22u)) % 16u &&
                (((values[i] - domain.client_delta) >> 12u) ^ ((values[i] - domain.client_delta) >> 22u)) % 16u == (((values[k] - domain.client_delta) >> 12u) ^ ((values[k] - domain.client_delta) >> 22u)) % 16u) {
                first = values[i]; second = values[j]; third = values[k]; break;
            }
    if (!third) return 0;
    held = page_domain_class_allocation(second);
    wow_fixture_fail_window_free(second - domain.client_delta);
    if (!held || mvdm_softpc_wow_page_domain_retire_class(second) ||
            wow_fixture_window_free_failures() != 1 || page_domain_class_allocation(second) != held ||
            !page_domain_class_allocation(first) || !page_domain_class_allocation(third)) return 0;
    wow_fixture_fail_window_free(0);
    if (!mvdm_softpc_wow_page_domain_retire_class(second) || page_domain_class_allocation(second) ||
            !page_domain_class_allocation(first) || !page_domain_class_allocation(third)) return 0;
    for (i = 0; i < 48; ++i) if (values[i] == second) { values[i] = 0; break; }
    reuse = mvdm_softpc_wow_page_domain_publish_named_class(0x123, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IndexFixture", 13);
    if (!reuse || !page_domain_class_allocation(reuse)) return 0;
    fprintf(stderr, "CLASS_INDEX_FAULT collisions=3 failure_retained=1 freed_missing=1 reused_address=%d\n", reuse == second);
    if (!mvdm_softpc_wow_page_domain_retire_class(reuse)) return 0;
    for (i = 0; i < 48; ++i) if (values[i] && !mvdm_softpc_wow_page_domain_retire_class(values[i])) return 0;
    return 1;
}

/* Session disposal owns final class storage. Test both the canonical list and
 * any derived lookup index when the latter is present. */
int __cdecl wow_fixture_class_storage_empty(void)
{
    if (class_allocations) return 0;
    if (window_borrows) return 0;
    {
        unsigned i;
        for (i = 0; i < WINDOW_INDEX_BUCKETS; ++i)
            if (window_index[i]) return 0;
    }
#ifdef CLASS_INDEX_BUCKETS
    {
        unsigned i;
        for (i = 0; i < CLASS_INDEX_BUCKETS; ++i)
            if (class_index[i]) return 0;
    }
#endif
    return 1;
}
