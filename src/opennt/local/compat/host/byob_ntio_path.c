/* Explicit BYOB source selection for the historical NTIO loader seam. */
#include <windows.h>

BOOL ntdos64_byob_ntio_path(CHAR *destination, DWORD destination_size) {
    DWORD length;

    if (destination == NULL || destination_size == 0) return FALSE;
    length = GetEnvironmentVariableA("NTDOS64_NTIO_PATH", destination,
                                     destination_size);
    return length != 0 && length < destination_size;
}
