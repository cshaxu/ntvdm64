#include "wow_user_message_bridge.h"
#include "wow_task_order_bindings.h"
#include <stdio.h>

static unsigned errors;
static volatile LONG sends;
static volatile LONG posts;

#define CHECK(value) do { if (!(value)) { ++errors; \
    printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static LRESULT CALLBACK receiver(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    UNREFERENCED_PARAMETER(window);
    UNREFERENCED_PARAMETER(wp);
    UNREFERENCED_PARAMETER(lp);
    if (message == WM_APP + 40) {
        InterlockedIncrement(&sends);
        return 73;
    }
    if (message == WM_APP + 41) {
        InterlockedIncrement(&posts);
        return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}

typedef struct sender_context {
    HWND window;
    HANDLE started;
    LRESULT result;
} sender_context;

static DWORD WINAPI sender(void *parameter)
{
    sender_context *context = parameter;
    SetEvent(context->started);
    context->result = SendMessageW(context->window, WM_APP + 40, 0, 0);
    return context->result == 73 ? 0 : 1;
}

int __cdecl main(void)
{
    WNDCLASSW wc = {0};
    wow_user_message_bridge bridge = {0};
    wow_task_order_queue queue = {0};
    wow_task_order_thread thread = {0};
    sender_context context = {0};
    HANDLE sender_thread;
    HWND window;
    MSG message;
    DWORD code;

    wc.lpfnWndProc = receiver;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"S40WowUserMessageBridge";
    CHECK(RegisterClassW(&wc) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    window = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
        HWND_MESSAGE, NULL, wc.hInstance, NULL);
    CHECK(window != NULL);
    thread.pcti = &queue;
    CHECK(wow_user_message_bridge_initialize(&bridge));
    CHECK(wow_user_message_bridge_sync(&bridge, &thread));
    CHECK(!(queue.fsWakeBits & QS_SENDMESSAGE));
    context.window = window;
    context.started = CreateEventW(NULL, TRUE, FALSE, NULL);
    sender_thread = CreateThread(NULL, 0, sender, &context, 0, NULL);
    CHECK(context.started != NULL && sender_thread != NULL);
    CHECK(WaitForSingleObject(context.started, 1000) == WAIT_OBJECT_0);
    /* The sender must remain blocked until the receiver processes the real
     * synchronous send.  No synthetic queue bit may make this pass early. */
    CHECK(WaitForSingleObject(sender_thread, 0) == WAIT_TIMEOUT);
    CHECK(MsgWaitForMultipleObjectsEx(0, NULL, 1000, QS_SENDMESSAGE,
        MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0);
    CHECK(wow_user_message_bridge_receive(&bridge, &thread));
    CHECK(WaitForSingleObject(sender_thread, 1000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(sender_thread, &code) && code == 0);
    CHECK(context.result == 73 && sends == 1);
    CHECK(!(queue.fsWakeBits & QS_SENDMESSAGE));
    CHECK(wow_user_message_bridge_receive(&bridge, &thread));
    CHECK(!(queue.fsWakeBits & QS_SENDMESSAGE));
    /* ReceiveMessages is not the application's posted-message loop. */
    CHECK(PostMessageW(window, WM_APP + 41, 0, 0));
    CHECK(wow_user_message_bridge_receive(&bridge, &thread));
    CHECK(posts == 0);
    CHECK(PeekMessageW(&message, window, WM_APP + 41, WM_APP + 41, PM_REMOVE));
    CHECK(message.message == WM_APP + 41);
    DispatchMessageW(&message);
    CHECK(posts == 1);
    CHECK(PostThreadMessageW(GetCurrentThreadId(), WM_QUIT, 41, 0));
    CHECK(wow_user_message_bridge_receive(&bridge, &thread));
    CHECK(PeekMessageW(&message, NULL, WM_QUIT, WM_QUIT, PM_REMOVE));
    CHECK(message.message == WM_QUIT && message.wParam == 41);
    wow_user_message_bridge_dispose(&bridge);
    CHECK(!wow_user_message_bridge_sync(&bridge, &thread));
    CHECK(DestroyWindow(window));
    CHECK(CloseHandle(sender_thread));
    CHECK(CloseHandle(context.started));
    printf("WOW_USER_MESSAGE_BRIDGE errors=%u sends=%ld posts=%ld quit=1\n",
        errors, sends, posts);
    return errors != 0;
}
