#include "mvdm_redirector_worker_copy.h"

#include "ntvdm-exe/softpc/include/mvdm_softpc_effective_address.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

int mvdm_redirector_worker_copy_from(uint16_t segment, uint16_t offset,
    uint8_t *bytes,
    uint32_t byte_count)
{
    uint32_t address;
    if (!mvdm_softpc_effective_address(segment, offset, &address)) return 0;
    return mvdm_softpc_guest_memory_copy_from(address, bytes, byte_count);
}

int mvdm_redirector_worker_copy_to(uint16_t segment, uint16_t offset,
    uint8_t const *bytes,
    uint32_t byte_count)
{
    uint32_t address;
    if (!mvdm_softpc_effective_address(segment, offset, &address)) return 0;
    return mvdm_softpc_guest_memory_copy_to(address, bytes, byte_count);
}
