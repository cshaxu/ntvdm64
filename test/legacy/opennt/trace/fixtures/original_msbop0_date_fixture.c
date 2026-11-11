/*
 * R1.2 diagnostic split: call original MS_bop_0 at its documented entry.
 * The service byte is placed at CS:IP, so Sim32GetVDMPointer, DemDispatch,
 * the post-dispatch IP increment, and original register forwarding all stay
 * historical.  No BIOS replacement or guest execution is involved here.
 */
#include <stdint.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern void MS_bop_0(void);
extern void setCS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern uint8_t getDH(void);
extern uint8_t getDL(void);
extern uint16_t getCX(void);

int main(void) {
    uint8_t *ram;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }
    ram[3] = 0x14u; /* SVC_DEMQUERYDATE */
    setCS(0);
    setIP(3);
    MS_bop_0();

    if (getIP() != 4u) {
        result |= 2;
    }
    if (getDH() == 0u || getDH() > 12u || getDL() == 0u ||
        getDL() > 31u || getCX() < 1980u) {
        result |= 4;
    }
    if (runner_ccpu_sm0_unexpected_calls() != 0) {
        result |= 8;
    }
    c_cpu_terminate();
    sas_term();
    return result;
}
