/* Compile the actual adapter in this test TU so its private TLS can be
 * observed without adding a production export or test branch. The modern
 * SHELL32 provider is still real; no callback dispatch is mocked. */
#include "../../../src/wow32-dll/source/wow_shell_callback.c"

LPFNWOWSHELLEXECCB WINAPI ShellProbeCurrentCallback(void)
{
    return current_callback;
}
