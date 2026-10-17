#ifndef WOW_DIALOG_API_BINDINGS_H
#define WOW_DIALOG_API_BINDINGS_H
#include <windows.h>
BOOL WINAPI wow_original_MapDialogRect(HWND, LPRECT);
int WINAPI wow_original_DlgDirListA(HWND, LPSTR, int, int, UINT);
int WINAPI wow_original_DlgDirListW(HWND, LPWSTR, int, int, UINT);
int WINAPI wow_original_DlgDirListComboBoxA(HWND, LPSTR, int, int, UINT);
int WINAPI wow_original_DlgDirListComboBoxW(HWND, LPWSTR, int, int, UINT);
BOOL WINAPI wow_original_DlgDirSelectExA(HWND, LPSTR, int, int);
BOOL WINAPI wow_original_DlgDirSelectExW(HWND, LPWSTR, int, int);
BOOL WINAPI wow_original_DlgDirSelectComboBoxExA(HWND, LPSTR, int, int);
BOOL WINAPI wow_original_DlgDirSelectComboBoxExW(HWND, LPWSTR, int, int);
BOOL WINAPI wow_user_map_dialog_rect(HWND, LPRECT);
int WINAPI wow_user_dlg_dir_listA(HWND, LPSTR, int, int, UINT);
int WINAPI wow_user_dlg_dir_list_comboA(HWND, LPSTR, int, int, UINT);
BOOL WINAPI wow_user_dlg_dir_selectA(HWND, LPSTR, int, int);
BOOL WINAPI wow_user_dlg_dir_select_comboA(HWND, LPSTR, int, int);
#endif
