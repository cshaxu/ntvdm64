#ifndef WOW_EDIT_CALL_BINDINGS_H
#define WOW_EDIT_CALL_BINDINGS_H
#include <windows.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT
BOOL WINAPI wow_edit_NtUserDefSetText(HWND, const LARGE_STRING *);
/* ADAPTER-WOW-061: original EDIT caller shape; existing native boundary
 * handles CTLCOLOR reentry. HWND/HDC/HBRUSH are already native identities. */
BOOL WINAPI wow_edit_FillWindow(HWND, HWND, HDC, HBRUSH);
/* Native GetControlColor service edge; original client policy precedes it. */
HBRUSH WINAPI wow_edit_NtUserGetControlColor(HWND, HWND, HDC, UINT);
HBRUSH WINAPI GetControlColor(HWND, HWND, HDC, UINT);
HBRUSH WINAPI GetControlBrush(HWND, HDC, UINT);
#if defined(WOW_ORIGINAL_CONTROL_COLOR) || defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
#include "wow_control_brush_bindings.h"
/* Original retail usercli.h, HANDLE_FIXUP=0 and DBG=0: no truncated-handle
 * repair or diagnostics. Recovered callers already carry native GDI handles. */
#define CHECK_HANDLE_WARNING(h, bZ)
#define FIXUP_HANDLE(h) { CHECK_HANDLE_WARNING(h,FALSE); }
#define NtUserGetControlColor wow_edit_NtUserGetControlColor
#define NtUserGetControlBrush wow_control_brush
#endif
struct wow_task_client_info;
/* Borrowed from the currently bound WOW task, valid within its EDIT scope. */
const struct wow_task_client_info *WINAPI wow_edit_client_info(void);
VOID WINAPI wow_edit_set_window_state(HWND, UINT);
VOID WINAPI wow_edit_clear_window_state(HWND, UINT);
VOID WINAPI wow_edit_alter_window_style(HWND, DWORD, DWORD);
VOID WINAPI wow_list_reset_double_click(void);
struct tagWND;
DWORD WINAPI wow_edit_window_thread_id(struct tagWND *);
/* Required original registration input. An unsupported internal FNID is a
 * binding invariant failure, never a zero size that bypasses validation. */
DWORD WINAPI wow_edit_control_extent(WORD);
#if defined(WOW_ORIGINAL_CONTROL_STATE_CALLS) || defined(WOW_ORIGINAL_EDIT_CLIENT)
#include "wow_user_client_view_layout.h"
#define WOW_ORIGINAL_CLIENT_HANDLES
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CLIENT_HANDLES
#endif
#if defined(WOW_ORIGINAL_CONTROL_STATE_CALLS) || defined(WOW_ORIGINAL_EDIT_CLIENT)
#include "wow_user_client_view_layout.h"
/* Symbolic build binding only, not NT4 SFI numbers or modern syscall IDs. */
#define SFI_SETWINDOWSTATE wow_edit_set_window_state
#define SFI_CLEARWINDOWSTATE wow_edit_clear_window_state
#define NtUserCallHwndParam(window, flags, service) service(window, flags)
#endif
#ifdef WOW_ORIGINAL_EDIT_CLIENT
#include "wow_edit_entry_bindings.h"
#include "wow_edit_native_bindings.h"
#include "wow_edit_state_bindings.h"
#include "wow_edit_conversion_bindings.h"
#include "wow_user_private_access.h"
#include "wow_default_window_bindings.h"
#include "wow_dialog_native_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_edit_menu_bindings.h"
#include "wow_window_client_state_bindings.h"
#include "wow_window_creation_binding.h"
#define gSharedInfo wow_default_message_state
#define CBFNID wow_edit_control_extent
#define GetClientInfo() wow_edit_client_info()
#define FillWindow wow_edit_FillWindow
#define _CreateWindowEx wow_control_create_window_ex
#define SendMessageWorker wow_window_client_send_message
/* ComboBoxWndProcWorker remains the original direct collaboration call.
 * EDIT and ComboBox must be selected together with the same retained WND. */
/* Complete NtUser leaf family reached by the seven original EDIT units.
 * Pure DC/clipboard/beep services keep their public native implementation;
 * reentrant/mutating window operations use the existing scoped boundary.
 * The original memory, layout and text algorithms remain untouched. */
#define NtUserBeginPaint wow_native_BeginPaint
#define NtUserEndPaint wow_native_EndPaint
#define NtUserSetScrollInfo wow_native_SetScrollInfo
#define NtUserCreateCaret wow_native_CreateCaret
#define NtUserDestroyCaret wow_native_DestroyCaret
#define NtUserSetCaretPos wow_native_SetCaretPos
#define NtUserHideCaret wow_native_HideCaret
#define NtUserShowCaret wow_native_ShowCaret
#define NtUserInvalidateRect wow_native_InvalidateRect
/* Shared original control/dialog resource operations retain the existing
 * native-call execution/return owner; do not bypass it with SDK aliases. */
#define NtUserEnableWindow wow_native_EnableWindow
#define NtUserMoveWindow wow_native_MoveWindow
#define NtUserSetWindowPos wow_native_SetWindowPos
#define NtUserSetParent wow_native_SetParent
#define NtUserSetActiveWindow wow_native_SetActiveWindow
#undef NtUserSetForegroundWindow
#define NtUserSetForegroundWindow wow_native_SetForegroundWindow
#undef NtUserSetWindowContextHelpId
#define NtUserSetWindowContextHelpId wow_native_SetWindowContextHelpId
#undef NtUserSetCursorPos
#define NtUserSetCursorPos wow_native_SetCursorPos
#undef NtUserShowCursor
#define NtUserShowCursor wow_native_ShowCursor
#define NtUserDragDetect wow_native_DragDetect
#define NtUserShowWindow wow_native_ShowWindow
/* Original public winuser.h's Win1.x spelling is SW_SHOWNORMAL (both 1),
 * not a private dialog-show operation. Some SDK profiles omit the alias. */
#ifndef SHOW_OPENWINDOW
#define SHOW_OPENWINDOW SW_SHOWNORMAL
#endif
C_ASSERT(SHOW_OPENWINDOW == SW_SHOWNORMAL);
#define NtUserScrollWindowEx wow_native_ScrollWindowEx
#define NtUserWaitMessage wow_user_wait_message
#define NtUserSetThreadState wow_dialog_set_thread_state
/* Bind the original symbolic operation, never its NT4 dispatch number. */
#define SFI_XXXSETDIALOGSYSTEMMENU wow_dialog_set_system_menu
#define NtUserCallHwndLock(window, service) service(window)
#define SFI_ZAPACTIVEANDFOCUS wow_dialog_zap_active_and_focus
#define SFI_SETDIALOGPOINTER wow_dialog_set_pointer_word
#define SFI__RESETDBLCLK wow_list_reset_double_click
#define NtUserCallNoParam(service) service()
#define NtUserReleaseDC ReleaseDC
#define NtUserSetCursor wow_native_SetCursor
#define NtUserSetCapture wow_native_SetCapture
#define NtUserReleaseCapture wow_native_ReleaseCapture
#define NtUserGetDC GetDC
#define NtUserScrollDC ScrollDC
#define NtUserMessageBeep MessageBeep
#define NtUserSetSystemTimer wow_edit_native_NtUserSetSystemTimer
#define NtUserKillSystemTimer wow_edit_native_NtUserKillSystemTimer
#define NtUserSetTimer wow_control_set_timer
#define NtUserKillTimer KillTimer
#define NtUserSetWindowFNID wow_edit_set_window_fnid
#define NtUserSetWindowLong wow_window_client_set_window_long
#define NtUserAlterWindowStyle wow_edit_alter_window_style
/* INITCONTROLLOOKASIDE's failure path is in usercli.h, not the .c callers. */
#define NtUserDestroyWindow wow_native_DestroyWindow
#undef CsSendMessage
#define CsSendMessage wow_edit_CsSendMessage
/* The seven original EDIT units use these only for ECGetControlBrush's
 * equality comparison. Do not expose this as a general THREADINFO provider. */
#undef PtiCurrent
#undef GETPTI
#define PtiCurrent() GetCurrentThreadId()
#define GETPTI(window) wow_edit_window_thread_id(window)
#endif
#endif
