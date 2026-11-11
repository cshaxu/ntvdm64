#include <windows.h>

extern BOOL runner_byob_ntio_path(CHAR *destination, DWORD destination_size);

int main(void) {
    CHAR path[MAX_PATH];

    SetEnvironmentVariableA("RUNNER_NTIO_PATH", NULL);
    if (runner_byob_ntio_path(path, sizeof(path))) return 1;
    if (!SetEnvironmentVariableA("RUNNER_NTIO_PATH", "C:\\byob\\ntio.sys")) return 2;
    if (!runner_byob_ntio_path(path, sizeof(path))) return 3;
    if (lstrcmpiA(path, "C:\\byob\\ntio.sys") != 0) return 4;
    SetEnvironmentVariableA("RUNNER_NTIO_PATH", NULL);
    return 0;
}
