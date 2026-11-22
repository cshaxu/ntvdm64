#include "include/wow_hard_error.h"

static wow_hard_error_fn *present;

BOOL __cdecl wow_hard_error_bind(wow_hard_error_fn *provider)
{
    PVOID previous;
    if (!provider) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&present, (PVOID)provider, NULL);
    if (previous && previous != (PVOID)provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}

LONG WINAPI wow_raise_hard_error(LONG status, ULONG count, ULONG mask,
    ULONG_PTR *parameters, ULONG options, ULONG *response)
{
    if (!present) return (LONG)0xC0000002L; /* STATUS_NOT_IMPLEMENTED */
    return present(status, count, mask, parameters, options, response);
}
