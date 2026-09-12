/*
 * CPU40 WOW32 pointer-flush compatibility.
 *
 * Original `mvdm-host/inc/softpc.h` selects this exact no-op success contract
 * under the non-CPU30 CCPU profile:
 *
 *   #define Sim32FlushVDMPointer(address, size, buffer, mode) TRUE
 *
 * WOW32's historical declaration remains an external function spelling, so a
 * late-loaded DLL needs one concrete cdecl definition.  It owns no pointer,
 * mapping, guest state or cache; callback-frame writes use the separate
 * bounded `wow_callback_frame_lease` before this source-shaped completion.
 */

#include <windows.h>

BOOL Sim32FlushVDMPointer(ULONG address, USHORT size, PBYTE buffer, BOOL mode)
{
    (void)address;
    (void)size;
    (void)buffer;
    (void)mode;
    return TRUE;
}
