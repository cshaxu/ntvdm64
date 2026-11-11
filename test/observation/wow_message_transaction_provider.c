/* Diagnostic-only whole original TU. No production object/source mutation.
 * The independent native peer calls the real original entry in its current
 * WOW task. Only its exact thread/HWND arm record selects this fault. */
#define W32Win16WndProcEx diagnostic_original_message_proc
#define CallBack16 diagnostic_message_callback
#include "message_original_owner.c"
#undef CallBack16
#undef W32Win16WndProcEx
#include <stdio.h>

extern BOOL WINAPI CallBack16(INT, PPARM16, VPPROC, PVPVOID);
extern BYTE vahdr[];
extern LPBYTE vpHeap16;
typedef struct transaction_call {
    DWORD key, cursor, init;
    unsigned slot, before;
    BOOL injected;
    char phase[24], path[MAX_PATH];
} transaction_call;
static __declspec(thread) transaction_call *live_call;

BOOL WINAPI diagnostic_message_callback(INT id, PPARM16 args, VPPROC proc, PVPVOID output)
{
    BOOL result;
    transaction_call *call = live_call;
    if (!call || call->injected || id != RET_WNDPROC || args->WndProc.wMsg != WM_GETTEXT)
        return CallBack16(id, args, proc, output);
    call->key = args->WndProc.lParam;
    call->slot = (call->key - (DWORD)vpHeap16) / 0x100u;
    if (call->key < (DWORD)vpHeap16 || call->slot >= 32u) call->slot = 32u;
    call->before = call->slot < 32u ? vahdr[call->slot] & 1u : 0u;
    call->injected = TRUE;
    if (!strcmp(call->phase, "BeforeCallback")) RaiseException(0xe4470006, 0, 0, NULL);
    result = CallBack16(id, args, proc, output);
    RaiseException(0xe4470006, 0, 0, NULL);
    return result;
}

LONG W32Win16WndProcEx(HWND hwnd, UINT message, UINT wp, LONG lp, VPWNDPROC proc, PWW words)
{
    transaction_call call = {0};
    char arm[MAX_PATH];
    DWORD select[2], bytes, written, error;
    HANDLE file;
    BOOL selected = FALSE, completed = FALSE;
    LONG result;
    if (!live_call && message == WM_GETTEXT &&
        GetEnvironmentVariableA("MVDM_TEST_MESSAGE_TRANSACTION", call.path, sizeof(call.path)) &&
        GetEnvironmentVariableA("MVDM_TEST_MESSAGE_PHASE", call.phase, sizeof(call.phase)) &&
        (!strcmp(call.phase, "BeforeCallback") || !strcmp(call.phase, "AfterCallback")) &&
        strlen(call.path) + 8 < sizeof(call.path)) {
        strcpy_s(arm, sizeof(arm), call.path);
        strcat_s(arm, sizeof(arm), ".arm");
        file = CreateFileA(arm, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            selected = ReadFile(file, select, sizeof(select), &bytes, NULL) && bytes == sizeof(select) &&
                select[0] == GetCurrentThreadId() && select[1] == (DWORD)hwnd;
            CloseHandle(file);
        }
        if (selected) {
            DeleteFileA(arm);
            call.cursor = CURRENTPTD()->vpCBStack;
            call.init = CURRENTPTD()->dwFlags & TDF_INITCALLBACKSTACK;
            call.slot = 32u;
            live_call = &call;
        }
    }
    __try {
        result = diagnostic_original_message_proc(hwnd, message, wp, lp, proc, words);
        completed = TRUE;
    } __finally {
        if (selected) {
            char line[320];
            error = GetLastError();
            live_call = NULL;
            strcat_s(call.path, sizeof(call.path), ".trace");
            file = CreateFileA(call.path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (file != INVALID_HANDLE_VALUE) {
                bytes = (DWORD)sprintf_s(line, sizeof(line),
                    "pid=%lu tid=%lu task=%04X phase=%s key=%08lX slot=%u before=%u after=%u cursor=%08lX/%08lX init=%lu/%lu injected=%u escaped=%u\r\n",
                    GetCurrentProcessId(), GetCurrentThreadId(), CURRENTPTD()->htask16, call.phase,
                    call.key, call.slot, call.before, call.slot < 32u ? vahdr[call.slot] & 1u : 0u,
                    call.cursor, CURRENTPTD()->vpCBStack, call.init,
                    CURRENTPTD()->dwFlags & TDF_INITCALLBACKSTACK, call.injected, !completed);
                WriteFile(file, line, bytes, &written, NULL);
                CloseHandle(file);
            }
            SetLastError(error);
        }
    }
    return result;
}
