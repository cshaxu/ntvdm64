#include "bx_ntvdm_mutation_overlay_v1.h"

#include <string.h>

static int overlay_owner_allowed(const bx_ntvdm_mutation_overlay_v1 *overlay,
    uint32_t owner_id, uint32_t mutation_class)
{
    uint32_t mode = 0u;
    return bx_ntvdm_mutation_profile_v1_authorize(&overlay->profile, owner_id,
        mutation_class, &mode) && mode == BX_NTVDM_MUTATION_MODE_V1_OVERLAY;
}

int bx_ntvdm_mutation_overlay_v1_valid(
    const bx_ntvdm_mutation_overlay_v1 *overlay)
{
    uint32_t index;
    if (overlay == 0 || overlay->magic != BX_NTVDM_MUTATION_OVERLAY_V1_MAGIC ||
        overlay->abi_version != BX_NTVDM_MUTATION_OVERLAY_V1_VERSION ||
        overlay->struct_bytes != sizeof(*overlay) || overlay->reserved0 != 0u ||
        overlay->record_count > BX_NTVDM_MUTATION_OVERLAY_V1_MAX_RECORDS ||
        !bx_ntvdm_mutation_profile_v1_valid(&overlay->profile) ||
        overlay->profile.mode != BX_NTVDM_MUTATION_MODE_V1_OVERLAY) return 0;
    for (index = 0u; index < overlay->record_count; ++index) {
        const bx_ntvdm_mutation_overlay_record_v1 *record =
            &overlay->records[index];
        uint32_t prior;
        if (record->owner_id == 0u ||
            record->value_bytes > BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES ||
            !overlay_owner_allowed(overlay, record->owner_id,
                record->mutation_class)) return 0;
        for (prior = 0u; prior < index; ++prior) {
            if (overlay->records[prior].owner_id == record->owner_id &&
                overlay->records[prior].mutation_class == record->mutation_class &&
                overlay->records[prior].key == record->key) return 0;
        }
    }
    for (; index < BX_NTVDM_MUTATION_OVERLAY_V1_MAX_RECORDS; ++index) {
        const bx_ntvdm_mutation_overlay_record_v1 *record =
            &overlay->records[index];
        uint32_t byte;
        if (record->owner_id != 0u || record->mutation_class != 0u ||
            record->key != 0u || record->value_bytes != 0u) return 0;
        for (byte = 0u; byte < BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES; ++byte) {
            if (record->value[byte] != 0u) return 0;
        }
    }
    return 1;
}

int bx_ntvdm_mutation_overlay_v1_initialize(
    bx_ntvdm_mutation_overlay_v1 *overlay,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    if (overlay == 0 || profile == 0 ||
        !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        profile->mode != BX_NTVDM_MUTATION_MODE_V1_OVERLAY) return 0;
    memset(overlay, 0, sizeof(*overlay));
    overlay->magic = BX_NTVDM_MUTATION_OVERLAY_V1_MAGIC;
    overlay->abi_version = BX_NTVDM_MUTATION_OVERLAY_V1_VERSION;
    overlay->struct_bytes = sizeof(*overlay);
    overlay->profile = *profile;
    return bx_ntvdm_mutation_overlay_v1_valid(overlay);
}

int bx_ntvdm_mutation_overlay_v1_record(
    bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, const uint8_t *value,
    uint32_t value_bytes)
{
    bx_ntvdm_mutation_overlay_record_v1 *record;
    uint32_t index;
    if (overlay == 0 || value == 0 || value_bytes == 0u ||
        value_bytes > BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES ||
        !bx_ntvdm_mutation_overlay_v1_valid(overlay) ||
        !overlay_owner_allowed(overlay, owner_id, mutation_class) ||
        overlay->record_count >= BX_NTVDM_MUTATION_OVERLAY_V1_MAX_RECORDS) return 0;
    for (index = 0u; index < overlay->record_count; ++index) {
        record = &overlay->records[index];
        if (record->owner_id == owner_id &&
            record->mutation_class == mutation_class && record->key == key)
            return 0;
    }
    record = &overlay->records[overlay->record_count++];
    record->owner_id = owner_id;
    record->mutation_class = mutation_class;
    record->key = key;
    record->value_bytes = value_bytes;
    memcpy(record->value, value, value_bytes);
    return bx_ntvdm_mutation_overlay_v1_valid(overlay);
}

int bx_ntvdm_mutation_overlay_v1_replace(
    bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, const uint8_t *value,
    uint32_t value_bytes)
{
    uint32_t index;
    if (overlay == 0 || value == 0 || value_bytes == 0u ||
        value_bytes > BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES ||
        !bx_ntvdm_mutation_overlay_v1_valid(overlay) ||
        !overlay_owner_allowed(overlay, owner_id, mutation_class)) return 0;
    for (index = 0u; index < overlay->record_count; ++index) {
        bx_ntvdm_mutation_overlay_record_v1 *record = &overlay->records[index];
        if (record->owner_id == owner_id &&
            record->mutation_class == mutation_class && record->key == key) {
            memset(record->value, 0, sizeof(record->value));
            memcpy(record->value, value, value_bytes);
            record->value_bytes = value_bytes;
            return bx_ntvdm_mutation_overlay_v1_valid(overlay);
        }
    }
    return bx_ntvdm_mutation_overlay_v1_record(overlay, owner_id,
        mutation_class, key, value, value_bytes);
}

int bx_ntvdm_mutation_overlay_v1_lookup(
    const bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, uint8_t *value,
    uint32_t value_capacity, uint32_t *value_bytes)
{
    uint32_t index;
    if (value_bytes == 0 || !bx_ntvdm_mutation_overlay_v1_valid(overlay) ||
        !overlay_owner_allowed(overlay, owner_id, mutation_class)) return 0;
    for (index = 0u; index < overlay->record_count; ++index) {
        const bx_ntvdm_mutation_overlay_record_v1 *record =
            &overlay->records[index];
        if (record->owner_id == owner_id &&
            record->mutation_class == mutation_class && record->key == key) {
            if (value == 0 || value_capacity < record->value_bytes) return 0;
            memcpy(value, record->value, record->value_bytes);
            *value_bytes = record->value_bytes;
            return 1;
        }
    }
    return 0;
}

void bx_ntvdm_mutation_overlay_v1_teardown(
    bx_ntvdm_mutation_overlay_v1 *overlay)
{
    if (overlay != 0) memset(overlay, 0, sizeof(*overlay));
}
