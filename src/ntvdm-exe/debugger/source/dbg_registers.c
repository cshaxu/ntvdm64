/*
 * CCPU40-only carrier retained beside the original mvdm/dbg/dbg.c body.
 *
 * The original debugger's ThreadSetDebugContext was a kernel VDM service.
 * Its six-DWORD DR0--DR3/DR6/DR7 order is retained here, while descriptor
 * decoding and guest-memory access stay bounded by the sole CCPU40 worker.
 */

#include <stdint.h>

#include "ntvdm-exe/softpc/include/mvdm_softpc_effective_address.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_addr.h>
#include <c_bsic.h>
#include <c_tlb.h>

extern uint16_t c_getSS(void);
extern uint16_t c_getSP(void);
extern void MOV_DR(ULONG index, ULONG value);
extern IBOOL c_getPG(void);
extern IBOOL c_getVM(void);
extern IUH c_getCPL(void);
extern IU32 c_sas_memory_size(void);
extern IU32 CCPU_DR[8];
extern BOOL debug_exception_pending;
extern void ccpu_restore_debug_context(const IU32 *registers, BOOL pending);
extern void ccpu_apply_debug_context(const IU32 *registers);

/* Original x86 monitor thread contexts are per thread; its DPMI debug
 * service broadcasts to every registered thread and supplies the initial
 * context of later threads. CCPU has one execution register bank, so retain
 * only the suspended thread contexts at this boundary. */
typedef struct debug_task_context {
    struct debug_task_context *next;
    DWORD thread_id;
    IU32 registers[6];
    BOOL pending;
    BOOL bootstrap;
} debug_task_context;

static SRWLOCK debug_context_lock = SRWLOCK_INIT;
static debug_task_context *debug_tasks;
static IU32 initial_debug_registers[6];
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
    ULONG index;
    for (index = 0; index != 4; ++index) task->registers[index] = CCPU_DR[index];
    task->registers[4] = CCPU_DR[6];
    task->registers[5] = CCPU_DR[7];
    task->pending = debug_exception_pending;
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
        if (new_task) ccpu_restore_debug_context(new_task->registers, new_task->pending);
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

static BOOL debug_readable_span(IU32 address, IU32 count, IUM8 access)
{
    IU32 i, physical, size = c_sas_memory_size();

    if (!count || address > UINT32_MAX - (count - 1)) return FALSE;
    for (i = 0; i < count; ++i) {
        physical = address + i;
        if (c_getPG() && !xtrn2phy(address + i, access, &physical)) return FALSE;
        if (physical >= size) return FALSE;
    }
    return TRUE;
}

BOOL mvdm_debugger_read_debug_registers(USHORT selector, USHORT offset,
    PULONG registers)
{
    IU32 descriptor_address, last = (IU32)offset + 23;
    CPU_DESCR descriptor;
    IUM8 access;

    if (registers == NULL || (selector & 0xfffc) == 0 ||
        selector_outside_GDT_LDT(selector, &descriptor_address) ||
        !debug_readable_span(descriptor_address, 8, 0)) return FALSE;
    read_descriptor_linear(descriptor_address, &descriptor);
    if ((descriptor.AR & 0x90) != 0x90 ||
        ((descriptor.AR & 8) && !(descriptor.AR & 2))) return FALSE;
    if ((descriptor.AR & 0x0c) == 4) {
        if (offset <= descriptor.limit ||
            last > ((descriptor.AR & 0x4000) ? UINT32_MAX : 0xffff)) return FALSE;
    } else if (last > descriptor.limit) return FALSE;
    if (descriptor.base > UINT32_MAX - last) return FALSE;
    access = c_getCPL() != 3 ? PG_U : 0;
    if (c_getVM()) access |= 4;
    if (!debug_readable_span(descriptor.base + offset, 24, access)) return FALSE;
    return mvdm_softpc_guest_memory_copy_from(descriptor.base + offset,
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
    ccpu_apply_debug_context(registers);
    ReleaseSRWLockExclusive(&debug_context_lock);
    return TRUE;
}
