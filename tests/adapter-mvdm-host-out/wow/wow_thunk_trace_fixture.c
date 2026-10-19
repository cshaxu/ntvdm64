#include "precomp.h"
#include "wow_user_thunk_scope.h"
#include "wow_user_borrow_scope.h"
#include "wow_task_order_bindings.h"
#include "wow_user_native_call.h"
#include <stdio.h>
#include <string.h>

/* Directly link the production diagnostic owner. These rejected, uncalled
 * scope dependencies make accidental dispatch/lifecycle work a hard failure;
 * they do not stand in for the real guest callback acceptance. */
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{ (void)scope; ExitProcess(91); return FALSE; }
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{ (void)scope; ExitProcess(92); }
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *name, DWORD error)
{ (void)name; (void)error; ExitProcess(93); }
/* The production TU also owns filter/modal-service boundaries. They are
 * outside this pure diagnostic test: fail closed if selected, and require
 * the link map to discard them. TLS declarations match the actual owner. */
__declspec(thread) BOOL fWhoCalled, fThunkDDEmsg;
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND target)
{ (void)call; (void)target; ExitProcess(94); return FALSE; }
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{ (void)call; ExitProcess(95); return FALSE; }
ULONG __cdecl WOWSysErrorBox(LPSTR title, LPSTR text, USHORT first,
    USHORT second, USHORT third)
{
    (void)title; (void)text; (void)first; (void)second; (void)third;
    ExitProcess(96); return 0;
}
INT WINAPI W32Exception(DWORD code, PEXCEPTION_POINTERS info)
{ (void)code; (void)info; ExitProcess(97); return EXCEPTION_CONTINUE_SEARCH; }

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%u error=%lu\n",__LINE__,GetLastError()); } } while (0)
static void pair(UINT message, DWORD raw, DWORD procedure, BOOL success, LONG result)
{
    SetLastError(0x4471);
    wow_user_trace_wndproc_enter(message, raw, procedure);
    CHECK(GetLastError() == 0x4471);
    SetLastError(0x4472);
    wow_user_trace_wndproc_return(message, procedure, success, result);
    CHECK(GetLastError() == 0x4472);
}

int __cdecl main(void)
{
    CHAR path[MAX_PATH], missing[MAX_PATH], expected[512], actual[512] = {0};
    CHAR *slash;
    DWORD length, bytes = 0;
    HANDLE file;
    length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (!length || length >= sizeof(path) || !(slash = strrchr(path, '\\'))) return 1;
    strcpy_s(slash + 1, sizeof(path) - (slash + 1 - path), "thunk-trace.txt");
    CHECK(GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES);
    SetEnvironmentVariableA("MVDM_WOW_CALLBACK_TRACE_PATH", NULL);
    pair(0x447, 0xffff5678, 0x7fff5678, TRUE, -49);
    CHECK(GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES);
    CHECK(SetEnvironmentVariableA("MVDM_WOW_CALLBACK_TRACE_PATH", path));
    pair(0x447, 0xffff5678, 0x7fff5678, TRUE, -49);
    pair(0x448, 0, 0x5678, FALSE, 0);
    /* Exclusive read proves neither moved diagnostic retained its writer. */
    file = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK(file != INVALID_HANDLE_VALUE);
    if (file != INVALID_HANDLE_VALUE) {
        CHECK(ReadFile(file, actual, sizeof(actual) - 1, &bytes, NULL));
        CHECK(CloseHandle(file));
    }
    sprintf_s(expected, sizeof(expected),
        "%lu W32WndProc-enter message=0447 raw=FFFF5678 proc=7FFF5678\r\n"
        "%lu W32WndProc message=0447 proc=7FFF5678 success=1 result=FFFFFFCF\r\n"
        "%lu W32WndProc-enter message=0448 raw=00000000 proc=00005678\r\n"
        "%lu W32WndProc message=0448 proc=00005678 success=0 result=00000000\r\n",
        GetCurrentProcessId(), GetCurrentProcessId(), GetCurrentProcessId(), GetCurrentProcessId());
    CHECK(bytes == strlen(expected) && !strcmp(actual, expected));
    strcpy_s(missing, sizeof(missing), path);
    slash = strrchr(missing, '\\');
    strcpy_s(slash + 1, sizeof(missing) - (slash + 1 - missing), "missing-trace-dir\\trace.txt");
    CHECK(SetEnvironmentVariableA("MVDM_WOW_CALLBACK_TRACE_PATH", missing));
    pair(0x449, 1, 2, TRUE, 3);
    CHECK(SetEnvironmentVariableA("MVDM_WOW_CALLBACK_TRACE_PATH", NULL));
    pair(0x44a, 1, 2, FALSE, -1);
    CHECK(GetFileAttributesA(missing) == INVALID_FILE_ATTRIBUTES);
    printf("WOW_THUNK_TRACE errors=%u disabled=1 enabled=2 failed_output=1 exact_text=1 last_error=1 no_scope=1\n", errors);
    return errors ? 1 : 0;
}
