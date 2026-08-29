#ifndef MVDM_HOST_OVERLAY_COMMAND_LENGTH_H
#define MVDM_HOST_OVERLAY_COMMAND_LENGTH_H

#include <windows.h>
#include <stddef.h>

/* The COMMAND mirrors retain their DOS/VDM USHORT and DWORD length fields.
 * These helpers keep a native host-side size_t intact until that fixed-width
 * source contract is checked at its actual boundary. */
BOOL mvdm_command_length_to_ushort(size_t value, USHORT *result);
BOOL mvdm_command_length_to_dword(size_t value, DWORD *result);
BOOL mvdm_command_length_add_dword(DWORD left, size_t right, DWORD *result);

#endif
