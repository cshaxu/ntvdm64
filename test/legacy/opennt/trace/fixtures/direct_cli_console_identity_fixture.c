/* Bounded no-side-effect check for the direct-CLI title uniqueness token. */
#include <windows.h>

extern ULONG runner_console_identity(void);

int main(void) {
    ULONG first = runner_console_identity();
    ULONG second = runner_console_identity();

    if (first == 0 || first != second) return 1;
    return first == GetCurrentProcessId() ? 0 : 2;
}
