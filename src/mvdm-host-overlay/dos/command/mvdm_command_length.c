#include "mvdm_command_length.h"

#include <limits.h>
#include <stdint.h>

BOOL mvdm_command_length_to_ushort(size_t value, USHORT *result)
{
    if (result == NULL || value > USHRT_MAX) {
        return FALSE;
    }

    *result = (USHORT)value;
    return TRUE;
}

BOOL mvdm_command_length_to_dword(size_t value, DWORD *result)
{
    if (result == NULL || value > UINT32_MAX) {
        return FALSE;
    }

    *result = (DWORD)value;
    return TRUE;
}

BOOL mvdm_command_length_add_dword(DWORD left, size_t right, DWORD *result)
{
    if (right > UINT32_MAX - left) {
        return FALSE;
    }

    return mvdm_command_length_to_dword((size_t)left + right, result);
}
