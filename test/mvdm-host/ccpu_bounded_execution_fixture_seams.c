/*
 * Link seams for the bounded CCPU fixture.
 *
 * They are deliberately test-only and never enter an MVDM host library.
 * The bounded instruction programs end in c_main's `D6 FE` direct unsimulate
 * branch. These seams omit unrelated host/device startup; explicit counters
 * below check selected call boundaries. This fixture does not establish real
 * BIOS, device, WOW scheduler or host teardown behavior.
 */
#include <stdint.h>
#include <stdlib.h>

/* nt_mem's original source is compiled through the host CRT redirect header.
 * This focused execution fixture has no product environment policy, so the
 * original "configuration absent" result is the only admissible seam. */
char * __cdecl mvdm_host_getenv(const char *name)
{
    (void)name;
    return NULL;
}

typedef void (*fixture_callback)(void);

void rom_init(void) {}
void copyROM(void) {}
/* The original nt_cpu.c host_simulate body is linked with the fixture.  The
 * D6 FE program exits before its recursive BIOS path is reached. */
void host_timer_event(void) {}
void LIM_b_write(void) {}
void LIM_w_write(void) {}
void host_exint_hook(void) {}
void host_swint_hook(void) {}
int host_hwint_hook(int32_t interrupt_number)
{
    (void)interrupt_number;
    abort(); /* This fixture does not execute a hardware interrupt handler. */
    return 0;
}
void dispatch_q_event(void) {}
void host_ica_lock(void) {}
void host_ica_unlock(void) {}

/* `ica.c` imports these original nt_eoi.c forms even though the bounded
 * initialize/request/INTACK proof cannot reach delayed-EOI or WOW-idle
 * scheduling.  Keep test-only hit counters so an accidental traversal fails
 * the fixture instead of turning into an invented controller behavior. */
unsigned fixture_eoi_hook_calls;
unsigned fixture_wow_idle_calls;
/* The execution adapter calls this after its bounded entry returns. Real
 * host startup/teardown is outside this CPU fixture; assert the call count
 * instead of presenting an empty test substitute as lifecycle acceptance. */
unsigned fixture_host_close_calls;
void host_applClose(void) { ++fixture_host_close_calls; }
uint32_t DelayIrqLine;
void host_EOI_hook(int irq_line, int call_count)
{
    (void)irq_line;
    (void)call_count;
    ++fixture_eoi_hook_calls;
}
void WOWIdle(int force)
{
    (void)force;
    ++fixture_wow_idle_calls;
}
void host_terminate(void) {}
void read_descriptor(void) {}
int selector_outside_table(void) { return 1; }
void sas_overwrite_memory(void) {}
short host_error(int error, int options, char *text)
{
    (void)error;
    (void)options;
    (void)text;
    return 0;
}

fixture_callback BIOS[256];
fixture_callback read_pointers;
void *trace_file;
void *Gdp;
/* No process-entry seam: the product now links original ntvdm.c::main. */
