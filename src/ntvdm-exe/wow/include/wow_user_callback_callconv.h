#ifndef ADAPTER_MVDM_HOST_OUT_WOW_USER_CALLBACK_CALLCONV_H
#define ADAPTER_MVDM_HOST_OUT_WOW_USER_CALLBACK_CALLCONV_H

#include <windows.h>

VOID WINAPI mvdm_wow_user_empty_clipboard(VOID);
VOID WINAPI mvdm_wow_user_set_fake_dialog_class(HWND window);
VOID WINAPI mvdm_wow_user_cb_store_handle(WORD format, WORD handle16);
BOOL WINAPI mvdm_wow_user_global_free_hook(HGLOBAL memory);

#endif /* ADAPTER_MVDM_HOST_OUT_WOW_USER_CALLBACK_CALLCONV_H */
