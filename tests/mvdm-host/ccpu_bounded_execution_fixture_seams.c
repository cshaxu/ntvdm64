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
#include <stdio.h>
#include <stdlib.h>

typedef void (*fixture_callback)(void);

void rom_init(void) {}
void copyROM(void) {}
void EDL_fast_bop(uint32_t value) { (void)value; }
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
void ica_intack(void) {}
void ica_hw_interrupt(void) {}
void host_terminate(void) {}
void read_descriptor(void) {}
int selector_outside_table(void) { return 1; }
void sas_overwrite_memory(void) {}
/* `nt_mem.c` retains this unresolved historical external-page interface.  The
 * bounded CCPU proof never calls VdmAdd/RemoveVirtualMemory, so a no-op would
 * hide an accidental route instead of proving the selected interval. */
void VdmSetPhysRecStructs(uintptr_t host_address, uint32_t intel_address,
    uint32_t byte_count)
{
    (void)host_address;
    (void)intel_address;
    (void)byte_count;
    fputs("unexpected VdmSetPhysRecStructs in bounded CCPU fixture\n", stderr);
    abort();
}
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
void *Ios_in_adapter_table;
fixture_callback Ios_inb_function;
fixture_callback Ios_inw_function;
void *Ios_out_adapter_table;
fixture_callback Ios_outb_function;
fixture_callback Ios_outw_function;
