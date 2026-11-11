/* Native ABI characterization only, not a WOW capability test. All windows
 * are owned by this process; no private structure or desktop switch. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

int main(void)
{
    WNDCLASSA cls = {0};
    HWND window, forms[3];
    DWORD i, pid, tid, error;
    BOOL valid;
    cls.lpfnWndProc = DefWindowProcA;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpszClassName = "WOW_NATIVE_HANDLE_CONTRACT";
    if (!RegisterClassA(&cls)) return 2;
    window = CreateWindowA(cls.lpszClassName, "", WS_POPUP,
        0, 0, 1, 1, NULL, NULL, cls.hInstance, NULL);
    if (!window) return 3;
    forms[0] = window;
    forms[1] = (HWND)(ULONG_PTR)LOWORD(window);
    forms[2] = (HWND)(LONG_PTR)(SHORT)LOWORD(window);
    for (i = 0; i != 3; ++i) {
        pid = 0xcccccccc;
        SetLastError(ERROR_SUCCESS);
        tid = GetWindowThreadProcessId(forms[i], &pid);
        error = GetLastError();
        valid = IsWindow(forms[i]);
        printf("NATIVE_WINDOW_HANDLE form=%lu hwnd=%p tid=%lu pid=%lu query_error=%lu valid=%d expected_tid=%lu expected_pid=%lu\n",
            i, (void *)forms[i], tid, pid, error, valid,
            GetCurrentThreadId(), GetCurrentProcessId());
        if (!i && (tid != GetCurrentThreadId() || pid != GetCurrentProcessId() || !valid)) return 4;
    }
    if (!DestroyWindow(window) || !UnregisterClassA(cls.lpszClassName, cls.hInstance)) return 5;
    return 0;
}
