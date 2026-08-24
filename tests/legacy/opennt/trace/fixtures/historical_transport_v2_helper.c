#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "historical_bios_bridge_v2.h"

static int read_exact(HANDLE pipe, void *buffer, DWORD bytes)
{
    DWORD done = 0u;
    return ReadFile(pipe, buffer, bytes, &done, NULL) && done == bytes;
}

static int write_exact(HANDLE pipe, const void *buffer, DWORD bytes)
{
    DWORD done = 0u;
    return WriteFile(pipe, buffer, bytes, &done, NULL) && done == bytes;
}

int main(int argc, char **argv)
{
    HANDLE pipe;
    char *end = NULL;
    unsigned long token;
    ntdos64_historical_transport_v2_request request;
    ntdos64_historical_transport_v2_response response;
    int result = 1;

    if (argc != 5 || strcmp(argv[1], "--pipe") != 0 ||
        strcmp(argv[3], "--token") != 0) return 2;
    token = strtoul(argv[4], &end, 16);
    if (end == argv[4] || *end != '\0' || token == 0ul || token > 0xfffffffful) return 3;
    pipe = CreateFileA(argv[2], GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (pipe == INVALID_HANDLE_VALUE) return 4;
    if (!ntdos64_historical_bios_bridge_v2_initialize()) {
        CloseHandle(pipe);
        return 5;
    }
    if (!read_exact(pipe, &request, (DWORD)sizeof(request))) goto cleanup;
    if (!ntdos64_historical_bios_bridge_v2_invoke(&request, &response, (uint32_t)token)) {
        goto cleanup;
    }
    if (!write_exact(pipe, &response, (DWORD)sizeof(response))) goto cleanup;
    result = 0;

cleanup:
    ntdos64_historical_bios_bridge_v2_terminate();
    CloseHandle(pipe);
    return result;
}
