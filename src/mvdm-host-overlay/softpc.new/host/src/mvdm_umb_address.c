#include "mvdm_umb_address.h"

#include <stdint.h>

BOOL mvdm_umb_address_decode(PVOID encoded, DWORD *address)
{
    uintptr_t value = (uintptr_t)encoded;

    if (address == NULL || value > UINT32_MAX) return FALSE;
    *address = (DWORD)value;
    return TRUE;
}

PVOID mvdm_umb_address_encode(DWORD address)
{
    return (PVOID)(uintptr_t)address;
}
