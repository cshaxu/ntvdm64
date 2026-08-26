#ifndef RUNTIME_IVT_WATCH_H
#define RUNTIME_IVT_WATCH_H

#include <stdint.h>

/* One explicitly configured real-mode IVT entry.  This machine capability
 * carries only a vector number and a fixed-width segment:offset pair; it has
 * no guest-service, device, or higher-level policy vocabulary. */
#ifdef __cplusplus
extern "C" {
#endif

int runtime_ivt_watch_initialize(uint32_t vector);
int runtime_ivt_watch_read_current(uint32_t vector,
    uint16_t *offset, uint16_t *segment);
int runtime_ivt_watch_copy_expected(uint32_t vector,
    uint16_t *offset, uint16_t *segment);
int runtime_ivt_watch_store_expected(uint32_t vector,
    uint16_t offset, uint16_t segment);
void runtime_ivt_watch_reset(void);

#ifdef __cplusplus
}
#endif

#endif
