#ifndef WOW_USER_CLIENT_VIEW_LAYOUT_H
#define WOW_USER_CLIENT_VIEW_LAYOUT_H

#include <windows.h>
/* Public COM and private USER have unrelated CLIPDATA names. Keep both
 * original layouts, separating only the public declaration namespace. */
#pragma push_macro("tagCLIPDATA")
#pragma push_macro("CLIPDATA")
#define tagCLIPDATA tagWOW_PUBLIC_CLIPDATA
#define CLIPDATA WOW_PUBLIC_CLIPDATA
#include <ole2.h>
#pragma pop_macro("CLIPDATA")
#pragma pop_macro("tagCLIPDATA")
#include <stddef.h>
#include "non-mvdm/public/internal/windows/inc/w32w64.h"

/* Fixed x86 XP USER client ABI, not the current host USER internals. Keep
 * the original structure bodies in their original mirror owner. This header
 * supplies only their compile boundary; it owns no objects or handles. */
/* XP shared-pointer spellings in this fixed x86 client view. */
typedef BYTE *KPBYTE;
typedef INT *KPINT;
typedef ULONG_PTR *KPULONG_PTR;
/* Existing WOW help payload is declared by wow32_provider_private.h. */
typedef struct _WOW32_HLP *LPHLP;
typedef WCHAR *KLPWSTR;
/* XP user.h:702 opaque keyboard-table declaration; no table storage. */
typedef struct tagKBDTABLE_MULT_INTERNAL *PKBDTABLE_MULTI_INTERNAL;
/* XP windows/published/winuser.w:12626-12629. */
typedef struct tagMSGMASK { BYTE *rgb; DWORD cb; } MSGMASK, *PMSGMASK;
typedef struct tagDESKTOP *PDESKTOP;
typedef struct tagTHREADINFO *PWOW_CLIENT_THREADINFO;
typedef struct tagQ *PWOW_CLIENT_QUEUE;
typedef struct tagCLS *PCLS;
typedef struct tagSBINFO *PSBINFO;
typedef struct tagWND *PWND;
typedef LRESULT (APIENTRY *WNDPROC_PWND)(PWND, UINT, WPARAM, LPARAM);

#pragma warning(push)
#pragma warning(disable:4201)
#include "wow_shared_user_abi.h"
#include "wow_user_globals_binding.h"
#define WOW_ORIGINAL_CLIENT_VIEW_LAYOUT
/* The original-layout opaque pointer is not the cleanup/input adapter's
 * PTHREADINFO typedef. Restrict this spelling to the retained layout body. */
#pragma push_macro("PTHREADINFO")
#pragma push_macro("PTDB")
#pragma push_macro("PPROCESSINFO")
#pragma push_macro("PWOWTHREADINFO")
#define PTDB PWOW_CLIENT_TDB
#define PPROCESSINFO PWOW_CLIENT_PROCESSINFO
#define PWOWTHREADINFO PWOW_CLIENT_WOWTHREADINFO
#define PTHREADINFO PWOW_CLIENT_THREADINFO
#pragma push_macro("PQ")
#define PQ PWOW_CLIENT_QUEUE
#include "compat/include/opennt_user_multipass.h"
#pragma pop_macro("PQ")
#pragma pop_macro("PTHREADINFO")
#pragma pop_macro("PWOWTHREADINFO")
#pragma pop_macro("PPROCESSINFO")
#pragma pop_macro("PTDB")
#undef WOW_ORIGINAL_CLIENT_VIEW_LAYOUT
#pragma warning(pop)

/* Original declarations precede the registered-callback expression macros. */
#include "non-mvdm/windows/core/ntuser/client/globals.h"

#define WOW_VIEW_ASSERT(name, condition) typedef char name[(condition) ? 1 : -1]
WOW_VIEW_ASSERT(wow_view_x86, sizeof(void *) == 4);
WOW_VIEW_ASSERT(wow_view_proc_head_size, sizeof(PROCOBJHEAD) == 12);
WOW_VIEW_ASSERT(wow_view_menu_size, sizeof(MENU) == 88);
WOW_VIEW_ASSERT(wow_view_menu_handle, offsetof(MENU, head.h) == 0);
WOW_VIEW_ASSERT(wow_view_menu_self, offsetof(MENU, head.pSelf) == 16);
WOW_VIEW_ASSERT(wow_view_menu_task, offsetof(MENU, head.hTaskWow) == 8);
WOW_VIEW_ASSERT(wow_view_menu_capacity, offsetof(MENU, cAlloced) == 28);
WOW_VIEW_ASSERT(wow_view_menu_flags, offsetof(MENU, fFlags) == 20);
WOW_VIEW_ASSERT(wow_view_menu_count, offsetof(MENU, cItems) == 32);
WOW_VIEW_ASSERT(wow_view_menu_items, offsetof(MENU, rgItems) == 52);
WOW_VIEW_ASSERT(wow_view_item_size, sizeof(ITEM) == 76);
WOW_VIEW_ASSERT(wow_view_item_type, offsetof(ITEM, fType) == 0);
WOW_VIEW_ASSERT(wow_view_item_state, offsetof(ITEM, fState) == 4);
WOW_VIEW_ASSERT(wow_view_item_id, offsetof(ITEM, wID) == 8);
WOW_VIEW_ASSERT(wow_view_item_submenu, offsetof(ITEM, spSubMenu) == 12);
WOW_VIEW_ASSERT(wow_view_wnd_size, sizeof(WND) == 160);
WOW_VIEW_ASSERT(wow_view_desktop_info_size, sizeof(DESKTOPINFO) == 108);
WOW_VIEW_ASSERT(wow_view_cls_size, sizeof(CLS) == 92);
WOW_VIEW_ASSERT(wow_view_cls_atom, offsetof(CLS, atomClassName) == 4);
WOW_VIEW_ASSERT(wow_view_cls_atom_nv, offsetof(CLS, atomNVClassName) == 6);
WOW_VIEW_ASSERT(wow_view_cls_fnid, offsetof(CLS, fnid) == 8);
WOW_VIEW_ASSERT(wow_view_cls_task, offsetof(CLS, hTaskWow) == 20);
WOW_VIEW_ASSERT(wow_view_cls_flags, offsetof(CLS, CSF_flags) == 22);
WOW_VIEW_ASSERT(wow_view_cls_task_width, sizeof(((CLS *)0)->hTaskWow) == 2);
WOW_VIEW_ASSERT(wow_view_cls_flags_width, sizeof(((CLS *)0)->CSF_flags) == 2);
WOW_VIEW_ASSERT(wow_view_wc_size, sizeof(WC) == 8);
WOW_VIEW_ASSERT(wow_view_cls_style, offsetof(CLS, style) == 48);
WOW_VIEW_ASSERT(wow_view_cls_procedure, offsetof(CLS, lpfnWndProc) == 52);
WOW_VIEW_ASSERT(wow_view_cls_extra, offsetof(CLS, cbclsExtra) == 56);
WOW_VIEW_ASSERT(wow_view_cls_window_extra, offsetof(CLS, cbwndExtra) == 60);
WOW_VIEW_ASSERT(wow_view_cls_window_count,
    offsetof(CLS, cWndReferenceCount) == 44);
WOW_VIEW_ASSERT(wow_view_cls_module, offsetof(CLS, hModule) == 64);
WOW_VIEW_ASSERT(wow_view_cls_ansi_name, offsetof(CLS, lpszAnsiClassName) == 84);
WOW_VIEW_ASSERT(wow_view_head_handle, offsetof(HEAD, h) == 0);
WOW_VIEW_ASSERT(wow_view_self, offsetof(WND, head.pSelf) == 16);
WOW_VIEW_ASSERT(wow_view_next, offsetof(WND, spwndNext) == 44);
WOW_VIEW_ASSERT(wow_view_parent, offsetof(WND, spwndParent) == 52);
WOW_VIEW_ASSERT(wow_view_child, offsetof(WND, spwndChild) == 56);
WOW_VIEW_ASSERT(wow_view_owner, offsetof(WND, spwndOwner) == 60);
WOW_VIEW_ASSERT(wow_view_window_rect, offsetof(WND, rcWindow) == 64);
WOW_VIEW_ASSERT(wow_view_client_rect, offsetof(WND, rcClient) == 80);
WOW_VIEW_ASSERT(wow_view_class, offsetof(WND, pcls) == 100);
WOW_VIEW_ASSERT(wow_view_menu, offsetof(WND, spmenu) == 120);
WOW_VIEW_ASSERT(wow_view_fnid, offsetof(WND, fnid) == 42);
WOW_VIEW_ASSERT(wow_view_exstyle, offsetof(WND, ExStyle) == 28);
WOW_VIEW_ASSERT(wow_view_style, offsetof(WND, style) == 32);
WOW_VIEW_ASSERT(wow_view_desktop_window, offsetof(DESKTOPINFO, spwnd) == 8);
WOW_VIEW_ASSERT(wow_view_ww_size, sizeof(WW) == 24);
WOW_VIEW_ASSERT(wow_view_ww_state, offsetof(WND, state) == sizeof(THRDESKHEAD));
WOW_VIEW_ASSERT(wow_view_prev, offsetof(WND, spwndPrev) == 48);
#undef WOW_VIEW_ASSERT

#endif
