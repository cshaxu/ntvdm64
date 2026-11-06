#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include "mvdm_vm_notification.h"
#include <windows.h>

/* Integration-only link seam. Compile the production notification object
 * with its BOP entry renamed to mvdm_vm_bridge_bop_direct, then link this
 * object before the production archives. No product build selects this file.
 * Only the witness's single targeted publish moves to a native thread; the
 * queue, ICA, PIC, CCPU and guest driver remain the real implementations. */
extern int mvdm_vm_bridge_bop_direct(uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t, int, mvdm_vm_bridge_result *);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern void ica_hw_interrupt_cancel(ULONG, ULONG);
extern BOOL RegisterEOIHook(int, void (*)(int, int));
extern BOOL RemoveEOIHook(int, void (*)(int, int));
extern WORD VDDReserveIrqLine(HANDLE, WORD);
extern BOOL VDDReleaseIrqLine(HANDLE, WORD);
static void (*notification_hook)(int, int);
static LONG cancelled;
static void observe_eoi(int irq, int count)
{
    if (count == -1) InterlockedIncrement(&cancelled);
    notification_hook(irq, count);
}
BOOL test_register_eoi(int irq, void (*hook)(int, int))
{
    if (!RegisterEOIHook(irq, observe_eoi)) return FALSE;
    notification_hook = hook;
    return TRUE;
}
BOOL test_remove_eoi(int irq, void (*hook)(int, int))
{
    if (hook != notification_hook || !RemoveEOIHook(irq, observe_eoi)) return FALSE;
    notification_hook = NULL;
    return TRUE;
}

static struct {
    uint32_t vm;
    uint64_t generation;
    HANDLE thread;
    WORD irq;
} producer;

static DWORD WINAPI publish(void *unused)
{
    uint64_t serial;
    DWORD error;
    LONG before;
    (void)unused;
    /* Force cancellation while the real IRQ is pending, before CCPU can
     * consume it. The original EOI cancellation hook must rearm the work. */
    host_ica_lock();
    before = cancelled;
    error = mvdm_vm_wake_publish(producer.vm, producer.generation, &serial);
    if (!error) ica_hw_interrupt_cancel(producer.irq >> 3, producer.irq & 7);
    if (!error && cancelled == before) error = ERROR_INVALID_STATE;
    host_ica_unlock();
    return error;
}

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result)
{
    DWORD error;
    uint64_t serial, generation = ((uint64_t)high << 32) | low;
    mvdm_vm_wake_identity abandoned;
    if (signature != MVDM_VM_BRIDGE_SIGNATURE || !protected_ring0)
        return mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, protected_ring0, result);
    if (operation == 8 && !mvdm_vm_notification_snapshot(&serial)) {
        /* The retirement witness has no other queued work. Create a pending
         * completion before the actual guest lifecycle call retires its VM;
         * it must disappear and later completion must be rejected. */
        host_ica_lock();
        if (mvdm_vm_wake_publish(vm, generation, &serial))
            ExitProcess(ERROR_INVALID_STATE);
        error = mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, protected_ring0, result);
        if (result->status ||
            mvdm_vm_wake_publish(vm, generation, &serial) != ERROR_NOT_FOUND ||
            mvdm_vm_wake_take(&abandoned) != ERROR_NO_MORE_ITEMS)
            ExitProcess(ERROR_INVALID_STATE);
        host_ica_unlock();
        return (int)error;
    }
    if (operation == 9) {
        ZeroMemory(result, sizeof(*result));
        if (producer.thread) {
            result->status = ERROR_ALREADY_EXISTS;
            return 1;
        }
        producer.vm = vm;
        producer.generation = ((uint64_t)high << 32) | low;
        producer.thread = CreateThread(NULL, 0, publish, NULL, 0, NULL);
        result->status = producer.thread ? ERROR_SUCCESS : GetLastError();
        return 1;
    }
    if (operation == 5 && producer.thread) {
        /* No join on the CPU thread. Guest normal exit occurs only after
         * the wake was consumed; also verify the producer has returned.
         * A live/failed producer is a failed witness, never a success. */
        if (WaitForSingleObject(producer.thread, 0) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(producer.thread, &error) || error)
            ExitProcess(ERROR_INVALID_STATE);
        CloseHandle(producer.thread);
        producer.thread = NULL;
    }
    error = mvdm_vm_bridge_bop_direct(signature, operation, low, high,
        vm, protected_ring0, result);
    if (operation == 0 && !result->status) producer.irq = (WORD)result->value;
    if (operation == 5 && !result->status) {
        host_ica_lock();
        if (VDDReserveIrqLine(&producer, producer.irq) != producer.irq ||
            !RegisterEOIHook(producer.irq, observe_eoi) ||
            !RemoveEOIHook(producer.irq, observe_eoi) ||
            !VDDReleaseIrqLine(&producer, producer.irq))
            ExitProcess(ERROR_INVALID_STATE);
        host_ica_unlock();
    }
    return (int)error;
}
