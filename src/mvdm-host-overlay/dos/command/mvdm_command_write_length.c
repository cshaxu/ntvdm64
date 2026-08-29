#include "mvdm_command_write_length.h"

#include <stdint.h>

BOOL mvdm_command_write_length_to_dword(size_t value, DWORD *result)
{
    if (result == NULL || value > UINT32_MAX) {
        return FALSE;
    }

    *result = (DWORD)value;
    return TRUE;
}
