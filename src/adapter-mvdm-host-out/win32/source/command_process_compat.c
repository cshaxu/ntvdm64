#include "command_process_compat.h"

#undef SetStdHandle
#undef CreateProcess

typedef struct opennt_command_standard_handles {
    unsigned int initialized;
    unsigned int overridden[3];
    HANDLE values[3];
} opennt_command_standard_handles;

static __declspec(thread) opennt_command_standard_handles current_handles;

static int opennt_command_standard_handle_index(DWORD standard_handle)
{
    switch (standard_handle) {
    case STD_INPUT_HANDLE: return 0;
    case STD_OUTPUT_HANDLE: return 1;
    case STD_ERROR_HANDLE: return 2;
    default: return -1;
    }
}

static void opennt_command_initialize_standard_handles(void)
{
    if (current_handles.initialized != 0u) return;
    current_handles.values[0] = GetStdHandle(STD_INPUT_HANDLE);
    current_handles.values[1] = GetStdHandle(STD_OUTPUT_HANDLE);
    current_handles.values[2] = GetStdHandle(STD_ERROR_HANDLE);
    current_handles.initialized = 1u;
}

BOOL opennt_command_set_std_handle(DWORD standard_handle, HANDLE handle)
{
    int index = opennt_command_standard_handle_index(standard_handle);
    if (index < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    current_handles.values[index] = handle;
    current_handles.overridden[index] = 1u;
    return TRUE;
}

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
    int use_child_streams;

    if (startup_info == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    use_child_streams = current_handles.overridden[0] != 0u ||
        current_handles.overridden[1] != 0u ||
        current_handles.overridden[2] != 0u;
    if (!use_child_streams) {
        return CreateProcessA(application_name, command_line,
            process_attributes, thread_attributes, inherit_handles,
            creation_flags, environment, current_directory, startup_info,
            process_information);
    }

    local_startup = *startup_info;
    local_startup.dwFlags |= STARTF_USESTDHANDLES;
    local_startup.hStdInput = current_handles.values[0];
    local_startup.hStdOutput = current_handles.values[1];
    local_startup.hStdError = current_handles.values[2];
    return CreateProcessA(application_name, command_line,
        process_attributes, thread_attributes, inherit_handles,
        creation_flags, environment, current_directory, &local_startup,
        process_information);
}
