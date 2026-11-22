#ifndef WOW_CONTROL_ENTRY_FIXTURE_PROVIDER_H
#define WOW_CONTROL_ENTRY_FIXTURE_PROVIDER_H
#include "compat/include/wow_edit_entry_bindings.h"
#include <winternl.h>
#include "compat/include/opennt_user_multipass.h"
/* Bind actual /Gz control bodies in /Gd fixtures, as product DLL attach does. */
LONG WINAPI fixture_EditWndProcA(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_EditWndProcA@16=_EditWndProcA@16")
LONG WINAPI fixture_EditWndProcW(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_EditWndProcW@16=_EditWndProcW@16")
LONG WINAPI fixture_ComboBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ComboBoxWndProcA@16=_ComboBoxWndProcA@16")
LONG WINAPI fixture_ComboBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ComboBoxWndProcW@16=_ComboBoxWndProcW@16")
LONG WINAPI fixture_ListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ListBoxWndProcA@16=_ListBoxWndProcA@16")
LONG WINAPI fixture_ListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ListBoxWndProcW@16=_ListBoxWndProcW@16")
LONG WINAPI fixture_ComboListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ComboListBoxWndProcA@16=_ComboListBoxWndProcA@16")
LONG WINAPI fixture_ComboListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ComboListBoxWndProcW@16=_ComboListBoxWndProcW@16")
LONG WINAPI fixture_ButtonWndProcA(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ButtonWndProcA@16=_ButtonWndProcA@16")
LONG WINAPI fixture_ButtonWndProcW(HWND, UINT, WPARAM, LPARAM);
#pragma comment(linker, "/alternatename:_fixture_ButtonWndProcW@16=_ButtonWndProcW@16")
LONG WINAPI fixture_xxxLBoxCaretBlinker(HWND, UINT, UINT, LONG);
#pragma comment(linker, "/alternatename:_fixture_xxxLBoxCaretBlinker@16=_xxxLBoxCaretBlinker@16")
BOOL WINAPI wow_original_list_caret_ready(struct tagWND *);
static const wow_control_entry_provider fixture_control_entry_provider = {
    fixture_EditWndProcA, fixture_EditWndProcW, fixture_ComboBoxWndProcA, fixture_ComboBoxWndProcW, fixture_ListBoxWndProcA, fixture_ListBoxWndProcW, fixture_ComboListBoxWndProcA, fixture_ComboListBoxWndProcW, fixture_ButtonWndProcA, fixture_ButtonWndProcW,
    fixture_xxxLBoxCaretBlinker, FNID_BUTTON, FNID_EDIT, FNID_COMBOBOX,
    FNID_LISTBOX, FNID_COMBOLISTBOX, ICLS_EDIT, ICLS_COMBOLISTBOX,
    wow_original_list_caret_ready
};
#endif
