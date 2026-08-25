#include "multi_write_abi.h"

#include <string.h>

static int runtime_multi_write_ranges_overlap(uint64_t first_address,
    uint64_t first_bytes, uint64_t second_address, uint64_t second_bytes)
{
    return first_address < second_address + second_bytes &&
        second_address < first_address + first_bytes;
}

void runtime_multi_write_initialize(runtime_multi_write *value)
{
    if (value == 0)
        return;
    memset(value, 0, sizeof(*value));
    value->magic = RUNTIME_MULTI_WRITE_ABI_MAGIC;
    value->abi_version = RUNTIME_MULTI_WRITE_ABI_VERSION;
    value->struct_bytes = sizeof(*value);
}

int runtime_multi_write_add(runtime_multi_write *value,
    uint64_t address, uint64_t bytes, uint64_t payload_offset)
{
    uint64_t payload_end;

    if (value == 0 ||
        value->write_count >= RUNTIME_MULTI_WRITE_MAX_WRITES ||
        bytes == 0u || bytes > UINT64_MAX - payload_offset)
        return 0;
    payload_end = payload_offset + bytes;
    if (payload_end > RUNTIME_MULTI_WRITE_MAX_PAYLOAD)
        return 0;
    runtime_guest_write_initialize(&value->writes[value->write_count],
        address, bytes, payload_offset);
    ++value->write_count;
    if (payload_end > value->payload_bytes)
        value->payload_bytes = payload_end;
    return 1;
}

int runtime_multi_write_preflight(const runtime_multi_write *value,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    uint32_t index;
    uint32_t prior_index;

    if (value == 0 || value->magic != RUNTIME_MULTI_WRITE_ABI_MAGIC ||
        value->abi_version != RUNTIME_MULTI_WRITE_ABI_VERSION ||
        value->struct_bytes != sizeof(*value) || value->write_count == 0u ||
        value->write_count > RUNTIME_MULTI_WRITE_MAX_WRITES ||
        value->payload_bytes != payload_bytes ||
        payload_bytes > RUNTIME_MULTI_WRITE_MAX_PAYLOAD)
        return 0;
    for (index = 0u; index < value->write_count; ++index) {
        const runtime_guest_write *write = &value->writes[index];

        if (!runtime_guest_write_preflight(write, aperture_bytes,
                payload_bytes))
            return 0;
        for (prior_index = 0u; prior_index < index; ++prior_index) {
            const runtime_guest_write *prior =
                &value->writes[prior_index];

            if (runtime_multi_write_ranges_overlap(
                    write->guest_physical_address, write->byte_count,
                    prior->guest_physical_address, prior->byte_count) ||
                runtime_multi_write_ranges_overlap(write->payload_offset,
                    write->byte_count, prior->payload_offset,
                    prior->byte_count))
                return 0;
        }
    }
    return 1;
}
