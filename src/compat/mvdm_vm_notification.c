#include "include/mvdm_vm_notification.h"

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
