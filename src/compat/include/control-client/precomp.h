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

#include <windows.h>
#include <imm.h>
#include <winternl.h>
#pragma warning(push)
#define WOW_ORIGINAL_BUTTON_RESOURCES
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_BUTTON_RESOURCES
#pragma warning(pop)
#include "wow_user_client_view_layout.h"
#include <limits.h> /* Original client/precomp.h: ListBox signed bounds. */
/* Original client/precomp.h includes windowsx.h. Its SelectFont/DeleteFont
 * macros call the public GDI object API; do not declare substitute functions. */
#include <windowsx.h>
/* OPENNT-HOST-110: original user.h prefix contract, no parser here. */
#define CH_PREFIX TEXT('&')
#define CH_ENGLISHPREFIX 0x1E /* XP user.h English mnemonic prefix. */
#define MENUSYSMENU TEXT(' ') /* Original user.h system-menu mnemonic. */
LONG GetPrefixCount(LPCWSTR lpstr, int cb, LPWSTR lpstrCopy, int cbCopy);
#include "wow_task_init_bindings.h"
#define CI_16BIT WOW_TASK_CI_16BIT
#define TIF_16BIT WOW_TASK_TIF_16BIT
#ifndef VER40
#define VER40 0x0400
#endif
#include "compat/include/wow_control_resource_abi.h"
/* Resource sizes include original two-byte packing; the packing header
 * must restore the surrounding ABI before any owned state is declared. */
C_ASSERT(sizeof(DLGTEMPLATE2) == 26);
C_ASSERT(sizeof(DLGITEMTEMPLATE2) == 24);
#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "wow_edit_call_bindings.h"
/* OPENNT-HOST-118: original construction, bounded native storage service. */
VOID RtlInitLargeAnsiString(PLARGE_ANSI_STRING, LPCSTR, UINT);
BOOL WINAPI _DefSetText(HWND, LPCWSTR, BOOL);
#define NtUserDefSetText wow_edit_NtUserDefSetText
#include "wow_edit_memory_bindings.h"
#include "wow_edit_drawing_bindings.h"
#include "wow_control_draw_state.h"
/* Original list/button caller -> original effects, not host DrawState. */
#undef DrawState
#define DrawState wow_control_draw_state
#include "wow_edit_native_bindings.h"
#include "non-mvdm/windows/core/w32inc/w32gdip.h"
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
#ifdef WOW_ORIGINAL_PRIVATE_WORD_SETTER
/* The selected original server body can access only our retained local
 * behavior state. Resource snapshots and foreign/system windows are not
 * an alternate path into native USER private storage. */
#define FCallerOk wow_window_client_owns_state
DWORD WINAPI xxxSetWindowLong(PWND, int, DWORD, BOOL);
DWORD WINAPI xxxSetWindowData(PWND, int, DWORD, BOOL);
#endif
#include "compat/include/opennt_usercli_multipass.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#undef WOW_ORIGINAL_DIALOG_GETSET_LAYOUT

/* General resource identity is distinct from the private-state validator.
 * Original usercli.h makes RevalidateHwnd exactly the NoRip operation. */
#define ValidateHwnd wow_window_client_validate_window
#define ValidateHwndNoRip wow_window_client_validate_window_no_rip
#define RevalidateHwnd(hwnd) ValidateHwndNoRip(hwnd)
#define ConnectIfNecessary(...) do { \
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
#define gHighestUserAddress ((ULONG_PTR)wow_edit_native_highest_user_address())
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
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int, DWORD);
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
VOID WINAPI BltColor(HDC, HBRUSH, HDC, int, int, int, int, int, int, BOOL);
/* XP finite control inputs. Original algorithms remain in the client sources. */
#include "wow_char_conversion_abi.h"
#define THREAD_CODEPAGE() OpenNtUserThreadCodePage()
#define IS_DBCS_ENABLED() GetSystemMetrics(SM_DBCSENABLED)
WORD DbcsCombine(HWND, WORD);
#define CrackCombinedDbcsLB(c) ((BYTE)(c))
#define CrackCombinedDbcsTB(c) ((c) >> 8)
#define KPRECT_TO_PRECT(p) (p)
/* Original w32inc/w32wow64.h non-WOW64 pointer conversion. */
#define KPVOID_TO_PVOID(p) (p)
#define KHANDLE_TO_HANDLE(h) ((HANDLE)(h))
#define KHWND_TO_HWND(h) ((HWND)(h))
#define KERNEL_LRESULT_TO_LRESULT(v) ((LRESULT)(v))
#define KERNEL_INT_PTR_TO_INT_PTR(v) ((INT_PTR)(v))
#ifndef CW_FLAGS_ANSI
#define CW_FLAGS_ANSI 0x00000001
#endif
#ifndef PTR_TO_ID
#define PTR_TO_ID(p) ((USHORT)(((ULONG_PTR)(p)) & MAXWORD))
#endif
#ifndef IS_PTR
/* XP ntuser/inc/user.h:467. */
#ifndef MAXUSHORT
#define MAXUSHORT MAXWORD
#endif
#define IS_PTR(p) ((((ULONG_PTR)(p)) & ~MAXUSHORT) != 0)
#endif
#define NtUserGetComboBoxInfo wow_control_get_combo_info
#define NtUserTrackMouseEvent TrackMouseEvent
#define INDEX_COMBOBOX 0
#define INDEX_COMBOBOX_BUTTON 2
#define CMS_QANIMATION 165
#define GACF2_ANIMATIONOFF 0x00000001
#define PUSIF_COMBOBOXANIMATION 0x00000004
#define PUSIF_LISTBOXSMOOTHSCROLLING 0x00000008
#define GACF_NOSMOOTHSCROLLING 0x40000000
/* XP remote-session preference, queried from the native USER owner. */
#define ISREMOTESESSION() GetSystemMetrics(SM_REMOTESESSION)
#define PUSIF_UIEFFECTS 0x80000000
#define TEST_EffectPUSIF(f) ((gpsi->PUSIFlags & ((f) | PUSIF_UIEFFECTS)) == ((f) | PUSIF_UIEFFECTS))
#define _MonitorFromWindow(window, flags) wow_control_monitor(HWq(window), flags)
DWORD GetAppCompatFlags2(WORD);
/* Original userrtl.h declaration, with the unchanged rtl/winmgr.c owner. */
BOOL NeedsWindowEdge(DWORD, DWORD, BOOL);
/* Original ntuser/inc/user.h declaration; rtl/winmgr.c owns the body. */
BOOL _FChildVisible(PWND pwnd);

/* XP private UI flags are owned by native USER, not our retained NT4 WND.
 * Query its existing state through the same synchronous callout boundary. */
#define WEFLAYOUTRTL 0x0A40 /* Original XP WS_EX_LAYOUTRTL state bit. */
#define WEFPUIFOCUSHIDDEN 0x0B80
#define WEFPUIACCELHIDDEN 0x0B40
#undef TestWF
#define TestWF(window, flag) \
    ((flag) == WEFPUIFOCUSHIDDEN || (flag) == WEFPUIACCELHIDDEN ? \
        ((wow_native_DefWindowProcW(HWq(window), WM_QUERYUISTATE, 0, 0) & \
          ((flag) == WEFPUIFOCUSHIDDEN ? UISF_HIDEFOCUS : UISF_HIDEACCEL)) ? LOBYTE(flag) : 0) : \
        (*(((BYTE *)(window)) + STATEOFFSET + (int)HIBYTE(flag)) & LOBYTE(flag)))
#ifdef WOW_ORIGINAL_BUTTON_CLIENT
#include "wow_button_lifetime.h"
/* Keep the Button atlas/lifetime separate from nested DrawState resources.
 * The helper implementation remains in its original mngrayc.c owner. */
#undef oemInfo
#define oemInfo (*wow_button_current_resources())
#undef gpsi
#define gpsi wow_button_current_context()
#define MIRRORED_HDC(dc) ((GetLayout(dc) & LAYOUT_RTL) != 0)
#define PTR_TO_ID(p) ((USHORT)(((ULONG_PTR)(p)) & MAXWORD))
#define BitBltSysBmp wow_button_BitBltSysBmp
#define NtUserBitBltSysBmp wow_button_atlas_blit
#define NtUserGetIconSize wow_control_icon_size
#define FreeLookasideEntry wow_button_free_state
VOID WINAPI DrawPushButton(HDC, LPRECT, UINT, UINT);
#endif

/* Original winerrp.h retail semantics: diagnostics do not evaluate their
 * formatting operands; RIPERR still sets the caller's Win32 error. */
#ifndef TAGMSG0
#define TAGMSG0(...) ((void)0)
#endif
#ifndef FUNCLOG2
#define FUNCLOG2(...)
#endif
#ifndef FUNCLOG1
#define FUNCLOG1(...)
#endif
#ifndef FUNCLOG3
#define FUNCLOG3(...)
#endif
/* XP w32err.h:899, retail without PRERELEASE/USER_INSTRUMENTATION. */
#define FRE_RIPMSG0(flags, text)
#ifndef RIPMSG0
#define FUNCLOG6(...)
#define FUNCLOG4(...)
#define FUNCLOG5(...)
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
INT_PTR WINAPI InternalDialogBox(HANDLE, LPDLGTEMPLATE, HWND, DLGPROC, LPARAM, UINT);
#endif
#ifdef WOW_ORIGINAL_DIALOG_DEFAULT
/* Retain original helper bodies without defining SDK dllimport symbols. */
#undef GetDlgCtrlID
#undef SetDlgItemInt
#undef CheckDlgButton
#undef GetDlgItemInt
#undef CheckRadioButton
#undef IsDlgButtonChecked
#define GetDlgCtrlID wow_original_GetDlgCtrlID
#define SetDlgItemInt wow_original_SetDlgItemInt
#define CheckDlgButton wow_original_CheckDlgButton
#define GetDlgItemInt wow_original_GetDlgItemInt
#define CheckRadioButton wow_original_CheckRadioButton
#define IsDlgButtonChecked wow_original_IsDlgButtonChecked
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
BOOL WINAPI wow_original_EndDialog(HWND, INT_PTR);
#endif
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
/* XP message policy uses the original table supplied by clmsg.c. */
#define TESTSYNCONLYMESSAGE(msg, wParam) (((msg) < WM_USER) ? \
    (MessageTable[msg].bSyncOnlyMessage || \
     ((msg) == WM_DEVICECHANGE && ((wParam) & 0x8000))) : 0)
#define MSGFLAG_WOW_RESERVED 0x00010000
/* Define recovered bodies under private names, not SDK dllimport names. */
#undef IsDialogMessageA
#undef IsDialogMessageW
#define IsDialogMessageA wow_original_IsDialogMessageA
#define IsDialogMessageW wow_original_IsDialogMessageW
BOOL WINAPI wow_original_IsDialogMessageA(HWND, LPMSG);
BOOL WINAPI wow_original_IsDialogMessageW(HWND, LPMSG);
#endif
/* Retain the original navigation wrappers under private body symbols. No
 * external USER32 route is redirected by these source-definition aliases. */
#ifdef WOW_ORIGINAL_DIALOG_MANAGER
/* The retained CLS identity is the registration's immutable native atom.
 * Its pre-XP field is atomClassName; no versioned-class namespace is exposed. */
#define IS_BUTTON(pwnd) (gpsi->atomSysClass[ICLS_BUTTON] == (pwnd)->pcls->atomClassName)
#undef GetNextDlgTabItem
#undef GetNextDlgGroupItem
#define GetNextDlgTabItem wow_original_GetNextDlgTabItem
#define GetNextDlgGroupItem wow_original_GetNextDlgGroupItem
HWND WINAPI wow_original_GetNextDlgTabItem(HWND, HWND, BOOL);
HWND WINAPI wow_original_GetNextDlgGroupItem(HWND, HWND, BOOL);
#endif
BOOL RtlMBMessageWParamCharToWCS(DWORD, PDWORD);
PWND GetParentDialog(PWND);
PWND xxxGotoNextMnem(PWND, PWND, WCHAR);
#undef CallMsgFilter
#define CallMsgFilter wow_native_CallMsgFilterW
/* Original 14-argument creator must enter the enrolled creation adapter,
 * never resolve directly to the modern 17-argument native service. */
#define NtUserCreateWindowEx wow_user_create_dialog_window_ex
/* XP's versioned client creator has no WOW-word argument. Both original
 * call shapes enter the same enrollment/creation owner. */
#define VerNtUserCreateWindowEx(...) wow_user_create_dialog_window_ex(__VA_ARGS__, NULL)
#define CW_FLAGS_VERSIONCLASS 0x40000000
#ifndef CW2_USEDEFAULT
#define CW2_USEDEFAULT 0x8000
#endif
#define KHFONT_TO_HFONT(h) ((HFONT)(h))
#define RealAdjustWindowRectEx _AdjustWindowRectEx
#define MIRRORED_HWND(hwnd) ((GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0)
#define NtUserGetForegroundWindow GetForegroundWindow
#define NtUserSetWindowLongPtr NtUserSetWindowLong
#define IS_IME_ENABLED() GetSystemMetrics(SM_IMMENABLED)
#ifndef WFMSGBOX
#define WFMSGBOX 0x0020
#endif
#define PUSIF_SNAPTO 0x00000002
#undef TEST_PUSIF
#define TEST_PUSIF(flag) ((gpsi->PUSIFlags & (flag)) != 0)
#undef _MonitorFromWindow
#define _MonitorFromWindow(window, flags) wow_dialog_monitor(MonitorFromWindow(HWq(window), flags))
#define _MonitorFromPoint(point, flags) wow_dialog_monitor(MonitorFromPoint(point, flags))
#define _MonitorFromRect(rect, flags) wow_dialog_monitor(MonitorFromRect(rect, flags))
#define GetPrimaryMonitor wow_dialog_primary_monitor
#undef ptCursor
/* XP user.h keyboard-cue policy, supplied by the dialog's native snapshot. */
#define PUSIF_KEYBOARDCUES 0x00000020
#define GACF2_KCOFF 0x00000002
#define TEST_KbdCuesPUSIF (!gpsi->bKeyboardPref && \
    !(gpsi->PUSIFlags & PUSIF_KEYBOARDCUES) && \
    (gpsi->PUSIFlags & PUSIF_UIEFFECTS) && \
    !(GetAppCompatFlags2(VER40) & GACF2_KCOFF))
/* Original wingdip.h exports, not a substitute font lookup algorithm. */
#define GetTextFaceAliasW wow_edit_native_GetTextFaceAliasW
#ifndef IS_ANY_DBCS_CHARSET
#define IS_ANY_DBCS_CHARSET(c) ((c) == SHIFTJIS_CHARSET || (c) == HANGEUL_CHARSET || \
    (c) == GB2312_CHARSET || (c) == CHINESEBIG5_CHARSET)
#endif
/* Symbolic operation selection: never pass old NT4 enum values to USER. */
#define UserThreadStateIsForeground wow_dialog_current_foreground
#define WindowIsForegroundThread wow_dialog_window_foreground
#define NtUserGetThreadState(operation) operation()
#define NtUserQueryWindow(window, operation) operation(window)
#undef gpsi
#undef oemInfo
#define gpsi (&wow_dialog_current->server)
#endif
#include "wow_control_state_provider.h"
#define SetWindowState wow_control_set_window_state
#define ClearWindowState wow_control_clear_window_state
#endif /* !WOW_ORIGINAL_DEFAULT_WINDOW */

/* XP EDIT calls existing native IMM operations synchronously. */
#define NULL_HIMC ((HIMC)0)
#define THREAD_HKL() GetKeyboardLayout(0)
#undef GETAPPVER
#define GETAPPVER() wow_edit_client_info()->dwExpWinVer
#define ES_FMTMASK 0x0003L
#define FAREAST_CHARSET_BITS (FS_JISJAPAN | FS_CHINESESIMP | FS_WANSUNG | FS_CHINESETRAD)
UINT GetACPCharSet(void);
#define QueryFontAssocStatus wow_edit_native_QueryFontAssocStatus
#define fpImmAssociateContext ImmAssociateContext
#define fpImmEscapeA ImmEscapeA
#define fpImmEscapeW ImmEscapeW
#define fpImmGetCompositionStringA ImmGetCompositionStringA
#define fpImmGetCompositionStringW ImmGetCompositionStringW
#define fpImmGetCompositionWindow ImmGetCompositionWindow
#define fpImmGetContext ImmGetContext
#define fpImmIsIME ImmIsIME
#define fpImmNotifyIME ImmNotifyIME
#define fpImmReleaseContext ImmReleaseContext
#define fpImmSetCompositionFontW ImmSetCompositionFontW
#define fpImmSetCompositionWindow ImmSetCompositionWindow

/* Public IMM provider operations used by the original XP EDIT body. */
#define fpImmEnumInputContext ImmEnumInputContext
#define fpImmGetConversionStatus ImmGetConversionStatus
#define fpImmGetOpenStatus ImmGetOpenStatus
#define fpImmGetProperty ImmGetProperty
#define fpImmSetCompositionStringA ImmSetCompositionStringA
#define fpImmSetCompositionStringW ImmSetCompositionStringW
#define fpImmSetOpenStatus ImmSetOpenStatus
BOOL WINAPI SyncSoftKbdState(HIMC, LPARAM);
#define GACF2_NO_INIT_ECFLAGS_ON_SETFOCUS 0x00020000
#define VER50 0x0500
#define UserVerify(f) ((void)(f))
#ifndef IS_IME_ENABLED
#define IS_IME_ENABLED() GetSystemMetrics(SM_IMMENABLED)
#endif
#define RTL_UI() (GetUserDefaultUILanguage() == MAKELANGID(LANG_ARABIC, SUBLANG_DEFAULT) || \
    GetUserDefaultUILanguage() == MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT))
#endif
