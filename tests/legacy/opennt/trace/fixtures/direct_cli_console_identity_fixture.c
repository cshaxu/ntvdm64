/* Bounded no-side-effect check for the direct-CLI title uniqueness token. */
#include <windows.h>

extern ULONG ntdos64_legacy_console_identity(void);

int main(void) {
    ULONG first = ntdos64_legacy_console_identity();
    ULONG second = ntdos64_legacy_console_identity();

    if (first == 0 || first != second) return 1;
    return first == GetCurrentProcessId() ? 0 : 2;
}
