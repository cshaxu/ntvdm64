/* Pure native reference for the identical authored protected-input protocol.
 * No WOW/provider/CCPU or project process is loaded. Endpoints are test-only. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_reply_payload_lifetime.h"

static reply_payload_lifetime probe;
static BOOL bad_reply;

static LRESULT CALLBACK receiver_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_COPYDATA) {
        const COPYDATASTRUCT *copy=(const COPYDATASTRUCT *)lp;
        probe.receiver_alias=(const BYTE *)copy == probe.page;
        probe.receiver_header=(DWORD)(ULONG_PTR)copy;
        probe.receiver_payload=(DWORD)(ULONG_PTR)copy->lpData;
        if (copy->dwData != (probe.timeout ? TIMEOUT_PAYLOAD_DATA : REPLY_PAYLOAD_DATA) || copy->cbData != 8 ||
            !copy->lpData || memcmp(copy->lpData,"M4copy!!",8))
            InterlockedIncrement(&probe.errors);
        if (!(InSendMessageEx(NULL) & ISMEX_SEND)) InterlockedIncrement(&probe.errors);
        InterlockedExchange(&probe.accepted,1);
        if (!probe.timeout && !ReplyMessage(bad_reply ? REPLY_PAYLOAD_VALUE+1 : REPLY_PAYLOAD_VALUE))
            InterlockedIncrement(&probe.errors);
        (void)reply_payload_wait(&probe);
        /* No input read after ReplyMessage, including during cleanup. */
        PostQuitMessage(0);
        return REPLY_PAYLOAD_VALUE+2;
    }
    return DefWindowProcA(window,message,wp,lp);
}

static DWORD WINAPI send_main(PVOID argument)
{
    HWND receiver=(HWND)argument;
    return reply_payload_send(&probe,receiver,receiver) ? 0 : 1;
}

int main(int argc, char **argv)
{
    WNDCLASSA cls={0};
    HWND receiver;
    HANDLE thread;
    MSG message;
    DWORD code=MAXDWORD;
    BOOL result;
    if (argc < 2 || argc > 3) return 2;
    bad_reply=argc == 3 && strcmp(argv[2],"--bad-reply") == 0;
    if (argc == 3 && !bad_reply && strcmp(argv[2],"--timeout")) return 2;
    if (!reply_payload_create(&probe)) return 3;
    if (argc == 3 && !strcmp(argv[2],"--timeout")) {
        probe.timeout=TRUE; ((COPYDATASTRUCT *)probe.page)->dwData=TIMEOUT_PAYLOAD_DATA;
    }
    cls.lpfnWndProc=receiver_proc; cls.hInstance=GetModuleHandleA(NULL);
    cls.lpszClassName="T447_REPLY_NATIVE_CONTROL";
    if (!RegisterClassA(&cls)) return 4;
    receiver=CreateWindowA(cls.lpszClassName,"Native payload control",0,
        0,0,1,1,NULL,NULL,cls.hInstance,NULL);
    if (!receiver) return 5;
    thread=CreateThread(NULL,0,send_main,receiver,0,NULL);
    if (!thread) return 6;
    while ((result=GetMessageA(&message,NULL,0,0)) > 0) DispatchMessageA(&message);
    if (WaitForSingleObject(thread,6000) != WAIT_OBJECT_0) return 7;
    if (result != 0 || !GetExitCodeThread(thread,&code) || code || !CloseHandle(thread))
        InterlockedIncrement(&probe.errors);
    if (!DestroyWindow(receiver) || !UnregisterClassA(cls.lpszClassName,cls.hInstance))
        InterlockedIncrement(&probe.errors);
    result=reply_payload_finish(&probe,argv[1]);
    printf("NATIVE_REPLY_PAYLOAD errors=%ld returned=%ld protected=%ld unreadable=%ld continued=%ld alias=%lu synthetic=%u timeout=%u status=%lu error=%lu\n",
        probe.errors,probe.sender_returned,probe.protected_input,probe.unreadable,
        probe.continued,probe.receiver_alias,(unsigned)bad_reply,(unsigned)probe.timeout,
        probe.send_status,probe.send_error);
    return result ? 0 : 1;
}
