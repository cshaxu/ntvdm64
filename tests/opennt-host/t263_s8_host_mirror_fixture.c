#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <lmerr.h>

#define VDMREDIR_DLL
#include "opennt-host/inc/vrnmpipe.h"
#include "opennt-host/vdmredir/vrputil.h"

BOOLEAN runtime_vr_initialized_provider(VOID);

static int pipe_round_trip(void)
{
    HANDLE server = INVALID_HANDLE_VALUE;
    HANDLE client = INVALID_HANDLE_VALUE;
    DWORD bytes = 0u;
    DWORD error = ERROR_SUCCESS;
    char read_buffer[4] = { 0 };
    char pipe_name[] = "\\\\localhost\\PIPE\\ntdos64-t263-s8";

    server = CreateNamedPipeW(L"\\\\.\\pipe\\ntdos64-t263-s8",
        PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1u, 64u, 64u, 0u, NULL);
    if (server == INVALID_HANDLE_VALUE) return 0;
    client = CreateFileW(L"\\\\.\\pipe\\ntdos64-t263-s8",
        GENERIC_READ | GENERIC_WRITE, 0u, NULL, OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED, NULL);
    if (client == INVALID_HANDLE_VALUE ||
        (ConnectNamedPipe(server, NULL) == FALSE &&
            GetLastError() != ERROR_PIPE_CONNECTED) ||
        !VrAddOpenNamedPipeInfo(client, pipe_name) ||
        !VrIsNamedPipeHandle(client) ||
        !VrWriteNamedPipe(client, (LPBYTE)"ok", 2u, &bytes) || bytes != 2u ||
        !ReadFile(server, read_buffer, 2u, &bytes, NULL) || bytes != 2u ||
        memcmp(read_buffer, "ok", 2u) != 0 ||
        !WriteFile(server, "go", 2u, &bytes, NULL) || bytes != 2u ||
        !VrReadNamedPipe(client, (LPBYTE)read_buffer, 2u, &bytes, &error) ||
        bytes != 2u || error != ERROR_SUCCESS ||
        memcmp(read_buffer, "go", 2u) != 0 ||
        !VrRemoveOpenNamedPipeInfo(client)) {
        if (client != INVALID_HANDLE_VALUE) CloseHandle(client);
        CloseHandle(server);
        return 0;
    }
    CloseHandle(client);
    CloseHandle(server);
    return 1;
}

int main(void)
{
    char source[] = "//server/share";
    char output[64] = { 0 };
    LPSTR input_cursor = source;
    LPSTR output_cursor = output;

    if (VrpMapDosError(NERR_UseNotFound) != ERROR_PATH_NOT_FOUND) return 1;
    SetLastError(NERR_UseNotFound);
    if (VrpMapLastError() != ERROR_PATH_NOT_FOUND) return 2;
    if (VrpTranslateDosNetPath(&input_cursor, &output_cursor) != 0u ||
        strcmp(output, "\\\\SERVER\\SHARE") != 0) return 3;
    if (!VrIsNamedPipeName("\\\\remote\\PIPE\\name") ||
        VrIsNamedPipeName("\\MAILSLOT\\name")) return 4;
    if (runtime_vr_initialized_provider()) return 5;
    if (!pipe_round_trip()) return 6;
    puts("T263 S9 OpenNT host mirror: utility and synchronous pipe paths pass");
    return 0;
}
