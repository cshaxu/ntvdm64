/*
 * Source-derived CCPU host_simulate bridge for a default-disabled link
 * diagnostic. OpenNT host/src/nt_cpu.c delegates recursive BIOS simulation to
 * cpu_simulate(); with CCPU selected, cpu4gen.h aliases that to
 * c_cpu_simulate(). The historic IcaLock assertions are NT-host-specific and
 * deliberately remain outside this software-interpreter bridge.
 */
void c_cpu_simulate(void);

void host_simulate(void) {
    c_cpu_simulate();
}
