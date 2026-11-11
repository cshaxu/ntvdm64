/* Link the production PIC owner.  The callback below is the deliberately
 * narrow host boundary: it records the actual reset cancellation notification
 * and may issue one new IRQ exactly as a real device callback is allowed to
 * do.  It does not implement PIC state, timer policy, or a synthetic EOI. */
#include <stdio.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <ica.h>
#include <cmos.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void);
extern void ica0_init(void), ica1_init(void);
extern void ica_outb(io_addr, IU8), ica_hw_interrupt(IU32, IU32, IS32);
extern VDMVIRTUALICA VirtualIca[2];
extern ULONG EoiPending, EoiIntsPending;
extern void TimerEoiHook(int, int);
extern void RtcIntEoiHook(int, int), MouseEoiHook(int, int);
extern BOOL RtcEoiPending;
extern int RtcLastAlarm, MouseEBufNxtEvtInx, MouseEBufNxtFreeInx;
extern unsigned char PendingCReg;
extern IU32 rtc_period_mSeconds;
extern half_word cmos[];
extern BOOLEAN bMseEoiPending;
extern ULONG MseIntLazyCount;
extern BOOL MoreMouseEvents(void);

static unsigned assertions, failures, cancel_count[16];
static int requeue_irq = -1;
static int use_timer_eoi_hook;
static int use_rtc_eoi_hook, use_mouse_eoi_hook;
static unsigned delayed[16];
static ULONG last_delay;
#define CHECK(c) do { ++assertions; if (!(c)) { ++failures; \
    fprintf(stderr, "FAIL line %u: %s\n", (unsigned)__LINE__, #c); } } while (0)

void host_EOI_hook(int irq, int count)
{
    if (irq >= 0 && irq < 16 && count == -1) ++cancel_count[irq];
    if (irq == 0 && use_timer_eoi_hook) TimerEoiHook(irq, count);
    if (irq == 8 && use_rtc_eoi_hook) RtcIntEoiHook(irq, count);
    if (irq == 9 && use_mouse_eoi_hook) MouseEoiHook(irq, count);
    if (irq == requeue_irq && count == -1) {
        requeue_irq = -1;
        ica_hw_interrupt((IU32)(irq >> 3), (IU32)(irq & 7), 1);
    }
}

ULONG host_DelayHwInterrupt(int irq, int count, ULONG delay)
{
    if (irq >= 0 && irq < 16) delayed[irq] += (unsigned)count;
    last_delay = delay;
    return 0;
}
void HostIdleNoActivity(void) { }

static void initialize_pic(IU16 master_icw4, IU16 slave_icw4)
{
    ica_outb(0x20, 0x11); ica_outb(0x21, 0x08); ica_outb(0x21, 0x04);
    ica_outb(0x21, (IU8)master_icw4); ica_outb(0x21, 0xff);
    ica_outb(0xa0, 0x11); ica_outb(0xa1, 0x70); ica_outb(0xa1, 0x02);
    ica_outb(0xa1, (IU8)slave_icw4); ica_outb(0xa1, 0xff);
}

static void reset_counts(void)
{
    int index;
    for (index = 0; index < 16; ++index) cancel_count[index] = delayed[index] = 0;
    requeue_irq = -1;
    use_timer_eoi_hook = 0;
    use_rtc_eoi_hook = use_mouse_eoi_hook = 0;
    last_delay = 0;
}

int main(void)
{
    session owner;
    session_initialize(&owner, 1);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000); c_cpu_init(); ica0_init(); ica1_init();
    initialize_pic(1, 1);

    /* IRQ0 enters service, then IRQ1 remains pending: ICW1 must cancel both
     * distinct states exactly once. */
    /* Establish the two architectural ownership states directly.  INTACK
     * delivery itself is covered by keyboard-controller-pic-test; this test
     * isolates what ICW1 retires once those states already exist. */
    VirtualIca[ICA_MASTER].ica_isr = 1;
    VirtualIca[ICA_MASTER].ica_irr = 2;
    VirtualIca[ICA_MASTER].ica_count[1] = 1;
    CHECK((VirtualIca[ICA_MASTER].ica_isr & 1) != 0);
    CHECK((VirtualIca[ICA_MASTER].ica_irr & 2) != 0);
    reset_counts(); use_timer_eoi_hook = 1;
    EoiPending = 1; EoiIntsPending = 7;
    ica_outb(0x20, 0x11);
    CHECK(cancel_count[0] == 1 && cancel_count[1] == 1);
    CHECK(EoiPending == 0 && EoiIntsPending == 0);
    CHECK(VirtualIca[ICA_MASTER].ica_irr == 0 &&
          VirtualIca[ICA_MASTER].ica_isr == 0 &&
          VirtualIca[ICA_MASTER].ica_count[0] == 0 &&
          VirtualIca[ICA_MASTER].ica_count[1] == 0);
    fprintf(stderr, "PASS PIC reset retires pending and in-service master ownership\n");

    /* Empty and repeated resets do not fabricate cancellation notifications. */
    reset_counts();
    ica_outb(0x20, 0x11); ica_outb(0x20, 0x11);
    CHECK(cancel_count[0] == 0 && cancel_count[1] == 0);

    initialize_pic(1, 1);
    /* A slave's state belongs to the slave reset. The master cascade state
     * remains untouched until its own ICW1, as on two independently reset
     * controllers. */
    VirtualIca[ICA_SLAVE].ica_irr = 1; VirtualIca[ICA_SLAVE].ica_count[0] = 1;
    VirtualIca[ICA_MASTER].ica_isr |= (1 << 2);
    reset_counts();
    ica_outb(0xa0, 0x11);
    CHECK(cancel_count[8] == 1 && cancel_count[2] == 0);
    CHECK(VirtualIca[ICA_SLAVE].ica_irr == 0 && VirtualIca[ICA_SLAVE].ica_isr == 0);
    CHECK((VirtualIca[ICA_MASTER].ica_isr & (1 << 2)) != 0);
    ica_outb(0x20, 0x11);
    CHECK(cancel_count[2] == 1 && VirtualIca[ICA_MASTER].ica_isr == 0);
    fprintf(stderr, "PASS PIC reset retains controller-local cascade ownership\n");

    initialize_pic(1, 1);
    VirtualIca[ICA_MASTER].ica_irr = (1 << 3);
    VirtualIca[ICA_MASTER].ica_count[3] = 1;
    reset_counts(); requeue_irq = 3;
    ica_outb(0x20, 0x11);
    CHECK(cancel_count[3] == 1 && (VirtualIca[ICA_MASTER].ica_irr & (1 << 3)) != 0);
    ica_outb(0x20, 0x11);
    CHECK(cancel_count[3] == 2 && VirtualIca[ICA_MASTER].ica_irr == 0);
    fprintf(stderr, "PASS PIC reset distinguishes callback-requeued ownership\n");

    initialize_pic(3, 1); /* ICW4 AEOI: no in-service ownership persists. */
    CHECK((VirtualIca[ICA_MASTER].ica_mode & ICA_AEOI) != 0);
    reset_counts(); ica_outb(0x20, 0x11);
    CHECK(cancel_count[0] == 0);
    fprintf(stderr, "PASS PIC automatic EOI leaves no stale reset ownership\n");

    /* Real RTC cancellation releases its gate. With periodic delivery still
     * enabled, the original callback legitimately schedules a NEW delayed
     * request: reset must not cancel this post-snapshot device ownership. */
    initialize_pic(1, 1);
    cmos[CMOS_REG_B] = 0; RtcLastAlarm = 0; PendingCReg = 0;
    RtcEoiPending = TRUE;
    VirtualIca[ICA_SLAVE].ica_irr = 1;
    reset_counts(); use_rtc_eoi_hook = 1;
    ica_outb(0xa0, 0x11);
    CHECK(cancel_count[8] == 1 && !RtcEoiPending);
    CHECK(delayed[8] == 0);
    CHECK((VirtualIca[ICA_SLAVE].ica_irr & 1) == 0);
    initialize_pic(1, 1);
    cmos[CMOS_REG_B] = PIE; rtc_period_mSeconds = 976;
    RtcEoiPending = TRUE; PendingCReg = 0;
    VirtualIca[ICA_SLAVE].ica_isr = 1;
    reset_counts(); use_rtc_eoi_hook = 1;
    ica_outb(0xa0, 0x11);
    CHECK(cancel_count[8] == 1 && RtcEoiPending);
    CHECK(delayed[8] == 1 && last_delay == 976);
    CHECK((cmos[CMOS_REG_C] & C_PF) != 0 && PendingCReg == 0);
    cmos[CMOS_REG_B] = 0; RtcEoiPending = FALSE;
    fprintf(stderr, "PASS actual RTC reset cancellation and periodic requeue\n");

    /* Real mouse cancellation flushes both its queue and lazy EOI backlog;
     * it is deliberately NOT the normal EOI path that schedules more input. */
    initialize_pic(1, 1);
    bMseEoiPending = TRUE; MseIntLazyCount = 3;
    MouseEBufNxtEvtInx = 0; MouseEBufNxtFreeInx = 1;
    VirtualIca[ICA_SLAVE].ica_irr = 2;
    reset_counts(); use_mouse_eoi_hook = 1;
    ica_outb(0xa0, 0x11);
    CHECK(cancel_count[9] == 1 && !bMseEoiPending && MseIntLazyCount == 0);
    CHECK(MouseEBufNxtEvtInx == 0 && MouseEBufNxtFreeInx == 0 && !MoreMouseEvents());
    CHECK(delayed[9] == 0);
    ica_outb(0xa0, 0x11);
    CHECK(cancel_count[9] == 1);
    fprintf(stderr, "PASS actual mouse reset cancellation flushes without requeue\n");

    sas_term(); (void)session_thread_unbind(&owner); (void)session_dispose(&owner);
    fprintf(stderr, "PIC_RESET_EOI assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
