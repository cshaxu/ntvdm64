#ifndef WOW_WINDOW_ACCESS_FIXTURE_PROVIDER_H
#define WOW_WINDOW_ACCESS_FIXTURE_PROVIDER_H
#include "compat/include/wow_window_access_provider.h"
#include "compat/include/opennt_user_multipass.h"
/* Original headers inherit /Gz, while mixed host fixtures use /Gd. Link
 * explicitly typed aliases to the product symbols without changing header
 * inclusion order or other original declarations. */
DWORD WINAPI xxxSetWindowLong(struct tagWND *, int, DWORD, BOOL);
DWORD WINAPI fixture_GetWindowLong(struct tagWND *, int, BOOL);
LONG WINAPI fixture_SetWindowLong(HWND, int, LONG, BOOL);
#pragma comment(linker, "/alternatename:_fixture_GetWindowLong@12=__GetWindowLong@12")
#pragma comment(linker, "/alternatename:_fixture_SetWindowLong@16=__SetWindowLong@16")
HMENU WINAPI wow_user_load_menuW(HINSTANCE, LPCWSTR);
HWND WINAPI wow_original_window_handle(struct tagWND *);
VOID WINAPI wow_original_restore_window_words(struct _WW *, const struct mvdm_softpc_wow_window_words_view *);
const wow_window_layout *WINAPI wow_original_window_layout(void);
VOID WINAPI wow_original_capture_window_words(const struct _WW *, struct mvdm_softpc_wow_window_words_view *);
VOID WINAPI wow_original_initialize_window_state(struct tagWND *, const wow_window_state_initialization *);
SIZE_T WINAPI wow_original_window_state_extent(const struct tagWND *);
WCHAR *WINAPI wow_original_promote_window_state(struct tagWND *, struct tagWND *, struct tagCLS *);
VOID WINAPI wow_original_initialize_resource_window(struct tagWND *, struct tagCLS *, const struct mvdm_softpc_wow_window_observation *, HWND);
WCHAR *WINAPI wow_original_refresh_window_state(struct tagWND *, struct _WW *, const WINDOWINFO *, DWORD, DWORD, WCHAR *, ULONG, ULONG);
VOID WINAPI wow_original_publish_window_links(struct tagWND *, struct tagCLS *, ATOM, struct tagWND *, struct tagWND *, struct tagWND *, struct tagWND *);
WCHAR *WINAPI wow_original_window_name(struct tagWND *);
WORD WINAPI wow_original_window_fnid(struct tagWND *);
WORD WINAPI wow_original_mark_window_destroyed(struct tagWND *);
BOOL WINAPI wow_original_creation_class_name(const struct _LARGE_STRING *, struct _UNICODE_STRING *);
static const wow_window_access_provider wow_fixture_window_access_provider = {
    xxxSetWindowLong, fixture_GetWindowLong, fixture_SetWindowLong, wow_user_load_menuW,
    wow_original_window_handle, FNID_DEFWINDOWPROC,
    wow_original_restore_window_words, wow_original_window_layout, wow_original_capture_window_words, wow_original_initialize_window_state, wow_original_window_state_extent, wow_original_promote_window_state, wow_original_initialize_resource_window, wow_original_refresh_window_state, wow_original_publish_window_links, wow_original_window_name, wow_original_window_fnid, wow_original_mark_window_destroyed, wow_original_creation_class_name
};
#endif
