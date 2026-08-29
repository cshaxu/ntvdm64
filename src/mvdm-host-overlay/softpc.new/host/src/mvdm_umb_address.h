#ifndef MVDM_HOST_OVERLAY_UMB_ADDRESS_H
#define MVDM_HOST_OVERLAY_UMB_ADDRESS_H

#include <windows.h>

/* Legacy UMB APIs spell a guest physical address as PVOID.  It is never a
 * native host pointer in the selected SoftPC path. */
BOOL mvdm_umb_address_decode(PVOID encoded, DWORD *address);
PVOID mvdm_umb_address_encode(DWORD address);

#endif
