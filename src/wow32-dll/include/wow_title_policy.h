#ifndef WOW_TITLE_POLICY_H
#define WOW_TITLE_POLICY_H
#include <windows.h>
BOOL WINAPI wow_title_policy(DWORD *policy);
BOOL WINAPI wow_title_apply(HWND window,DWORD policy);
INT_PTR WINAPI wow_title_modal(HINSTANCE module,LPCDLGTEMPLATEW resource,HWND owner,
    DLGPROC procedure,LPARAM parameter,BOOL ansi,DWORD policy);
int WINAPI wow_title_message_box_a(HWND owner,LPCSTR text,LPCSTR caption,UINT type);
#ifdef WOW_TITLE_MESSAGE_BINDING
#undef MessageBox
#define MessageBox wow_title_message_box_a
#endif
#endif
