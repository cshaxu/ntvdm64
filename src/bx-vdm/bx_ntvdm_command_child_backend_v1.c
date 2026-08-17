#include "bx_ntvdm_command_child_backend_v1.h"

#include <windows.h>
#include <string.h>

static int terminated(const uint8_t *value, uint32_t bytes)
{ return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u; }
static int environment_terminated(const uint8_t *value, uint32_t bytes)
{ return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u && value[bytes - 2u] == 0u; }

void bx_ntvdm_command_child_backend_v1_result_clear(
    bx_ntvdm_command_child_backend_v1_result *result)
{
    if (!result) return;
    memset(result, 0, sizeof(*result));
    result->magic = BX_NTVDM_COMMAND_CHILD_BACKEND_V1_MAGIC;
    result->abi_version = BX_NTVDM_COMMAND_CHILD_BACKEND_V1_VERSION;
    result->struct_bytes = (uint32_t)sizeof(*result);
}

int bx_ntvdm_command_child_backend_v1_request_valid(
    const bx_ntvdm_command_child_backend_v1_request *request)
{
    return request != 0 && request->magic == BX_NTVDM_COMMAND_CHILD_BACKEND_V1_MAGIC &&
        request->abi_version == BX_NTVDM_COMMAND_CHILD_BACKEND_V1_VERSION &&
        request->struct_bytes == sizeof(*request) &&
        (request->launch_service == 8u || request->launch_service == 10u) &&
        terminated(request->command, request->command_bytes) &&
        request->command_bytes <= BX_NTVDM_COMMAND_CHILD_BACKEND_V1_COMMAND_BYTES &&
        environment_terminated(request->environment, request->environment_bytes) &&
        request->environment_bytes <= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES &&
        bx_ntvdm_command_host_context_v1_valid(request->host_context) &&
        (request->launch_service == 10u ||
         bx_ntvdm_command_stream_session_v1_valid(request->stream));
}

int bx_ntvdm_command_child_backend_v1_result_valid(
    const bx_ntvdm_command_child_backend_v1_result *result)
{
    return result != 0 && result->magic == BX_NTVDM_COMMAND_CHILD_BACKEND_V1_MAGIC &&
        result->abi_version == BX_NTVDM_COMMAND_CHILD_BACKEND_V1_VERSION &&
        result->struct_bytes == sizeof(*result) && result->completed <= 1u &&
        result->reserved0 == 0u && result->reserved1 == 0u;
}

int bx_ntvdm_command_child_backend_v1_execute_direct(
    const bx_ntvdm_command_child_backend_v1_request *request,
    bx_ntvdm_command_child_backend_v1_result *result)
{
    wchar_t command_wide[BX_NTVDM_COMMAND_CHILD_BACKEND_V1_COMMAND_BYTES];
    wchar_t environment_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES];
    wchar_t directory_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    HANDLE duplicated[3] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
    STARTUPINFOW startup = { 0 }; PROCESS_INFORMATION process = { 0 };
    DWORD exit_code = 0u, error = ERROR_NOT_SUPPORTED; uint32_t slot;
    if (!bx_ntvdm_command_child_backend_v1_request_valid(request) || !result) return 0;
    bx_ntvdm_command_child_backend_v1_result_clear(result);
    if (!MultiByteToWideChar(CP_OEMCP, 0, (const char *)request->command,
            (int)request->command_bytes, command_wide, (int)_countof(command_wide)) ||
        !MultiByteToWideChar(CP_OEMCP, 0, (const char *)request->environment,
            (int)request->environment_bytes, environment_wide, (int)_countof(environment_wide))) {
        error = GetLastError(); goto done;
    }
    startup.cb = sizeof(startup);
    if (request->launch_service == 8u) {
        for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT; ++slot)
            if (!DuplicateHandle(GetCurrentProcess(),
                    (HANDLE)request->stream->private_handle_values[slot],
                    GetCurrentProcess(), &duplicated[slot], 0u, TRUE,
                    DUPLICATE_SAME_ACCESS)) { error = GetLastError(); goto done; }
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdInput = duplicated[0]; startup.hStdOutput = duplicated[1];
        startup.hStdError = duplicated[2];
    }
    if (!MultiByteToWideChar(CP_OEMCP, 0,
            (const char *)request->host_context->selected_directory,
            (int)request->host_context->directory_bytes, directory_wide,
            (int)_countof(directory_wide))) { error = GetLastError(); goto done; }
    if (!CreateProcessW(0, command_wide, 0, 0, TRUE, CREATE_UNICODE_ENVIRONMENT,
            environment_wide, GetFileAttributesW(directory_wide) == INVALID_FILE_ATTRIBUTES ?
            0 : directory_wide, &startup, &process)) { error = GetLastError(); goto done; }
    if (WaitForSingleObject(process.hProcess, INFINITE) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, &exit_code)) { error = GetLastError(); goto done; }
    result->completed = 1u; result->exit_code = exit_code; error = ERROR_SUCCESS;
done:
    if (process.hThread) CloseHandle(process.hThread);
    if (process.hProcess) CloseHandle(process.hProcess);
    for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT; ++slot)
        if (duplicated[slot] != INVALID_HANDLE_VALUE) CloseHandle(duplicated[slot]);
    result->host_error = error;
    return bx_ntvdm_command_child_backend_v1_result_valid(result);
}