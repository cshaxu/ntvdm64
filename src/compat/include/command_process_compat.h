#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_COMMAND_PROCESS_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_COMMAND_PROCESS_COMPAT_H

#include <windows.h>

/*
 * DIVERGENCE(ADAPTER-WIN32-037): COMMAND's original child path temporarily
 * changes ntvdm.exe-wide standard handles before CreateProcess.  The app owns
 * its own standard streams, so retain the original SetStdHandle/CreateProcess
 * call sequence while making its endpoints child-local STARTUPINFO state.
 */
BOOL opennt_command_set_std_handle(DWORD standard_handle, HANDLE handle);
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
    LPPROCESS_INFORMATION process_information);

#undef SetStdHandle
#define SetStdHandle(standard_handle, handle) \
    opennt_command_set_std_handle((standard_handle), (handle))
#undef CreateProcess
#define CreateProcess(application_name, command_line, process_attributes, thread_attributes, inherit_handles, creation_flags, environment, current_directory, startup_info, process_information) \
    opennt_command_create_process_a((application_name), (command_line), \
        (process_attributes), (thread_attributes), (inherit_handles), \
        (creation_flags), (environment), (current_directory), \
        (startup_info), (process_information))

#endif
