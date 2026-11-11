/* Deliberately live but never broker-registered: startup-deadline fixture. */
#include <windows.h>

int wmain(void)
{
    Sleep(20000);
    return 0;
}
