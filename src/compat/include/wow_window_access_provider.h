#ifndef WOW_WINDOW_ACCESS_PROVIDER_H
#define WOW_WINDOW_ACCESS_PROVIDER_H
#include <windows.h>
struct tagWND;
struct _UNICODE_STRING;
struct _WW;
struct mvdm_softpc_wow_window_words_view;

struct tagCLS;
struct _LARGE_STRING;
struct mvdm_softpc_wow_window_observation;
typedef struct wow_window_layout {
    SIZE_T state_bytes, words_bytes, class_bytes;
    ULONG word_count;
    DWORD ansi_creation_flag, ansi_creator_style;
    WORD procedure_handle_tag, dialog_fnid;
} wow_window_layout;
/* Call-local creation input; name transfers only with the existing successful
 * attachment. No second owner or persistent copy of original window state. */
typedef struct wow_window_state_initialization {
    HWND window;
    WINDOWINFO info;
    int class_extra_bytes;
    DWORD module_version, compatibility_flags, control_id;
    BOOL ansi_creator;
    WCHAR *name;
    ULONG name_length, name_capacity;
    const struct _WW *words;
} wow_window_state_initialization;

/* Original object operations, borrowed synchronously in the caller's existing
 * window scope. No private layout or additional object registry is exposed. */
typedef struct wow_window_access_provider {
    DWORD (WINAPI *set_long)(struct tagWND *, int, DWORD, BOOL);
    DWORD (WINAPI *get_long)(struct tagWND *, int, BOOL);
    LONG (WINAPI *set_client_long)(HWND, int, LONG, BOOL);
    HMENU (WINAPI *load_menu)(HINSTANCE, LPCWSTR);
    /* Read the existing borrowed object; never resolve a new HWND owner. */
    HWND (WINAPI *window_handle)(struct tagWND *);
    DWORD default_function;
    /* Same retained WW allocation; no new owner or guest address. */
    DWORD *(WINAPI *words)(struct _WW *);
    VOID (WINAPI *restore_words)(struct _WW *,
        const struct mvdm_softpc_wow_window_words_view *);
    const wow_window_layout *(WINAPI *layout)(void);
    VOID (WINAPI *capture_words)(const struct _WW *,
        struct mvdm_softpc_wow_window_words_view *);
    VOID (WINAPI *initialize_state)(struct tagWND *,
        const wow_window_state_initialization *);
    SIZE_T (WINAPI *state_extent)(const struct tagWND *);
    WCHAR *(WINAPI *promote_state)(struct tagWND *, struct tagWND *, struct tagCLS *);
    VOID (WINAPI *initialize_resource)(struct tagWND *, struct tagCLS *,
        const struct mvdm_softpc_wow_window_observation *, HWND);
    WCHAR *(WINAPI *refresh_state)(struct tagWND *, struct _WW *,
        const WINDOWINFO *, DWORD, DWORD, WCHAR *, ULONG, ULONG);
    VOID (WINAPI *publish_links)(struct tagWND *, struct tagCLS *, ATOM,
        struct tagWND *, struct tagWND *, struct tagWND *, struct tagWND *);
    WCHAR *(WINAPI *state_name)(struct tagWND *);
    WORD (WINAPI *state_fnid)(struct tagWND *);
    WORD (WINAPI *mark_destroyed)(struct tagWND *);
    BOOL (WINAPI *creation_class_name)(const struct _LARGE_STRING *,
        struct _UNICODE_STRING *);
} wow_window_access_provider;

BOOL WINAPI wow_window_access_bind(const wow_window_access_provider *);
const wow_window_access_provider *WINAPI wow_window_access_current(void);
#endif
