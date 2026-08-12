/* SH-10 bounded complete-CCPU initialization proof; no guest execution. */
#include <stdint.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern unsigned long ntdos64_ccpu_host_unexpected_calls(void);
extern uint16_t c_getAX(void);
extern uint16_t c_getDX(void);
extern uint16_t c_getSTATUS(void);

extern uint16_t getAX(void);
extern uint16_t getDX(void);
extern uint16_t getDS(void);
extern uint16_t getES(void);
extern uint32_t getZF(void);
extern void setAX(uint16_t value);
extern void setDX(uint16_t value);
extern void setCF(uint32_t value);
extern void setZF(uint32_t value);

int main(void) {
    c_cpu_init();
    if (ntdos64_ccpu_host_unexpected_calls() != 0) return 5;
    if (getAX() != 0 || getDX() != 0x0303 || getDS() != 0 || getES() != 0) return 1;

    setAX(0x1234);
    setDX(0xBEEF);
    setZF(1);
    setCF(1);
    if (c_getAX() != 0x1234 || c_getDX() != 0xBEEF || getZF() != 1) return 2;
    if ((c_getSTATUS() & 0x41) != 0x41) return 3;

    setZF(0);
    setCF(0);
    c_cpu_terminate();
    return (c_getSTATUS() & 0x41) == 0 ? 0 : 4;
}
