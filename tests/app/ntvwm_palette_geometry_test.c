/* Exercise real Console APIs and the production body, observing the setter
 * before its caller can repair an incorrectly shrunken viewport. */
#include "ntvwm-exe/console_state.h"
#include <stdio.h>
#include <string.h>

static BOOL WINAPI observed_infoex(HANDLE output,PCONSOLE_SCREEN_BUFFER_INFOEX info)
{
    CONSOLE_SCREEN_BUFFER_INFOEX before={sizeof(before)},after={sizeof(after)};
    BOOL observe=GetConsoleScreenBufferInfoEx(output,&before) &&
        !memcmp(&before.dwSize,&info->dwSize,sizeof(info->dwSize)) &&
        memcmp(before.ColorTable,info->ColorTable,sizeof(info->ColorTable));
    BOOL result;
    DWORD error;
    if(observe && (info->srWindow.Right!=before.srWindow.Right+1 ||
        info->srWindow.Bottom!=before.srWindow.Bottom+1)) {
        fputs("FAIL palette setter missing exclusive right/bottom conversion\n",stderr);
        ExitProcess(90);
    }
    result=SetConsoleScreenBufferInfoEx(output,info);
    error=GetLastError();
    /* Host wrap-text policy can expand a narrow viewport to buffer width.
     * Assert immediate geometry on full-width views; the ordinary fixture
     * separately verifies restoration of narrow/scrolled viewports. */
    if(result && observe && before.srWindow.Left==0 &&
        before.srWindow.Right==before.dwSize.X-1 &&
        (!GetConsoleScreenBufferInfoEx(output,&after) ||
        memcmp(&before.dwSize,&after.dwSize,sizeof(before.dwSize)) ||
        memcmp(&before.srWindow,&after.srWindow,sizeof(before.srWindow)) ||
        memcmp(&before.dwCursorPosition,&after.dwCursorPosition,sizeof(before.dwCursorPosition)))) {
        fputs("FAIL palette setter changed geometry before caller restoration\n",stderr);
        fprintf(stderr,"before=%d,%d view=%d,%d,%d,%d cursor=%d,%d; request=%d,%d,%d,%d; after=%d,%d view=%d,%d,%d,%d cursor=%d,%d\n",
            before.dwSize.X,before.dwSize.Y,before.srWindow.Left,before.srWindow.Top,before.srWindow.Right,before.srWindow.Bottom,
            before.dwCursorPosition.X,before.dwCursorPosition.Y,info->srWindow.Left,info->srWindow.Top,info->srWindow.Right,info->srWindow.Bottom,
            after.dwSize.X,after.dwSize.Y,after.srWindow.Left,after.srWindow.Top,after.srWindow.Right,after.srWindow.Bottom,
            after.dwCursorPosition.X,after.dwCursorPosition.Y);
        ExitProcess(91);
    }
    if(result && observe) fputs("PASS palette setter exclusive conversion\n",stderr);
    SetLastError(error);
    return result;
}

#define SetConsoleScreenBufferInfoEx observed_infoex
#include "ntvwm-exe/console_state.c"
#undef SetConsoleScreenBufferInfoEx
#include "native_console_capture_test.c"
