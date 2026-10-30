#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    STARTUPINFOA si = {0}; PROCESS_INFORMATION pi = {0};
    char command[1024]; DWORD result = STILL_ACTIVE; FILE *log;
    if (argc != 3 || fopen_s(&log, argv[2], "wb")) return 2;
    si.cb = sizeof(si);
    if (sprintf_s(command, sizeof(command), "\"%s\"", argv[1]) < 0 ||
        !CreateProcessA(argv[1], command, NULL, NULL, FALSE,
                        CREATE_SEPARATE_WOW_VDM, NULL, NULL, &si, &pi)) {
        fprintf(log, "create-error=%lu\n", GetLastError()); fclose(log); return 3;
    }
    {
        char image[MAX_PATH]; DWORD length = sizeof(image);
        if (QueryFullProcessImageNameA(pi.hProcess, 0, image, &length))
            fprintf(log, "child-pid=%lu image=%s\n", pi.dwProcessId, image);
        else fprintf(log, "child-image-error=%lu\n", GetLastError());
        fflush(log);
    }
    CloseHandle(pi.hThread);
    if (WaitForSingleObject(pi.hProcess, 30000) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(pi.hProcess, &result)) {
        fprintf(log, "wait-error=%lu\n", GetLastError());
        CloseHandle(pi.hProcess); fclose(log); return 4;
    }
    CloseHandle(pi.hProcess);
    fprintf(log, "separate-startup-exit=%lu\n", result); fclose(log);
    return result == 255 ? 0 : 5;
}
