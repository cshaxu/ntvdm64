#ifndef COMPAT_COMMAND_PROCESS_H
#define COMPAT_COMMAND_PROCESS_H

#include <windows.h>

struct compat_host_context;

typedef struct compat_command_child_request {
    const struct compat_host_context *host;
    LPCSTR application;
    LPSTR command;
    LPSECURITY_ATTRIBUTES process_attributes, thread_attributes;
    BOOL inherit;
    DWORD flags;
    LPVOID environment;
    LPCSTR directory;
    LPSTARTUPINFOA startup;
    LPPROCESS_INFORMATION process;
    unsigned int service;
    const char *service_tail;
    HANDLE streams[3];
    DWORD stream_kinds[3];
    HANDLE completed;
    BOOL created;
    DWORD error;
} compat_command_child_request;

/* Only the submitting thread borrows this record until the worker signals
 * completion. The product owns the launch policy, never the guest snapshot. */
DWORD mvdm_softpc_post_command_child(compat_command_child_request *);
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

#endif
