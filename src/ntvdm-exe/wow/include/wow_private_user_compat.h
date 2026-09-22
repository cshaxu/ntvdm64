#ifndef ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H

#include <windows.h>

WORD WINAPI wow_private_user_get_def_window_proc_bits(PBYTE bits,
    WORD byte_count);
int WINAPI wow_private_user_get_id_from_directory(PBYTE resource_bits,
    UINT resource_type);
HCURSOR WINAPI wow_private_user_load_create_cursor_icon(HANDLE module,
    LPTSTR module_name, DWORD expected_windows_version, LPCTSTR name,
    DWORD resource_size, PVOID resource_bits, LPTSTR resource_type,
    BOOL client_side);
HMENU WINAPI wow_private_user_load_create_menu(HANDLE module,
    LPTSTR name, CONST LPMENUTEMPLATE menu_template, DWORD resource_size,
    BOOL call_client);
DWORD WINAPI wow_private_user_get_menu_index(HMENU menu, HMENU submenu);
BOOL WINAPI wow_private_user_fill_window(HWND brush_window, HWND paint_window,
    HDC dc, HBRUSH brush);

#endif /* ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H */
