#include "include/mvdm_cpu_access.h"
#include "include/mvdm_vm_notification.h"

/* Original NT VDD/ICA ABI. Avoid importing historical CPU selection macros
 * into this host-owned component. Register/remove are serialized by ICA. */

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
        mvdm_cpu_access_current()->raise_interrupt(notification.irq >> 3, notification.irq & 7, 1);
    }
}

static void notification_eoi(int irq, int count)
{
    mvdm_cpu_access_current()->ica_lock();
    if (notification.open && irq == notification.irq) {
        /* Neither a physical EOI nor reset/cancel (-1) consumes host work.
         * A reset retires this IRQ assertion and must rearm pending work. */
        (void)count;
        notification.asserted = FALSE;
        raise_pending();
    }
    mvdm_cpu_access_current()->ica_unlock();
}

DWORD mvdm_vm_notification_open(WORD *irq)
{
    DWORD error = ERROR_SUCCESS;
    if (!irq) return ERROR_INVALID_PARAMETER;
    *irq = 0xffff;
    mvdm_cpu_access_current()->ica_lock();
    if (notification.open) error = ERROR_ALREADY_EXISTS;
    else {
        WORD line = mvdm_cpu_access_current()->reserve_irq(&notification, 0xffff);
        if (line == 0xffff) error = ERROR_BUSY;
        else if (!mvdm_cpu_access_current()->register_eoi(line, notification_eoi)) {
            if (!mvdm_cpu_access_current()->release_irq(&notification, line))
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
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_ready(void)
{
    DWORD error = ERROR_SUCCESS;
    mvdm_cpu_access_current()->ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else { notification.ready = TRUE; raise_pending(); }
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_post(uint64_t *serial)
{
    DWORD error = ERROR_SUCCESS;
    if (!serial) return ERROR_INVALID_PARAMETER;
    *serial = 0;
    mvdm_cpu_access_current()->ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else if (notification.published == UINT64_MAX) error = ERROR_ARITHMETIC_OVERFLOW;
    else { *serial = ++notification.published; raise_pending(); }
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_snapshot(uint64_t *serial)
{
    DWORD error = ERROR_SUCCESS;
    if (!serial) return ERROR_INVALID_PARAMETER;
    *serial = 0;
    mvdm_cpu_access_current()->ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else *serial = notification.published;
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_acknowledge(uint64_t serial)
{
    DWORD error = ERROR_SUCCESS;
    mvdm_cpu_access_current()->ica_lock();
    if (!notification.open) error = ERROR_NOT_READY;
    else if (serial < notification.consumed || serial > notification.published)
        error = ERROR_INVALID_DATA;
    else notification.consumed = serial;
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}

DWORD mvdm_vm_notification_close(void)
{
    DWORD error = ERROR_SUCCESS;
    mvdm_cpu_access_current()->ica_lock();
    if (notification.open) {
        notification.ready = FALSE; /* cancel callback must not rearm */
        mvdm_cpu_access_current()->cancel_interrupt(notification.irq >> 3, notification.irq & 7);
        if (!mvdm_cpu_access_current()->remove_eoi(notification.irq, notification_eoi))
            error = ERROR_INVALID_STATE;
        else if (!mvdm_cpu_access_current()->release_irq(&notification, notification.irq))
            error = ERROR_INVALID_STATE;
        else notification.open = notification.asserted = FALSE;
    }
    mvdm_cpu_access_current()->ica_unlock();
    return error;
}
