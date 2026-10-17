#include "wow_default_window_bindings.h"
#include <stdio.h>

/* Native input-capture witness. Links production state/messages/drawing
 * bindings and the original message table slice, not the default wrapper. */
__declspec(thread) wow_default_context *wow_default_current;
static int failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

int main(void)
{
    wow_default_context context;
    unsigned i;
    SetLastError(456);
    CHECK(wow_default_capture_context(&context, WM_CTLCOLOREDIT));
    CHECK(GetLastError() == 456 && !wow_default_current);
    CHECK(context.server.uiShellMsg == RegisterWindowMessageW(L"SHELLHOOK"));
    for (i = 0; i < WOW_EDIT_COLOR_COUNT; ++i)
        CHECK(context.server.argbSystem[i] == GetSysColor(i) &&
            wow_edit_system_brushes[i] == GetSysColorBrush(i));
    CHECK(wow_edit_gray_brush != NULL);
    SetLastError(789);
    CHECK(wow_default_capture_context(&context, WM_CTLCOLORSCROLLBAR));
    CHECK(context.screen.BitCount > 0 && GetLastError() == 789);
    {
        int border = -1;
        CHECK(SystemParametersInfoW(SPI_GETBORDER, 0, &border, 0));
        CHECK(wow_default_capture_context(&context, WM_NCHITTEST));
        CHECK(context.server.gclBorder == border);
    }
    CHECK(!wow_default_ensure_shell_message(NULL));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    wow_default_current = &context;
    context.server.uiShellMsg = 0;
    CHECK(!wow_default_ensure_shell_message((HWND)(ULONG_PTR)1));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && !context.server.uiShellMsg);
    SetLastError(123);
    CHECK(wow_default_ensure_shell_message(NULL));
    CHECK(GetLastError() == 123 && context.server.uiShellMsg ==
        wow_default_message_state.uiShellMsg);
    wow_default_current = NULL;
    CHECK(!wow_default_capture_context(NULL, WM_HELP));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    printf("default_state_failures=%d\n", failures);
    return failures != 0;
}
