/*
 * S1 historical-host closure fixture.
 *
 * This invokes the original BIOS[50h] table entry directly. The service byte
 * remains guest data in the historical SAS route, and BIOS[50h] retains its
 * original MS_bop_0 -> DemDispatch path. No guest instruction decoder or
 * replacement BIOS/BOP/DEM handler participates in this fixture.
 */
#include <stdint.h>

typedef void (*ntdos64_bios_entry)(void);

extern ntdos64_bios_entry BIOS[];
extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern void setCS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern uint8_t getAL(void);
extern uint8_t getDH(void);
extern uint8_t getDL(void);
extern uint16_t getCX(void);

int main(void)
{
    uint8_t *ram;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = ntdos64_ccpu_sm0_ram();
    if (ram == 0 || BIOS[0x50u] == 0) {
        result = 1;
        goto cleanup;
    }

    ram[3] = 0x14u; /* SVC_DEMQUERYDATE */
    setCS(0);
    setIP(3);
    BIOS[0x50u]();

    if (getIP() != 4u) {
        result |= 2;
    }
    if (getAL() > 6u || getDH() == 0u || getDH() > 12u ||
        getDL() == 0u || getDL() > 31u || getCX() < 1980u) {
        result |= 4;
    }
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0u) {
        result |= 8;
    }

cleanup:
    c_cpu_terminate();
    sas_term();
    return result;
}
