/* SM-0: CCPU owns BOP-FE recognition and returns through unsimulate. */
#include <stdint.h>
#include <stdlib.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void c_cpu_simulate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);

extern void setCS(uint16_t value);
extern void setIP(uint16_t value);

int main(void) {
    uint8_t *ram;

    /* Historical config() establishes SAS before it initializes CCPU. */
    sas_init(1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }

    /* CCPU decodes this as C4 C4 / BOP FE and invokes c_cpu_unsimulate(). */
    ram[0] = 0xc4;
    ram[1] = 0xc4;
    ram[2] = 0xfe;
    setCS(0);
    setIP(0);
    c_cpu_simulate();

    if (runner_ccpu_sm0_unexpected_calls() != 0) {
        return 2;
    }
    c_cpu_terminate();
    sas_term();
    return 0;
}
