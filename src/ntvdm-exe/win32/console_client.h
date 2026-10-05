#ifndef NTVDM_CONSOLE_CLIENT_H
#define NTVDM_CONSOLE_CLIENT_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "common/protocol/console_video.h"
#include "console_graphics.h"
ntvdm_console_graphics *ntvdm_console_graphics_context(void);
/* NULL description retires graphics for a text transition. Called only after
 * releasing the painter mutex; transport serializes the complete frame. */
BOOL ntvdm_console_publish_video(const console_video_description *,const void *,size_t);
DWORD ntvdm_console_client_begin(session *);
/* 0 unrelated, 1 input, 2 output; local identities, never UI resources. */
DWORD ntvdm_console_handle_kind(HANDLE);
HANDLE WINAPI MvdmCreateFileA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE WINAPI MvdmCreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
DWORD WINAPI MvdmGetFileType(HANDLE);
BOOL ntvdm_console_set_active(BOOL);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR);
HANDLE ntvdm_console_input_wait_handle(void);
BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *,HANDLE *);
BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID *,DWORD,LPDWORD,LPVOID);
BOOL ntvdm_console_prepend_keys(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
/* -1: no frontend bound; 0: remote failure; 1: copied result available. */
int ntvdm_console_window_query(DWORD query,LONG values[4]);
/* Latest original resolved text palette, owned by this worker endpoint. */
BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16]);
/* Guest owner toggles publication at route and final-paint boundaries. */
BOOL ntvdm_console_video_async(BOOL);
#endif
