/*
 * Link seams for the single-instruction CCPU fixture.
 *
 * They are deliberately test-only and never enter an MVDM host library.
 * The exercised guest program is `D6 FE`; it uses c_main's original direct
 * unsimulate branch before BIOS/BOP dispatch, devices, interrupts, ROM media
 * or I/O-table handlers.  Every seam below is therefore an asserted-unreached
 * original external form, not a substitute product implementation.
 */
#include <stdint.h>

#include "mvdm_softpc_termination.h"

typedef void (*fixture_callback)(void);

void rom_init(void) {}
void copyROM(void) {}
/* The original nt_cpu.c host_simulate body is linked with the fixture.  The
 * D6 FE program exits before its recursive BIOS path is reached. */
void host_timer_event(void) {}
void LIM_b_write(void) {}
void LIM_w_write(void) {}
void c_sas_touch(void) {}
void c_VirtualiseInstruction(void) {}
void host_exint_hook(void) {}
void host_swint_hook(void) {}
void dispatch_q_event(void) {}
void host_ica_lock(void) {}
void host_ica_unlock(void) {}

/* `ica.c` imports these original nt_eoi.c forms even though the bounded
 * initialize/request/INTACK proof cannot reach delayed-EOI or WOW-idle
 * scheduling.  Keep test-only hit counters so an accidental traversal fails
 * the fixture instead of turning into an invented controller behavior. */
unsigned fixture_eoi_hook_calls;
unsigned fixture_wow_idle_calls;
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

/* The fixture exercises `mvdm_softpc_execution_run_until_return`, not the
 * process-lifetime original `obj.vdm/ntvdm.c` entry.  The execution adapter
 * exports both source-shaped routes, so retain this asserted-unreached test
 * link seam rather than pull startup/UI roots into the bounded CPU proof. */
int mvdm_softpc_original_entry(int argc, char **argv)
{
    (void)argv;
    if (argc == 37)
        (void)mvdm_softpc_terminate_current_session(0u, 37u);
    return 1;
}
