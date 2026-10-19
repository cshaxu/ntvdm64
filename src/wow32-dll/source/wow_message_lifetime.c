/* ADAPTER-WOW-095: original unthunkers own rollback and release. */
#include "precomp.h"
#include "wow_message_lifetime.h"

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
    SetLastError(error);
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
