/*
 * Staging-only adapter between DEM's historical softpc.h accessor ABI and
 * the selected CCPU archive. It intentionally owns no CPU state.
 */
#include <stdint.h>

#ifndef CCPU_USE_HISTORICAL_REGISTER_BRIDGE

extern uint8_t c_getAL(void);
extern uint8_t c_getBL(void);
extern uint8_t c_getDL(void);
extern uint16_t c_getAX(void);
extern uint16_t c_getBP(void);
extern uint16_t c_getBX(void);
extern uint16_t c_getCX(void);
extern uint16_t c_getDI(void);
extern uint16_t c_getDS(void);
extern uint16_t c_getDX(void);
extern uint16_t c_getES(void);
extern uint16_t c_getSI(void);
extern int c_getZF(void);

extern void c_setAX(uint16_t value);
extern void c_setBP(uint16_t value);
extern void c_setBX(uint16_t value);
extern void c_setCF(int value);
extern void c_setCX(uint16_t value);
extern void c_setDI(uint16_t value);
extern void c_setDX(uint16_t value);
extern void c_setSI(uint16_t value);
extern void c_setZF(int value);

uint8_t getAL(void) { return c_getAL(); }
uint8_t getBL(void) { return c_getBL(); }
uint8_t getDL(void) { return c_getDL(); }
uint16_t getAX(void) { return c_getAX(); }
uint16_t getBP(void) { return c_getBP(); }
uint16_t getBX(void) { return c_getBX(); }
uint16_t getCX(void) { return c_getCX(); }
uint16_t getDI(void) { return c_getDI(); }
uint16_t getDS(void) { return c_getDS(); }
uint16_t getDX(void) { return c_getDX(); }
uint16_t getES(void) { return c_getES(); }
uint16_t getSI(void) { return c_getSI(); }
uint32_t getZF(void) { return (uint32_t)c_getZF(); }

void setAX(uint16_t value) { c_setAX(value); }
void setBP(uint16_t value) { c_setBP(value); }
void setBX(uint16_t value) { c_setBX(value); }
void setCF(uint32_t value) { c_setCF((int)value); }
void setCX(uint16_t value) { c_setCX(value); }
void setDI(uint16_t value) { c_setDI(value); }
void setDX(uint16_t value) { c_setDX(value); }
void setSI(uint16_t value) { c_setSI(value); }
void setZF(uint32_t value) { c_setZF((int)value); }

#endif
