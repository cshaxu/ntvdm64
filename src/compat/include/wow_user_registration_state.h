#ifndef WOW_USER_REGISTRATION_STATE_H
#define WOW_USER_REGISTRATION_STATE_H
#include "wow_user_task_lifecycle.h"
#include "wow_user_registered_callbacks.h"
wow_user_task_lifecycle *WINAPI wow_user_registered_lifecycle(void);
void WINAPI wow_user_set_registered_callbacks(struct tagAPFNWOWHANDLERSIN *input);
/* Independent USER operations selected by the original registration ABI. */
BOOL wow_user_ensure_registration_lifecycle(void);
BOOL WINAPI registered_init_task(UINT, DWORD, DWORD, LPCSTR, LPCSTR, DWORD, DWORD, DWORD,
    DWORD, DWORD, DWORD, DWORD);
void wow_user_registration_trace(const char *, DWORD);
HWND WINAPI registered_create_window_ex(DWORD ex_style,
    LPCTSTR class_name, LPCTSTR window_name, DWORD style, int x, int y,
    int width, int height, HWND parent, HMENU menu, HANDLE instance,
    LPVOID param, DWORD flags);
ATOM WINAPI registered_register_class(PVOID a, LPDWORD b);
BOOL WINAPI registered_hung_handlers(VOID (APIENTRY *callback)(VOID), HANDLE event);
BOOL WINAPI registered_cleanup(HANDLE instance, DWORD task,
    PNEMODULESEG selectors, DWORD count);
DWORD WINAPI registered_full_handle(WORD handle);
VOID WINAPI registered_directed_yield(DWORD thread_id);
BOOL WINAPI registered_yield_task(void);
BOOL WINAPI registered_wait_for_message(HANDLE event);
VOID WINAPI registered_fill_window(HWND parent, HWND window, HDC dc,
    HANDLE brush);
#endif
