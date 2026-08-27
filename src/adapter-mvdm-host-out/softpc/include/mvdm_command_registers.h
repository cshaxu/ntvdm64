#ifndef MVDM_COMMAND_REGISTERS_H
#define MVDM_COMMAND_REGISTERS_H

#include <stdint.h>

/* Same-shaped original softpc.h monitor imports reached by the admitted
 * COMMAND environment/current-directory source group. */
#ifdef __cplusplus
extern "C" {
#endif

uint16_t getAX(void);
uint8_t getAL(void);
uint16_t getBX(void);
uint16_t getCX(void);
uint16_t getDX(void);
uint16_t getBP(void);
uint16_t getSP(void);
uint16_t getDI(void);
uint16_t getIP(void);
uint16_t getSI(void);
uint16_t getCS(void);
uint16_t getDS(void);
uint16_t getES(void);
uint16_t getSS(void);
uint16_t getFS(void);
uint16_t getGS(void);
uint16_t getMSW(void);
uint32_t getEAX(void);
uint32_t getEBX(void);
uint32_t getECX(void);
uint32_t getEDX(void);
uint32_t getESI(void);
uint32_t getEDI(void);
uint32_t getEBP(void);
uint32_t getESP(void);
uint32_t getEIP(void);
uint32_t getPE(void);
void setAL(uint8_t value);
void setBL(uint8_t value);
void setAX(uint16_t value);
void setBX(uint16_t value);
void setDX(uint16_t value);
void setCX(uint16_t value);
void setSP(uint16_t value);
void setSI(uint16_t value);
void setDI(uint16_t value);
void setIP(uint16_t value);
void setEAX(uint32_t value);
void setEBX(uint32_t value);
void setECX(uint32_t value);
void setEDX(uint32_t value);
void setESI(uint32_t value);
void setEDI(uint32_t value);
void setEBP(uint32_t value);
void setESP(uint32_t value);
void setEIP(uint32_t value);
void setCF(uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
