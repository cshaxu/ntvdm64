#include <windows.h>

/*
 * Original owner: base/mvdm/v86/scaffold/i386/softpc.c.  The full scaffold
 * also owns entry and PC/AT setup, which this trace fixture must not import.
 * The admitted non-WOW profile requires only this already-existing host state.
 */
BOOL VDMForWOW = FALSE;
