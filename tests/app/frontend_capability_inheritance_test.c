/* Native boundary evidence only: no guest, broker policy, or product provider.
 * Prove that a synchronize-only unnamed event survives actual CMD nesting;
 * the environment carries a handle locator, not authority by itself. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

typedef BOOL (WINAPI *COMPARE_OBJECT_HANDLES)(HANDLE, HANDLE);
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %u error %lu: %s\n", \
    (unsigned)__LINE__, GetLastError(), #x); return 1; } } while (0)

static int console_contains(DWORD wanted)
{
    DWORD members[32],count=GetConsoleProcessList(members,32),index;
    if (!count || count>32) return -1;
    for (index=0;index<count;++index) if (members[index]==wanted) return 1;
    return 0;
}

static int child(BOOL survive_helper)
{
    WCHAR text[32];
    HANDLE capability, ready;
    DWORD flags;
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_ROOT_PID", text, 32));
    CHECK(console_contains(wcstoul(text,NULL,10))==0);
    CHECK(!GetConsoleWindow() || !IsWindowVisible(GetConsoleWindow()));
    if (GetEnvironmentVariableW(L"NTVDM_TEST_HELPER_PID",text,32))
        CHECK(console_contains(wcstoul(text,NULL,10))==1);
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_CAPABILITY", text, 32));
    capability = (HANDLE)(uintptr_t)wcstoul(text, NULL, 16);
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_READY", text, 32));
    ready = (HANDLE)(uintptr_t)wcstoul(text, NULL, 16);
    CHECK(GetHandleInformation(capability, &flags));
    CHECK(flags & HANDLE_FLAG_INHERIT);
    CHECK(WaitForSingleObject(capability, 0) == WAIT_TIMEOUT);
    SetLastError(0);
    CHECK(!SetEvent(capability) && GetLastError() == ERROR_ACCESS_DENIED);
    if (survive_helper) {
        HANDLE mapping;
        DWORD *pid;
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_CHILD_MAP",text,32));
        mapping=(HANDLE)(uintptr_t)wcstoul(text,NULL,16);
        pid=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(*pid));
        CHECK(pid);
        *pid=GetCurrentProcessId();
        CHECK(UnmapViewOfFile(pid));
    }
    CHECK(SetEvent(ready));
    CHECK(WaitForSingleObject(capability, 10000) == WAIT_OBJECT_0);
    if (survive_helper) {
        HANDLE input,output;
        INPUT_RECORD sent={0},received={0};
        CHAR_INFO cell={0},readback={0};
        COORD size={1,1},origin={0,0};
        SMALL_RECT rectangle={0,0,0,0};
        DWORD count;
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_HELPER_PID",text,32));
        CHECK(console_contains(wcstoul(text,NULL,10))==0);
        CHECK(!GetConsoleWindow() || !IsWindowVisible(GetConsoleWindow()));
        input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        CHECK(input!=INVALID_HANDLE_VALUE && output!=INVALID_HANDLE_VALUE);
        CHECK(SetConsoleMode(input,0) && FlushConsoleInputBuffer(input));
        sent.EventType=KEY_EVENT;sent.Event.KeyEvent.bKeyDown=TRUE;
        sent.Event.KeyEvent.wRepeatCount=1;sent.Event.KeyEvent.wVirtualKeyCode='Z';
        sent.Event.KeyEvent.uChar.UnicodeChar=L'Z';
        CHECK(WriteConsoleInputW(input,&sent,1,&count) && count==1);
        CHECK(ReadConsoleInputW(input,&received,1,&count) && count==1);
        CHECK(received.EventType==KEY_EVENT &&
            received.Event.KeyEvent.uChar.UnicodeChar==L'Z');
        cell.Char.UnicodeChar=L'Z';cell.Attributes=FOREGROUND_GREEN;
        CHECK(WriteConsoleOutputW(output,&cell,size,origin,&rectangle));
        CHECK(ReadConsoleOutputW(output,&readback,size,origin,&rectangle));
        CHECK(readback.Char.UnicodeChar==L'Z' && readback.Attributes==cell.Attributes);
        CloseHandle(output);CloseHandle(input);
    }
    return 37;
}

static int launch(const WCHAR *command, HANDLE capability, HANDLE ready, DWORD flags)
{
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION process = {0};
    WCHAR copy[4096];
    DWORD result, wait;
    HANDLE waits[2];
    CHECK(!wcscpy_s(copy, 4096, command));
    CHECK(ResetEvent(capability) && ResetEvent(ready));
    startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    CHECK(CreateProcessW(NULL, copy, NULL, NULL, TRUE, flags,
                         NULL, NULL, &startup, &process));
    CloseHandle(process.hThread);
    waits[0] = ready;
    waits[1] = process.hProcess;
    wait = WaitForMultipleObjects(2, waits, FALSE, 15000);
    if (wait != WAIT_OBJECT_0) {
        /* Only this fixture's exact created process is eligible for cleanup. */
        TerminateProcess(process.hProcess, 99);
        WaitForSingleObject(process.hProcess, 5000);
        CloseHandle(process.hProcess);
        fprintf(stderr, "child did not acknowledge capability: %lu\n", wait);
        return 1;
    }
    CHECK(SetEvent(capability));
    wait = WaitForSingleObject(process.hProcess, 15000);
    if (wait != WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess, 99);
        WaitForSingleObject(process.hProcess, 5000);
        CloseHandle(process.hProcess);
        return 1;
    }
    CHECK(GetExitCodeProcess(process.hProcess, &result));
    CloseHandle(process.hProcess);
    CHECK(result == 37);
    return 0;
}

/* Preserve the capability while changing only this helper's attachment.
 * This proves the OS boundary, not a broker execution-context grant. */
static int rebind_helper(BOOL survive_helper)
{
    WCHAR text[32],executable[MAX_PATH],cmd[MAX_PATH],command[4096];
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};
    DWORD root,result;
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_ROOT_PID",text,32));
    root=wcstoul(text,NULL,10);
    CHECK(console_contains(root)==1);
    CHECK(FreeConsole());
    CHECK(AllocConsole());
    CHECK(console_contains(root)==0);
    CHECK(!GetConsoleWindow() || !IsWindowVisible(GetConsoleWindow()));
    swprintf_s(text,32,L"%lu",GetCurrentProcessId());
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_HELPER_PID",text));
    CHECK(GetModuleFileNameW(NULL,executable,MAX_PATH));
    CHECK(GetEnvironmentVariableW(L"COMSPEC",cmd,MAX_PATH));
    if (survive_helper)
        CHECK(swprintf_s(command,4096,L"\"%ls\" --survivor-child",executable)>0);
    else
        CHECK(swprintf_s(command,4096,L"\"%ls\" /d /s /c \"\"%ls\" --child\"",cmd,executable)>0);
    CHECK(CreateProcessW(NULL,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&process));
    CloseHandle(process.hThread);
    if (WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);
        CloseHandle(process.hProcess);return 1;
    }
    CHECK(GetExitCodeProcess(process.hProcess,&result));
    CloseHandle(process.hProcess);
    CHECK(result==37);
    return 37;
}

/* Kill only the fixture helper after its child has published readiness. The
 * child remains blocked on capability, so its PID cannot be reused between
 * publication and OpenProcess. Retain its HANDLE before killing the helper.
 * This proves that a helper-owned broker connection alone cannot represent
 * the lifetime of the hidden Console's execution association. */
static int helper_loss(const WCHAR *executable,HANDLE capability,HANDLE ready)
{
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION helper={0};
    WCHAR command[4096],text[32];
    HANDLE mapping=NULL,child_process=NULL,waits[2];
    DWORD *child_pid=NULL,result=0;
    int failed=1;
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"helper loss line %u error %lu: %s\n", \
    (unsigned)__LINE__,GetLastError(),#x);goto done; } } while (0)
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,&security,PAGE_READWRITE,0,sizeof(DWORD),NULL);
    REQUIRE(mapping);
    child_pid=MapViewOfFile(mapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,sizeof(DWORD));
    REQUIRE(child_pid);*child_pid=0;
    swprintf_s(text,32,L"%lx",(unsigned long)(uintptr_t)mapping);
    REQUIRE(SetEnvironmentVariableW(L"NTVDM_TEST_CHILD_MAP",text));
    REQUIRE(ResetEvent(capability) && ResetEvent(ready));
    REQUIRE(swprintf_s(command,4096,L"\"%ls\" --survivor-helper",executable)>0);
    startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    REQUIRE(CreateProcessW(NULL,command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&helper));
    CloseHandle(helper.hThread);helper.hThread=NULL;
    waits[0]=ready;waits[1]=helper.hProcess;
    REQUIRE(WaitForMultipleObjects(2,waits,FALSE,15000)==WAIT_OBJECT_0);
    REQUIRE(*child_pid && *child_pid!=GetCurrentProcessId() && *child_pid!=helper.dwProcessId);
    child_process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,
        FALSE,*child_pid);
    REQUIRE(child_process && WaitForSingleObject(child_process,0)==WAIT_TIMEOUT);
    REQUIRE(TerminateProcess(helper.hProcess,99));
    REQUIRE(WaitForSingleObject(helper.hProcess,5000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(helper.hProcess,&result) && result==99);
    REQUIRE(WaitForSingleObject(child_process,0)==WAIT_TIMEOUT);
    REQUIRE(console_contains(GetCurrentProcessId())==1 && console_contains(*child_pid)==0);
    REQUIRE(SetEvent(capability));
    REQUIRE(WaitForSingleObject(child_process,15000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(child_process,&result) && result==37);
    failed=0;
done:
    if (child_process) {
        if (WaitForSingleObject(child_process,0)==WAIT_TIMEOUT) {
            TerminateProcess(child_process,99);WaitForSingleObject(child_process,5000);
        }
        CloseHandle(child_process);
    }
    if (helper.hProcess) {
        if (WaitForSingleObject(helper.hProcess,0)==WAIT_TIMEOUT) {
            TerminateProcess(helper.hProcess,99);WaitForSingleObject(helper.hProcess,5000);
        }
        CloseHandle(helper.hProcess);
    }
    if (child_pid) UnmapViewOfFile(child_pid);
    if (mapping) CloseHandle(mapping);
    SetEnvironmentVariableW(L"NTVDM_TEST_CHILD_MAP",NULL);
#undef REQUIRE
    return failed;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR executable[MAX_PATH], command[4096], cmd[MAX_PATH], text[32];
    SECURITY_ATTRIBUTES security = { sizeof(security), NULL, TRUE };
    HANDLE event, capability, ready, other;
    COMPARE_OBJECT_HANDLES compare;
    if (argc == 2 && !wcscmp(argv[1], L"--child")) return child(FALSE);
    if (argc == 2 && !wcscmp(argv[1], L"--survivor-child")) return child(TRUE);
    if (argc == 2 && !wcscmp(argv[1], L"--rebind-helper")) return rebind_helper(FALSE);
    if (argc == 2 && !wcscmp(argv[1], L"--survivor-helper")) return rebind_helper(TRUE);
    swprintf_s(text,32,L"%lu",GetCurrentProcessId());
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_ROOT_PID",text));
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_HELPER_PID",NULL));
    compare = (COMPARE_OBJECT_HANDLES)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"), "CompareObjectHandles");
    CHECK(compare);
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    other = CreateEventW(NULL, TRUE, FALSE, NULL);
    ready = CreateEventW(&security, TRUE, FALSE, NULL);
    CHECK(event && other && ready);
    CHECK(DuplicateHandle(GetCurrentProcess(), event, GetCurrentProcess(),
                          &capability, SYNCHRONIZE, TRUE, 0));
    CHECK(compare(event, capability));
    CHECK(!compare(event, other));
    swprintf_s(text, 32, L"%lx", (unsigned long)(uintptr_t)capability);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_CAPABILITY", text));
    swprintf_s(text, 32, L"%lx", (unsigned long)(uintptr_t)ready);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_READY", text));
    CHECK(GetModuleFileNameW(NULL, executable, MAX_PATH));
    CHECK(GetEnvironmentVariableW(L"COMSPEC", cmd, MAX_PATH));
    CHECK(swprintf_s(command, 4096, L"\"%ls\" --child", executable) > 0);
    CHECK(!launch(command, event, ready,CREATE_NO_WINDOW));
    puts("PASS direct inheritance, restricted rights, completion=37");
    CHECK(swprintf_s(command, 4096, L"\"%ls\" /d /s /c \"\"%ls\" --child\"",
                     cmd, executable) > 0);
    CHECK(!launch(command, event, ready,CREATE_NO_WINDOW));
    puts("PASS CMD inheritance, restricted rights, completion=37");
    CHECK(swprintf_s(command, 4096,
        L"\"%ls\" /d /s /c \"\"%ls\" /d /s /c \"\"%ls\" --child\"\"",
        cmd, cmd, executable) > 0);
    CHECK(!launch(command, event, ready,CREATE_NO_WINDOW));
    puts("PASS nested CMD inheritance, restricted rights, completion=37");
    CHECK(swprintf_s(command,4096,L"\"%ls\" --rebind-helper",executable)>0);
    CHECK(!launch(command,event,ready,0));
    puts("PASS inherited visible membership -> hidden helper -> CMD child, capability retained, completion=37");
    CHECK(!helper_loss(executable,event,ready));
    puts("PASS helper death=99, native child survives with hidden Console input/output, completion=37");
    CloseHandle(capability);
    CloseHandle(event);
    CloseHandle(other);
    CloseHandle(ready);
    return 0;
}
