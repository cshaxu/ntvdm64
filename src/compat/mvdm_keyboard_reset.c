#include "include/mvdm_cpu_access.h"

/* The original reset clears the 8042 queue but leaves a pending host IRQ.
 * At a DOS/native handoff that IRQ can replay a returned key into the next
 * DOS command. The standalone host owns the delayed IRQ and PIC carrier. */

enum { MVDM_MASTER_PIC = 0, MVDM_KEYBOARD_IRQ = 1 };

void mvdm_keyboard_reset_pending_irq(int pending)
{
    /* The caller has emptied the device queues. Cancel only the captured
     * user-originated output, not an internal controller response. */
    if (!pending) return;
    (void)mvdm_cpu_access_current()->delay_interrupt(MVDM_KEYBOARD_IRQ, 0, 0xffffffff);
    mvdm_cpu_access_current()->cancel_interrupt(MVDM_MASTER_PIC, MVDM_KEYBOARD_IRQ);
}
