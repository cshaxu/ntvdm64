#ifndef MVDM_HOST_OVERLAY_GDP_STATE_H
#define MVDM_HOST_OVERLAY_GDP_STATE_H

#include <stddef.h>

/*
 * Private to the selected CCPU/CVIDC mirror.  GDP fields are native SoftPC
 * controller state, not guest addresses or externally visible identities.
 */
void *mvdm_gdp_create(void);
void mvdm_gdp_destroy(void *state);
void *mvdm_gdp_slot(const void *state, unsigned int original_offset,
    size_t native_width);

#endif
