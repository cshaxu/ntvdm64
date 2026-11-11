/*
 * Fail-closed callbacks for original nt_pif.c when it is linked with the
 * original config.c owner. config.c owns pfdata; this fixture must not define
 * another copy of that historical global.
 */
#include <windows.h>

extern void runner_config_prefix_unexpected(int reason);

#ifndef CONFIG_PREFIX_USE_HISTORICAL_PIF_FASTPASTE
BOOL bPifFastPaste = TRUE;
#endif

DWORD WINAPI ExpandEnvironmentStringsOem(LPCSTR source, LPSTR destination,
                                         DWORD destination_size) {
    (void)source;
    (void)destination;
    (void)destination_size;
    runner_config_prefix_unexpected(16);
    return 0;
}

int RcMessageBox(UINT message_id, CHAR *message_1, CHAR *message_2,
                 DWORD options) {
    (void)message_id;
    (void)message_1;
    (void)message_2;
    (void)options;
    runner_config_prefix_unexpected(17);
    return 0;
}
