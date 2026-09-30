#include "insignia.h"
#include "ica.h"
#include "nt_eoi.h"
#include "include/mvdm_keyboard_history.h"

/* The original reset clears the 8042 queue but leaves a pending host IRQ.
 * At a DOS/native handoff that IRQ can replay a returned key into the next
 * DOS command. The standalone host owns the delayed IRQ and PIC carrier. */
extern int output_full;
extern mvdm_keyboard_history nt_keyboard_history;

enum { MVDM_MASTER_PIC = 0, MVDM_KEYBOARD_IRQ = 1 };

void mvdm_keyboard_reset_pending_irq(void)
{
    /* A filled 8042 slot can also be an internal device response. Only a
     * user-originated output is being returned to the native input owner. */
    if (!output_full || !nt_keyboard_history.output) return;
    (void)host_DelayHwInterrupt(MVDM_KEYBOARD_IRQ, 0, 0xffffffff);
    ica_hw_interrupt_cancel(MVDM_MASTER_PIC, MVDM_KEYBOARD_IRQ);
}
