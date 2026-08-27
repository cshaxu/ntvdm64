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
uint16_t getDX(void);
uint16_t getSI(void);
uint16_t getDS(void);
uint16_t getES(void);
void setAL(uint8_t value);
void setAX(uint16_t value);
void setBX(uint16_t value);
void setCF(uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
