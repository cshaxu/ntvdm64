#include "include/command_process.h"
#include "include/command_services.h"
#include "include/console_services.h"
#include "include/host_context.h"
#include "include/mvdm_command_native_child.h"

BOOL opennt_command_create_process_a(
    LPCSTR application_name,
    LPSTR command_line,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information)
{
    STARTUPINFOA local_startup;
    compat_command_child_request request = {0};
    DWORD error;
    unsigned int i;

    if (!startup_info) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    request.host = compat_host_context_current();
    request.application = application_name;
    request.command = command_line;
    request.process_attributes = process_attributes;
    request.thread_attributes = thread_attributes;
    request.inherit = inherit_handles;
    request.flags = creation_flags;
    request.environment = environment;
    request.directory = current_directory;
    request.startup = opennt_command_child_startup(startup_info, &local_startup);
    request.process = process_information;
    if (!application_name && command_line && *command_line)
        request.service = mvdm_command_native_child_command_service(&request.service_tail);

    /* Identity belongs to the original bound COMMAND thread. The product
     * receives the same resolved identities without projecting that TLS. */
    if (request.startup->dwFlags & STARTF_USESTDHANDLES) {
        request.streams[0] = request.startup->hStdInput;
        request.streams[1] = request.startup->hStdOutput;
        request.streams[2] = request.startup->hStdError;
    } else {
        request.streams[0] = GetStdHandle(STD_INPUT_HANDLE);
        request.streams[1] = GetStdHandle(STD_OUTPUT_HANDLE);
        request.streams[2] = GetStdHandle(STD_ERROR_HANDLE);
    }

    for (i = 0; i < 3; ++i)
        request.stream_kinds[i] = ntvdm_console_handle_kind(request.streams[i]);

    request.completed = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!request.completed) return FALSE;
    error = mvdm_softpc_post_command_child(&request);
    if (!error) {
        if (WaitForSingleObject(request.completed, INFINITE) != WAIT_OBJECT_0)
            RaiseFailFastException(NULL, NULL, 0);
        error = request.error;
    }
    CloseHandle(request.completed);
    SetLastError(error);
    return request.created;
}
