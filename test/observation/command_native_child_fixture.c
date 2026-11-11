/* Test-only native child for T420 S34.  COMMAND must launch this Win32
 * process through its original cmdExec32 path and give it the redirected
 * standard output inherited from the guest COMMAND invocation. */
#include <windows.h>

int main(void)
{
    static const char marker[] = "S34_NATIVE_CHILD_STDOUT_OK\r\n";
    DWORD written = 0;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

    return output != INVALID_HANDLE_VALUE && output != NULL &&
        WriteFile(output, marker, (DWORD)(sizeof(marker) - 1u), &written, NULL) &&
        written == sizeof(marker) - 1u ? 0 : 1;
}
