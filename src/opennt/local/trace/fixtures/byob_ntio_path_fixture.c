#include <windows.h>

extern BOOL ntdos64_byob_ntio_path(CHAR *destination, DWORD destination_size);

int main(void) {
    CHAR path[MAX_PATH];

    SetEnvironmentVariableA("NTDOS64_NTIO_PATH", NULL);
    if (ntdos64_byob_ntio_path(path, sizeof(path))) return 1;
    if (!SetEnvironmentVariableA("NTDOS64_NTIO_PATH", "C:\\byob\\ntio.sys")) return 2;
    if (!ntdos64_byob_ntio_path(path, sizeof(path))) return 3;
    if (lstrcmpiA(path, "C:\\byob\\ntio.sys") != 0) return 4;
    SetEnvironmentVariableA("NTDOS64_NTIO_PATH", NULL);
    return 0;
}
