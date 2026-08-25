/*
 * R2 trace-only proof of the original SAS allocation owner.
 *
 * This fixture invokes only sas_init/sas_term and ordinary memory access.
 * It deliberately does not enter the CPU, BIOS, BOP, DEM, device, or DOS path.
 */
#include <stdint.h>

extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint32_t c_sas_memory_size(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);

int main(void) {
    const uint32_t size = 2u * 1024u * 1024u;
    uint8_t *ram;

    sas_init(size);
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }
    if (c_sas_memory_size() != size) {
        return 2;
    }

    ram[0x2000u] = 0x5au;
    if (ram[0x2000u] != 0x5au) {
        return 3;
    }
    if (runner_ccpu_sm0_unexpected_calls() != 0u) {
        return 4;
    }

    sas_term();
    if (runner_ccpu_sm0_ram() != NULL) {
        return 5;
    }
    return 0;
}
