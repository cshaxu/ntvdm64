#ifndef OPENNT_HOST_CHILD_RUNTIME_H
#define OPENNT_HOST_CHILD_RUNTIME_H

#include <stdint.h>
#include <windows.h>

#include "adapter-softpc/host_handle_manager.h"

#define OPENNT_HOST_CHILD_COMMAND_MAX 256u
#define OPENNT_HOST_CHILD_ENV_MAX 65535u

/* Modern host-owned continuation for the reached OpenNT cmdexec process
 * sequence.  It holds copied text and opaque handle IDs only; no guest
 * pointer, BOP selector/service, CPU state or raw HANDLE crosses its ABI. */
typedef struct opennt_host_child_record {
    uint32_t generation;
    uint32_t state;
    uint32_t service;
    uint32_t error;
    uint32_t cancel_requested;
    uint32_t worker_token;
    uint32_t completion_event_token;
    uint32_t job_token;
    uint32_t standard_handle_tokens[3];
    uint32_t command_bytes;
    uint32_t environment_bytes;
    uint32_t exit_code;
    uint32_t create_process_attempted;
    uint32_t create_process_last_error;
    uint32_t create_process_environment_bytes;
    uint32_t create_process_environment_flags;
    CHAR command[OPENNT_HOST_CHILD_COMMAND_MAX];
    CHAR environment[OPENNT_HOST_CHILD_ENV_MAX];
} opennt_host_child_record;

typedef DWORD (WINAPI *opennt_host_child_worker_proc)(LPVOID context);

enum opennt_host_child_state {
    OPENNT_HOST_CHILD_IDLE = 0u,
    OPENNT_HOST_CHILD_STARTING = 1u,
    OPENNT_HOST_CHILD_COMPLETED = 2u,
    OPENNT_HOST_CHILD_FAILED = 3u,
    OPENNT_HOST_CHILD_PENDING = 4u,
    OPENNT_HOST_CHILD_CANCELLED = 5u
};

BOOL opennt_host_child_prepare_startup(const opennt_host_child_record *child,
    runtime_host_handle_manager *handles, STARTUPINFO *startup);
BOOL opennt_host_child_create_process(opennt_host_child_record *child,
    LPCSTR application, LPSTR command, LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes, BOOL inherit_handles,
    DWORD creation_flags, LPVOID environment, LPCSTR current_directory,
    LPSTARTUPINFOA startup, LPPROCESS_INFORMATION process_information);
void opennt_host_child_attach_process(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, HANDLE process);
void opennt_host_child_finish(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, BOOL child_created, DWORD exit_code);
BOOL opennt_host_child_start(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, opennt_host_child_worker_proc worker,
    LPVOID context);
BOOL opennt_host_child_complete(opennt_host_child_record *child,
    runtime_host_handle_manager *handles);
BOOL opennt_host_child_reentry_pending(const opennt_host_child_record *child);
void opennt_host_child_dispose(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, BOOL cancel);

#endif
