#include "precomp.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_thunk_scope.h"

#include <stdio.h>

/* Both sides of a traced thunk must use the same bounded admission state.
 * The prior one-sided limit stopped entries after 128 but kept writing every
 * return, so a crash trace's tail could not be paired to a call.  This
 * host-only observer remains disabled unless its path variable is set. */
static LONG trace_count;
#define WOW_TRACE_STACK_MAXIMUM 64
static __declspec(thread) unsigned char trace_admitted[WOW_TRACE_STACK_MAXIMUM];
static __declspec(thread) unsigned char trace_depth;

static void trace_thunk(PVDMFRAME frame, LPFNW32 thunk)
{
    char path[MAX_PATH];
    char line[192];
    HANDLE file;
    DWORD bytes;
    DWORD written;
    HMODULE module;
    ULONG_PTR rva;

    unsigned char slot;

    if (trace_depth == WOW_TRACE_STACK_MAXIMUM)
        return;
    slot = trace_depth++;
    trace_admitted[slot] = 0;
    if (!frame || InterlockedIncrement(&trace_count) > 2048 ||
            !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", path,
                sizeof(path)))
        return;
    trace_admitted[slot] = 1;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    module = GetModuleHandleA("WOW32.DLL");
    rva = module && (ULONG_PTR)thunk >= (ULONG_PTR)module ?
        (ULONG_PTR)thunk - (ULONG_PTR)module : 0;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu W32Dispatch call=%04X thunk=%08lX rva=%08lX task=%04X return=%08lX ax=%04X args=%u\r\n",
        (unsigned long)GetCurrentProcessId(), frame->wCallID,
        (unsigned long)(ULONG_PTR)thunk, (unsigned long)rva, frame->wTDB,
        (unsigned long)frame->vpCSIP, frame->wAX, (unsigned)frame->cbArgs);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void trace_return(PVDMFRAME frame, ULONG result)
{
    char path[MAX_PATH];
    char line[112];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    unsigned char admitted;

    if (trace_depth == 0)
        return;
    admitted = trace_admitted[--trace_depth];
    if (!frame || !admitted ||
            !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH",
            path, sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu W32Return call=%04X result=%08lX task=%04X\r\n",
        (unsigned long)GetCurrentProcessId(), frame->wCallID,
        (unsigned long)result, frame->wTDB);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

/* Observation-only: WK32FileOpen owns normalization and DOS error mapping.
 * Record the two source-shaped names only after that original body has failed;
 * this helper neither opens files nor changes last error. */
VOID WINAPI wow_user_trace_file_open_failure(LPCSTR source, LPCSTR normalized,
    WORD access, DWORD error)
{
    char path[MAX_PATH];
    char line[MAX_PATH * 2];
    HANDLE file;
    DWORD bytes;
    DWORD written;
    DWORD saved = GetLastError();

    if (!GetEnvironmentVariableA("MVDM_WOW_FILE_TRACE_PATH", path,
            sizeof(path))) {
        SetLastError(saved);
        return;
    }
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        SetLastError(saved);
        return;
    }
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu WOWFileOpen failed access=%04X error=%lu source=%s normalized=%s\r\n",
        (unsigned long)GetCurrentProcessId(), (unsigned)access,
        (unsigned long)error, source ? source : "<null>",
        normalized ? normalized : "<null>");
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
    SetLastError(saved);
}

/* ADAPTER-WOW-032: retain private USER aliases only for the original thunk
 * invocation. Do not catch/translate exceptions or alter frame/result state;
 * original W32Dispatch/W32TryCall remain the exception-policy owners. */
ULONG WINAPI wow_user_invoke_thunk(LPFNW32 thunk, PVDMFRAME frame)
{
    wow_user_borrow_scope scope;
    ULONG result;
    trace_thunk(frame, thunk);
    wow_user_borrow_enter(&scope);
    __try {
        result = (*thunk)(frame);
    } __finally {
        wow_user_borrow_leave(&scope);
    }
    trace_return(frame, result);
    return result;
}
