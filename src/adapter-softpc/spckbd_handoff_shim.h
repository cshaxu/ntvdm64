#ifndef RUNTIME_BOP_SHIM_SPCKBD_HANDOFF_SHIM_H
#define RUNTIME_BOP_SHIM_SPCKBD_HANDOFF_SHIM_H

#include <stdint.h>

/* This intentionally names only the fixed-width CCPU/SAS substitutions used
 * by the selected source fragment.  It is not a general SoftPC interface. */
typedef uint16_t word;

int runtime_spckbd_handoff_begin(uint16_t cs, uint16_t ds, uint16_t si,
    uint16_t ax, uint8_t screen_state);
int runtime_spckbd_handoff_end(void);
void runtime_spckbd_handoff_reset(void);

word getCS(void);
word getDS(void);
word getSI(void);
word getAX(void);
uint8_t getNtScreenState(void);
int runtime_spckbd_table_word(uint16_t index, word *value);
void sas_store_no_check(uint32_t address, uint8_t value);
void runtime_spckbd_handoff_fail(void);
int runtime_spckbd_handoff_failed(void);
void setCF(uint16_t value);

#endif
