/* Independent native reference for the fixture's queue/quit assumptions. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
int main(void)
{
    unsigned wide, producer, errors = 0;
    MSG message;
    (void)PeekMessageW(&message, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD);
    for (producer = 0; producer != 2; ++producer) for (wide = 0; wide != 2; ++wide) {
        unsigned seen = 0, step;
        BOOL first_was_nonquit = FALSE;
        const UINT sentinel = WM_APP + 56 + wide;
        if (!(wide ? PostThreadMessageW(GetCurrentThreadId(), sentinel, 141 + wide, 0) :
                PostThreadMessageA(GetCurrentThreadId(), sentinel, 141 + wide, 0))) return 2;
        if (producer) PostQuitMessage(41 + (int)wide);
        else if (!(wide ? PostThreadMessageW(GetCurrentThreadId(), WM_QUIT, 41 + wide, 0) :
                PostThreadMessageA(GetCurrentThreadId(), WM_QUIT, 41 + wide, 0))) return 2;
        for (step = 0; step < 64; ++step) {
            BOOL result = wide ? GetMessageW(&message, NULL, 0, 0) :
                GetMessageA(&message, NULL, 0, 0);
            if (!step) first_was_nonquit = result > 0;
            if (result <= 0) {
                if (result != FALSE || message.message != WM_QUIT ||
                    message.wParam != 41 + wide || seen != 1) ++errors;
                break;
            }
            if (message.message == sentinel) {
                if (seen || message.hwnd || message.wParam != 141 + wide) ++errors;
                ++seen;
            } else if (message.hwnd) DispatchMessageW(&message);
        }
        if (step == 64 || !first_was_nonquit || seen != 1) ++errors;
        printf("NATIVE_QUEUE_REFERENCE wide=%u post_quit=%u ordinary_before_quit=%u sentinel=%u bound=64\n",
            wide, producer, first_was_nonquit, seen);
    }
    printf("NATIVE_QUEUE_REFERENCE errors=%u old_first_call_quit_assumption_rejected=4\n", errors);
    return errors ? 1 : 0;
}
