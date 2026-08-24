/*
 * Modern-host compatibility seam for OpenNT's retired console notification.
 * The historical call has no result and this trace profile owns no console
 * server event channel, so acknowledgement is the only faithful local action.
 */
#include <windows.h>

void WINAPI SetLastConsoleEventActive(void) {
}
