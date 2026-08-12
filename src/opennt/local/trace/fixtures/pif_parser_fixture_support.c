/* Fail-closed support for the original PIF parser's unexercised branches. */
#include <windows.h>

#include "nt_pif.h"

BOOL bPifFastPaste = TRUE;
PIF_DATA pfdata;
static unsigned long unexpected_pif_callback_count;

DWORD WINAPI ExpandEnvironmentStringsOem(LPCSTR source, LPSTR destination,
                                         DWORD destination_size) {
    (void)source;
    (void)destination;
    (void)destination_size;
    ++unexpected_pif_callback_count;
    return 0;
}

int RcMessageBox(UINT message_id, CHAR *message_1, CHAR *message_2,
                 DWORD options) {
    (void)message_id;
    (void)message_1;
    (void)message_2;
    (void)options;
    ++unexpected_pif_callback_count;
    return 0;
}

unsigned long ntdos64_pif_fixture_unexpected_callbacks(void) {
    return unexpected_pif_callback_count;
}
