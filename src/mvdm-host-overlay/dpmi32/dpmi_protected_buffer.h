#ifndef MVDM_HOST_OVERLAY_DPMI_PROTECTED_BUFFER_H
#define MVDM_HOST_OVERLAY_DPMI_PROTECTED_BUFFER_H

#include <stdint.h>

int mvdm_dpmi_protected_buffer_load(uint16_t selector, uint32_t offset,
    uint32_t byte_count, void **bytes_out);
int mvdm_dpmi_protected_buffer_store(uint16_t selector, uint32_t offset,
    uint32_t byte_count, const void *bytes);
void mvdm_dpmi_protected_buffer_dispose(void *bytes);

#endif
