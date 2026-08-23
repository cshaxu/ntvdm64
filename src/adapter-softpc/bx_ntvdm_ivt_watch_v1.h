#ifndef BX_NTVDM_IVT_WATCH_V1_H
#define BX_NTVDM_IVT_WATCH_V1_H

#include <stdint.h>

/* One explicitly configured real-mode IVT entry.  This mantle capability
 * carries only a vector number and a fixed-width segment:offset pair; it has
 * no guest-service, device, or higher-level policy vocabulary. */
#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_ivt_watch_v1_initialize(uint32_t vector);
int bx_ntvdm_ivt_watch_v1_read_current(uint32_t vector,
    uint16_t *offset, uint16_t *segment);
int bx_ntvdm_ivt_watch_v1_copy_expected(uint32_t vector,
    uint16_t *offset, uint16_t *segment);
int bx_ntvdm_ivt_watch_v1_store_expected(uint32_t vector,
    uint16_t offset, uint16_t segment);
void bx_ntvdm_ivt_watch_v1_reset(void);

#ifdef __cplusplus
}
#endif

#endif
