#ifndef WOW_WINDOW_ACCESS_UNREACHED_H
#define WOW_WINDOW_ACCESS_UNREACHED_H
#include "wow_window_access_provider.h"
/* Shared narrow fixture binding: real get/set entries; all unrelated calls fail. */
static HMENU WINAPI unexpected_menu(HINSTANCE module, LPCWSTR name)
{ (void)module; (void)name; ++failures; return NULL; }
static HWND WINAPI fixture_window_handle(struct tagWND *window)
{ return window->head.h; }
static VOID WINAPI unexpected_restore_words(struct _WW *words,
    const struct mvdm_softpc_wow_window_words_view *view)
{ (void)words; (void)view; ++failures; }
static const wow_window_layout *WINAPI unexpected_layout(void)
{ ++failures; return NULL;}

static VOID WINAPI unexpected_capture_words(const struct _WW * a0, struct mvdm_softpc_wow_window_words_view * a1)
{(void)a0; (void)a1; ++failures;}

static VOID WINAPI unexpected_initialize_state(struct tagWND * a0, const wow_window_state_initialization * a1)
{(void)a0; (void)a1; ++failures;}

static SIZE_T WINAPI unexpected_state_extent(const struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static WCHAR *WINAPI unexpected_promote_state(struct tagWND * a0, struct tagWND * a1, struct tagCLS * a2)
{(void)a0; (void)a1; (void)a2; ++failures; return NULL;}

static VOID WINAPI unexpected_initialize_resource(struct tagWND * a0, struct tagCLS * a1, const struct mvdm_softpc_wow_window_observation * a2, HWND a3)
{(void)a0; (void)a1; (void)a2; (void)a3; ++failures;}

static WCHAR *WINAPI unexpected_refresh_state(struct tagWND * a0, struct _WW * a1, const WINDOWINFO * a2, DWORD a3, DWORD a4, WCHAR * a5, ULONG a6, ULONG a7)
{(void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; ++failures; return NULL;}

static VOID WINAPI unexpected_publish_links(struct tagWND * a0, struct tagCLS * a1, ATOM a2, struct tagWND * a3, struct tagWND * a4, struct tagWND * a5, struct tagWND * a6)
{(void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; ++failures;}

static WCHAR *WINAPI unexpected_state_name(struct tagWND * a0)
{(void)a0; ++failures; return NULL;}

static WORD WINAPI unexpected_state_fnid(struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static WORD WINAPI unexpected_mark_destroyed(struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static BOOL WINAPI unexpected_creation_class_name(const struct _LARGE_STRING * a0, struct _UNICODE_STRING * a1)
{(void)a0; (void)a1; ++failures; return 0;}

static const wow_window_access_provider access_provider = {
    xxxSetWindowLong, _GetWindowLong, _SetWindowLong, unexpected_menu,
    fixture_window_handle, FNID_DEFWINDOWPROC, unexpected_restore_words,
    unexpected_layout, unexpected_capture_words, unexpected_initialize_state, unexpected_state_extent, unexpected_promote_state, unexpected_initialize_resource, unexpected_refresh_state, unexpected_publish_links, unexpected_state_name, unexpected_state_fnid, unexpected_mark_destroyed, unexpected_creation_class_name
};
#endif
