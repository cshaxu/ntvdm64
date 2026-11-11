/*
 * R1.4 bounded event/idle observation.  An out-of-range original DEM service
 * takes MS_bop_0's non-date IDLE_disk branch, then CCPU returns via BOP FE.
 * It defines no BOP, DEM, idle, IRQ, or device replacement behavior.
 */
#include <stdint.h>
#include <string.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void c_cpu_simulate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern int getCF(void);

int main(void) {
    static const uint8_t code[] = {
        0xc4, 0xc4, 0x50, 0xff, /* BOP 50, invalid DEM service */
        0xc4, 0xc4, 0xfe
    };
    uint8_t *ram;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) return 1;
    memcpy(ram, code, sizeof(code));
    setCS(0); setDS(0); setIP(0);
    c_cpu_simulate();

    if (getIP() != sizeof(code)) result |= 2;
    if (!getCF()) result |= 4;
    if (runner_ccpu_sm0_unexpected_calls() != 0) result |= 8;
    c_cpu_terminate();
    sas_term();
    return result;
}
