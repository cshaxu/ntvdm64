#include "mvdm_vm_notification.h"
#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include "mvdm_vm_continuation.h"
#include "mvdm_vm_ingress.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"

/* Original NT VDD/ICA ABI. Avoid importing historical CPU selection macros
 * into this host-owned component. Register/remove are serialized by ICA. */
extern WORD VDDReserveIrqLine(HANDLE, WORD);
extern BOOL VDDReleaseIrqLine(HANDLE, WORD);
extern BOOL RegisterEOIHook(int, void (*)(int, int));
extern BOOL RemoveEOIHook(int, void (*)(int, int));
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern void ica_hw_interrupt(ULONG, ULONG, LONG);
extern void ica_hw_interrupt_cancel(ULONG, ULONG);

static struct {
    WORD irq;
    BOOL open, ready, asserted;
    uint64_t published, consumed;
} notification;

/* ICA is recursive and excludes device posts, EOI, reset and release. */
static void raise_pending(void)
{
    if (notification.ready && !notification.asserted &&
        notification.published != notification.consumed) {
        notification.asserted = TRUE;
        ica_hw_interrupt(notification.irq >> 3, notification.irq & 7, 1);
    }
}

static void notification_eoi(int irq, int count)
{
    host_ica_lock();
    if (notification.open && irq == notification.irq) {
        /* Neither a physical EOI nor reset/cancel (-1) consumes host work.
         * A reset retires this IRQ assertion and must rearm pending work. */
        (void)count;
        notification.asserted = FALSE;
        raise_pending();
    }
    host_ica_unlock();
}

DWORD mvdm_vm_notification_open(WORD *irq)
{
    DWORD error = ERROR_SUCCESS;
    if (!irq) return ERROR_INVALID_PARAMETER;
    *irq = 0xffff;
    host_ica_lock();
    if (notification.open) error = ERROR_ALREADY_EXISTS;
    else {
        WORD line = VDDReserveIrqLine(&notification, 0xffff);
        if (line == 0xffff) error = ERROR_BUSY;
        else if (!RegisterEOIHook(line, notification_eoi)) {
            if (!VDDReleaseIrqLine(&notification, line))
                error = ERROR_INVALID_STATE;
            else error = ERROR_BUSY;
        } else {
            notification.irq = line;
            notification.open = TRUE;
            notification.ready = notification.asserted = FALSE;
            notification.consumed = notification.published;
            *irq = line;
        }
    }
    host_ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_ready(void)
{
    DWORD error = ERROR_SUCCESS;
    host_ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else { notification.ready = TRUE; raise_pending(); }
    host_ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_post(uint64_t *serial)
{
    DWORD error = ERROR_SUCCESS;
    if (!serial) return ERROR_INVALID_PARAMETER;
    *serial = 0;
    host_ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else if (notification.published == UINT64_MAX) error = ERROR_ARITHMETIC_OVERFLOW;
    else { *serial = ++notification.published; raise_pending(); }
    host_ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_snapshot(uint64_t *serial)
{
    DWORD error = ERROR_SUCCESS;
    if (!serial) return ERROR_INVALID_PARAMETER;
    *serial = 0;
    host_ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else *serial = notification.published;
    host_ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_acknowledge(uint64_t serial)
{
    DWORD error = ERROR_SUCCESS;
    host_ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else if (serial < notification.consumed || serial > notification.published)
        error = ERROR_INVALID_DATA;
    else notification.consumed = serial;
    host_ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_close(void)
{
    DWORD error = ERROR_SUCCESS;
    host_ica_lock();
    if (notification.open) {
        notification.ready = FALSE; /* cancel callback must not rearm */
        ica_hw_interrupt_cancel(notification.irq >> 3, notification.irq & 7);
        if (!RemoveEOIHook(notification.irq, notification_eoi))
            error = ERROR_INVALID_STATE;
        else if (!VDDReleaseIrqLine(&notification, notification.irq))
            error = ERROR_INVALID_STATE;
        else notification.open = notification.asserted = FALSE;
    }
    host_ica_unlock();
    return error;
}

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int protected_ring0,
    mvdm_vm_bridge_result *result)
{
    uint64_t serial = ((uint64_t)high << 32) | low;
    WORD irq = 0xffff;
    mvdm_vm_wake_identity wake;
    if (signature != MVDM_VM_BRIDGE_SIGNATURE) return 0;
    result->value = 0xffff;
    result->serial_low = result->serial_high = 0;
    result->status = ERROR_ACCESS_DENIED;
    if (!protected_ring0) return 1;
    switch (operation) {
    case 0: result->status = mvdm_vm_notification_open(&irq); break;
    case 1: result->status = mvdm_vm_notification_ready(); break;
    case 2: result->status = mvdm_vm_notification_post(&serial); break;
    case 3: result->status = mvdm_vm_notification_snapshot(&serial); break;
    case 4: result->status = mvdm_vm_notification_acknowledge(serial); break;
    case 5:
        mvdm_vm_ingress_reset();
        host_ica_lock();
        result->status = mvdm_vm_notification_close();
        if (!result->status) {
            mvdm_vm_continuation_reset();
            mvdm_vm_wake_reset();
        }
        host_ica_unlock();
        break;
    case 7: result->status = mvdm_vm_wake_register(vm, serial); break;
    case 8:
        host_ica_lock();
        result->status = mvdm_vm_wake_retire(vm, serial);
        if (!result->status) mvdm_vm_continuation_retire(vm, serial);
        host_ica_unlock();
        break;
    case 9: result->status = mvdm_vm_wake_publish(vm, serial, &serial); break;
    case 10:
        result->status = mvdm_vm_wake_take(&wake);
        if (!result->status) {
            result->value = wake.vm;
            result->serial_low = (uint32_t)wake.generation;
            result->serial_high = (uint32_t)(wake.generation >> 32);
        }
        return 1;
    case 11:
        result->status = mvdm_vm_command_admit(vm, low, high, &serial);
        result->value = (uint32_t)serial;
        result->serial_low = (uint32_t)(serial >> 32);
        return 1;
    case 15:
        result->status = high ? ERROR_INVALID_PARAMETER : mvdm_vm_ingress_register(low);
        break;
    case 16:
        result->status = mvdm_vm_executor_key_bop(mvdm_standalone_worker_executor(),
            vm, low, high);
        break;
    case 12: {
        mvdm_vm_completion completion;
        result->value = 0;
        result->status = mvdm_vm_continuation_consume(serial, vm, &completion);
        if (!result->status) {
            result->status = completion.error;
            result->value = completion.exit_code;
            result->serial_low = completion.reentry;
        }
        return 1;
    }
    case 13: case 14:
        result->status = mvdm_vm_application_bop(operation, vm, low, high);
        break;
    default: result->status = ERROR_INVALID_FUNCTION; break;
    }
    if (!result->status) {
        result->value = irq;
        if (operation == 2 || operation == 3 || operation == 9) {
            result->serial_low = (uint32_t)serial;
            result->serial_high = (uint32_t)(serial >> 32);
        }
    }
    return 1;
}
