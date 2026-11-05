#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "lib-win32/process-hook/payload.h"

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "payload check failed at line %d\n", __LINE__); return 1; } ++checks; } while (0)

typedef struct receiver { unsigned calls; BOOL fail; } receiver;
static DWORD receive_payload(const void *data, DWORD bytes, void *context)
{
    receiver *r = (receiver *)context;
    ++r->calls;
    if (bytes != 4 || memcmp(data, "test", 4)) return ERROR_INVALID_DATA;
    return r->fail ? ERROR_RETRY : ERROR_SUCCESS;
}
int main(void)
{
    unsigned checks = 0;
    receiver r = {0};
    BOOL found = TRUE;
    HANDLE self = GetCurrentProcess();
    CHECK(process_hook_payload_write(NULL, "test", 4) == ERROR_INVALID_PARAMETER);
    CHECK(process_hook_payload_write(self, NULL, 4) == ERROR_INVALID_PARAMETER);
    CHECK(process_hook_payload_write(self, "test", 0) == ERROR_INVALID_PARAMETER);
    CHECK(process_hook_payload_take(NULL, &r, &found) == ERROR_INVALID_PARAMETER);
    CHECK(process_hook_payload_take(receive_payload, &r, &found) == ERROR_SUCCESS && !found);
    CHECK(process_hook_payload_write(self, "test", 4) == ERROR_SUCCESS);
    CHECK(process_hook_payload_write(self, "test", 4) == ERROR_ALREADY_EXISTS);
    r.fail = TRUE;
    CHECK(process_hook_payload_take(receive_payload, &r, &found) == ERROR_RETRY && found && r.calls == 1);
    r.fail = FALSE;
    CHECK(process_hook_payload_take(receive_payload, &r, &found) == ERROR_SUCCESS && found && r.calls == 2);
    CHECK(process_hook_payload_take(receive_payload, &r, &found) == ERROR_SUCCESS && !found && r.calls == 2);
    printf("PASS process-hook direct payload checks=%u\n", checks);
    return 0;
}
