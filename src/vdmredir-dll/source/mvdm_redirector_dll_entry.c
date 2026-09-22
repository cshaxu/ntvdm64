/*
 * The original VDMREDIR sources manifest selects
 * DLLENTRY=VrDllInitialize.  Its NT4 build supplied that function with the
 * loader's stdcall ABI.  The mirrored source is deliberately preserved with
 * its original cdecl spelling; this standalone-only adapter supplies the
 * modern loader ABI and immediately transfers to the original initializer.
 */

#include <windows.h>

extern BOOLEAN VrDllInitialize(PVOID DllHandle, ULONG Reason, PCONTEXT Context);

BOOL WINAPI
mvdm_vdmredir_dll_entry(
    HINSTANCE instance,
    DWORD reason,
    LPVOID reserved
    )
{
    return VrDllInitialize(instance, reason, reserved) ? TRUE : FALSE;
}
