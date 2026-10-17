#ifndef WOW_DEFAULT_WINDOW_BINDINGS_H
#define WOW_DEFAULT_WINDOW_BINDINGS_H
#include <windows.h>
#include "wow_default_message_bindings.h"
#include "wow_edit_drawing_bindings.h"
/* Finite per-invocation inputs, not a native SERVERINFO/OEMINFO overlay. */
typedef struct wow_default_context {
    struct {
        COLORREF argbSystem[WOW_EDIT_COLOR_COUNT];
        UINT uiShellMsg;
        int gclBorder;
    } server;
    struct { UINT BitCount; } screen;
} wow_default_context;
extern __declspec(thread) wow_default_context *wow_default_current;
BOOL WINAPI wow_default_capture_context(wow_default_context *, UINT);
BOOL WINAPI wow_default_ensure_shell_message(HWND);
int WINAPI GetWindowBorders(LONG, DWORD, BOOL, BOOL);
struct tagWND;
/* Owner-approved ordinary native/custom NTW16S nonclient policy boundary. */
int WINAPI wow_default_find_nc_hit(struct tagWND *, LONG);
/* Caller prepares the relationship graph through the control/dialog entry
 * and retains the original WND through its existing task/borrow scope.
 * This worker retains that exact allocation, never reselects it by HWND.
 * Only synchronous stored-name reads take the per-binding name lock. */
LONG WINAPI wow_default_window_proc(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
/* ECGetControlBrush different-thread default entry only: lp is a retained
 * source PWND, unlike public CTLCOLOR's HWND. Never use for general dispatch. */
LONG WINAPI wow_edit_default_control_color(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
/* Original clmsg.c service shape. Accept only its zero-extra default-window
 * operation; native USER owns service execution, not this adapter. */
LONG WINAPI wow_default_CsSendMessage(HWND, UINT, WPARAM, LPARAM,
    DWORD, DWORD, BOOL);
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#include "wow_window_client_state_bindings.h"
#include "wow_user_private_access.h"
/* DEBUG's gfTurboDWP fallback can send to USER while reading a name. This
 * cohort is the product Release build; do not silently admit that fallback
 * beneath the name lock in a future debug graph. */
#if defined(DEBUG)
#error Recovered default-window cohort requires Release dispatch
#endif
#define DefWindowProcWorker wow_original_default_window_proc
#define gSharedInfo wow_default_message_state
#define gpsi (&wow_default_current->server)
#define oemInfo (wow_default_current->screen)
#define SYSRGB(index) (wow_default_current->server.argbSystem[COLOR_##index])
#define ahbrSystem wow_edit_system_brushes
#define ghbrGray wow_edit_gray_brush
#define SetTaskmanWindow wow_default_ensure_shell_message
#ifdef CsSendMessage
#undef CsSendMessage
#endif
#define CsSendMessage wow_default_CsSendMessage
#define SendMessageWorker wow_window_client_send_message
#define SendMessageW wow_native_SendMessageW
#elif defined(WOW_ORIGINAL_EDIT_CLIENT) || defined(WOW_ORIGINAL_DIALOG_CLIENT)
#define DefWindowProcWorker wow_default_window_proc
#endif
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW) || defined(WOW_ORIGINAL_EDIT_CLIENT) || defined(WOW_ORIGINAL_DIALOG_CLIENT)
#include "opennt-host/windows/core/ntuser/rtl/userrtl.h"
#define FindNCHit wow_default_find_nc_hit
#endif
#ifdef WOW_ORIGINAL_DIALOG_CLIENT
#include "wow_dialog_context_bindings.h"
#endif
#endif
