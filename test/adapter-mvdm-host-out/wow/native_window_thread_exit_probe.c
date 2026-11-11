#include <windows.h>
#include <stdio.h>

/* Host observation only. No guest, USER replacement or product linkage.
 * Determine whether native thread teardown delivers the callbacks on which
 * the current WOW binding relies to withdraw its private window property. */
typedef struct exit_case {
    HWND window;
    WNDPROC previous;
    LONG destroy, nc_destroy;
    BOOL control, explicit_destroy, direct_exit;
    DWORD create_error;
} exit_case;

static __declspec(thread) exit_case *current_case;

static LRESULT CALLBACK observe(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    exit_case *test = current_case;
    if (test) {
        if (message == WM_DESTROY) InterlockedIncrement(&test->destroy);
        if (message == WM_NCDESTROY) InterlockedIncrement(&test->nc_destroy);
        if (test->previous)
            return CallWindowProcA(test->previous, window, message, wp, lp);
    }
    return DefWindowProcA(window, message, wp, lp);
}

static DWORD WINAPI run_case(LPVOID argument)
{
    exit_case *test = argument;
    current_case = test;
    test->window = CreateWindowExA(0,
        test->control ? "BUTTON" : "NTVDM.ThreadExitProbe", "",
        WS_POPUP, 0, 0, 16, 16, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!test->window) {
        test->create_error = GetLastError();
        return 1;
    }
    if (test->control) {
        test->previous = (WNDPROC)SetWindowLongPtrA(test->window,
            GWLP_WNDPROC, (LONG_PTR)observe);
        if (!test->previous) return 2;
    }
    if (test->explicit_destroy && !DestroyWindow(test->window)) return 3;
    if (test->direct_exit) ExitThread(0);
    return 0;
}

int main(void)
{
    WNDCLASSA cls = {0};
    unsigned i;
    int errors = 0;
    cls.lpfnWndProc = observe;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = "NTVDM.ThreadExitProbe";
    if (!RegisterClassA(&cls)) return 1;
    for (i = 0; i < 6; ++i) {
        exit_case test = {0};
        DWORD result = STILL_ACTIVE;
        HANDLE thread;
        test.control = i & 1;
        test.explicit_destroy = i >= 4;
        test.direct_exit = i >= 2 && i < 4;
        thread = CreateThread(NULL, 0, run_case, &test, 0, NULL);
        if (!thread) return 2;
        /* Infinite is safe here: no modal UI, external input or message wait
         * is part of the child. An outer runner supplies the process timeout. */
        if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0) return 3;
        GetExitCodeThread(thread, &result);
        printf("case=%u control=%d explicit=%d exitthread=%d exit=%lu "
            "create_error=%lu alive=%d destroy=%ld ncdestroy=%ld\n",
            i, test.control, test.explicit_destroy, test.direct_exit, result,
            test.create_error, IsWindow(test.window), test.destroy, test.nc_destroy);
        if (result || !test.window || IsWindow(test.window) ||
            (test.explicit_destroy &&
                (test.destroy != 1 || test.nc_destroy != 1))) ++errors;
        CloseHandle(thread);
    }
    if (!UnregisterClassA(cls.lpszClassName, cls.hInstance)) ++errors;
    return errors ? 1 : 0;
}
