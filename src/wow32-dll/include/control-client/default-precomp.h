#ifndef WOW_DEFAULT_CLIENT_PRECOMP_H
#define WOW_DEFAULT_CLIENT_PRECOMP_H
/* OPENNT-HOST-089/090: finite environment for the original clmsg worker.
 * No EDIT context or native private USER layout belongs to this entry. */
#include "wow_user_client_view_layout.h"
#define WOW_ORIGINAL_CLIENT_HANDLES
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CLIENT_HANDLES
#include "wow_default_window_bindings.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_menu_resource_bindings.h"

/* Exact operands/types from pinned OpenNT user.h and winuserp.h. These
 * describe copied message data, not modern private resource structures. */
#define GetChildParent(pwnd) (TestwndChild(pwnd) ? pwnd->spwndParent : NULL)
#define DO_DROPFILE 0x454C4946L
#define WFMAXIMIZED             0x0F01
#define SWP_STATECHANGE     0x8000
#define SWP_NOCLIENTSIZE    0x0800
#define SWP_NOCLIENTMOVE    0x1000
/* The public SDK already supplies the identical SHELLHOOKINFO payload. */
#define WCSToMB(pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar, bAllocateMem) \
    WCSToMBEx(0, pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar, bAllocateMem)
BOOL _GetClientRect(PWND, LPRECT);
#endif
