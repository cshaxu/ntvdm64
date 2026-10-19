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
    SetLastError(error);
}

void WINAPI wow_message_unwind16(LPMSGPARAMEX message)
{
    DWORD error = GetLastError();
    if (message->Parm16.WndProc.wMsg == WM_COPYDATA &&
            message->MsgBuffer->copydata.flags)
        UnThunkWMMsg16(message);
    SetLastError(error);
}
