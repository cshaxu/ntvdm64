/* The isolated modal/context unit does not exercise native ShellAbout or
 * MessageBox. Resolve their object-level references with fail-loud sentinels,
 * not successful execution doubles. Actual native/modal behavior remains a
 * separate required production-owner/CCPU acceptance gate. Never link this
 * file into the provider or native/real-guest acceptance executables. */
#include "wow_user_task_lifecycle.h"
#include <stdio.h>

__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    fprintf(stderr, "UNREACHED_NATIVE_MODAL stage=%s error=%lu\n", stage, error);
    ExitProcess(97);
}

BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND window)
{
    (void)call; (void)window;
    wow_user_native_call_failed("unexpected-native-entry", ERROR_INVALID_STATE);
}

BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    (void)call;
    wow_user_native_call_failed("unexpected-native-return", ERROR_INVALID_STATE);
}
