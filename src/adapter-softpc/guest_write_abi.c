#include "guest_write_abi.h"

#include <string.h>

#include "guest_range.h"

void runtime_guest_write_v1_initialize(runtime_guest_write_v1 *write,
    uint64_t guest_physical_address, uint64_t byte_count,
    uint64_t payload_offset)
{
    if (write == 0) return;
    memset(write, 0, sizeof(*write));
    write->magic = RUNTIME_GUEST_WRITE_ABI_MAGIC;
    write->abi_version = RUNTIME_GUEST_WRITE_ABI_VERSION;
    write->struct_bytes = sizeof(*write);
    write->guest_physical_address = guest_physical_address;
    write->byte_count = byte_count;
    write->payload_offset = payload_offset;
}

int runtime_guest_write_v1_preflight(const runtime_guest_write_v1 *write,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    runtime_guest_range range;
    if (write == 0 || write->magic != RUNTIME_GUEST_WRITE_ABI_MAGIC ||
        write->abi_version != RUNTIME_GUEST_WRITE_ABI_VERSION ||
        write->struct_bytes != sizeof(*write) || write->flags != 0u ||
        write->payload_offset > payload_bytes) return 0;
    range.address = write->guest_physical_address;
    range.length = write->byte_count;
    return runtime_guest_range_within(aperture_bytes, &range) &&
        write->byte_count <= payload_bytes - write->payload_offset;
}
