/*
 * The original WOW32 bodies retain cdecl source definitions, while the
 * historical USER callback table invokes these four slots with WINAPI.
 * Keep the original algorithms and provide only the required ABI bridge.
 */
#include "mvdm/wow32/precomp.h"
#include "ntvdm-exe/wow/include/wow_user_callback_callconv.h"

VOID WINAPI mvdm_wow_user_empty_clipboard(VOID)
{
    W32EmptyClipboard();
}

VOID WINAPI mvdm_wow_user_set_fake_dialog_class(HWND window)
{
    SetFakeDialogClass(window);
}

VOID WINAPI mvdm_wow_user_cb_store_handle(WORD format, WORD handle16)
{
    WU32ICBStoreHandle(format, handle16);
}

BOOL WINAPI mvdm_wow_user_global_free_hook(HGLOBAL memory)
{
    return W32DDEFreeGlobalMem32(memory);
}
