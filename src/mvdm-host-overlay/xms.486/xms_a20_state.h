#ifndef MVDM_HOST_OVERLAY_XMS_A20_STATE_H
#define MVDM_HOST_OVERLAY_XMS_A20_STATE_H

#include <stdint.h>

/* Private to the matching mvdm-host XMS mirror. */
int mvdm_xms_himem_a20_state_bind(uint16_t segment, uint16_t offset);
int mvdm_xms_himem_a20_state_write(uint8_t value);

#endif
