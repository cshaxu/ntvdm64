/* ADAPTER-WOW-095: original unthunkers own rollback and release. */
#include "precomp.h"
#include "wow_message_lifetime.h"

extern BOOL WINAPI wow_user_worker_active(void);
/* Original wcall16.h aliases these names but its referenced winntwow.h
 * declarations are absent from this selected SDK facade. Exact x86 ABI. */
extern VPVOID WINAPI WOWGlobalLockSize16(HMEM16, LPDWORD);
extern HMEM16 WINAPI WOWGlobalUnlockFree16(VPVOID);

/* ADAPTER-WOW-101: use the original allocators, not a second resource map.
 * dwTmp[1] is the captured heap token except in CREATE/MDICREATE/COPYDATA.
 * No other original converter uses that slot (CB_GETLBTEXT uses dwTmp[0]). */
DWORD WINAPI wow_message_heap32(LPWM32MSGPARAMEX message, DWORD value)
{
    message->dwTmp[1] = value;
    return value;
}

void WINAPI wow_message_free16(DWORD *key, BOOL class_atom)
{
    DWORD value = *key;
    *key = 0;
    if (value && (!class_atom || HIWORD(value)) && wow_user_worker_active())
        free16(value);
}

static void release_mdi(DWORD *class_key, DWORD *title_key, DWORD *record_key)
{
    __try { wow_message_free16(title_key, FALSE); }
    __finally {
        __try { wow_message_free16(class_key, TRUE); }
        __finally { wow_message_free16(record_key, FALSE); }
    }
}

void WINAPI wow_message_release32(LPWM32MSGPARAMEX message)
{
    DWORD value, handle, unlocked;
    if (message->uMsg == WM_COPYDATA) return; /* Original protocol owns it. */
    if (message->lpfnM32 == WM32Create) {
        __try { release_mdi(&message->dwParam, &message->dwTmp[2], &message->dwTmp[3]); }
        __finally {
            __try { wow_message_free16(&message->dwTmp[0], TRUE); }
            __finally { wow_message_free16(&message->dwTmp[1], FALSE); }
        }
    } else if (message->lpfnM32 == WM32MDICreate) {
        release_mdi(&message->dwParam, &message->dwTmp[0], &message->dwTmp[1]);
    } else if (message->lpfnM32 == WM32SizeClipBoard || message->lpfnM32 == WM32WinHelp) {
        value = message->dwParam;
        handle = message->dwTmp[2];
        unlocked = message->dwTmp[0];
        message->dwParam = message->dwTmp[0] = message->dwTmp[2] = 0;
        if (value && wow_user_worker_active()) {
            if (message->lpfnM32 == WM32SizeClipBoard && unlocked)
                value = WOWGlobalLockSize16((HMEM16)handle, NULL);
            WOWGlobalUnlockFree16(value);
        }
    } else {
        wow_message_free16(&message->dwTmp[1], FALSE);
    }
}

void WINAPI wow_message_stack_enter(wow_message_stack *scope)
{
    DWORD error = GetLastError();
    PTD ptd = CURRENTPTD();
    scope->task = ptd;
    scope->cursor = ptd ? ptd->vpCBStack : 0;
    scope->init = ptd ? ptd->dwFlags & TDF_INITCALLBACKSTACK : 0;
    scope->task16 = ptd ? ptd->htask16 : 0;
    scope->completed = FALSE;
    SetLastError(error);
}

void WINAPI wow_message_stack_leave(wow_message_stack *scope, BOOL abnormal)
{
    DWORD error = GetLastError();
    PTD ptd;
    if ((!scope->completed || abnormal) && wow_user_worker_active()) {
        ptd = CURRENTPTD();
        /* Inspect only the live authority, never dereference a retired TD. */
        if (ptd && ptd == scope->task && ptd->htask16 == scope->task16) {
            ptd->vpCBStack = scope->cursor;
            if (!(ptd->dwFlags & TDF_IGNOREINPUT))
                ptd->dwFlags = (ptd->dwFlags & ~TDF_INITCALLBACKSTACK) | scope->init;
        }
    }
    SetLastError(error);
}

void WINAPI wow_message_unwind32(LPWM32MSGPARAMEX message)
{
    DWORD error = GetLastError();
    if (message->uMsg == WM_COPYDATA && message->dwParam) {
        message->fThunk = UNTHUNKMSG;
        (void)WM32CopyData(message);
    }
    /* ADAPTER-WOW-098: only these original input-only unthunkers are pure. */
    if ((message->lpfnM32 == WM32SetText || message->lpfnM32 == WM32CBDir) &&
            message->dwParam) {
        message->fThunk = UNTHUNKMSG;
        (void)message->lpfnM32(message);
    }
    /* Release only captured call-owned keys; never replay output unthunkers. */
    __try { wow_message_release32(message); }
    __finally { SetLastError(error); }
}

void WINAPI wow_message_unwind16(LPMSGPARAMEX message)
{
    DWORD error = GetLastError();
    if (message->Parm16.WndProc.wMsg == WM_COPYDATA &&
            message->MsgBuffer->copydata.flags)
        UnThunkWMMsg16(message);
    if (((message->lpfnUnThunk16 == UnThunkWMMsg16 &&
              (message->Parm16.WndProc.wMsg == WM_SETTEXT ||
               message->Parm16.WndProc.wMsg == WM_WININICHANGE ||
               message->Parm16.WndProc.wMsg == WM_DEVMODECHANGE)) ||
             (message->lpfnUnThunk16 == UnThunkCBMsg16 &&
              message->Parm16.WndProc.wMsg == OLDCB_DIR)) &&
            (message->MsgBuffer->text.flags & WOW_TEXT_ACTIVE))
        message->lpfnUnThunk16(message);
    SetLastError(error);
}
