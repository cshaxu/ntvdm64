#include "mvdm_redirector_handle.h"

#include <stdint.h>

HANDLE mvdm_redirector_handle_from_words(WORD high_word, WORD low_word)
{
    uint32_t identity = ((uint32_t)high_word << 16) | (uint32_t)low_word;
    if (identity == 0u) {
        SetLastError(ERROR_INVALID_HANDLE);
        return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)(uintptr_t)identity;
}
