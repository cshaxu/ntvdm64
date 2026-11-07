/* Link actual production keyba/PIC. Test-only external host boundaries do not
 * replace controller response, port reads, queue or PIC algorithms. */
#include <stdio.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <ica.h>
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_keyboard_history.h"
extern mvdm_keyboard_history nt_keyboard_history;
extern void Reset6805and8042(void);

extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void);
extern void AT_kbd_init(void), kbd_outb(io_addr, half_word);
extern void kbd_inb(io_addr, half_word *), KbdEOIHook(int, int);
extern void AddTo6805BuffImm(half_word), continue_output(void);
extern void host_key_down(int), host_key_up(int);
extern BOOL bBiosOwnsKbdHdw, bKbdEoiPending, bForceDelayInts;
extern BOOL bDelayIntPending;
extern ULONG DelayIrqLine;
extern int output_full, pending_8042, input_port_val;
extern char output_contents, KbdData;
extern half_word cmd_byte_8042, op_port_remembered_bits;
extern void ica_inb(io_addr, IU8 *), ica_outb(io_addr, IU8);
extern IS32 ica_intack(IU32 *);

static unsigned assertions, failures, eoi_calls, delayed_calls;
static int delayed_keyboard_pending;
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
    if (line == 1) delayed_keyboard_pending = count != 0;
    (void)delay;
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

/* Exercise the actual physical-key path, including translation, controller
 * output and PIC acknowledgement. The two orderings model the two guest ISR
 * contracts; each transition is completed before the next physical key. */
static void chord_key(int key, BOOL down, half_word expected, BOOL early_eoi)
{
    half_word value = 0;
    IU32 hook = 0;
    bDelayIntPending = FALSE;
    if (down) host_key_down(key); else host_key_up(key);
    CHECK(output_full && bKbdEoiPending);
    if (!(pic(0x0a) & 2)) ica_hw_interrupt(0, 1, 1);
    CHECK(ica_intack(&hook) == 9);
    if (early_eoi) ica_outb(0x20, 0x20);
    kbd_inb(0x60, &value);
    if (value != expected)
        fprintf(stderr, "key=%d down=%d early=%d got=%02x expected=%02x\n",
            key, down, early_eoi, value, expected);
    CHECK(value == expected);
    if (!early_eoi) ica_outb(0x20, 0x20);
    CHECK(!output_full && !bKbdEoiPending);
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

    /* Controller replies replace stale output and do not raise a keyboard IRQ. */
    CHECK(input_port_val == 0xbf);
    output_contents = 0x5a; KbdData = 0x33;
    before = eoi_calls;
    kbd_outb(0x64, 0xc0);
    kbd_inb(0x64, &value); CHECK(value & 1);
    kbd_inb(0x60, &value);
    CHECK(value == input_port_val && eoi_calls == before);
    output_contents = 0x27; KbdData = 0x44;
    kbd_outb(0x64, 0xc0); kbd_inb(0x60, &value);
    CHECK(value == input_port_val);
    kbd_outb(0x64, 0xaa); kbd_inb(0x60, &value); CHECK(value == 0x55);
    kbd_outb(0x64, 0xab); kbd_inb(0x60, &value); CHECK(value == 0x00);
    kbd_outb(0x64, 0x20); kbd_inb(0x60, &value); CHECK(value == cmd_byte_8042);
    kbd_outb(0x64, 0xd0); kbd_inb(0x60, &value); CHECK(value == (half_word)(0xc1 + op_port_remembered_bits));
    kbd_outb(0x64, 0xe0); kbd_inb(0x60, &value); CHECK(value == 0x02);

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

    /* A status-port observation is not a data read.  VKD may complete the
     * PIC EOI before it reads 60h; that must retain this exact byte and must
     * not manufacture another IRQ for it. */
    bDelayIntPending = FALSE; DelayIrqLine = 0;
    AddTo6805BuffImm(0xfe); AddTo6805BuffImm(0xfa); continue_output();
    CHECK(output_full && (unsigned char)output_contents == 0xfa);
    CHECK(bKbdEoiPending && (pic(0x0a) & 2));
    before = eoi_calls;
    CHECK(ica_intack(&hook) == 9);
    ica_outb(0x20, 0x20);
    CHECK(eoi_calls == before + 1 && !bKbdEoiPending);
    CHECK(output_full && (unsigned char)output_contents == 0xfa);
    kbd_inb(0x64, &value); CHECK((value & 1) && output_full);
    kbd_inb(0x64, &value); CHECK((value & 1) && output_full);
    CHECK(!(pic(0x0a) & 2) && !(pic(0x0b) & 2));
    kbd_inb(0x60, &value); CHECK(value == 0xfa);
    CHECK(output_full && (unsigned char)output_contents == 0xfe);
    CHECK(bKbdEoiPending && bDelayIntPending);
    ica_hw_interrupt(0, 1, 1);
    kbd_inb(0x60, &value); CHECK(value == 0xfe);
    CHECK(ica_intack(&hook) == 9);
    ica_outb(0x20, 0x20);
    CHECK(!output_full && !bKbdEoiPending);
    fprintf(stderr, "PASS early EOI retains unread port-60 byte and advances once\n");

    /* ICW1 is a cancellation, not an early normal EOI.  Preserve the
     * original cancellation behavior: retire the old unread slot exactly
     * once, then let the original PIC initialization sequence resume. */
    bDelayIntPending = FALSE; DelayIrqLine = 0;
    AddTo6805BuffImm(0xfa); continue_output();
    CHECK(output_full && (unsigned char)output_contents == 0xfa && bKbdEoiPending);
    before = eoi_calls;
    ica_outb(0x20, 0x11);
    CHECK(eoi_calls == before + 1 && !output_full && !bKbdEoiPending);
    ica_outb(0x21, 8); ica_outb(0x21, 4); ica_outb(0x21, 1); ica_outb(0x21, 0xfd);
    fprintf(stderr, "PASS PIC reset cancels rather than early-acknowledges unread keyboard output\n");

    /* A direct controller reply replaces the output byte without consuming
     * the queued scan-code slot or fabricating an EOI. */
    bDelayIntPending = FALSE; DelayIrqLine = 0;
    AddTo6805BuffImm(0xfe); AddTo6805BuffImm(0xfa); continue_output();
    CHECK(output_full && bKbdEoiPending);
    before = eoi_calls;
    kbd_outb(0x64, 0xc0); kbd_inb(0x60, &value);
    CHECK(value == input_port_val && bKbdEoiPending && eoi_calls == before);
    CHECK(ica_intack(&hook) == 9);
    ica_outb(0x20, 0x20);
    CHECK(eoi_calls > before && output_full && (unsigned char)output_contents == 0xfe);
    ica_hw_interrupt(0, 1, 1);
    kbd_inb(0x60, &value);
    CHECK(ica_intack(&hook) == 9);
    ica_outb(0x20, 0x20);
    CHECK(!output_full && !bKbdEoiPending && !pending_8042);
    /* NT key numbers 60 and 43 are left Alt and Enter. The guest sees the
     * translated set-1 make/break sequence; neither ISR ordering may strand
     * a modifier or lose the following ordinary key. */
    chord_key(60, TRUE,  0x38, FALSE);
    chord_key(43, TRUE,  0x1c, FALSE);
    chord_key(43, FALSE, 0x9c, FALSE);
    chord_key(60, FALSE, 0xb8, FALSE);
    chord_key(60, TRUE,  0x38, TRUE);
    chord_key(43, TRUE,  0x1c, TRUE);
    chord_key(43, FALSE, 0x9c, TRUE);
    chord_key(60, FALSE, 0xb8, TRUE);
    chord_key(49, TRUE,  0x2f, TRUE);  /* V */
    chord_key(49, FALSE, 0xaf, TRUE);
    fprintf(stderr, "PASS physical Alt+Enter and following key through controller/PIC\n");
    /* Whole-device handoff reset must not schedule the next queued byte
     * from the PIC cancellation callback while discarding that same queue. */
    bDelayIntPending = FALSE; DelayIrqLine = 0;
    AddTo6805BuffImm(0x17); AddTo6805BuffImm(0x14); continue_output();
    nt_keyboard_history.output = 1; /* A returnable source-owned key. */
    CHECK(output_full && bKbdEoiPending);
    if (!(pic(0x0a) & 2)) ica_hw_interrupt(0, 1, 1);
    delayed_keyboard_pending = 0;
    Reset6805and8042();
    CHECK(!output_full && !pending_8042);
    CHECK(!delayed_keyboard_pending);
    sas_term(); (void)session_thread_unbind(&owner); (void)session_dispose(&owner);
    fprintf(stderr, "KEYBOARD_PIC_PROFILE assertions=%u failures=%u\n", assertions, failures);
    return failures ? 1 : 0;
}
