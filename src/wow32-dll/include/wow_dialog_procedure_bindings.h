#ifndef WOW_DIALOG_PROCEDURE_BINDINGS_H
#define WOW_DIALOG_PROCEDURE_BINDINGS_H
#include <windows.h>
/* ADAPTER-WOW-063: actual HWND owns these two public dialog slots. Caller
 * retains the recovered window and task; no native CPD layout is exposed. */
BOOL WINAPI wow_dialog_host_initialize(HWND);
LONG WINAPI wow_dialog_host_get(HWND, int, BOOL);
LONG WINAPI wow_dialog_host_set(HWND, int, LONG, BOOL);
LRESULT WINAPI wow_dialog_host_call(HWND, UINT, WPARAM, LPARAM, BOOL);
LRESULT WINAPI wow_dialog_host_notify(HWND, UINT, WPARAM, LPARAM, BOOL);
VOID WINAPI wow_dialog_host_retire(HWND);
/* Task-exit target: entered through the existing native-call boundary. */
LRESULT CALLBACK wow_dialog_host_terminal_proc(HWND, UINT, WPARAM, LPARAM);
#endif
