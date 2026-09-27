/* Observer self-test only: the private-desktop watchdog must identify a
 * modal startup wait before it terminates this fixture. Not product code. */
#include <windows.h>
int main(void)
{
    MessageBoxA(NULL, "Intentional observer timeout self-test",
        "NTVDM-STARTUP-MODAL-WITNESS", MB_OK);
    return 1; /* A dismissed modal is not the timeout under test. */
}
