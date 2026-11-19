#ifndef WOW_GDI_ALIAS_H
#define WOW_GDI_ALIAS_H

#include <windows.h>

/* The original WOW GDI handle is a 14-bit index shifted left two places.
 * NT4 obtained its current native value from an internal GDI table.  Modern
 * GDI does not expose that table, so this is the one worker-local substitute
 * for that unavailable lookup boundary.  These functions deliberately carry
 * no type or ownership policy: native GDI remains the validator and the
 * worker process remains the cleanup boundary. */
WORD __cdecl mvdm_wow_gdi_handle_from_native(HANDLE);
HANDLE __cdecl mvdm_wow_gdi_handle_to_native(WORD);
void __cdecl mvdm_wow_gdi_handle_retire(WORD);

#endif
