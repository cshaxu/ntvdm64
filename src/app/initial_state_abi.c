#include "initial_state_abi.h"

#include <string.h>

static int all_zero(const uint8_t *bytes, uint32_t count)
{
    uint32_t index;
    for (index = 0u; index < count; ++index) if (bytes[index] != 0u) return 0;
    return 1;
}

static int overlaps(const bx_ntvdm_guest_write_v1 *left,
    const bx_ntvdm_guest_write_v1 *right)
{
    return left->guest_physical_address < right->guest_physical_address + right->byte_count &&
        right->guest_physical_address < left->guest_physical_address + left->byte_count;
}

void bx_ntvdm_initial_state_v1_clear(bx_ntvdm_initial_state_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_INITIAL_STATE_ABI_MAGIC;
    value->abi_version = BX_NTVDM_INITIAL_STATE_ABI_VERSION;
    value->struct_bytes = sizeof(*value);
}

int bx_ntvdm_initial_state_v1_valid(const bx_ntvdm_initial_state_v1 *value)
{
    uint32_t index;
    if (value == 0 || value->magic != BX_NTVDM_INITIAL_STATE_ABI_MAGIC ||
        value->abi_version != BX_NTVDM_INITIAL_STATE_ABI_VERSION ||
        value->struct_bytes != sizeof(*value)) return 0;
    if (value->disposition == BX_NTVDM_INITIAL_STATE_V1_ABSENT)
        return value->range_count == 0u && value->payload_bytes == 0u &&
            all_zero(value->evidence_sha256, BX_NTVDM_INITIAL_STATE_V1_EVIDENCE_SHA256_BYTES) &&
            all_zero(value->payload, BX_NTVDM_INITIAL_STATE_V1_MAX_BYTES) &&
            all_zero((const uint8_t *)value->ranges, (uint32_t)sizeof(value->ranges));
    if (value->disposition != BX_NTVDM_INITIAL_STATE_V1_PRESENT ||
        value->range_count == 0u ||
        value->range_count > BX_NTVDM_INITIAL_STATE_V1_MAX_RANGES ||
        value->payload_bytes == 0u ||
        value->payload_bytes > BX_NTVDM_INITIAL_STATE_V1_MAX_BYTES ||
        all_zero(value->evidence_sha256, BX_NTVDM_INITIAL_STATE_V1_EVIDENCE_SHA256_BYTES)) return 0;
    for (index = 0u; index < value->range_count; ++index)
        if (!bx_ntvdm_guest_write_v1_preflight(&value->ranges[index], UINT64_MAX,
                value->payload_bytes)) return 0;
    return 1;
}

int bx_ntvdm_initial_state_v1_admitted(const bx_ntvdm_initial_state_v1 *value,
    uint64_t aperture_bytes)
{
    uint32_t index, other;
    uint64_t total = 0u;
    if (!bx_ntvdm_initial_state_v1_valid(value) ||
        value->disposition != BX_NTVDM_INITIAL_STATE_V1_PRESENT) return 0;
    for (index = 0u; index < value->range_count; ++index) {
        const bx_ntvdm_guest_write_v1 *range = &value->ranges[index];
        if (!bx_ntvdm_guest_write_v1_preflight(range, aperture_bytes,
                value->payload_bytes) || UINT64_MAX - total < range->byte_count)
            return 0;
        for (other = 0u; other < index; ++other)
            if (overlaps(range, &value->ranges[other])) return 0;
        total += range->byte_count;
    }
    return total == value->payload_bytes;
}
