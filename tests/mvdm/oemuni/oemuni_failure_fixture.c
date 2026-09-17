#include <windows.h>

#include "oemuni.h"

int
main(void)
{
    CHAR path[MAX_PATH];
    LPSTR file_part;

    SetLastError(0);
    if (GetShortPathNameOem(NULL, path, sizeof(path)) != 0u ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;

    file_part = NULL;
    if (GetFullPathNameOem("oemuni_probe.txt", sizeof(path), path, &file_part) == 0u ||
        file_part == NULL || file_part < path ||
        file_part >= path + sizeof(path)) return 2;

    return 0;
}
