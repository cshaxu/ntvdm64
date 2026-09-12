#ifndef ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H

#include <windows.h>

/* The NT4 USER registration slot supplies a callback from its private hung
 * application broker back into WOW32. The standalone product has no such
 * broker; this adapter preserves successful registration as an inert local
 * capability rather than exposing a fabricated USER service. */
BOOL wow_private_user_register_hung_app_handlers(void *end_task_callback,
    HANDLE wowexec_event);
WORD WINAPI wow_private_user_get_def_window_proc_bits(PBYTE bits,
    WORD byte_count);
BOOL WINAPI wow_private_user_init_task(UINT expected_windows_version,
    LPCSTR application_name, DWORD wow_task, DWORD hotkey, DWORD task_id,
    DWORD x, DWORD y, DWORD width, DWORD height, WORD show_command);
BOOL WINAPI wow_private_user_yield_task(void);
VOID WINAPI wow_private_user_directed_yield(DWORD thread_id);
BOOL WINAPI wow_private_user_cleanup(HANDLE instance, DWORD wow_task,
    void *module_segments, DWORD module_segment_count);
int WINAPI wow_private_user_get_id_from_directory(PBYTE resource_bits,
    UINT resource_type);
HCURSOR WINAPI wow_private_user_load_create_cursor_icon(HANDLE module,
    LPTSTR module_name, DWORD expected_windows_version, LPCTSTR name,
    DWORD resource_size, PVOID resource_bits, LPTSTR resource_type,
    BOOL client_side);
HMENU WINAPI wow_private_user_load_create_menu(HANDLE module,
    LPTSTR name, CONST LPMENUTEMPLATE menu_template, DWORD resource_size,
    BOOL call_client);

#endif /* ADAPTER_MVDM_HOST_OUT_WOW_PRIVATE_USER_COMPAT_H */
