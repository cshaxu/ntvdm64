#include <nt.h>
#include <stdio.h>

BOOL VrIsNamedPipeName(LPSTR name);
BOOL VrAddOpenNamedPipeInfo(HANDLE handle, LPSTR name);
BOOL VrRemoveOpenNamedPipeInfo(HANDLE handle);
BOOL VrIsNamedPipeHandle(HANDLE handle);
BOOL VrReadNamedPipe(HANDLE handle, LPBYTE buffer, DWORD byte_count,
    LPDWORD bytes_read, LPDWORD error);
BOOL VrWriteNamedPipe(HANDLE handle, LPBYTE buffer, DWORD byte_count,
    LPDWORD bytes_written);

extern CRITICAL_SECTION VrNamedPipeCancelCritSec;

VOID WaitIfIdle(VOID) {}
USHORT getAX(VOID) { return 0u; }
USHORT getBX(VOID) { return 0u; }
USHORT getBP(VOID) { return 0u; }
USHORT getCX(VOID) { return 0u; }
USHORT getDX(VOID) { return 0u; }
USHORT getSI(VOID) { return 0u; }
USHORT getDI(VOID) { return 0u; }
USHORT getDS(VOID) { return 0u; }
USHORT getES(VOID) { return 0u; }
VOID setAL(UCHAR value) { (void)value; }
VOID setAX(USHORT value) { (void)value; }
VOID setBX(USHORT value) { (void)value; }
VOID setCX(USHORT value) { (void)value; }
VOID setDX(USHORT value) { (void)value; }
VOID setSI(USHORT value) { (void)value; }
VOID setDI(USHORT value) { (void)value; }
VOID setDS(USHORT value) { (void)value; }
VOID setES(USHORT value) { (void)value; }
VOID setCF(ULONG value) { (void)value; }
VOID setZF(ULONG value) { (void)value; }
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    (void)segment;
    (void)offset;
    return NULL;
}
VOID VrRaiseInterrupt(VOID) {}
VOID VrQueueCompletionHandler(VOID (*routine)(VOID)) { (void)routine; }
WORD VrpMapLastError(VOID) { return ERROR_NOT_SUPPORTED; }

int main(void)
{
    char remote[] = "\\\\server\\PIPE\\queue";
    char remote_slash[] = "\\\\server\\pipe/queue";
    char local[] = "\\pipe\\queue";
    char missing_name[] = "\\\\server\\pipe";
    char ordinary[] = "C:\\queue";
    HANDLE handle = (HANDLE)(ULONG_PTR)0x7131u;
    char pipe_name[MAX_PATH];
    HANDLE server;
    HANDLE client;
    DWORD bytes;
    DWORD error;
    BYTE inbound[] = { 0x51u, 0x52u, 0x53u };
    BYTE outbound[] = { 0x71u, 0x72u };
    BYTE received[sizeof(inbound)];
    BYTE written[sizeof(outbound)];

    if (!VrIsNamedPipeName(remote) || !VrIsNamedPipeName(remote_slash) ||
        VrIsNamedPipeName(local) || VrIsNamedPipeName(missing_name) ||
        VrIsNamedPipeName(ordinary)) return 1;
    if (!VrAddOpenNamedPipeInfo(handle, remote) || !VrIsNamedPipeHandle(handle))
        return 2;
    if (!VrRemoveOpenNamedPipeInfo(handle) || VrIsNamedPipeHandle(handle))
        return 3;
    if (VrRemoveOpenNamedPipeInfo(handle))
        return 4;

    if (sprintf_s(pipe_name, sizeof(pipe_name), "\\\\.\\pipe\\t290-s3-%lu",
            (unsigned long)GetCurrentProcessId()) < 0)
        return 5;
    server = CreateNamedPipeA(pipe_name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1u,
        256u, 256u, 0u, NULL);
    if (server == INVALID_HANDLE_VALUE)
        return 6;
    client = CreateFileA(pipe_name, GENERIC_READ | GENERIC_WRITE, 0u, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (client == INVALID_HANDLE_VALUE) {
        CloseHandle(server);
        return 7;
    }
    if (!ConnectNamedPipe(server, NULL) && GetLastError() != ERROR_PIPE_CONNECTED) {
        CloseHandle(client);
        CloseHandle(server);
        return 8;
    }

    InitializeCriticalSection(&VrNamedPipeCancelCritSec);
    if (!WriteFile(server, inbound, sizeof(inbound), &bytes, NULL) ||
        bytes != sizeof(inbound)) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 9;
    }
    bytes = 0u;
    error = ERROR_GEN_FAILURE;
    if (!VrReadNamedPipe(client, received, sizeof(received), &bytes, &error) ||
        bytes != sizeof(received) || error != NO_ERROR ||
        memcmp(received, inbound, sizeof(inbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 10;
    }
    bytes = 0u;
    if (!VrWriteNamedPipe(client, outbound, sizeof(outbound), &bytes) ||
        bytes != sizeof(outbound) ||
        !ReadFile(server, written, sizeof(written), &bytes, NULL) ||
        bytes != sizeof(written) || memcmp(written, outbound, sizeof(outbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 11;
    }
    DeleteCriticalSection(&VrNamedPipeCancelCritSec);
    CloseHandle(client);
    CloseHandle(server);
    return 0;
}
