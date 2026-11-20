#ifndef WOW_COMMON_DIALOG_CALLS_H
#define WOW_COMMON_DIALOG_CALLS_H
#include <windows.h>
#include <commdlg.h>

BOOL WINAPI wow_native_choose_color(LPCHOOSECOLORA);
BOOL WINAPI wow_native_choose_font(LPCHOOSEFONTA);
BOOL WINAPI wow_native_open_file(LPOPENFILENAMEA);
BOOL WINAPI wow_native_save_file(LPOPENFILENAMEA);
HWND WINAPI wow_native_find_text(LPFINDREPLACEA);
HWND WINAPI wow_native_replace_text(LPFINDREPLACEA);
BOOL WINAPI wow_native_print_dialog(LPPRINTDLGA);

/* ADAPTER-WOW-097: only original wcommdlg receives these native-leaf
 * bindings, after SDK declarations. Conversion and COMMDLGTD stay original. */
#ifdef WOW_COMMON_DIALOG_BINDING
#define ChooseColorA wow_native_choose_color
#define ChooseFontA wow_native_choose_font
#define GetOpenFileNameA wow_native_open_file
#define GetSaveFileNameA wow_native_save_file
#define FindTextA wow_native_find_text
#define ReplaceTextA wow_native_replace_text
#define PrintDlgA wow_native_print_dialog
#endif
#endif
