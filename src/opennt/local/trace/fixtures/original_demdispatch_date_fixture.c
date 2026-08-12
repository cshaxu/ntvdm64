/*
 * R1.2 diagnostic split: exercise the original DEM dispatcher and its
 * date service without entering guest execution.  This distinguishes a
 * selector-50/CCPU return problem from the original DEM service itself.
 */
#include <stdint.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern int DemDispatch(unsigned long service);
extern uint8_t getAL(void);
extern uint8_t getDH(void);
extern uint8_t getDL(void);
extern uint16_t getCX(void);

int main(void) {
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    if (!DemDispatch(0x14u)) {
        result |= 1;
    }
    if (getAL() > 6u || getDH() == 0u || getDH() > 12u ||
        getDL() == 0u || getDL() > 31u || getCX() < 1980u) {
        result |= 2;
    }
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0) {
        result |= 4;
    }
    c_cpu_terminate();
    sas_term();
    return result;
}
