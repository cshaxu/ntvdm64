#ifndef WOW_CONTROL_CLIENT_PRECOMP_H
#define WOW_CONTROL_CLIENT_PRECOMP_H

#ifdef WOW_ORIGINAL_DEFAULT_WINDOW
#include "default-precomp.h"
#else

/* ADAPTER-WOW-058/063: composition point for the admitted original EDIT,
 * combo/list and dialog client group. No algorithm or native private layout
 * is supplied here. Not a standalone USER build environment.
 * Dialog translation units additionally define WOW_ORIGINAL_DIALOG_CLIENT
 * and WOW_ORIGINAL_DIALOG_MANAGER; the shared client bindings are required
 * for those units too. Production selection awaits the whole dependency
 * closure, not merely successful inclusion of this header. */
#if defined(WOW) || defined(DEBUG)
#error Recovered native control clients require the Release host ABI
#endif
#ifndef WOW_ORIGINAL_EDIT_CLIENT
#error Select the shared original control-client bindings explicitly
#endif
#ifdef WOW_ORIGINAL_DIALOG_CLIENT
#define WOW_ORIGINAL_BUTTON_STATE
#include "wow_button_lifetime.h"
#endif

#include "wow_user_client_view_layout.h"
#include <limits.h> /* Original client/precomp.h: ListBox signed bounds. */
/* Original client/precomp.h includes windowsx.h. Its SelectFont/DeleteFont
 * macros call the public GDI object API; do not declare substitute functions. */
#include <windowsx.h>
/* OPENNT-HOST-110: original user.h prefix contract, no parser here. */
#define CH_PREFIX TEXT('&')
#define MENUSYSMENU TEXT(' ') /* Original user.h system-menu mnemonic. */
LONG GetPrefixCount(LPCWSTR lpstr, int cb, LPWSTR lpstrCopy, int cbCopy);
#include "wow_task_init_bindings.h"
#define CI_16BIT WOW_TASK_CI_16BIT
#define TIF_16BIT WOW_TASK_TIF_16BIT
#ifndef VER40
#define VER40 0x0400
#endif
#include "opennt-abi/host-compat/include/wow_control_resource_abi.h"
/* Resource sizes include original two-byte packing; the packing header
 * must restore the surrounding ABI before any owned state is declared. */
C_ASSERT(sizeof(DLGTEMPLATE2) == 26);
C_ASSERT(sizeof(DLGITEMTEMPLATE2) == 24);
#define WOW_ORIGINAL_COMBO_STATE_LAYOUT
#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#define WOW_ORIGINAL_CONTROL_PROTOTYPES
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "wow_edit_call_bindings.h"
/* OPENNT-HOST-118: original construction, bounded native storage service. */
VOID RtlInitLargeAnsiString(PLARGE_ANSI_STRING, LPCSTR, UINT);
VOID RtlInitLargeUnicodeString(PLARGE_UNICODE_STRING, LPCWSTR, UINT);
BOOL WINAPI _DefSetText(HWND, LPCWSTR, BOOL);
#define NtUserDefSetText wow_edit_NtUserDefSetText
#include "wow_edit_memory_bindings.h"
#include "wow_edit_drawing_bindings.h"
#include "wow_control_draw_state.h"
/* Original list/button caller -> original effects, not host DrawState. */
#undef DrawState
#define DrawState wow_control_draw_state
#include "wow_edit_native_bindings.h"
#ifdef WOW_ORIGINAL_CLIENT_CPD
/* OPENNT-HOST-120: outgoing native token allocation only. The original
 * client retains its HW conversion; never reinterpret a native CPD. */
#define NtUserGetCPD wow_edit_native_NtUserGetCPD
#endif
#include "wow_dialog_state_bindings.h"
#include "wow_dialog_procedure_bindings.h"
#define WOW_NATIVE_DIALOG_PROCEDURE_OWNER
#include "wow_window_client_state_bindings.h"
#if defined(WOW_ORIGINAL_PRIVATE_WORD_GETTER) || defined(WOW_ORIGINAL_DIALOG_GETSET)
/* Selected callers validate and retain local original state before entering
 * these private-word owners. Resource-only observations are not process-owned
 * control storage and must not authorize dereferencing original PED/PDLG. */
#define TestWindowProcess wow_window_client_owns_state
#endif
#ifdef WOW_ORIGINAL_DIALOG_GETSET
#include "opennt-host/windows/core/ntuser/inc/ntsend.h"
#endif
#ifdef WOW_ORIGINAL_PRIVATE_WORD_SETTER
/* The selected original server body can access only our retained local
 * behavior state. Resource snapshots and foreign/system windows are not
 * an alternate path into native USER private storage. */
#define FCallerOk wow_window_client_owns_state
DWORD WINAPI xxxSetWindowLong(PWND, int, DWORD, BOOL);
DWORD WINAPI xxxSetWindowData(PWND, int, DWORD, BOOL);
#endif
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#undef WOW_ORIGINAL_CONTROL_PROTOTYPES
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#undef WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#undef WOW_ORIGINAL_COMBO_STATE_LAYOUT

/* General resource identity is distinct from the private-state validator.
 * Original usercli.h makes RevalidateHwnd exactly the NoRip operation. */
#define ValidateHwnd wow_window_client_validate_window
#define ValidateHwndNoRip wow_window_client_validate_window_no_rip
#define RevalidateHwnd(hwnd) ValidateHwndNoRip(hwnd)
#define ConnectIfNecessary() do { \
    if (!wow_edit_native_ensure_gui_thread()) return 0; \
} while (0)

/* Resource/metric leaves match the existing original drawing cohort.
 * Retained client procedures own geometry and painting decisions. */
#define ahbrSystem wow_edit_system_brushes
/* Original user.h SYSHBR expression, using the existing brush owner. */
#define SYSHBR(index) ahbrSystem[COLOR_##index]
#define ghbrGray wow_edit_gray_brush
#define ghFontSys ((HFONT)GetStockObject(SYSTEM_FONT))
#define SYSMET(index) GetSystemMetrics(SM_##index)
#ifndef WOW_ORIGINAL_DIALOG_CLIENT
#define rcScreen (wow_edit_current->screen_rect)
#endif
#define COLOR_MAX WOW_EDIT_COLOR_COUNT
/* The geometry context deliberately captures border units, not colors.
 * Query the actual host palette, as the existing bitmap binding does;
 * never read the zero/unpopulated geometry snapshot as a color table. */
#define SYSRGB(index) GetSysColor(COLOR_##index)
#define GdiGetCharDimensions wow_edit_native_GdiGetCharDimensions
#define GetCharWidthInfo wow_edit_native_GetCharWidthInfo
#define UserGetHFONT(dc) ((HFONT)GetCurrentObject((dc), OBJ_FONT))
#define UserGetMapMode GetMapMode
#define MM_HIGHEST_USER_ADDRESS wow_edit_native_highest_user_address()
#define UserPolyPatBlt wow_edit_native_PolyPatBlt
/* Original C sources use the platform SEH spellings. Preserve their
 * exception bodies and unwind, never turn these into ordinary branches. */
#ifndef try
#define try __try
#endif
#ifndef except
#define except __except
#endif
#ifndef finally
#define finally __finally
#endif
BOOL APIENTRY DrawFrame(HDC, PRECT, int, int);
BOOL APIENTRY CopyInflateRect(LPRECT, CONST RECT *, int, int);
/* OPENNT-HOST-114/115 retain the original brush and mnemonic algorithms. */
BOOL WINAPI PaintRect(HWND, HWND, HDC, HBRUSH, LPRECT);
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int);
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
VOID WINAPI BltColor(HDC, HBRUSH, HDC, int, int, int, int, int, int, BOOL);
/* Original userrtl.h declaration, with the unchanged rtl/winmgr.c owner. */
BOOL NeedsWindowEdge(DWORD, DWORD, BOOL);
/* Original ntuser/inc/user.h declaration; rtl/winmgr.c owns the body. */
BOOL _FChildVisible(PWND pwnd);

#ifdef WOW_ORIGINAL_BUTTON_CLIENT
#include "wow_button_lifetime.h"
/* Keep the Button atlas/lifetime separate from nested DrawState resources.
 * The helper implementation remains in its original mngrayc.c owner. */
#undef oemInfo
#define oemInfo (*wow_button_current_resources())
#define BitBltSysBmp wow_button_BitBltSysBmp
#define NtUserBitBltSysBmp wow_button_atlas_blit
#define NtUserGetIconSize wow_control_icon_size
#define FreeLookasideEntry wow_button_free_state
VOID WINAPI DrawPushButton(HDC, LPRECT, UINT, UINT);
#endif

/* Original winerrp.h retail semantics: diagnostics do not evaluate their
 * formatting operands; RIPERR still sets the caller's Win32 error. */
#ifndef RIPMSG0
#define RIPMSG0(level, text) ((void)0)
#endif
#ifndef RIPMSG1
#define RIPMSG1(level, text, a) ((void)0)
#endif
#ifndef RIPMSG2
#define RIPMSG2(level, text, a, b) ((void)0)
#endif
#ifndef RIPMSG3
#define RIPMSG3(level, text, a, b, c) ((void)0)
#endif
#ifndef RIPERR0
#define RIPERR0(error, level, text) SetLastError(error)
#endif
#ifndef RIPERR1
#define RIPERR1(error, level, text, a) SetLastError(error)
#endif
#ifndef RIPERR2
#define RIPERR2(error, level, text, a, b) SetLastError(error)
#endif
#ifndef RIPERR3
#define RIPERR3(error, level, text, a, b, c) SetLastError(error)
#endif

/* Keep the dialog override last: its class/font/work-area inputs have the
 * dialog entry's lifetime, not the nested EDIT entry's context. */
/* ADAPTER-WOW-063: original API-shaped bodies are not USER32 import thunks.
 * Internal A-to-W calls stay in the original group; MVDM public calls enter
 * the task/borrow/context adapter through wow_user_private_access.h. */
#include "wow_dialog_api_bindings.h"
#undef MapDialogRect
#undef DlgDirListA
#undef DlgDirListComboBoxA
#undef DlgDirSelectExA
#undef DlgDirSelectComboBoxExA
#define MapDialogRect wow_original_MapDialogRect
#define DlgDirListA wow_original_DlgDirListA
#define DlgDirListW wow_original_DlgDirListW
#define DlgDirListComboBoxA wow_original_DlgDirListComboBoxA
#define DlgDirListComboBoxW wow_original_DlgDirListComboBoxW
#define DlgDirSelectExA wow_original_DlgDirSelectExA
#define DlgDirSelectExW wow_original_DlgDirSelectExW
#define DlgDirSelectComboBoxExA wow_original_DlgDirSelectComboBoxExA
#define DlgDirSelectComboBoxExW wow_original_DlgDirSelectComboBoxExW
#ifdef WOW_ORIGINAL_DIALOG_CLIENT
#include "wow_dialog_context_bindings.h"
#include "wow_dialog_creation_binding.h"
#ifdef WOW_ORIGINAL_DIALOG_TEMPLATE
#undef DialogBoxIndirectParamAorW
#define DialogBoxIndirectParamAorW wow_original_DialogBoxIndirectParamAorW
int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE, LPCDLGTEMPLATEW,
    HWND, DLGPROC, LPARAM, UINT);
int WINAPI InternalDialogBox(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
#endif
#ifdef WOW_ORIGINAL_DIALOG_DEFAULT
/* Keep original public-shaped bodies distinct from both SDK imports and
 * the adapter that selects recovered versus untouched native dialogs. */
#undef DefDlgProcA
#undef DefDlgProcW
#define DefDlgProcA wow_original_DefDlgProcA
#define DefDlgProcW wow_original_DefDlgProcW
LONG WINAPI wow_original_DefDlgProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI wow_original_DefDlgProcW(HWND, UINT, WPARAM, LPARAM);
#endif
#ifdef WOW_ORIGINAL_DIALOG_END
/* Compile the original implementation under its private body name. Calls
 * from other units must still enter wow_user_end_dialog's ownership scope. */
#undef EndDialog
#define EndDialog wow_original_EndDialog
BOOL WINAPI wow_original_EndDialog(HWND, int);
#endif
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
/* Define recovered bodies under private names, not SDK dllimport names. */
#undef IsDialogMessageA
#undef IsDialogMessageW
#define IsDialogMessageA wow_original_IsDialogMessageA
#define IsDialogMessageW wow_original_IsDialogMessageW
BOOL WINAPI wow_original_IsDialogMessageA(HWND, LPMSG);
BOOL WINAPI wow_original_IsDialogMessageW(HWND, LPMSG);
#endif
BOOL RtlMBMessageWParamCharToWCS(DWORD, PDWORD);
PWND GetParentDialog(PWND);
PWND xxxGotoNextMnem(PWND, PWND, WCHAR);
#undef CallMsgFilter
#define CallMsgFilter wow_native_CallMsgFilterW
/* Original 14-argument creator must enter the enrolled creation adapter,
 * never resolve directly to the modern 17-argument native service. */
#define NtUserCreateWindowEx wow_user_create_dialog_window_ex
/* Symbolic operation selection: never pass old NT4 enum values to USER. */
#define UserThreadStateIsForeground wow_dialog_current_foreground
#define WindowIsForegroundThread wow_dialog_window_foreground
#define ptCursor wow_dialog_cursor_position()
#define NtUserGetThreadState(operation) operation()
#define NtUserQueryWindow(window, operation) operation(window)
#undef gpsi
#undef oemInfo
#define gpsi (&wow_dialog_current->server)
#define oemInfo (wow_dialog_current->screen)
#endif
#endif /* !WOW_ORIGINAL_DEFAULT_WINDOW */
#endif
