/*
 * Native proof for the original mvdm/dbg/dbg.c::SendVDMEvent transport.
 *
 * The original worker raises STATUS_VDM_EVENT with four fixed-width payload
 * words and returns normally only when a debugger handles that exception.
 * This test deliberately uses only the documented Win32 debug-event path;
 * it neither starts a VDM nor supplies a replacement debugger protocol.
 */

#include <windows.h>
#include <stdio.h>

#define STATUS_VDM_EVENT ((DWORD)0x40000005L)
#define EVENT_WORD0 ((ULONG_PTR)0x0000000aU)
#define EVENT_WORD1 ((ULONG_PTR)0x11111111U)
#define EVENT_WORD2 ((ULONG_PTR)0x22222222U)
#define EVENT_WORD3 ((ULONG_PTR)0x33333333U)
#define EVENT_PAYLOAD_INITIAL ((DWORD)0x5a5a1234UL)
#define EVENT_PAYLOAD_UPDATED ((DWORD)0x9876abcdUL)
#define EVENT_CONTEXT_INITIAL ((DWORD)0x12345678UL)

typedef struct _EVENT_INTERNAL {
    DWORD *context;
    DWORD marker;
} EVENT_INTERNAL;

static int child_handled(void)
{
    DWORD context = EVENT_CONTEXT_INITIAL;
    DWORD payload = EVENT_PAYLOAD_INITIAL;
    EVENT_INTERNAL internal;
    ULONG_PTR parameters[4];

    internal.context = &context;
    internal.marker = EVENT_WORD3;
    parameters[0] = EVENT_WORD0;
    parameters[1] = EVENT_WORD1;
    parameters[2] = (ULONG_PTR)&payload;
    parameters[3] = (ULONG_PTR)&internal;

    RaiseException(STATUS_VDM_EVENT, 0, 4, parameters);
    if (payload != EVENT_PAYLOAD_UPDATED || context != EVENT_CONTEXT_INITIAL)
        return 2;
    puts("S43_DEBUGGER_EVENT_CHILD_CONTINUED");
    return 73;
}

static int child_unhandled(void)
{
    ULONG_PTR parameters[4] = { 0, 0, 0, 0 };

    __try {
        RaiseException(STATUS_VDM_EVENT, 0, 4, parameters);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        puts("S43_DEBUGGER_EVENT_CHILD_SEH_FALLBACK");
        return 74;
    }
    return 1;
}

static int run_unhandled(void)
{
    int code = child_unhandled();
    if (code != 74) {
        puts("FAIL unhandled fallback");
        return 1;
    }
    return 0;
}

static int run_debugger(void)
{
    WCHAR image[MAX_PATH];
    WCHAR command[(MAX_PATH * 2) + 32];
    STARTUPINFOW startup;
    PROCESS_INFORMATION process;
    DEBUG_EVENT event;
    DWORD exit_code = 0;
    BOOL saw_vdm_event = FALSE;
    BOOL child_exited = FALSE;
    HANDLE child_process = NULL;

    if (!GetModuleFileNameW(NULL, image, MAX_PATH)) {
        puts("FAIL GetModuleFileName");
        return 1;
    }
    if (wsprintfW(command, L"\"%s\" --child", image) == 0) {
        puts("FAIL command overflow");
        return 1;
    }
    ZeroMemory(&startup, sizeof(startup));
    startup.cb = sizeof(startup);
    ZeroMemory(&process, sizeof(process));
    if (!CreateProcessW(image, command, NULL, NULL, FALSE,
            DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &startup, &process)) {
        puts("FAIL CreateProcess");
        return 1;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);

    while (!child_exited) {
        DWORD continue_status = DBG_CONTINUE;

        if (!WaitForDebugEvent(&event, 10000)) {
            puts("FAIL WaitForDebugEvent");
            return 1;
        }
        if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
            EXCEPTION_RECORD *record = &event.u.Exception.ExceptionRecord;

            if (record->ExceptionCode == STATUS_VDM_EVENT) {
                DWORD payload;
                EVENT_INTERNAL internal;
                DWORD context;
                DWORD updated = EVENT_PAYLOAD_UPDATED;
                SIZE_T transferred;

                if (child_process == NULL || record->NumberParameters != 4 ||
                    record->ExceptionInformation[0] != EVENT_WORD0 ||
                    record->ExceptionInformation[1] != EVENT_WORD1 ||
                    record->ExceptionInformation[2] == 0 ||
                    record->ExceptionInformation[3] == 0) {
                    puts("FAIL STATUS_VDM_EVENT argument layout");
                    return 1;
                }
                if (!ReadProcessMemory(child_process,
                        (LPCVOID)record->ExceptionInformation[2], &payload,
                        sizeof(payload), &transferred) ||
                    transferred != sizeof(payload) ||
                    payload != EVENT_PAYLOAD_INITIAL ||
                    !ReadProcessMemory(child_process,
                        (LPCVOID)record->ExceptionInformation[3], &internal,
                        sizeof(internal), &transferred) ||
                    transferred != sizeof(internal) ||
                    internal.marker != EVENT_WORD3 || internal.context == NULL ||
                    !ReadProcessMemory(child_process, internal.context, &context,
                        sizeof(context), &transferred) ||
                    transferred != sizeof(context) ||
                    context != EVENT_CONTEXT_INITIAL ||
                    !WriteProcessMemory(child_process,
                        (LPVOID)record->ExceptionInformation[2],
                        &updated, sizeof(updated),
                        &transferred) || transferred != sizeof(DWORD)) {
                    puts("FAIL STATUS_VDM_EVENT payload access");
                    return 1;
                }
                saw_vdm_event = TRUE;
            }
        } else if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            child_process = event.u.CreateProcessInfo.hProcess;
            if (event.u.CreateProcessInfo.hFile != NULL)
                CloseHandle(event.u.CreateProcessInfo.hFile);
        } else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
            if (event.u.LoadDll.hFile != NULL)
                CloseHandle(event.u.LoadDll.hFile);
        } else if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
            exit_code = event.u.ExitProcess.dwExitCode;
            child_exited = TRUE;
        }
        if (!ContinueDebugEvent(event.dwProcessId, event.dwThreadId,
                continue_status)) {
            puts("FAIL ContinueDebugEvent");
            return 1;
        }
    }
    if (!saw_vdm_event || exit_code != 73) {
        puts("FAIL event or exit");
        return 1;
    }
    if (child_process != NULL)
        CloseHandle(child_process);
    puts("S43_DEBUGGER_EVENT_TRANSPORT_OK");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && lstrcmpA(argv[1], "--child") == 0)
        return child_handled();
    if (argc == 2 && lstrcmpA(argv[1], "--unhandled") == 0)
        return run_unhandled();
    if (argc != 1) {
        puts("FAIL unexpected arguments");
        return 1;
    }
    if (run_unhandled() != 0)
        return 1;
    return run_debugger();
}
