/*
 * Host-context seam for config.c's obsolete PEB ConsoleHandle formatting
 * field. The value is used only to make a temporary title string unique.
 */
#include <windows.h>

ULONG ntdos64_legacy_console_identity(void) {
    return GetCurrentProcessId();
}
