/*
 * Worker-local replacement contract for the unavailable NT4 GdiQueryTable.
 *
 * The original WOW16 alias is only a 14-bit table index shifted by two.  It
 * has no guest-visible generation or type field: a released index may denote
 * a later object when the native table reuses it.  Keep that observable rule
 * explicit, while ensuring a released nonzero alias cannot be mistaken for
 * a NULL/screen DC by a native GDI call.
 */
#include <windows.h>
#include <stdio.h>

#include "ntvdm-exe/wow/include/wow_gdi_alias.h"

static int fail(const char *what)
{
    printf("FAIL %s\n", what);
    return 1;
}

int main(void)
{
    HANDLE first = (HANDLE)(ULONG_PTR)0x11111111;
    HANDLE replacement = (HANDLE)(ULONG_PTR)0x22222222;
    WORD alias;
    WORD i;

    if (mvdm_wow_gdi_handle_from_native(NULL) != 0 ||
        mvdm_wow_gdi_handle_to_native(0) != NULL) {
        return fail("zero");
    }

    alias = mvdm_wow_gdi_handle_from_native(first);
    if (alias == 0 || (alias & 3) != 0 ||
        mvdm_wow_gdi_handle_from_native(first) != alias ||
        mvdm_wow_gdi_handle_to_native(alias) != first) {
        return fail("stable-alias");
    }

    mvdm_wow_gdi_handle_retire(alias);
    if (mvdm_wow_gdi_handle_to_native(alias) != INVALID_HANDLE_VALUE) {
        return fail("retired-is-nonnull-invalid");
    }

    if (mvdm_wow_gdi_handle_from_native(replacement) != alias ||
        mvdm_wow_gdi_handle_to_native(alias) != replacement) {
        return fail("original-slot-reuse");
    }
    mvdm_wow_gdi_handle_retire(alias);

    for (i = 1; i < 0x4000; ++i) {
        HANDLE value = (HANDLE)(ULONG_PTR)(0x30000000u + i);
        if (mvdm_wow_gdi_handle_from_native(value) != (WORD)(i << 2)) {
            return fail("table-fill");
        }
    }
    SetLastError(ERROR_SUCCESS);
    if (mvdm_wow_gdi_handle_from_native((HANDLE)(ULONG_PTR)0x50000000) != 0 ||
        GetLastError() != ERROR_NOT_ENOUGH_MEMORY) {
        return fail("table-full");
    }

    printf("WOW_GDI_ALIAS errors=0\n");
    return 0;
}
