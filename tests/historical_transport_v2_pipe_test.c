#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "historical_transport_v2.h"

static int io_exact(int write, HANDLE pipe, void *buffer, DWORD bytes)
{
    DWORD done = 0u;
    return write ? (WriteFile(pipe, buffer, bytes, &done, NULL) && done == bytes) :
        (ReadFile(pipe, buffer, bytes, &done, NULL) && done == bytes);
}

int main(int argc, char **argv)
{
    char pipe_name[96];
    char command_line[512];
    HANDLE pipe = INVALID_HANDLE_VALUE;
    STARTUPINFOA startup = { .cb = sizeof(startup) };
    PROCESS_INFORMATION process = {0};
    ntdos64_historical_transport_v2_request request = {0};
    ntdos64_historical_transport_v2_response response = {0};
    const uint32_t token = 0x13579bdfu;
    DWORD exit_code = 1u;
    int result = 1;

    if (argc != 2) return 2;
    snprintf(pipe_name, sizeof(pipe_name), "\\\\.\\pipe\\ntdos64-s5-%lu", GetCurrentProcessId());
    pipe = CreateNamedPipeA(pipe_name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1u,
        sizeof(response), sizeof(request), 0u, NULL);
    if (pipe == INVALID_HANDLE_VALUE) goto cleanup;
    if (snprintf(command_line, sizeof(command_line), "\"%s\" --pipe \"%s\" --token %08lx",
            argv[1], pipe_name, (unsigned long)token) < 0) goto cleanup;
    if (!CreateProcessA(NULL, command_line, NULL, NULL, FALSE, 0u, NULL, NULL,
            &startup, &process)) goto cleanup;
    if (!ConnectNamedPipe(pipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED) goto cleanup;

    request.magic = NTDOS64_HISTORICAL_TRANSPORT_V2_MAGIC;
    request.abi_version = NTDOS64_HISTORICAL_TRANSPORT_V2_ABI;
    request.struct_bytes = (uint32_t)sizeof(request);
    request.session_token = token;
    request.sequence = 1u;
    request.selector = 0x50u;
    request.service_byte = 0x14u;
    request.state.eax = 0xc0de5a00u;
    request.state.ecx = 0xbeef1111u;
    request.state.edx = 0xabcd2222u;
    request.state.cs = 0x0070u;
    request.state.ip = 0x0478u;
    request.state.flags = 0x0002u;
    if (!io_exact(1, pipe, &request, (DWORD)sizeof(request)) ||
        !io_exact(0, pipe, &response, (DWORD)sizeof(response))) goto cleanup;
    if (response.magic != NTDOS64_HISTORICAL_TRANSPORT_V2_MAGIC ||
        response.abi_version != NTDOS64_HISTORICAL_TRANSPORT_V2_ABI ||
        response.struct_bytes != sizeof(response) || response.session_token != token ||
        response.sequence != 1u || response.disposition != NTDOS64_HISTORICAL_TRANSPORT_V2_COMPLETED ||
        response.state.ip != 0x0479u || (response.state.eax & 0xffff0000u) != 0xc0de0000u ||
        (response.state.ecx & 0xffffu) < 1980u) goto cleanup;
    result = 0;

cleanup:
    if (process.hProcess != NULL) {
        WaitForSingleObject(process.hProcess, 5000u);
        GetExitCodeProcess(process.hProcess, &exit_code);
        if (exit_code != 0u) result = 1;
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
    }
    if (pipe != INVALID_HANDLE_VALUE) CloseHandle(pipe);
    return result;
}
