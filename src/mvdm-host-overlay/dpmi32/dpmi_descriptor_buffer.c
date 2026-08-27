#include "dpmi_descriptor_buffer.h"

#include <stdlib.h>

#include "mvdm_protected_span.h"

static int descriptor_byte_count(uint32_t count, uint32_t entry_bytes,
    uint32_t *byte_count_out)
{
    if (byte_count_out != NULL) *byte_count_out = 0u;
    if (byte_count_out == NULL || count == 0u || entry_bytes == 0u ||
        count > UINT32_MAX / entry_bytes) return 0;
    *byte_count_out = count * entry_bytes;
    return 1;
}

int mvdm_dpmi_descriptor_buffer_load(uint16_t selector, uint32_t offset,
    uint32_t count, uint32_t entry_bytes, void **entries_out)
{
    uint32_t byte_count;
    void *entries;
    if (entries_out != NULL) *entries_out = NULL;
    if (entries_out == NULL || !descriptor_byte_count(count, entry_bytes,
        &byte_count)) return 0;
    entries = calloc(1u, byte_count);
    if (entries == NULL || !mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_READ,
        selector, offset,
        byte_count, (uint8_t *)entries)) {
        free(entries);
        return 0;
    }
    *entries_out = entries;
    return 1;
}

int mvdm_dpmi_descriptor_buffer_store(uint16_t selector, uint32_t offset,
    uint32_t count, uint32_t entry_bytes, const void *entries)
{
    uint32_t byte_count;
    return entries != NULL && descriptor_byte_count(count, entry_bytes,
        &byte_count) && mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_WRITE,
        selector, offset,
        byte_count, (uint8_t *)entries);
}

void mvdm_dpmi_descriptor_buffer_dispose(void *entries)
{
    free(entries);
}
