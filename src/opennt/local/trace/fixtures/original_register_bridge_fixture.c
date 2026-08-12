/*
 * R1.3 bounded proof of the original nt_aorc.c register forwarding layer.
 * It invokes no guest instruction, BIOS, BOP, DEM, or device code.
 */
#include <stdint.h>
#include <stdlib.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern uint16_t c_getAX(void);
extern uint16_t c_getDX(void);
extern uint16_t c_getSTATUS(void);

extern uint16_t getAX(void);
extern uint16_t getDS(void);
extern uint16_t getDX(void);
extern uint16_t getES(void);
extern uint16_t getZF(void);
extern void setAX(uint16_t value);
extern void setCF(uint16_t value);
extern void setDX(uint16_t value);
extern void setZF(uint16_t value);

/* The test remains in CCPU's initialized real-mode state. These retain a
 * fail-closed link boundary for the completion's protected-mode branch; they
 * are not descriptor emulation and must never be called by this fixture. */
int selector_outside_table(uint16_t selector, uint32_t *descriptor_address) {
    (void)selector;
    (void)descriptor_address;
    abort();
    return 1;
}

int main(void) {
    c_cpu_init();
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0) return 5;
    if (getAX() != 0 || getDX() != 0x0303 || getDS() != 0 || getES() != 0)
        return 1;

    setAX(0x1234);
    setDX(0xbeef);
    setZF(1);
    setCF(1);
    if (c_getAX() != 0x1234 || c_getDX() != 0xbeef || getZF() != 1)
        return 2;
    if ((c_getSTATUS() & 0x41) != 0x41) return 3;

    setZF(0);
    setCF(0);
    c_cpu_terminate();
    return (c_getSTATUS() & 0x41) == 0 ? 0 : 4;
}
