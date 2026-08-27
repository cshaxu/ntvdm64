#include "dpmi_protected_buffer.h"

#include <stdlib.h>

#include "mvdm_protected_span.h"

int mvdm_dpmi_protected_buffer_load(uint16_t selector, uint32_t offset,
    uint32_t byte_count, void **bytes_out)
{
    void *bytes;
    if (bytes_out != NULL) *bytes_out = NULL;
    if (bytes_out == NULL || byte_count == 0u) return 0;
    bytes = calloc(1u, byte_count);
    if (bytes == NULL || !mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_READ,
        selector, offset, byte_count, (uint8_t *)bytes)) {
        free(bytes);
        return 0;
    }
    *bytes_out = bytes;
    return 1;
}

int mvdm_dpmi_protected_buffer_store(uint16_t selector, uint32_t offset,
    uint32_t byte_count, const void *bytes)
{
    return bytes != NULL && byte_count != 0u &&
        mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_WRITE, selector, offset,
            byte_count, (uint8_t *)bytes);
}

void mvdm_dpmi_protected_buffer_dispose(void *bytes)
{
    free(bytes);
}
