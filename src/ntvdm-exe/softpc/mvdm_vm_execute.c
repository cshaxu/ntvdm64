#include "mvdm_vm_execute.h"

static DWORD available(HANDLE invalidated)
{
    DWORD status = WaitForSingleObject(invalidated, 0);
    return status == WAIT_TIMEOUT ? ERROR_SUCCESS :
        status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
}

DWORD mvdm_vm_command_execute(mvdm_vm_continuation *request,
    ntvdm_command_launch *launch, ntvdm_vm_host *host,
    const common_frontend_location *location, DWORD *exit_code)
{
    HANDLE frontend = NULL, execution = NULL, waits[2];
    PROCESS_INFORMATION child = {0};
    DWORD error, status;
    if (!exit_code) return ERROR_INVALID_PARAMETER;
    *exit_code = 0;
    if (!request || !launch || !host || !location)
        return ERROR_INVALID_PARAMETER;
    waits[1] = mvdm_vm_continuation_invalidated(request);
    error = available(waits[1]);
    if (error) return error;
    error = ntvdm_vm_host_launch_context(host, location, &frontend, &execution);
    if (error) return error;
    error = available(waits[1]);
    if (!error)
        error = ntvdm_command_launch_start(launch, frontend, execution, 0, &child);
    CloseHandle(execution);
    CloseHandle(frontend);
    if (error) return error;
    CloseHandle(child.hThread);
    waits[0] = child.hProcess;
    status = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    if (status == WAIT_OBJECT_0) {
        /* 259 is a valid terminal exit code once the process is signaled. */
        error = GetExitCodeProcess(child.hProcess, exit_code) ? ERROR_SUCCESS : GetLastError();
    } else {
        DWORD retired;
        error = status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
        /* Includes cancellation racing successful creation. The registry owns
         * authentication, NTSRV close, callback joins and concurrent retirement.
         * Failure remains visible to its owner; do not fabricate task completion. */
        retired = ntvdm_vm_host_retire(host, location);
        if (retired && retired != ERROR_NOT_FOUND) error = retired;
    }
    CloseHandle(child.hProcess);
    return error;
}
