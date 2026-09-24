#ifndef WOW_USER_CLIENT_VIEW_LAYOUT_H
#define WOW_USER_CLIENT_VIEW_LAYOUT_H

#include <windows.h>
#include <stddef.h>

/* Fixed x86 NT4 USER client ABI, not the current host USER internals. Keep
 * the original structure bodies in their original mirror owner. This header
 * supplies only their compile boundary; it owns no objects or handles. */
typedef struct tagDESKTOP *PDESKTOP;
typedef struct tagTHREADINFO *PTHREADINFO;
typedef struct tagCLS *PCLS;
typedef struct tagSBINFO *PSBINFO;
typedef struct tagWND *PWND;
typedef LONG (APIENTRY *WNDPROC_PWND)(PWND, UINT, DWORD, LONG);
typedef VOID (APIENTRY *PFNW32ET)(VOID);

#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
/* Original Win4 profile: winuser.h WH_MIN=-1, WH_MAX=12. The modern SDK
 * adds hook types and must not silently enlarge the immutable guest ABI. */
#define CWINHOOKS 14
#define WOW_ORIGINAL_CLIENT_VIEW_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CLIENT_VIEW_LAYOUT
#undef CWINHOOKS
#pragma warning(pop)

#define WOW_VIEW_ASSERT(name, condition) typedef char name[(condition) ? 1 : -1]
WOW_VIEW_ASSERT(wow_view_x86, sizeof(void *) == 4);
WOW_VIEW_ASSERT(wow_view_wnd_size, sizeof(WND) == 176);
WOW_VIEW_ASSERT(wow_view_desktop_info_size, sizeof(DESKTOPINFO) == 100);
WOW_VIEW_ASSERT(wow_view_cls_size, sizeof(CLS) == 108);
WOW_VIEW_ASSERT(wow_view_cls_atom, offsetof(CLS, atomClassName) == 4);
WOW_VIEW_ASSERT(wow_view_cls_window_count,
    offsetof(CLS, cWndReferenceCount) == 20);
WOW_VIEW_ASSERT(wow_view_cls_wow_words, offsetof(CLS, adwWOW) == 36);
WOW_VIEW_ASSERT(wow_view_cls_module, offsetof(CLS, hModule) == 80);
WOW_VIEW_ASSERT(wow_view_head_handle, offsetof(HEAD, h) == 0);
WOW_VIEW_ASSERT(wow_view_self, offsetof(WND, head.pSelf) == 12);
WOW_VIEW_ASSERT(wow_view_next, offsetof(WND, spwndNext) == 20);
WOW_VIEW_ASSERT(wow_view_parent, offsetof(WND, spwndParent) == 24);
WOW_VIEW_ASSERT(wow_view_child, offsetof(WND, spwndChild) == 28);
WOW_VIEW_ASSERT(wow_view_owner, offsetof(WND, spwndOwner) == 32);
WOW_VIEW_ASSERT(wow_view_window_rect, offsetof(WND, rcWindow) == 40);
WOW_VIEW_ASSERT(wow_view_client_rect, offsetof(WND, rcClient) == 56);
WOW_VIEW_ASSERT(wow_view_class, offsetof(WND, pcls) == 76);
WOW_VIEW_ASSERT(wow_view_menu, offsetof(WND, spmenu) == 104);
WOW_VIEW_ASSERT(wow_view_fnid, offsetof(WND, fnid) == 122);
WOW_VIEW_ASSERT(wow_view_exstyle, offsetof(WND, ExStyle) == 164);
WOW_VIEW_ASSERT(wow_view_style, offsetof(WND, style) == 168);
WOW_VIEW_ASSERT(wow_view_desktop_window, offsetof(DESKTOPINFO, spwnd) == 8);
#undef WOW_VIEW_ASSERT

#endif
