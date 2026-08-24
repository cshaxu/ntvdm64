/*
 * R1.2 bounded instrumentation only.
 *
 * The guest stream deliberately contains no local BOP/SVC implementation:
 * CCPU decodes BOP 50, the complete historical BIOS[] table selects
 * MS_bop_0, and that function dispatches original service 14h through the
 * original DEM table.  BOP FE is CCPU's historical return mechanism.
 *
 * This is not an NTVDM runtime or bootstrap path.  It records only the
 * selector-50 service-byte/IP contract with a date service that has no
 * filesystem, device, DOS-image, or idle callback dependency.
 */
#include <stdint.h>
#include <string.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void c_cpu_simulate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern uint8_t getAL(void);
extern uint8_t getDH(void);
extern uint8_t getDL(void);
extern uint16_t getCX(void);

int main(void) {
    static const uint8_t code[] = {
        0xc4, 0xc4, 0x50, 0x14, /* BOP 50, SVC_DEMQUERYDATE */
        0xc4, 0xc4, 0xfe        /* historical CCPU return */
    };
    uint8_t *ram;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = ntdos64_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }

    memcpy(ram, code, sizeof(code));
    setCS(0);
    setDS(0);
    setIP(0);
    c_cpu_simulate();

    /* MS_bop_0 consumes exactly the service byte before BOP FE returns. */
    if (getIP() != sizeof(code)) {
        result |= 2;
    }
    /* demQueryDate's documented result is AL weekday, DH month, DL day, CX year. */
    if (getAL() > 6u || getDH() == 0u || getDH() > 12u ||
        getDL() == 0u || getDL() > 31u || getCX() < 1980u) {
        result |= 4;
    }
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0) {
        result |= 8;
    }

    c_cpu_terminate();
    sas_term();
    return result;
}
