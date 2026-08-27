#ifndef MVDM_SAS_H
#define MVDM_SAS_H

#include <stdint.h>

/* Same-shaped numeric SoftPC SAS entry points.  Guest addresses stay numeric;
 * native pointers are limited to the caller's synchronous source/destination
 * buffer and never describe guest RAM. */
#ifdef __cplusplus
extern "C" {
#endif

void sas_init(uint32_t size);
void sas_term(void);
uint32_t sas_memory_size(void);
uint8_t sas_hw_at(uint32_t address);
uint16_t sas_w_at(uint32_t address);
uint32_t sas_dw_at(uint32_t address);
void sas_load(uint32_t address, uint8_t *value);
void sas_loadw(uint32_t address, uint16_t *value);
void sas_store(uint32_t address, uint8_t value);
void sas_storew(uint32_t address, uint16_t value);
void sas_storedw(uint32_t address, uint32_t value);
void sas_loads(uint32_t address, uint8_t *bytes, uint32_t count);
void sas_stores(uint32_t address, const uint8_t *bytes, uint32_t count);
void sas_move_bytes_forward(uint32_t source, uint32_t destination,
    uint32_t count);
void sas_move_bytes_backward(uint32_t source, uint32_t destination,
    uint32_t count);
void sas_move_words_forward(uint32_t source, uint32_t destination,
    uint32_t count);
void sas_move_words_backward(uint32_t source, uint32_t destination,
    uint32_t count);
void sas_fills(uint32_t address, uint8_t value, uint32_t count);
void sas_fillsw(uint32_t address, uint16_t value, uint32_t count);
uint8_t sas_hw_at_no_check(uint32_t address);
uint16_t sas_w_at_no_check(uint32_t address);
uint32_t sas_dw_at_no_check(uint32_t address);
void sas_store_no_check(uint32_t address, uint8_t value);
void sas_storew_no_check(uint32_t address, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif
