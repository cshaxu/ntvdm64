#ifndef MVDM_HOST_OVERLAY_COMMAND_WRITE_LENGTH_H
#define MVDM_HOST_OVERLAY_COMMAND_WRITE_LENGTH_H

#include <windows.h>
#include <stddef.h>

/* Win32 WriteFile accepts a DWORD count.  This boundary keeps the native
 * host-side size_t value intact until the selected COMMAND owner decides how
 * an unrepresentable value follows its existing failure direction. */
BOOL mvdm_command_write_length_to_dword(size_t value, DWORD *result);

#endif
