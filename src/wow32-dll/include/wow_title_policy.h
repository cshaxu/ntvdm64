#ifndef WOW_TITLE_POLICY_H
#define WOW_TITLE_POLICY_H
#include <windows.h>
BOOL WINAPI wow_title_policy(DWORD *policy);
BOOL WINAPI wow_style_creation_begin(DWORD policy,void **scope);
DWORD WINAPI wow_style_creation_end(void *scope);
BOOL WINAPI wow_style_window_terminal(HWND);
BOOL WINAPI wow_style_hit_test(HWND,LPARAM,LRESULT *);
int WINAPI wow_style_set_scroll_pos(HWND,int,int,BOOL);
BOOL WINAPI wow_style_set_scroll_range(HWND,int,int,int,BOOL);
BOOL WINAPI wow_style_show_scroll_bar(HWND,int,BOOL);
BOOL WINAPI wow_style_enable_scroll_bar(HWND,UINT,UINT);
INT_PTR WINAPI wow_title_modal(HINSTANCE module,LPCDLGTEMPLATEW resource,HWND owner,
    DLGPROC procedure,LPARAM parameter,UINT flags,DWORD policy);
int WINAPI wow_title_message_box_a(HWND owner,LPCSTR text,LPCSTR caption,UINT type);
int WINAPI wow_style_shell_about_a(HWND owner,LPCSTR application,LPCSTR text,HICON icon);
#ifdef WOW_STYLE_SHELL_BINDING
#include <shellapi.h>
#undef ShellAbout
#define ShellAbout wow_style_shell_about_a
#endif
#ifdef WOW_TITLE_MESSAGE_BINDING
#undef MessageBox
#define MessageBox wow_title_message_box_a
#endif
#ifdef WOW_STYLE_SCROLL_BINDING
#define SetScrollPos wow_style_set_scroll_pos
#define SetScrollRange wow_style_set_scroll_range
#define ShowScrollBar wow_style_show_scroll_bar
#define EnableScrollBar wow_style_enable_scroll_bar
#endif
#endif
