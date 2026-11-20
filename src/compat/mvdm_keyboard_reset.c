#include "insignia.h"

/* The original reset clears the 8042 queue but leaves a pending host IRQ.
 * At a DOS/native handoff that IRQ can replay a returned key into the next
 * DOS command. The standalone host owns the delayed IRQ and PIC carrier. */

extern BOOL host_DelayHwInterrupt(int irq_line, int call_count, ULONG delay);
extern void ica_hw_interrupt_cancel(IU32 adapter, IU32 line_no);

enum { MVDM_MASTER_PIC = 0, MVDM_KEYBOARD_IRQ = 1 };

void mvdm_keyboard_reset_pending_irq(int pending)
{
    /* The caller has emptied the device queues. Cancel only the captured
     * user-originated output, not an internal controller response. */
    if (!pending) return;
    (void)host_DelayHwInterrupt(MVDM_KEYBOARD_IRQ, 0, 0xffffffff);
    ica_hw_interrupt_cancel(MVDM_MASTER_PIC, MVDM_KEYBOARD_IRQ);
}
