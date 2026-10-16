#include "precomp.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "wow_window_creation_binding.h"
#include "wow_user_runtime.h"
#include "wow_menu_native_bindings.h"

/* Original wowhooks.c owns hook identity, conversion, guest-stack allocation
 * and dispatch. Protect that whole interval, not just its final CallBack16.
 * All 32 unchanged native hook stubs still call WU32StdHookProc. */
LONG WINAPI WowOriginalStdHookProc(INT, LONG, LONG, INT);

/* wowhooks.c owns conversion. These are only the native HWND operands that
 * its selected hook converter exposes to the guest. Do not interpret hook
 * codes without their original table type, nor numeric keyboard/journal
 * parameters as windows. Message payloads require their separate converter
 * boundary; collecting the MSG/CWP receiver does not replace that boundary. */
static ULONG hook_window_roots(const HOOKSTATEDATA *hook, INT code,
    LONG wp, LONG lp, HWND roots[7])
{
    ULONG count = 0;
    if (!hook->InUse || code < 0) return 0;
    switch (hook->iHook) {
    case WH_CBT:
        switch (code) {
        case HCBT_MOVESIZE:
        case HCBT_MINMAX:
        case HCBT_DESTROYWND:
            roots[count++] = (HWND)wp;
            break;
        case HCBT_SETFOCUS:
            roots[count++] = (HWND)wp;
            roots[count++] = (HWND)lp;
            break;
        case HCBT_ACTIVATE:
            roots[count++] = (HWND)wp;
            if (lp) roots[count++] = ((CBTACTIVATESTRUCT *)lp)->hWndActive;
            break;
        case HCBT_CREATEWND:
            /* wp is the required, separately enrolled receiver. */
            if (lp) {
                CBT_CREATEWNDA *create = (CBT_CREATEWNDA *)lp;
                HWND after = create->hwndInsertAfter;
                if (create->lpcs) roots[count++] = create->lpcs->hwndParent;
                if (after != HWND_TOP && after != HWND_BOTTOM &&
                        after != HWND_TOPMOST && after != HWND_NOTOPMOST)
                    roots[count++] = after;
            }
            break;
        case HCBT_CLICKSKIPPED:
            if (lp) roots[count++] = ((MOUSEHOOKSTRUCT *)lp)->hwnd;
            break;
        }
        break;
    case WH_MOUSE:
        if (lp) roots[count++] = ((MOUSEHOOKSTRUCT *)lp)->hwnd;
        break;
    case WH_CALLWNDPROC:
        if (lp) {
            const CWPSTRUCT *message = (const CWPSTRUCT *)lp;
            roots[count++] = message->hwnd;
            count += wow_user_message_window_roots(message->message,
                message->wParam, message->lParam, roots + count);
        }
        break;
    case WH_MSGFILTER:
    case WH_SYSMSGFILTER:
    case WH_GETMESSAGE:
        if (lp) {
            const MSG *message = (const MSG *)lp;
            roots[count++] = message->hwnd;
            count += wow_user_message_window_roots(message->message,
                message->wParam, message->lParam, roots + count);
        }
        break;
    case WH_SHELL:
        if (code == HSHELL_WINDOWCREATED || code == HSHELL_WINDOWDESTROYED)
            roots[count++] = (HWND)wp;
        break;
    }
    return count;
}

LONG WINAPI WU32StdHookProc(INT code, LONG wp, LONG lp, INT index)
{
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    LONG result = 0;
    BOOL borrowed = FALSE;
    BOOL creation = FALSE, completed = FALSE, task_left = FALSE;
    HOOKSTATEDATA hook = {0};
    HWND roots[7];
    ULONG root_count;
    wow_window_creation_enrollment enrollment = {0};
    wow_user_runtime_thread *runtime;
    if (!wow_task_callback_enter(&task)) return 0;
    __try {
        /* Read identity through the original table owner, not the numerical
         * hook code alone (codes overlap between different hook types).
         * A newer guest hook can run before our native creation observer. */
        if (index > 0 && index <= 255) {
            hook.iIndex = (BYTE)index;
            creation = W32GetHookStateData(&hook) && hook.InUse &&
                hook.iHook == WH_CBT && code == HCBT_CREATEWND && lp;
        }
        if (creation) result = 1; /* Incomplete setup vetoes creation. */
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        runtime = wow_user_runtime_current();
        if (creation && !wow_window_creation_enroll_internal(
                runtime ? runtime->thread : NULL, (HWND)wp,
                ((CBT_CREATEWNDA *)lp)->lpcs, &enrollment)) __leave;
        root_count = hook_window_roots(&hook, code, wp, lp, roots);
        if (!wow_user_window_refresh_payload(creation ? (HWND)wp : NULL,
                roots, root_count)) __leave;
        if (hook.InUse && code >= 0 && lp) {
            if (hook.iHook == WH_CALLWNDPROC) {
                const CWPSTRUCT *message = (const CWPSTRUCT *)lp;
                if (!wow_menu_native_refresh_message(message->message,
                        message->wParam, message->lParam)) __leave;
            } else if (hook.iHook == WH_GETMESSAGE || hook.iHook == WH_MSGFILTER ||
                    hook.iHook == WH_SYSMSGFILTER) {
                const MSG *message = (const MSG *)lp;
                if (!wow_menu_native_refresh_message(message->message,
                        message->wParam, message->lParam)) __leave;
            }
        }
        result = WowOriginalStdHookProc(code, wp, lp, index);
        completed = TRUE;
    } __finally {
        __try {
            if (creation) {
                BOOL accepted = completed && result == 0;
                /* Keep the exact enrollment borrowed until the final veto
                 * decision. Never reacquire a possibly reused HWND later.
                 * No guest alias escapes this synchronous hook call. */
                completed = FALSE;
                task_left = TRUE;
                if (!wow_task_callback_leave(&task)) result = 1;
                else completed = accepted;
            }
        } __finally {
            wow_window_creation_finish(&enrollment, completed && result == 0);
            /* Final window/class references still retire before alias
             * collection, including a failed task-scope return. */
            if (borrowed) wow_user_borrow_leave_required(&borrow);
            if (!task_left && !wow_task_callback_leave(&task)) result = 0;
        }
    }
    return result;
}
