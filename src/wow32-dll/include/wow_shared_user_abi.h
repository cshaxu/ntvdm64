#ifndef WOW_SHARED_USER_ABI_H
#define WOW_SHARED_USER_ABI_H
#include <windows.h>
#include <winternl.h>

/* One inclusion owner for the unguarded original declarations. Preserve
 * the mirror and let cleanup/client-layout consumers share the same ABI. */
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)
#endif
