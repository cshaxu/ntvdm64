#include <nt.h>
#include <stdio.h>
#include <string.h>

#include "vrnmpipe.h"

#include "adapter-mvdm-host-out/softpc/include/mvdm_host_identity.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_redirector_pointer_scope.h"
#include "session/session.h"

/* The production source is force-included with its S4 async-worker facade.
 * This fixture's local public-pipe peer is not Redirector production work. */
#undef CreateThread

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
VOID VrSetNamedPipeHandleState(VOID);
VOID VrPeekNamedPipe(VOID);
VOID VrTransactNamedPipe(VOID);
VOID VrWaitNamedPipe(VOID);
VOID VrCallNamedPipe(VOID);
VOID VrNetHandleGetInfo(VOID);
VOID VrNetHandleSetInfo(VOID);
VOID VrReadWriteAsyncNmPipe(VOID);
VOID VrCancelPipeIo(DWORD thread);

extern CRITICAL_SECTION VrNamedPipeCancelCritSec;

VOID WaitIfIdle(VOID) {}
static USHORT fixture_ax;
static USHORT fixture_bx;
static USHORT fixture_bp;
static USHORT fixture_cx;
static USHORT fixture_dx;
static USHORT fixture_ds;
static USHORT fixture_si;
static USHORT fixture_di;
static USHORT fixture_es;
static ULONG fixture_carry;
USHORT getAX(VOID) { return fixture_ax; }
USHORT getBX(VOID) { return fixture_bx; }
USHORT getBP(VOID) { return fixture_bp; }
USHORT getCX(VOID) { return fixture_cx; }
USHORT getDX(VOID) { return fixture_dx; }
USHORT getSI(VOID) { return fixture_si; }
USHORT getDI(VOID) { return fixture_di; }
USHORT getDS(VOID) { return fixture_ds; }
USHORT getES(VOID) { return fixture_es; }
VOID setAL(UCHAR value) { (void)value; }
VOID setAX(USHORT value) { fixture_ax = value; }
VOID setBX(USHORT value) { fixture_bx = value; }
VOID setCX(USHORT value) { fixture_cx = value; }
VOID setDX(USHORT value) { (void)value; }
VOID setSI(USHORT value) { (void)value; }
VOID setDI(USHORT value) { fixture_di = value; }
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

typedef struct transact_server_context {
    HANDLE pipe;
    DWORD result;
} transact_server_context;

static DWORD WINAPI transact_server_thread(LPVOID parameter)
{
    transact_server_context *context = (transact_server_context *)parameter;
    BYTE request[2];
    BYTE response[] = { 0x6fu, 0x6bu };
    DWORD bytes;
    if (context == NULL || !ReadFile(context->pipe, request, sizeof(request),
        &bytes, NULL) || bytes != sizeof(request) || request[0] != 0x72u ||
        request[1] != 0x71u || !WriteFile(context->pipe, response,
        sizeof(response), &bytes, NULL) || bytes != sizeof(response)) {
        if (context != NULL) context->result = 0u;
        return 1u;
    }
    context->result = 1u;
    return 0u;
}

static DWORD WINAPI call_server_thread(LPVOID parameter)
{
    transact_server_context *context = (transact_server_context *)parameter;
    if (context == NULL ||
        (!ConnectNamedPipe(context->pipe, NULL) &&
         GetLastError() != ERROR_PIPE_CONNECTED)) {
        if (context != NULL) context->result = 0u;
        return 1u;
    }
    return transact_server_thread(parameter);
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
    DWORD collection_value;
    DWORD collect_count;
    DWORD collect_time;
    WORD expected_error;
    BYTE inbound[] = { 0x51u, 0x52u, 0x53u };
    BYTE outbound[] = { 0x71u, 0x72u };
    BYTE received[sizeof(inbound)];
    BYTE written[sizeof(outbound)];
    session instance;
    uint32_t identity;
    uint32_t transact_identity;
    fixture_memory memory;
    HANDLE transact_server;
    HANDLE transact_client;
    HANDLE transact_thread;
    transact_server_context transact_context;
    char transact_pipe_name[MAX_PATH];
    char wait_pipe_name[MAX_PATH];
    HANDLE wait_server;
    char call_pipe_name[MAX_PATH];
    HANDLE call_server;
    HANDLE call_thread;
    transact_server_context call_context;
    DOS_CALL_NAMED_PIPE_STRUCT *call_structure;
    DWORD pipe_mode;
    DOS_ASYNC_NAMED_PIPE_STRUCT *async_request;

    if (!VrIsNamedPipeName(remote) || !VrIsNamedPipeName(remote_slash) ||
        VrIsNamedPipeName(local) || VrIsNamedPipeName(missing_name) ||
        VrIsNamedPipeName(ordinary)) return 1;
    if (!VrAddOpenNamedPipeInfo(handle, remote) || !VrIsNamedPipeHandle(handle))
        return 2;
    if (!VrRemoveOpenNamedPipeInfo(handle) || VrIsNamedPipeHandle(handle))
        return 3;
    /* In non-DBG OpenNT builds this is a void internal removal and the public
     * wrapper intentionally returns TRUE even if the entry was already gone. */
    if (!VrRemoveOpenNamedPipeInfo(handle))
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
    InitializeCriticalSection(&VrNamedPipeCancelCritSec);
    async_request = (DOS_ASYNC_NAMED_PIPE_STRUCT *)(memory.bytes + 0x1b00u);
    memset(async_request, 0, sizeof(*async_request));
    async_request->lpBytesRead = 0x1c00u;
    async_request->BufferLength = 1u;
    async_request->lpBuffer = 0x1c10u;
    async_request->lpErrorCode = 0x1c20u;
    async_request->lpANR = 0u;
    async_request->PipeHandle = 1u;
    fixture_ax = 0x0086u;
    fixture_bp = (USHORT)(identity >> 16);
    fixture_bx = (USHORT)identity;
    fixture_ds = 0u;
    fixture_si = 0x1b00u;
    fixture_carry = 0u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1b00u,
            sizeof(*async_request), GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1c10u, 1u,
            GUEST_MEMORY_ACCESS_READ)) return 43;
    VrReadWriteAsyncNmPipe();
    if (!mvdm_redirector_pointer_scope_end(0) || fixture_carry == 0u ||
        fixture_ax != ERROR_NOT_ENOUGH_MEMORY) return 44;
    VrCancelPipeIo((DWORD)(ULONG_PTR)GetCurrentThread());
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
    fixture_cx = 0u;
    fixture_carry = 1u;
    VrSetNamedPipeHandleState();
    if (fixture_carry != 0u) {
        VrRemoveOpenNamedPipeInfo(client);
        mvdm_host_identity_release(identity);
        session_thread_unbind(&instance);
        session_guest_memory_end(&instance);
        session_dispose(&instance);
        CloseHandle(client);
        CloseHandle(server);
        return 12;
    }
    fixture_cx = 64u;
    fixture_ds = 0u;
    fixture_si = 0x1000u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 13;
    VrGetNamedPipeInfo();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        memory.bytes[0x1006u] != (BYTE)(strlen(remote) + 1u) ||
        strcmp((char *)(memory.bytes + 0x1007u), remote) != 0)
        return 14;
    collect_count = 0u;
    collect_time = 0u;
    if (GetNamedPipeHandleState(client, NULL, NULL, &collect_count,
        &collect_time, NULL, 0u))
        return 15;
    expected_error = (WORD)GetLastError();
    fixture_bp = (USHORT)(identity >> 16);
    fixture_bx = (USHORT)identity;
    fixture_cx = (USHORT)sizeof(VDM_HANDLE_INFO_1);
    fixture_si = 1u;
    fixture_dx = 0x1100u;
    fixture_ds = 0u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 16;
    VrNetHandleGetInfo();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry == 0u ||
        fixture_ax != expected_error)
        return 17;
    memory.bytes[0x1120u] = 1u;
    memory.bytes[0x1121u] = 0u;
    memory.bytes[0x1122u] = 0u;
    memory.bytes[0x1123u] = 0u;
    collection_value = 1u;
    if (SetNamedPipeHandleState(client, NULL, &collection_value, NULL))
        return 18;
    expected_error = (WORD)GetLastError();
    fixture_bp = (USHORT)(identity >> 16);
    fixture_bx = (USHORT)identity;
    fixture_cx = (USHORT)sizeof(DWORD);
    fixture_si = 1u;
    fixture_di = 1u;
    fixture_dx = 0x1120u;
    fixture_ds = 0u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1120u, sizeof(DWORD),
            GUEST_MEMORY_ACCESS_READ))
        return 19;
    VrNetHandleSetInfo();
    /* The unchanged source pre-decrements ParmNum, then tests the decremented
     * value against 1/2.  Its public API call therefore receives NULL
     * collection pointers and returns the original no-op success. */
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u)
        return 20;

    if (!WriteFile(server, inbound, sizeof(inbound), &bytes, NULL) ||
        bytes != sizeof(inbound)) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 21;
    }
    fixture_bp = (USHORT)(identity >> 16);
    fixture_bx = (USHORT)identity;
    fixture_cx = (USHORT)sizeof(inbound);
    fixture_ds = 0u;
    fixture_si = 0x1200u;
    fixture_di = 0u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 22;
    VrPeekNamedPipe();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        fixture_bx != sizeof(inbound) || fixture_cx != sizeof(inbound) ||
        fixture_di != 3u ||
        memcmp(memory.bytes + 0x1200u, inbound, sizeof(inbound)) != 0)
        return 23;
    bytes = 0u;
    error = ERROR_GEN_FAILURE;
    if (!VrReadNamedPipe(client, received, sizeof(received), &bytes, &error) ||
        bytes != sizeof(received) || error != NO_ERROR ||
        memcmp(received, inbound, sizeof(inbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 24;
    }
    bytes = 0u;
    if (!VrWriteNamedPipe(client, outbound, sizeof(outbound), &bytes) ||
        bytes != sizeof(outbound) ||
        !ReadFile(server, written, sizeof(written), &bytes, NULL) ||
        bytes != sizeof(written) || memcmp(written, outbound, sizeof(outbound)) != 0) {
        DeleteCriticalSection(&VrNamedPipeCancelCritSec);
        CloseHandle(client);
        CloseHandle(server);
        return 25;
    }
    if (sprintf_s(transact_pipe_name, sizeof(transact_pipe_name),
            "\\\\.\\pipe\\t290-s3-transact-%lu",
            (unsigned long)GetCurrentProcessId()) < 0)
        return 26;
    transact_server = CreateNamedPipeA(transact_pipe_name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1u,
        256u, 256u, 0u, NULL);
    if (transact_server == INVALID_HANDLE_VALUE)
        return 27;
    transact_client = CreateFileA(transact_pipe_name, GENERIC_READ | GENERIC_WRITE,
        0u, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (transact_client == INVALID_HANDLE_VALUE) {
        CloseHandle(transact_server);
        return 28;
    }
    if (!ConnectNamedPipe(transact_server, NULL) &&
        GetLastError() != ERROR_PIPE_CONNECTED) {
        CloseHandle(transact_client);
        CloseHandle(transact_server);
        return 29;
    }
    pipe_mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(transact_client, &pipe_mode, NULL, NULL) ||
        !mvdm_host_identity_publish((uintptr_t)transact_client,
            &transact_identity)) {
        CloseHandle(transact_client);
        CloseHandle(transact_server);
        return 30;
    }
    transact_context.pipe = transact_server;
    transact_context.result = 0u;
    transact_thread = CreateThread(NULL, 0u, transact_server_thread,
        &transact_context, 0u, NULL);
    if (transact_thread == NULL) {
        mvdm_host_identity_release(transact_identity);
        CloseHandle(transact_client);
        CloseHandle(transact_server);
        return 31;
    }
    memory.bytes[0x1400u] = 0x72u;
    memory.bytes[0x1401u] = 0x71u;
    fixture_bp = (USHORT)(transact_identity >> 16);
    fixture_bx = (USHORT)transact_identity;
    fixture_cx = 2u;
    fixture_dx = 2u;
    fixture_ds = 0u;
    fixture_si = 0x1400u;
    fixture_es = 0u;
    fixture_di = 0x1300u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 32;
    VrTransactNamedPipe();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        fixture_cx != 2u || memory.bytes[0x1300u] != 0x6fu ||
        memory.bytes[0x1301u] != 0x6bu ||
        WaitForSingleObject(transact_thread, INFINITE) != WAIT_OBJECT_0 ||
        transact_context.result == 0u || !mvdm_host_identity_release(transact_identity))
        return 33;
    CloseHandle(transact_thread);
    CloseHandle(transact_client);
    CloseHandle(transact_server);
    if (sprintf_s(wait_pipe_name, sizeof(wait_pipe_name),
            "\\\\.\\pipe\\t290-s3-wait-%lu",
            (unsigned long)GetCurrentProcessId()) < 0)
        return 34;
    wait_server = CreateNamedPipeA(wait_pipe_name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1u,
        256u, 256u, 0u, NULL);
    if (wait_server == INVALID_HANDLE_VALUE)
        return 35;
    memcpy(memory.bytes + 0x1500u, wait_pipe_name,
        strlen(wait_pipe_name) + 1u);
    fixture_ds = 0u;
    fixture_dx = 0x1500u;
    fixture_bx = 0u;
    fixture_cx = 1000u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin())
        return 36;
    VrWaitNamedPipe();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        fixture_ax != 0u) {
        CloseHandle(wait_server);
        return 37;
    }
    CloseHandle(wait_server);
    if (sprintf_s(call_pipe_name, sizeof(call_pipe_name),
            "\\\\.\\pipe\\t290-s3-call-%lu",
            (unsigned long)GetCurrentProcessId()) < 0)
        return 38;
    call_server = CreateNamedPipeA(call_pipe_name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1u,
        256u, 256u, 0u, NULL);
    if (call_server == INVALID_HANDLE_VALUE)
        return 39;
    call_context.pipe = call_server;
    call_context.result = 0u;
    call_thread = CreateThread(NULL, 0u, call_server_thread, &call_context,
        0u, NULL);
    if (call_thread == NULL) {
        CloseHandle(call_server);
        return 40;
    }
    call_structure = (DOS_CALL_NAMED_PIPE_STRUCT *)(memory.bytes + 0x1600u);
    memset(call_structure, 0, sizeof(*call_structure));
    call_structure->Timeout = 1000u;
    call_structure->lpBytesRead = 0x1a00u;
    call_structure->nOutBufferLen = 2u;
    call_structure->lpOutBuffer = 0x1900u;
    call_structure->nInBufferLen = 2u;
    call_structure->lpInBuffer = 0x1800u;
    call_structure->lpPipeName = 0x1700u;
    memcpy(memory.bytes + 0x1700u, call_pipe_name, strlen(call_pipe_name) + 1u);
    memory.bytes[0x1800u] = 0x72u;
    memory.bytes[0x1801u] = 0x71u;
    fixture_ds = 0u;
    fixture_si = 0x1600u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1600u,
            sizeof(*call_structure), GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1700u,
            (uint32_t)strlen(call_pipe_name) + 1u, GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1800u, 2u,
            GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1900u, 2u,
            GUEST_MEMORY_ACCESS_WRITE) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1a00u, 2u,
            GUEST_MEMORY_ACCESS_WRITE))
        return 41;
    VrCallNamedPipe();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        fixture_cx != 2u || memory.bytes[0x1900u] != 0x6fu ||
        memory.bytes[0x1901u] != 0x6bu || memory.bytes[0x1a00u] != 2u ||
        memory.bytes[0x1a01u] != 0u ||
        WaitForSingleObject(call_thread, INFINITE) != WAIT_OBJECT_0 ||
        call_context.result == 0u) {
        CloseHandle(call_thread);
        CloseHandle(call_server);
        return 42;
    }
    CloseHandle(call_thread);
    CloseHandle(call_server);
    DeleteCriticalSection(&VrNamedPipeCancelCritSec);
    if (!VrRemoveOpenNamedPipeInfo(client) ||
        !mvdm_host_identity_release(identity) || !session_thread_unbind(&instance)) {
        CloseHandle(client);
        CloseHandle(server);
        return 20;
    }
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) {
        CloseHandle(client);
        CloseHandle(server);
        return 21;
    }
    CloseHandle(client);
    CloseHandle(server);
    return 0;
}
