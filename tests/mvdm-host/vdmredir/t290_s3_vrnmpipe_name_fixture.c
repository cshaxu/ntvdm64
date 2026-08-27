#include <nt.h>
#include <stdio.h>
#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_host_identity.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_redirector_pointer_scope.h"
#include "session/session.h"

BOOL VrIsNamedPipeName(LPSTR name);
BOOL VrAddOpenNamedPipeInfo(HANDLE handle, LPSTR name);
BOOL VrRemoveOpenNamedPipeInfo(HANDLE handle);
BOOL VrIsNamedPipeHandle(HANDLE handle);
BOOL VrReadNamedPipe(HANDLE handle, LPBYTE buffer, DWORD byte_count,
    LPDWORD bytes_read, LPDWORD error);
BOOL VrWriteNamedPipe(HANDLE handle, LPBYTE buffer, DWORD byte_count,
    LPDWORD bytes_written);
VOID VrGetNamedPipeHandleState(VOID);
VOID VrGetNamedPipeInfo(VOID);

extern CRITICAL_SECTION VrNamedPipeCancelCritSec;

VOID WaitIfIdle(VOID) {}
static USHORT fixture_ax;
static USHORT fixture_bx;
static USHORT fixture_bp;
static USHORT fixture_cx;
static USHORT fixture_ds;
static USHORT fixture_si;
static ULONG fixture_carry;
USHORT getAX(VOID) { return fixture_ax; }
USHORT getBX(VOID) { return fixture_bx; }
USHORT getBP(VOID) { return fixture_bp; }
USHORT getCX(VOID) { return fixture_cx; }
USHORT getDX(VOID) { return 0u; }
USHORT getSI(VOID) { return fixture_si; }
USHORT getDI(VOID) { return 0u; }
USHORT getDS(VOID) { return fixture_ds; }
USHORT getES(VOID) { return 0u; }
VOID setAL(UCHAR value) { (void)value; }
VOID setAX(USHORT value) { fixture_ax = value; }
VOID setBX(USHORT value) { fixture_bx = value; }
VOID setCX(USHORT value) { (void)value; }
VOID setDX(USHORT value) { (void)value; }
VOID setSI(USHORT value) { (void)value; }
VOID setDI(USHORT value) { (void)value; }
VOID setDS(USHORT value) { (void)value; }
VOID setES(USHORT value) { (void)value; }
VOID setCF(ULONG value) { fixture_carry = value; }
VOID setZF(ULONG value) { (void)value; }
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    return mvdm_redirector_pointer_from_real_mode(segment, offset);
}
VOID VrRaiseInterrupt(VOID) {}
VOID VrQueueCompletionHandler(VOID (*routine)(VOID)) { (void)routine; }
WORD VrpMapLastError(VOID) { return ERROR_NOT_SUPPORTED; }

typedef struct fixture_memory {
    uint8_t bytes[0x20000];
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

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
    session instance;
    uint32_t identity;
    fixture_memory memory;

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
    memset(&memory, 0, sizeof(memory));
    session_initialize(&instance, 0x2903u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance) ||
        !mvdm_host_identity_publish((uintptr_t)client, &identity)) {
        CloseHandle(client);
        CloseHandle(server);
        return 9;
    }
    if (!VrAddOpenNamedPipeInfo(client, remote)) {
        mvdm_host_identity_release(identity);
        session_thread_unbind(&instance);
        session_guest_memory_end(&instance);
        session_dispose(&instance);
        CloseHandle(client);
        CloseHandle(server);
        return 10;
    }
    fixture_bp = (USHORT)(identity >> 16);
    fixture_bx = (USHORT)identity;
    fixture_ax = 0xffffu;
    fixture_carry = 1u;
    VrGetNamedPipeHandleState();
    if (fixture_carry != 0u) {
        mvdm_host_identity_release(identity);
        session_thread_unbind(&instance);
        session_dispose(&instance);
        CloseHandle(client);
        CloseHandle(server);
        return 11;
    }
    fixture_cx = 64u;
    fixture_ds = 0u;
    fixture_si = 0x1000u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 12;
    VrGetNamedPipeInfo();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        memory.bytes[0x1006u] != (BYTE)(strlen(remote) + 1u) ||
        strcmp((char *)(memory.bytes + 0x1007u), remote) != 0)
        return 13;

    InitializeCriticalSection(&VrNamedPipeCancelCritSec);
    if (!WriteFile(server, inbound, sizeof(inbound), &bytes, NULL) ||
        bytes != sizeof(inbound)) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 14;
    }
    bytes = 0u;
    error = ERROR_GEN_FAILURE;
    if (!VrReadNamedPipe(client, received, sizeof(received), &bytes, &error) ||
        bytes != sizeof(received) || error != NO_ERROR ||
        memcmp(received, inbound, sizeof(inbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 15;
    }
    bytes = 0u;
    if (!VrWriteNamedPipe(client, outbound, sizeof(outbound), &bytes) ||
        bytes != sizeof(outbound) ||
        !ReadFile(server, written, sizeof(written), &bytes, NULL) ||
        bytes != sizeof(written) || memcmp(written, outbound, sizeof(outbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 16;
    }
    DeleteCriticalSection(&VrNamedPipeCancelCritSec);
    if (!VrRemoveOpenNamedPipeInfo(client) ||
        !mvdm_host_identity_release(identity) || !session_thread_unbind(&instance)) {
        CloseHandle(client);
        CloseHandle(server);
        return 17;
    }
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) {
        CloseHandle(client);
        CloseHandle(server);
        return 18;
    }
    CloseHandle(client);
    CloseHandle(server);
    return 0;
}
