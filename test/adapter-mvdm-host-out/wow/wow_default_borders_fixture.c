#include "wow_default_window_bindings.h"
#include <stdio.h>

/* Links unchanged original GetWindowBorders under its finite selector.
 * Tests logical units, not modern frame pixels or complete hit testing. */
__declspec(thread) wow_default_context *wow_default_current;
static int failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

int main(void)
{
    wow_default_context context = {0};
    wow_default_current = &context;
    context.server.gclBorder = 3;
    CHECK(GetWindowBorders(0, 0, TRUE, TRUE) == 0);
    CHECK(GetWindowBorders(WS_BORDER, 0, TRUE, FALSE) == 1);
    CHECK(GetWindowBorders(WS_DLGFRAME, 0, TRUE, FALSE) == 1);
    CHECK(GetWindowBorders(0, WS_EX_DLGMODALFRAME, TRUE, FALSE) == 1);
    CHECK(GetWindowBorders(0, WS_EX_STATICEDGE, TRUE, FALSE) == 1);
    CHECK(GetWindowBorders(0, WS_EX_WINDOWEDGE | WS_EX_STATICEDGE, TRUE, FALSE) == 2);
    CHECK(GetWindowBorders(WS_THICKFRAME, 0, TRUE, FALSE) == 3);
    CHECK(GetWindowBorders(WS_CAPTION | WS_THICKFRAME,
        WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, TRUE, TRUE) == 8);
    CHECK(GetWindowBorders(WS_CAPTION | WS_THICKFRAME,
        WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, FALSE, TRUE) == 2);
    CHECK(GetWindowBorders(WS_CAPTION | WS_THICKFRAME,
        WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, FALSE, FALSE) == 0);
    context.server.gclBorder = 7;
    CHECK(GetWindowBorders(WS_THICKFRAME, 0, TRUE, FALSE) == 7);
    wow_default_current = NULL;
    printf("default_border_failures=%d\n", failures);
    return failures != 0;
}
