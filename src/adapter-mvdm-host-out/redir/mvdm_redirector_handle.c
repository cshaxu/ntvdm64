#include "mvdm_redirector_handle.h"

#include <stdint.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_host_identity.h"

HANDLE mvdm_redirector_handle_from_words(WORD high_word, WORD low_word)
{
    uintptr_t native_value;
    uint32_t identity = ((uint32_t)high_word << 16) | (uint32_t)low_word;
    if (identity == 0u || !mvdm_host_identity_resolve(identity, &native_value)) {
        SetLastError(ERROR_INVALID_HANDLE);
        return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)native_value;
}
