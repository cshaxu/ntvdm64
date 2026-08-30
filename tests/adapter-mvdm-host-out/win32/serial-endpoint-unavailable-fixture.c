#include <windows.h>

/* A deliberately impossible serial name proves only the public host-open
 * failure direction used by the original nt_com.c endpoint.  It never opens
 * an installed serial device, changes a DCB, starts a worker or writes data. */
int main(void)
{
    HANDLE endpoint = CreateFileA("\\\\.\\COM999",
                                  GENERIC_READ | GENERIC_WRITE,
                                  0,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                  NULL);

    if (endpoint != INVALID_HANDLE_VALUE) {
        CloseHandle(endpoint);
        return 1;
    }
    return 0;
}
