#ifndef WOW_SHARED_USER_ABI_H
#define WOW_SHARED_USER_ABI_H
#include <windows.h>
#include <winternl.h>

/* One inclusion owner for the unguarded original declarations. Preserve
 * the mirror and let cleanup/client-layout consumers share the same ABI. */
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
/* The selected non-i386 CCPU WOW producer clears FASTCALL and uses /Gz.
 * SDK FASTCALL means register arguments on x86; it must not change the
 * four callbacks consumed through this shared table. Keep other USER
 * internal FASTCALL declarations outside this inclusion unchanged. */
#pragma push_macro("FASTCALL")
#undef FASTCALL
#define FASTCALL WINAPI
#include "non-mvdm/public/internal/windows/inc/wowuserp.h"
#pragma pop_macro("FASTCALL")
#pragma warning(pop)
#endif
