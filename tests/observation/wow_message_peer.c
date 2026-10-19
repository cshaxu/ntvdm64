/* Independent native peer for win16_message_probe.asm. Loaded by the real
 * original KERNEL generic thunk. It neither binds a WOW task nor substitutes
 * a provider. Its window is a test endpoint, never a product helper HWND. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(M4_TIMEOUT_PAYLOAD) && !defined(M4_REPLY_PAYLOAD)
#error Timeout payload group requires the complete early-reply group.
#endif
#ifdef M4_REPLY_PAYLOAD
#ifndef M4_STRUCTURES
#error Reply payload group requires the full normal structure group.
#endif
#include "wow_reply_payload_lifetime.h"
static reply_payload_lifetime reply_payload;
static volatile LONG reply_driver_done;
#endif

#ifndef MESSAGE_EXTERNAL_PROGRAM
static HANDLE peer_thread;
#endif
static HANDLE peer_ready;
static HWND sender_window, receiver_window, peer_window;
static DWORD sender_thread, receiver_thread, peer_thread_id;
#ifdef MESSAGE_CROSS_WORKER
static DWORD receiver_process_id;
#endif
static volatile LONG errors;
#ifndef MESSAGE_EXTERNAL_HOST
static volatile LONG received, nested, replied, text;
#endif
static HMODULE peer_module;
#ifdef MESSAGE_EXTERNAL_HOST
static DWORD peer_process_id;
#endif
#ifdef M4_STRUCTURES
#ifndef MESSAGE_EXTERNAL_HOST
static volatile LONG structure_received;
#ifdef M4_COPYDATA_PROC32
static volatile LONG proc32_fresh, proc32_borrowed;
#endif
static const LONG minmax_input[10] = {0x1234,2,3,4,5,6,-7,8,9,10};
C_ASSERT(sizeof(MINMAXINFO) == sizeof(minmax_input));

static void check_minmax(const MINMAXINFO *value, BOOL output)
{
    LONG words[10], expected[10];
    memcpy(words,value,sizeof(words));
    memcpy(expected,minmax_input,sizeof(expected));
    if (output) { expected[0]=0x7abc; expected[9]=-11; }
    if (memcmp(words,expected,sizeof(words))) InterlockedIncrement(&errors);
}

static void check_position(const WINDOWPOS *value, HWND target, BOOL output)
{
    if (LOWORD((DWORD_PTR)value->hwnd) != LOWORD((DWORD_PTR)target) ||
        value->hwndInsertAfter != HWND_TOP || value->x != (output ? -23 : -12) ||
        value->y != (output ? 45 : 34) || value->cx != (output ? 90 : 56) ||
        value->cy != 78 || value->flags != (output ? 0x14u : 4u))
        InterlockedIncrement(&errors);
}
#endif
#endif

#ifndef MESSAGE_EXTERNAL_PROGRAM
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) peer_module = instance;
    return TRUE;
}
#endif

#ifdef M4_COPYDATA_PROC32
static void check_at(BOOL condition,int line)
{
    if(!condition){
        DWORD error=GetLastError(),written;char record[256];int length;HANDLE output;
        InterlockedIncrement(&errors);
        length=_snprintf_s(record,sizeof(record),_TRUNCATE,
            "line=%d tid=%lu sender=%lu peer=%lu sender_hwnd=%p peer_hwnd=%p structures=%ld fresh=%ld borrowed=%ld\r\n",
            line,GetCurrentThreadId(),sender_thread,peer_thread_id,
            (PVOID)sender_window,(PVOID)peer_window,structure_received,proc32_fresh,proc32_borrowed);
        output=CreateFileA("MSGPEER-FAIL.txt",FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(output!=INVALID_HANDLE_VALUE){if(length>0)WriteFile(output,record,(DWORD)length,&written,NULL);CloseHandle(output);}
        SetLastError(error);
    }
}
#define check(condition) check_at((condition),__LINE__)
#else
static void check(BOOL condition)
{
    if (!condition) InterlockedIncrement(&errors);
}
#endif

#ifndef MESSAGE_EXTERNAL_HOST
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
#ifdef MESSAGE_EXTERNAL_PROGRAM
    case 0x482:
        return structure_received; /* Test-only scalar completion query. */
#endif
#ifdef M4_REPLY_PAYLOAD
    case 0x481:
        {
            BOOL sent;
            check(GetCurrentThreadId() == peer_thread_id && reply_payload.page != NULL);
            sent=reply_payload_send(&reply_payload,sender_window,window);
            InterlockedExchange(&reply_driver_done,1);
            return sent ? 0x447a0055 : 0;
        }
#endif
#ifdef M4_STRUCTURES
    case WM_GETMINMAXINFO:
        if (wp != 0x4474) break;
        check_minmax((const MINMAXINFO *)lp,FALSE);
        ((MINMAXINFO *)lp)->ptReserved.x=0x7abc;
        ((MINMAXINFO *)lp)->ptMaxTrackSize.y=-11;
        InterlockedIncrement(&structure_received);
        return 0x447a0024;
    case WM_WINDOWPOSCHANGING:
        if (wp != 0x4474) break;
        check_position((const WINDOWPOS *)lp,window,FALSE);
        ((WINDOWPOS *)lp)->x=-23; ((WINDOWPOS *)lp)->y=45;
        ((WINDOWPOS *)lp)->cx=90; ((WINDOWPOS *)lp)->flags=0x14;
        InterlockedIncrement(&structure_received);
        return 0x447a0046;
    case WM_COPYDATA:
        if (((const COPYDATASTRUCT *)lp)->dwData != 0x456789ab) break;
        check(wp != 0 && ((const COPYDATASTRUCT *)lp)->cbData == 8);
        check(((const COPYDATASTRUCT *)lp)->lpData != NULL &&
            !memcmp(((const COPYDATASTRUCT *)lp)->lpData,"M4copy!!",8));
#ifdef M4_COPYDATA_PROC32
        if(GetCurrentThreadId()==sender_thread){
            /* Original HWND32 sign-extends USER16's value; original/native
             * CallWindowProc passes that argument to the procedure unchanged. */
            if(window==(HWND)(LONG_PTR)(SHORT)LOWORD(sender_window))InterlockedIncrement(&proc32_borrowed);
            else {check(window==(HWND)(LONG_PTR)(SHORT)LOWORD(peer_window));InterlockedIncrement(&proc32_fresh);}
            return 0x447a004a;
        }
#endif
        InterlockedIncrement(&structure_received);
        return 0x447a004a;
    case 0x480:
        {
            MINMAXINFO limits;
            WINDOWPOS position={sender_window,HWND_TOP,-12,34,56,78,4};
            COPYDATASTRUCT copy={0x456789ab,8,(PVOID)"M4copy!!"};
            memcpy(&limits,minmax_input,sizeof(limits));
            check((InSendMessageEx(NULL) & ISMEX_SEND) != 0);
            check(GetCurrentThreadId() == peer_thread_id);
            check(SendMessageTimeoutA(sender_window,WM_GETMINMAXINFO,0x4474,
                (LPARAM)&limits,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a0024); check_minmax(&limits,TRUE);
            InterlockedIncrement(&structure_received);
            check(SendMessageTimeoutA(sender_window,WM_WINDOWPOSCHANGING,0x4474,
                (LPARAM)&position,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a0046); check_position(&position,sender_window,TRUE);
            InterlockedIncrement(&structure_received);
            check(SendMessageTimeoutA(sender_window,WM_COPYDATA,(WPARAM)window,
                (LPARAM)&copy,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a004a);
            check(copy.dwData == 0x456789ab && copy.cbData == 8 &&
                !memcmp(copy.lpData,"M4copy!!",8));
            InterlockedIncrement(&structure_received);
            return 0x447a0020;
        }
#endif
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
#endif

#ifdef MESSAGE_EXTERNAL_PROGRAM
/* The same actual endpoint, but now in an independent32/64-bit process.
 * Only scalar HWND/PID and a ready-event name cross the test launch edge;
 * native USER alone captures and marshals the standard pointer messages. */
int main(int argc, char **argv)
{
    DWORD owner = 0, code, report[15] = {0}, written;
    HANDLE ready, output;
    if (argc != 4) return 2;
    sender_window = (HWND)(ULONG_PTR)strtoul(argv[2], NULL, 16);
    sender_thread = GetWindowThreadProcessId(sender_window, &owner);
    if (!sender_thread || owner != strtoul(argv[3], NULL, 10) ||
            owner == GetCurrentProcessId()) return 3;
    receiver_window = FindWindowA("T447_MSG_RECV", NULL);
    receiver_thread = GetWindowThreadProcessId(receiver_window, &written);
    if (!receiver_thread || written != owner) return 4;
    ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, argv[1]);
    if (!ready) return 5;
    peer_module = GetModuleHandleA(NULL);
    /* peer_main signals the shared event only after creating its real HWND. */
    peer_ready = ready;
    code = peer_main(NULL);
    check(CloseHandle(ready));
    check(received == 4 && nested == 1 && replied == 1 && text == 1 &&
        structure_received == 6);
    report[0]=0x47534d4e; report[1]=(DWORD)errors; report[2]=owner;
    report[3]=sender_thread; report[4]=receiver_thread; report[5]=peer_thread_id;
    report[6]=(DWORD)received; report[7]=(DWORD)nested; report[8]=(DWORD)replied;
    report[9]=(DWORD)text; report[10]=code; report[11]=IsWindow(peer_window);
    report[12]=(DWORD)structure_received; report[13]=GetCurrentProcessId();
    report[14]=sizeof(void *)*8;
    output=CreateFileA("MSGHOST.BIN",GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output==INVALID_HANDLE_VALUE) return 6;
    check(WriteFile(output,report,sizeof(report),&written,NULL) && written==sizeof(report));
    check(CloseHandle(output));
    return errors ? 1 : 0;
}
#else

DWORD WINAPI MessagePeer(DWORD operation)
{
    DWORD owner = 0, code = 1;
    DWORD report[
#ifdef MESSAGE_CROSS_WORKER
        14
#else
#ifdef MESSAGE_EXTERNAL_HOST
        15
#else
#ifdef M4_COPYDATA_PROC32
        15
#else
#ifdef M4_STRUCTURES
        13
#else
        12
#endif
#endif
#endif
#endif
    ] = {0};
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
#ifdef MESSAGE_EXTERNAL_HOST
        {
            char executable[MAX_PATH], directory[MAX_PATH], command[1024], name[96], *slash;
            STARTUPINFOA startup = {sizeof(startup)};
            PROCESS_INFORMATION process = {0};
            DWORD length = GetModuleFileNameA(peer_module, executable, MAX_PATH);
            if (!length || length >= MAX_PATH || !(slash=strrchr(executable,'\\'))) return 0;
            if (strcpy_s(slash+1, sizeof(executable)-(size_t)(slash+1-executable), "MSGPEER.EXE")) return 0;
            if (strcpy_s(directory,sizeof(directory),executable)) return 0;
            *strrchr(directory,'\\')=0;
            if (_snprintf_s(name,sizeof(name),_TRUNCATE,"Local\\T447-MSG-%lu-%lu-%lu",
                    GetCurrentProcessId(),GetCurrentThreadId(),GetTickCount())<0) return 0;
            peer_ready=CreateEventA(NULL,TRUE,FALSE,name);
            if (!peer_ready || GetLastError()==ERROR_ALREADY_EXISTS) return 0;
            if (_snprintf_s(command,sizeof(command),_TRUNCATE,"\"%s\" %s %lx %lu",
                    executable,name,(unsigned long)(ULONG_PTR)sender_window,owner)<0) return 0;
            if (!CreateProcessA(executable,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                    NULL,directory,&startup,&process)) return 0;
            peer_thread=process.hProcess; peer_process_id=process.dwProcessId;
            check(CloseHandle(process.hThread));
            if (WaitForSingleObject(peer_ready,5000)!=WAIT_OBJECT_0) return 0;
            peer_window=FindWindowA("T447_MSG_NATIVE",NULL);
            peer_thread_id=GetWindowThreadProcessId(peer_window,&owner);
            check(owner==peer_process_id && owner!=GetCurrentProcessId());
        }
#else
        peer_ready = CreateEventA(NULL, TRUE, FALSE, NULL);
        if (!peer_ready) return 0;
        peer_thread = CreateThread(NULL, 0, peer_main, NULL, 0, NULL);
        if (!peer_thread || WaitForSingleObject(peer_ready, 5000) != WAIT_OBJECT_0)
            return 0;
#endif
        check(peer_thread_id != sender_thread && peer_window != NULL);
        return errors ? 0 : 0x447a0001;
    }
    if (operation == 1) {
        receiver_window = FindWindowA("T447_MSG_RECV", NULL);
        if (!peer_thread || !receiver_window) return 0;
        receiver_thread = GetWindowThreadProcessId(receiver_window, &owner);
#ifdef MESSAGE_CROSS_WORKER
        receiver_process_id=owner;
        check(owner && owner != GetCurrentProcessId());
#else
        check(owner == GetCurrentProcessId());
#endif
        check(receiver_thread != sender_thread && receiver_thread != peer_thread_id);
        return errors ? 0 : 0x447a0002;
    }
#ifdef M4_STRUCTURES
    if (operation == 3) {
        DWORD_PTR value=0;
        check(peer_thread && GetCurrentThreadId() == sender_thread);
        check(SendMessageTimeoutA(peer_window,0x480,0,0,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,15000,&value) != 0);
        check(value == 0x447a0020);
#ifdef MESSAGE_EXTERNAL_HOST
        check(SendMessageA(peer_window,0x482,0,0)==6);
#else
        check(structure_received == 6);
#endif
        return errors ? 0 : 0x447a0020;
    }
#endif
#ifdef M4_REPLY_PAYLOAD
    if (operation == 5
#ifdef M4_TIMEOUT_PAYLOAD
        || operation == 8
#endif
    ) {
        DWORD_PTR value=0;
        LRESULT sent;
        if (GetCurrentThreadId() != sender_thread || reply_payload.page ||
            !reply_payload_create(&reply_payload)) return 0;
#ifdef M4_TIMEOUT_PAYLOAD
        if (operation == 8) {
            reply_payload.timeout=TRUE;
            ((COPYDATASTRUCT *)reply_payload.page)->dwData=TIMEOUT_PAYLOAD_DATA;
        }
#endif
        InterlockedExchange(&reply_driver_done,0);
        sent=SendMessageTimeoutA(peer_window,0x481,0,0,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,15000,&value);
        /* An incomplete/late peer may still own input. Do not free it on a
         * failed path; fail this test process instead of manufacturing cleanup. */
        if (!sent || value != 0x447a0055 || !reply_driver_done || !reply_payload.continued)
            return 0;
        check(reply_payload_finish(&reply_payload,reply_payload.timeout ? "MSGTIME.BIN" : "MSGREPLY.BIN"));
        check(!reply_payload.page && !reply_payload.acknowledgement);
        return errors ? 0 : operation == 8 ? 0x447a0058 : 0x447a0055;
    }
    if (operation == 7) {
        if (GetCurrentThreadId() != sender_thread || !reply_payload.page ||
            !(InSendMessageEx(NULL) & ISMEX_SEND) ||
            InterlockedCompareExchange(&reply_payload.accepted,1,0) != 0) return 0;
        return 0x447a0057;
    }
    if (operation == 6) {
        if (GetCurrentThreadId() != sender_thread || !reply_payload.page ||
            !reply_payload.accepted) return 0;
        return reply_payload_wait(&reply_payload) ? 0x447a0056 : 0;
    }
#endif
    if (operation != 2 || !peer_thread) return 0;
    check(PostMessageA(peer_window, WM_CLOSE, 0, 0));
    if (WaitForSingleObject(peer_thread, 5000) != WAIT_OBJECT_0) return 0;
#ifdef MESSAGE_EXTERNAL_HOST
    check(GetExitCodeProcess(peer_thread,&code) && code==0);
    output=CreateFileA("MSGHOST.BIN",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output==INVALID_HANDLE_VALUE) return 0;
    check(ReadFile(output,report,sizeof(report),&written,NULL) && written==sizeof(report));
    check(CloseHandle(output));
    check(report[0]==0x47534d4e && report[1]==0 && report[2]==GetCurrentProcessId() &&
        report[3]==sender_thread && report[4]==receiver_thread && report[5]==peer_thread_id &&
        report[13]==peer_process_id && (report[14]==32 || report[14]==64));
#else
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
#ifdef M4_STRUCTURES
    check(structure_received == 6);
    report[12]=(DWORD)structure_received;
    report[1]=(DWORD)errors;
#endif
#ifdef M4_COPYDATA_PROC32
    check(proc32_fresh==1 && proc32_borrowed==1);
    report[13]=(DWORD)proc32_fresh;report[14]=(DWORD)proc32_borrowed;
    report[1]=(DWORD)errors;
#endif
#ifdef MESSAGE_CROSS_WORKER
    report[13]=receiver_process_id;
#endif
    output = CreateFileA("MSGHOST.BIN", GENERIC_WRITE, 0, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) return 0;
    check(WriteFile(output, report, sizeof(report), &written, NULL) && written == sizeof(report));
    check(CloseHandle(output));
#endif
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
#endif
