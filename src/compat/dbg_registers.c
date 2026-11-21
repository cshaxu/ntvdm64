/*
 * CCPU40-only carrier retained beside the original mvdm/dbg/dbg.c body.
 *
 * The original debugger's ThreadSetDebugContext was a kernel VDM service.
 * Its six-DWORD DR0--DR3/DR6/DR7 order is retained here, while descriptor
 * decoding and guest-memory access stay bounded by the sole CCPU40 worker.
 */

#include <windows.h>
#include <stdint.h>
#include "include/mvdm_cpu_access.h"
#include "include/guest_memory_access.h"

/* Original x86 monitor thread contexts are per thread; its DPMI debug
 * service broadcasts to every registered thread and supplies the initial
 * context of later threads. CCPU has one execution register bank, so retain
 * only the suspended thread contexts at this boundary. */
typedef struct debug_task_context {
    struct debug_task_context *next;
    DWORD thread_id;
    ULONG registers[6];
    BOOL pending;
    BOOL bootstrap;
} debug_task_context;

static SRWLOCK debug_context_lock = SRWLOCK_INIT;
static debug_task_context *debug_tasks;
static ULONG initial_debug_registers[6];
static BOOL initial_debug_active;
static BOOL bootstrap_debug_registered;

static debug_task_context *debug_task_find(DWORD thread_id)
{
    debug_task_context *task;
    for (task = debug_tasks; task; task = task->next)
        if (task->thread_id == thread_id) return task;
    return NULL;
}

static void debug_context_capture(debug_task_context *task)
{
    mvdm_cpu_access_current()->capture_debug(task->registers, &task->pending);
}

BOOL __cdecl mvdm_debug_task_register(DWORD thread_id)
{
    debug_task_context *task;
    DWORD saved = GetLastError();
    if (!thread_id) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    task = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*task));
    if (!task) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    task->thread_id = thread_id;
    AcquireSRWLockExclusive(&debug_context_lock);
    if (debug_task_find(thread_id)) {
        ReleaseSRWLockExclusive(&debug_context_lock);
        HeapFree(GetProcessHeap(), 0, task);
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if (!bootstrap_debug_registered) {
        debug_context_capture(task);
        task->bootstrap = TRUE;
        bootstrap_debug_registered = TRUE;
    }
    else if (initial_debug_active)
        CopyMemory(task->registers, initial_debug_registers, sizeof(task->registers));
    task->next = debug_tasks;
    debug_tasks = task;
    ReleaseSRWLockExclusive(&debug_context_lock);
    SetLastError(saved);
    return TRUE;
}

BOOL __cdecl mvdm_debug_task_exchange(DWORD previous, DWORD next)
{
    debug_task_context *old_task, *new_task;
    DWORD saved = GetLastError();
    AcquireSRWLockExclusive(&debug_context_lock);
    old_task = previous ? debug_task_find(previous) : NULL;
    new_task = next ? debug_task_find(next) : NULL;
    if ((previous && !old_task) || (next && !new_task)) {
        ReleaseSRWLockExclusive(&debug_context_lock);
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (previous != next) {
        if (old_task) debug_context_capture(old_task);
        if (new_task) mvdm_cpu_access_current()->restore_debug(new_task->registers, new_task->pending);
    }
    ReleaseSRWLockExclusive(&debug_context_lock);
    SetLastError(saved);
    return TRUE;
}

void __cdecl mvdm_debug_task_retire(DWORD thread_id, BOOL rollback)
{
    debug_task_context **link, *task = NULL;
    DWORD saved = GetLastError();
    AcquireSRWLockExclusive(&debug_context_lock);
    for (link = &debug_tasks; *link; link = &(*link)->next) {
        if ((*link)->thread_id == thread_id) {
            task = *link;
            *link = task->next;
            if (rollback && task->bootstrap && !debug_tasks)
                bootstrap_debug_registered = FALSE;
            break;
        }
    }
    ReleaseSRWLockExclusive(&debug_context_lock);
    if (task) HeapFree(GetProcessHeap(), 0, task);
    SetLastError(saved);
}

static BOOL debug_readable_span(ULONG address, ULONG count, unsigned char access)
{
    ULONG i, physical, size = mvdm_cpu_access_current()->memory_size();

    if (!count || address > UINT32_MAX - (count - 1)) return FALSE;
    for (i = 0; i < count; ++i) {
        physical = address + i;
        if (mvdm_cpu_access_current()->paging() && !mvdm_cpu_access_current()->translate(address + i, access, &physical)) return FALSE;
        if (physical >= size) return FALSE;
    }
    return TRUE;
}

BOOL mvdm_debugger_read_debug_registers(USHORT selector, USHORT offset,
    PULONG registers)
{
    ULONG descriptor_address, last = (ULONG)offset + 23;
    ULONG base, limit;
    USHORT rights;
    unsigned char access;

    if (registers == NULL || (selector & 0xfffc) == 0 ||
        mvdm_cpu_access_current()->selector_outside(selector, &descriptor_address) ||
        !debug_readable_span(descriptor_address, 8, 0)) return FALSE;
    mvdm_cpu_access_current()->descriptor_fields(descriptor_address,
        &base, &limit, &rights);
    if ((rights & 0x90) != 0x90 ||
        ((rights & 8) && !(rights & 2))) return FALSE;
    if ((rights & 0x0c) == 4) {
        if (offset <= limit ||
            last > ((rights & 0x4000) ? UINT32_MAX : 0xffff)) return FALSE;
    } else if (last > limit) return FALSE;
    if (base > UINT32_MAX - last) return FALSE;
    access = mvdm_cpu_access_current()->privilege() != 3 ? MVDM_CPU_ACCESS_USER : 0;
    if (mvdm_cpu_access_current()->virtual_mode()) access |= MVDM_CPU_ACCESS_V86;
    if (!debug_readable_span(base + offset, 24, access)) return FALSE;
    return mvdm_cpu_access_current()->read_memory(base + offset,
        (uint8_t *)registers, 6 * sizeof(*registers));
}

BOOL ThreadSetDebugContext(PULONG registers)
{
    debug_task_context *task;

    if (registers == NULL) return FALSE;
    AcquireSRWLockExclusive(&debug_context_lock);
    CopyMemory(initial_debug_registers, registers, sizeof(initial_debug_registers));
    initial_debug_active = (registers[5] & 0x0f) != 0;
    for (task = debug_tasks; task; task = task->next)
        CopyMemory(task->registers, registers, sizeof(task->registers));
    mvdm_cpu_access_current()->apply_debug(registers);
    ReleaseSRWLockExclusive(&debug_context_lock);
    return TRUE;
}
