#include "precomp.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_thunk_scope.h"

#include <stdio.h>

static LONG trace_count;

static void trace_thunk(PVDMFRAME frame, LPFNW32 thunk)
{
    char path[MAX_PATH];
    char line[192];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    if (!frame || InterlockedIncrement(&trace_count) > 128 ||
            !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", path,
                sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu W32Dispatch call=%04X thunk=%08lX task=%04X return=%08lX ax=%04X args=%u\r\n",
        (unsigned long)GetCurrentProcessId(), frame->wCallID,
        (unsigned long)(ULONG_PTR)thunk, frame->wTDB,
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

    if (!frame || !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH",
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
