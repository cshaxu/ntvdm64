#ifndef COMMON_GUEST_MEMORY_REDIRECTOR_H
#define COMMON_GUEST_MEMORY_REDIRECTOR_H
#include <stdint.h>

/* Existing worker-exported, synchronous Redir guest-memory ABI. The worker
 * retains address validation and the single underlying memory authority. */
__declspec(dllexport) int mvdm_redirector_worker_copy_from(uint16_t segment,
    uint16_t offset,uint8_t *bytes,uint32_t byte_count);
__declspec(dllexport) int mvdm_redirector_worker_copy_to(uint16_t segment,
    uint16_t offset,uint8_t const *bytes,uint32_t byte_count);
#endif
