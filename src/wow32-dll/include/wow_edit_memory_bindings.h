#ifndef WOW_EDIT_MEMORY_BINDINGS_H
#define WOW_EDIT_MEMORY_BINDINGS_H
#include "wow_user_client_view_layout.h"
#include "wow_user_registered_callbacks.h"

/* ADAPTER-WOW-056: original globals.h's callback variables select the one
 * existing registered table. Original W32 callbacks retain native/guest
 * allocation, composite handles, parameter-map updates and word-break.
 * This header is private to the recovered EDIT/dialog translation units. */
#define pfnLocalAlloc (wow_user_registered_callbacks()->pfnLocalAlloc)
#define pfnLocalReAlloc (wow_user_registered_callbacks()->pfnLocalReAlloc)
#define pfnLocalLock (wow_user_registered_callbacks()->pfnLocalLock)
#define pfnLocalUnlock (wow_user_registered_callbacks()->pfnLocalUnlock)
#define pfnLocalSize (wow_user_registered_callbacks()->pfnLocalSize)
#define pfnLocalFree (wow_user_registered_callbacks()->pfnLocalFree)
#define pfn16GlobalAlloc (wow_user_registered_callbacks()->pfn16GlobalAlloc)
#define pfn16GlobalFree (wow_user_registered_callbacks()->pfn16GlobalFree)
#define pfnWowEditNextWord (wow_user_registered_callbacks()->pfnWowEditNextWord)
#define pfnWowEmptyClipBoard (wow_user_registered_callbacks()->pfnEmptyCB)
#define pfnGetExpWinVer (wow_user_registered_callbacks()->pfnGetExpWinVer)
/* OPENNT-HOST-081: use the already registered input table for original
 * creator/default-dialog callbacks, not a second registration owner. */
#define pfnInitDlgCallback (wow_user_registered_callbacks()->pfnInitDlgCb)
#define pfnWowSetFakeDialogClass (wow_user_registered_callbacks()->pfnWowSetFakeDialogClass)

/* Keep original argument order and the instance/DS and realloc pointer
 * contracts; do not substitute native Local* or a shadow text allocation. */
#include "opennt-host/windows/core/ntuser/client/globals.h"

/* Original retail UserAssert does not evaluate its argument. */
#ifndef UserAssert
#define UserAssert(condition) ((void)0)
#endif

HANDLE WINAPI GetEditDS(void);
VOID WINAPI ReleaseEditDS(HANDLE);
DWORD WINAPI WOWDlgInit(HWND, LONG);
#endif
