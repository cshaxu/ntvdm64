#ifndef MVDM_REDIRECTOR_WORKER_COPY_H
#define MVDM_REDIRECTOR_WORKER_COPY_H

#include <stdint.h>

/* Worker-owned synchronous guest-memory boundary for VDMREDIR.DLL. */
__declspec(dllexport) int mvdm_redirector_worker_copy_from(uint16_t segment,
    uint16_t offset, uint8_t *bytes,
    uint32_t byte_count);
__declspec(dllexport) int mvdm_redirector_worker_copy_to(uint16_t segment,
    uint16_t offset, uint8_t const *bytes,
    uint32_t byte_count);

#endif
