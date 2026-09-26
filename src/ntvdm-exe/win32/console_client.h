#ifndef NTVDM_CONSOLE_CLIENT_H
#define NTVDM_CONSOLE_CLIENT_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "product-abi/console_video.h"
#include "console_graphics.h"
ntvdm_console_graphics *ntvdm_console_graphics_context(void);
/* NULL description retires graphics for a text transition. Called only after
 * releasing the painter mutex; transport serializes the complete frame. */
BOOL ntvdm_console_publish_video(const console_video_description *,const void *,size_t);
DWORD ntvdm_console_client_begin(session *);
HANDLE ntvdm_console_input_wait_handle(void);
BOOL ntvdm_console_inherit_frontend_capability(HANDLE *);
BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID *,DWORD,LPDWORD,LPVOID);
BOOL ntvdm_console_prepend_keys(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
/* -1: no frontend bound; 0: remote failure; 1: copied result available. */
int ntvdm_console_window_query(DWORD query,LONG values[4]);
#endif
