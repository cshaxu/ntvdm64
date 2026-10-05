/* Link actual production keyba/PIC. Test-only external host boundaries do not
 * replace controller response, port reads, queue or PIC algorithms. */
#include <stdio.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <ica.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void);
extern void AT_kbd_init(void), kbd_outb(io_addr, half_word);
extern void kbd_inb(io_addr, half_word *), KbdEOIHook(int, int);
extern void AddTo6805BuffImm(half_word), continue_output(void);
extern BOOL bBiosOwnsKbdHdw, bKbdEoiPending, bForceDelayInts;
extern BOOL bDelayIntPending;
extern ULONG DelayIrqLine;
extern int output_full, pending_8042, input_port_val;
extern char output_contents, KbdData;
extern void ica_inb(io_addr, IU8 *), ica_outb(io_addr, IU8);
extern IS32 ica_intack(IU32 *);

static unsigned assertions, failures, eoi_calls, delayed_calls;
#define CHECK(c) do { ++assertions; if (!(c)) { ++failures; \
    fprintf(stderr, "FAIL line %u: %s\n", (unsigned)__LINE__, #c); } } while (0)

/* Actual PIC invokes this historical host callback; dispatch to actual
 * keyboard EOI. No synthetic EOI on a port read. */
void host_EOI_hook(int line, int count)
{
    ++eoi_calls;
    if (line == 1) KbdEOIHook(line, count);
}
ULONG host_DelayHwInterrupt(int line, int count, ULONG delay)
{
    (void)line; (void)count; (void)delay;
    ++delayed_calls;
    return 0; /* No timer/real BIOS ISR proof in this fixture. */
}
void HostIdleNoActivity(void) { }
ULONG WaitKbdHdw(ULONG timeout) { (void)timeout; return 0; }
void HostReleaseKbd(void) { }
int bios_buffer_size(void) { return 0; }

static IU8 pic(IU8 command)
{
    IU8 value = 0;
    ica_outb(0x20, command);
    ica_inb(0x20, &value);
    return value;
}

int main(void)
{
    session owner;
    half_word value;
    IU32 hook = 0;
    unsigned before;
    session_initialize(&owner, 1);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 2;
    sas_init(0x200000); c_cpu_init();
    ica0_init(); ica1_init();
    /* Actual PIC ICW sequence; mask all interrupts during polling. */
    ica_outb(0x20, 0x11); ica_outb(0x21, 8);
    ica_outb(0x21, 4); ica_outb(0x21, 1); ica_outb(0x21, 0xff);
    AT_kbd_init(); bBiosOwnsKbdHdw = TRUE;
    /* Original host startup blocks IRQs until the guest keyboard is ready.
     * This focused case begins after that readiness boundary. */
    DelayIrqLine = 0;
    bKbdEoiPending = FALSE; bForceDelayInts = FALSE;

    /* Retained original limitation, not a desired-C0 pass. Sentinels prove
     * the real selected NTVDM response branch doesn't install any reply. */
    CHECK(input_port_val == 0xbf);
    output_contents = 0x5a; KbdData = 0x33;
    kbd_outb(0x64, 0xc0); kbd_inb(0x60, &value);
    CHECK(value == 0x5a && value != input_port_val);
    output_contents = 0x27; KbdData = 0x44;
    kbd_outb(0x64, 0xc0); kbd_inb(0x60, &value);
    CHECK(value == 0x27 && value != input_port_val);
    fprintf(stderr, "RETAINED_LIMIT C0 returns stale output in selected original NTVDM branch\n");

    KbdData = -1;
    /* Immediate controller replies are prepended, not a FIFO append. */
    AddTo6805BuffImm(0xfe); AddTo6805BuffImm(0xfa); continue_output();
    CHECK(output_full && bKbdEoiPending);
    CHECK(pic(0x0a) & 2);
    before = eoi_calls;
    kbd_inb(0x60, &value);
    CHECK(value == 0xfa && output_full && bKbdEoiPending);
    CHECK(pic(0x0a) & 2); CHECK(eoi_calls == before);
    fprintf(stderr, "PASS masked port read preserves original EOI-owned slot and IRQ request\n");

    ica_outb(0x21, 0xfd);
    CHECK(ica_intack(&hook) == 9);
    CHECK(!(pic(0x0a) & 2)); CHECK(pic(0x0b) & 2);
    ica_outb(0x20, 0x20);
    CHECK(eoi_calls > before && !(pic(0x0b) & 2));
    CHECK(output_full && (unsigned char)output_contents == 0xfe);
    CHECK(delayed_calls && bDelayIntPending);
    /* Deliver the recorded delayed host IRQ, not another EOI or queue read. */
    ica_hw_interrupt(0, 1, 1);
    kbd_inb(0x60, &value); CHECK(value == 0xfe);
    CHECK(ica_intack(&hook) == 9);
    ica_outb(0x20, 0x20);
    CHECK(!output_full && !bKbdEoiPending && !pending_8042);
    CHECK(!(pic(0x0a) & 2) && !(pic(0x0b) & 2));
    fprintf(stderr, "PASS actual INTACK/EOI advances queued bytes and retires final slot\n");
    sas_term(); (void)session_thread_unbind(&owner); (void)session_dispose(&owner);
    fprintf(stderr, "KEYBOARD_PIC_PROFILE assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
