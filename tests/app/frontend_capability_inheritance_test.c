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

static int child(void)
{
    WCHAR text[32];
    HANDLE capability, ready;
    DWORD flags;
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_CAPABILITY", text, 32));
    capability = (HANDLE)(uintptr_t)wcstoul(text, NULL, 16);
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_READY", text, 32));
    ready = (HANDLE)(uintptr_t)wcstoul(text, NULL, 16);
    CHECK(GetHandleInformation(capability, &flags));
    CHECK(flags & HANDLE_FLAG_INHERIT);
    CHECK(WaitForSingleObject(capability, 0) == WAIT_TIMEOUT);
    SetLastError(0);
    CHECK(!SetEvent(capability) && GetLastError() == ERROR_ACCESS_DENIED);
    CHECK(SetEvent(ready));
    CHECK(WaitForSingleObject(capability, 10000) == WAIT_OBJECT_0);
    return 37;
}

static int launch(const WCHAR *command, HANDLE capability, HANDLE ready)
{
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION process = {0};
    WCHAR copy[4096];
    DWORD result, wait;
    HANDLE waits[2];
    CHECK(!wcscpy_s(copy, 4096, command));
    CHECK(ResetEvent(capability) && ResetEvent(ready));
    CHECK(CreateProcessW(NULL, copy, NULL, NULL, TRUE, CREATE_NO_WINDOW,
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

int wmain(int argc, WCHAR **argv)
{
    WCHAR executable[MAX_PATH], command[4096], cmd[MAX_PATH], text[32];
    SECURITY_ATTRIBUTES security = { sizeof(security), NULL, TRUE };
    HANDLE event, capability, ready, other;
    COMPARE_OBJECT_HANDLES compare;
    if (argc == 2 && !wcscmp(argv[1], L"--child")) return child();
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
    CHECK(!launch(command, event, ready));
    puts("PASS direct inheritance, restricted rights, completion=37");
    CHECK(swprintf_s(command, 4096, L"\"%ls\" /d /s /c \"\"%ls\" --child\"",
                     cmd, executable) > 0);
    CHECK(!launch(command, event, ready));
    puts("PASS CMD inheritance, restricted rights, completion=37");
    CHECK(swprintf_s(command, 4096,
        L"\"%ls\" /d /s /c \"\"%ls\" /d /s /c \"\"%ls\" --child\"\"",
        cmd, cmd, executable) > 0);
    CHECK(!launch(command, event, ready));
    puts("PASS nested CMD inheritance, restricted rights, completion=37");
    CloseHandle(capability);
    CloseHandle(event);
    CloseHandle(other);
    CloseHandle(ready);
    return 0;
}
