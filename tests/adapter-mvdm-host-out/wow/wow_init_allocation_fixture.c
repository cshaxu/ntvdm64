/* Controlled allocation boundary around the selected original queue.c.
 * No original InitTask body or production allocator is replaced. */
#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_init_bindings.h"

unsigned wow_init_alloc_calls, wow_init_fail_at, wow_init_name_live;
unsigned wow_init_name_releases;
static PVOID app_name;

static PVOID WINAPI allocate_original(SIZE_T bytes)
{
    PVOID memory;
    ++wow_init_alloc_calls;
    if (wow_init_alloc_calls == wow_init_fail_at) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    memory = HeapAlloc(GetProcessHeap(), 0, bytes);
    if (wow_init_alloc_calls == 1 && memory) {
        app_name = memory;
        ++wow_init_name_live;
    }
    return memory;
}

BOOL WINAPI wow_init_fixture_free(HANDLE heap, DWORD flags, PVOID memory)
{
    BOOL result = HeapFree(heap, flags, memory);
    if (result && memory && memory == app_name) {
        app_name = NULL;
        --wow_init_name_live;
        ++wow_init_name_releases;
    }
    /* Prove rollback retains the initiating error, not cleanup's last error. */
    SetLastError(ERROR_BUSY);
    return result;
}

#undef UserAllocPoolWithQuota
#define UserAllocPoolWithQuota(bytes, tag) allocate_original(bytes)
#undef UserFreePool
#define UserFreePool(pointer) wow_init_fixture_free(GetProcessHeap(), 0, pointer)
#include "../../../src/opennt-host/windows/core/ntuser/kernel/queue.c"
