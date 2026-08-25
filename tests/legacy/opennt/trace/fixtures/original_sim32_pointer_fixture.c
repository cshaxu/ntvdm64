/*
 * R1.2 pointer-route observation. This invokes the original Sim32 mapping
 * code after historical CCPU/SAS initialization and compares its returned
 * pointer with the initialized SAS aperture. It supplies no mapping logic.
 */
#include <stdint.h>
#include <stdio.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern uint8_t *c_GetLinAdd(uint32_t address);
extern uint8_t *Sim32pGetVDMPointer(uint32_t address, uint8_t protected_mode);

int main(void) {
    uint8_t *ram;
    uint8_t *direct_pointer;
    uint8_t *pointer;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }
    ram[3] = 0xa5u;
    fputs("sim32-pointer: before-c_GetLinAdd\n", stderr);
    direct_pointer = c_GetLinAdd(3u);
    fputs("sim32-pointer: after-c_GetLinAdd\n", stderr);
    if (direct_pointer != ram + 3 || *direct_pointer != 0xa5u) {
        result |= 2;
    }
    fputs("sim32-pointer: before-Sim32\n", stderr);
    pointer = Sim32pGetVDMPointer(0x00000003u, 0u);
    fputs("sim32-pointer: after-Sim32\n", stderr);

    if (pointer != ram + 3 || pointer == NULL || *pointer != 0xa5u) {
        result |= 8;
    }
    if (runner_ccpu_sm0_unexpected_calls() != 0) {
        result |= 16;
    }
    c_cpu_terminate();
    sas_term();
    return result;
}
