/* A prior type-only mirror include must not suppress the opt-in layouts. */
#include "opennt-host/windows/core/ntuser/inc/user.h"
#include "wow_user_client_view_layout.h"
#include <stdio.h>

int main(void)
{
    /* The header asserts the pinned USER.EXE's WND consumer offsets.
     * This is layout verification, not a fabricated runtime desktop. */
    fprintf(stderr, "WOW_ORIGINAL_USER_CLIENT_LAYOUT_OK wnd=%lu desktop=%lu\n",
        (unsigned long)sizeof(WND), (unsigned long)sizeof(DESKTOPINFO));
    return 0;
}
