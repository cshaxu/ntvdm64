#include "mvdm_vm_execute.h"
#include <stdio.h>
#include <stdlib.h>

static HANDLE invalidated, gate, observed;
static unsigned checks, starts, retires, mode;
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr, "FAIL %u: %s (%lu)\n", __LINE__, #x, GetLastError()); ExitProcess(1); } } while (0)
HANDLE mvdm_vm_continuation_invalidated(mvdm_vm_continuation *request)
{ (void)request; return invalidated; }
DWORD ntvdm_vm_host_launch_context(ntvdm_vm_host *host,
    const common_frontend_location *location, HANDLE *frontend, HANDLE *execution)
{
    (void)host; (void)location;
    if (mode == 1) return ERROR_ACCESS_DENIED;
    *frontend = CreateEventW(NULL, TRUE, FALSE, NULL);
    *execution = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(*frontend && *execution);
    if (mode == 2) CHECK(SetEvent(invalidated));
    return ERROR_SUCCESS;
}
DWORD ntvdm_command_launch_start(ntvdm_command_launch *launch, HANDLE frontend,
    HANDLE execution, DWORD flags, PROCESS_INFORMATION *child)
{
    WCHAR path[MAX_PATH], command[MAX_PATH + 80];
    STARTUPINFOW startup = {0};
    (void)launch;
    ++starts;
    CHECK(!flags && frontend && execution);
    if (mode == 3) return ERROR_FILE_NOT_FOUND;
    CHECK(GetModuleFileNameW(NULL, path, MAX_PATH));
    CHECK(swprintf_s(command, _countof(command), L"\"%s\" %llu", path,
        (unsigned long long)(ULONG_PTR)gate) > 0);
    startup.cb = sizeof(startup);
    CHECK(CreateProcessW(path, command, NULL, NULL, TRUE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, child));
    CHECK(DuplicateHandle(GetCurrentProcess(), child->hProcess, GetCurrentProcess(),
        &observed, SYNCHRONIZE, FALSE, 0));
    if (mode >= 4) CHECK(SetEvent(invalidated));
    else CHECK(SetEvent(gate));
    return ERROR_SUCCESS;
}
DWORD ntvdm_vm_host_retire(ntvdm_vm_host *host, const common_frontend_location *location)
{
    (void)host; (void)location;
    ++retires;
    CHECK(SetEvent(gate)); /* Stand-in for NTSRV-directed chain cleanup. */
    return mode == 5 ? ERROR_ACCESS_DENIED : mode == 6 ? ERROR_NOT_FOUND : ERROR_SUCCESS;
}
int wmain(int argc, WCHAR **argv)
{
    SECURITY_ATTRIBUTES inherit = {sizeof(inherit), NULL, TRUE};
    common_frontend_location location = {0};
    DWORD code, error;
    if (argc == 2) {
        HANDLE wait = (HANDLE)(ULONG_PTR)_wcstoui64(argv[1], NULL, 10);
        return WaitForSingleObject(wait, 10000) == WAIT_OBJECT_0 ? 259 : 1;
    }
    invalidated = CreateEventW(NULL, TRUE, FALSE, NULL);
    gate = CreateEventW(&inherit, TRUE, FALSE, NULL);
    CHECK(invalidated && gate);
    for (mode = 0; mode < 8; ++mode) {
        starts = retires = 0; observed = NULL;
        CHECK(ResetEvent(invalidated) && ResetEvent(gate));
        if (mode == 7) CHECK(SetEvent(invalidated));
        code = 123;
        error = mvdm_vm_command_execute((mvdm_vm_continuation *)1,
            (ntvdm_command_launch *)1, (ntvdm_vm_host *)1, &location, &code);
        CHECK(error == (DWORD)(mode == 0 ? ERROR_SUCCESS : mode == 1 || mode == 5 ?
            ERROR_ACCESS_DENIED : mode == 3 ? ERROR_FILE_NOT_FOUND : ERROR_OPERATION_ABORTED));
        CHECK(code == (mode == 0 ? 259u : 0u));
        CHECK(starts == (mode == 1 || mode == 2 || mode == 7 ? 0u : 1u));
        CHECK(retires == (mode >= 4 && mode <= 6 ? 1u : 0u));
        if (observed) {
            CHECK(WaitForSingleObject(observed, 10000) == WAIT_OBJECT_0);
            CloseHandle(observed);
        }
    }
    CloseHandle(gate); CloseHandle(invalidated);
    printf("PASS %u executor assertions; mocked broker/launcher, real child waits\n", checks);
    return 0;
}
