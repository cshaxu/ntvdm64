#ifndef MVDM_HOST_OVERLAY_DPMI_DESCRIPTOR_BUFFER_H
#define MVDM_HOST_OVERLAY_DPMI_DESCRIPTOR_BUFFER_H

#include <stdint.h>

/* Private companion for the original DPMI descriptor source.  It owns only a
 * transient host buffer; selector and descriptor semantics stay in dpmiselr.c. */
int mvdm_dpmi_descriptor_buffer_load(uint16_t selector, uint32_t offset,
    uint32_t count, uint32_t entry_bytes, void **entries_out);
int mvdm_dpmi_descriptor_buffer_store(uint16_t selector, uint32_t offset,
    uint32_t count, uint32_t entry_bytes, const void *entries);
void mvdm_dpmi_descriptor_buffer_dispose(void *entries);

#endif
