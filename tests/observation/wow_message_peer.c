/* Independent native peer for win16_message_probe.asm. Loaded by the real
 * original KERNEL generic thunk. It neither binds a WOW task nor substitutes
 * a provider. Its window is a test endpoint, never a product helper HWND. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HANDLE peer_thread, peer_ready;
static HWND sender_window, receiver_window, peer_window;
static DWORD sender_thread, receiver_thread, peer_thread_id;
static volatile LONG errors, received, nested, replied, text;
static HMODULE peer_module;

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) peer_module = instance;
    return TRUE;
}

static void check(BOOL condition)
{
    if (!condition) InterlockedIncrement(&errors);
}

static LRESULT CALLBACK peer_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    DWORD_PTR value = 0;
    char buffer[32] = {0};
    if (message >= 0x470 && message <= 0x473) {
        check((InSendMessageEx(NULL) & ISMEX_SEND) != 0);
        check(GetCurrentThreadId() == peer_thread_id);
        InterlockedIncrement(&received);
    }
    switch (message) {
    case 0x470:
        check(wp == 0x1234 && (DWORD)lp == 0x56789abc);
        return 0x447a0010;
    case 0x471:
        check(SendMessageTimeoutA(sender_window, 0x462, 0, 0x22334455,
            SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 5000, &value) != 0);
        check(value == 0x447a0002);
        InterlockedIncrement(&nested);
        return 0x447a0011;
    case 0x472:
        check(SendMessageTimeoutA(sender_window, 0x464, 0, 0,
            SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 5000, &value) != 0);
        check(value == 0x447a0004); /* Duplicate reply must not replace first. */
        InterlockedIncrement(&replied);
        return 0x447a0012;
    case 0x473:
        check(SendMessageTimeoutA(sender_window, WM_SETTEXT, 0,
            (LPARAM)"Native text", SMTO_ABORTIFHUNG, 5000, &value) != 0);
        check(value != 0);
        value = 0;
        check(SendMessageTimeoutA(sender_window, WM_GETTEXT, sizeof(buffer),
            (LPARAM)buffer, SMTO_ABORTIFHUNG, 5000, &value) != 0);
        check(value == 11 && strcmp(buffer, "Native text") == 0);
        InterlockedIncrement(&text);
        return 0x447a0013;
    case WM_CLOSE:
        check(DestroyWindow(window));
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static DWORD WINAPI peer_main(PVOID argument)
{
    WNDCLASSA cls = {0};
    MSG message;
    BOOL got;
    (void)argument;
    peer_thread_id = GetCurrentThreadId();
    cls.lpfnWndProc = peer_proc;
    cls.hInstance = peer_module;
    cls.lpszClassName = "T447_MSG_NATIVE";
    if (!RegisterClassA(&cls)) {
        InterlockedIncrement(&errors);
        SetEvent(peer_ready);
        return 1;
    }
    peer_window = CreateWindowA(cls.lpszClassName, "Native message peer", 0,
        0, 0, 1, 1, NULL, NULL, peer_module, NULL);
    check(peer_window != NULL);
    SetEvent(peer_ready);
    if (!peer_window) {
        UnregisterClassA(cls.lpszClassName, peer_module);
        return 1;
    }
    while ((got = GetMessageA(&message, NULL, 0, 0)) > 0)
        DispatchMessageA(&message);
    check(got == 0);
    check(!IsWindow(peer_window));
    check(UnregisterClassA(cls.lpszClassName, peer_module));
    return errors ? 1 : 0;
}

DWORD WINAPI MessagePeer(DWORD operation)
{
    DWORD owner = 0, code = 1;
    DWORD report[12] = {0};
    DWORD written;
    HANDLE output;
    if (operation == 0) {
        if (peer_thread || peer_ready) return 0;
        sender_window = FindWindowA("T447_MSG_SEND", NULL);
        if (!sender_window) return 0;
        sender_thread = GetWindowThreadProcessId(sender_window, &owner);
        if (owner != GetCurrentProcessId() || sender_thread != GetCurrentThreadId())
            return 0;
        if (!peer_module) return 0;
        peer_ready = CreateEventA(NULL, TRUE, FALSE, NULL);
        if (!peer_ready) return 0;
        peer_thread = CreateThread(NULL, 0, peer_main, NULL, 0, NULL);
        if (!peer_thread || WaitForSingleObject(peer_ready, 5000) != WAIT_OBJECT_0)
            return 0;
        check(peer_thread_id != sender_thread && peer_window != NULL);
        return errors ? 0 : 0x447a0001;
    }
    if (operation == 1) {
        receiver_window = FindWindowA("T447_MSG_RECV", NULL);
        if (!peer_thread || !receiver_window) return 0;
        receiver_thread = GetWindowThreadProcessId(receiver_window, &owner);
        check(owner == GetCurrentProcessId());
        check(receiver_thread != sender_thread && receiver_thread != peer_thread_id);
        return errors ? 0 : 0x447a0002;
    }
    if (operation != 2 || !peer_thread) return 0;
    check(PostMessageA(peer_window, WM_CLOSE, 0, 0));
    if (WaitForSingleObject(peer_thread, 5000) != WAIT_OBJECT_0) return 0;
    check(GetExitCodeThread(peer_thread, &code) && code == 0);
    check(received == 4 && nested == 1 && replied == 1 && text == 1);
    report[0] = 0x47534d4e; /* NMSG */
    report[1] = (DWORD)errors;
    report[2] = GetCurrentProcessId();
    report[3] = sender_thread;
    report[4] = receiver_thread;
    report[5] = peer_thread_id;
    report[6] = (DWORD)received;
    report[7] = (DWORD)nested;
    report[8] = (DWORD)replied;
    report[9] = (DWORD)text;
    report[10] = code;
    report[11] = IsWindow(peer_window);
    output = CreateFileA("MSGHOST.BIN", GENERIC_WRITE, 0, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) return 0;
    check(WriteFile(output, report, sizeof(report), &written, NULL) && written == sizeof(report));
    check(CloseHandle(output));
    check(CloseHandle(peer_thread)); peer_thread = NULL;
    check(CloseHandle(peer_ready)); peer_ready = NULL;
    return errors ? 0 : 0x447a0003;
}

/* Actual generic-thunk caller on the sender's bound task thread. Function
 * addresses come from the production DLL's public exports, never a fixture
 * scope owner or fabricated PTD. No User32 calls from DllMain are added. */
DWORD WINAPI MessageExtension(DWORD operation, DWORD procedure, DWORD receiver_task)
{
    typedef BOOL (WINAPI *callback_ex)(DWORD,DWORD,DWORD,PVOID,PDWORD);
    typedef DWORD (WINAPI *callback_one)(DWORD,DWORD);
    typedef VOID (WINAPI *yield_proc)(VOID);
    typedef VOID (WINAPI *directed_proc)(WORD);
    HMODULE wow = GetModuleHandleA("WOW32.DLL");
    callback_ex extended;
    callback_one simple;
    yield_proc yield;
    directed_proc directed_yield;
    FARPROC raw;
    DWORD arguments[8] = {0x12345678}; /* Original PARM16 capture extent. */
    DWORD value = 0x447affff, first, second, report[10], written;
    HANDLE output;
    BOOL invalid;
    if (!wow || operation != 0x79 || !procedure || !receiver_task ||
            receiver_task > 0xffff || GetCurrentThreadId() != sender_thread) return 0;
    raw = GetProcAddress(wow,"WOWCallback16Ex"); if (!raw) return 0;
    memcpy(&extended,&raw,sizeof(extended));
    raw = GetProcAddress(wow,"WOWCallback16"); if (!raw) return 0;
    memcpy(&simple,&raw,sizeof(simple));
    raw = GetProcAddress(wow,"WOWYield16"); if (!raw) return 0;
    memcpy(&yield,&raw,sizeof(yield));
    raw = GetProcAddress(wow,"WOWDirectedYield16"); if (!raw) return 0;
    memcpy(&directed_yield,&raw,sizeof(directed_yield));
    SetLastError(ERROR_SUCCESS);
    invalid = !extended(procedure,0,MAXDWORD,NULL,&value) &&
        GetLastError() == ERROR_INVALID_PARAMETER && value == 0x447affff;
    check(invalid);
    check(extended(procedure,0,sizeof(DWORD),arguments,&value));
    first = value;
    check(first == 0x447a0079);
    second = simple(procedure,0x12345678);
    check(second == 0x447a0079);
    yield();
    check(PostMessageA(receiver_window,0x475,0,0));
    directed_yield((WORD)receiver_task);
    report[0] = 0x54584547; /* GEXT */
    report[1] = (DWORD)errors;
    report[2] = GetCurrentProcessId();report[3] = GetCurrentThreadId();
    report[4] = receiver_task;report[5] = first;report[6] = second;
    report[7] = 1;report[8] = 1;report[9] = invalid;
    output = CreateFileA("MSGEXT.BIN",GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output == INVALID_HANDLE_VALUE) return 0;
    check(WriteFile(output,report,sizeof(report),&written,NULL) && written == sizeof(report));
    check(CloseHandle(output));
    return errors ? 0 : 0x447a0079;
}
