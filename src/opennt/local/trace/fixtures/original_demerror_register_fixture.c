/*
 * R1.3 original DEM caller link diagnostic. demClientErrorEx's access-denied
 * path would update AX/CF through nt_aorc.c without entering an SVC/device
 * path, but the whole historical source's unrelated imports remain visible.
 */
#include <stdint.h>
#include <stdlib.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern uint16_t c_getAX(void);
extern uint16_t c_getCF(void);
extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern unsigned long demClientErrorEx(void *file, char drive, int set_regs);
extern void __stdcall SetLastError(unsigned long error);

/* This fixture never leaves initialized real mode. The unresolved protected
 * selector helper must fail closed rather than becoming descriptor emulation. */
int selector_outside_table(uint16_t selector, uint32_t *descriptor_address) {
    (void)selector;
    (void)descriptor_address;
    abort();
    return 1;
}

int main(void) {
    c_cpu_init();
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0) return 5;

    SetLastError(5); /* ERROR_ACCESS_DENIED: non-hard-error path. */
    if (demClientErrorEx((void *)(intptr_t)-1, (char)-1, 1) != 5) return 1;
    if (c_getAX() != 5 || c_getCF() != 1) return 2;
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0) return 3;

    c_cpu_terminate();
    return 0;
}
