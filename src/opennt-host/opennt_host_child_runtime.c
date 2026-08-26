#include "opennt_host_child_runtime.h"

#include <string.h>

BOOL opennt_host_child_prepare_startup(const opennt_host_child_record *child,
    runtime_host_handle_manager *handles, STARTUPINFO *startup)
{
    uint32_t index;
    uint32_t explicit_streams = 0u;
    HANDLE *targets[3];
    if (child == NULL || startup == NULL) return FALSE;
    targets[0] = &startup->hStdError;
    targets[1] = &startup->hStdOutput;
    targets[2] = &startup->hStdInput;
    for (index = 0u; index < 3u; ++index) {
        uint32_t token = child->standard_handle_tokens[index];
        if (token == UINT32_MAX) continue;
        /* DIVERGENCE(HOST-DIV-031): OpenNT temporarily installed VDM
         * standard handles process-wide.  Map the same three ordered
         * endpoints only into this child through the opaque handle manager. */
        if (token == 0u || !runtime_host_handle_manager_lookup_handle(
                handles, token, targets[index])) {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }
        ++explicit_streams;
    }
    if (explicit_streams != 0u) startup->dwFlags |= STARTF_USESTDHANDLES;
    return TRUE;
}

BOOL opennt_host_child_create_process(opennt_host_child_record *child,
    LPCSTR application, LPSTR command, LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes, BOOL inherit_handles,
    DWORD creation_flags, LPVOID environment, LPCSTR current_directory,
    LPSTARTUPINFOA startup, LPPROCESS_INFORMATION process_information)
{
    const CHAR *entry = (const CHAR *)environment;
    uint32_t environment_bytes = 0u;
    uint32_t environment_flags = 0u;
    BOOL created;
    if (child != NULL) {
        child->create_process_attempted = 1u;
        child->create_process_last_error = ERROR_SUCCESS;
        while (entry != NULL && environment_bytes < OPENNT_HOST_CHILD_ENV_MAX) {
            size_t entry_bytes = strlen(entry);
            if (entry_bytes == 0u) {
                ++environment_bytes;
                break;
            }
            if (entry_bytes >= OPENNT_HOST_CHILD_ENV_MAX - environment_bytes)
                break;
            if (_strnicmp(entry, "COMSPEC=", 8u) == 0) environment_flags |= 0x01u;
            if (_strnicmp(entry, "SystemRoot=", 11u) == 0) environment_flags |= 0x02u;
            if (_strnicmp(entry, "PATH=", 5u) == 0) environment_flags |= 0x04u;
            environment_bytes += (uint32_t)entry_bytes + 1u;
            entry += entry_bytes + 1u;
        }
        child->create_process_environment_bytes = environment_bytes;
        child->create_process_environment_flags = environment_flags;
    }
    /* DIVERGENCE(HOST-DIV-031): cmdexec.c's source buffers are ANSI. Keep
     * that argument contract and use public CreateProcessA on current hosts. */
    created = CreateProcessA(application, command, process_attributes,
        thread_attributes, inherit_handles, creation_flags, environment,
        current_directory, startup, process_information);
    if (child != NULL)
        child->create_process_last_error = created ? ERROR_SUCCESS : GetLastError();
    return created;
}

void opennt_host_child_attach_process(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, HANDLE process)
{
    HANDLE job;
    DWORD error = ERROR_NOT_ENOUGH_MEMORY;
    if (child == NULL || process == NULL || process == INVALID_HANDLE_VALUE) return;
    job = CreateJobObjectA(NULL, NULL);
    if (job != NULL && AssignProcessToJobObject(job, process) &&
        runtime_host_handle_manager_publish(handles, job,
            RUNTIME_HOST_HANDLE_OWNED, &child->job_token, &error)) {
        if (child->cancel_requested != 0u)
            (void)TerminateJobObject(job, ERROR_CANCELLED);
        return;
    }
    if (job != NULL) CloseHandle(job);
}

void opennt_host_child_finish(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, BOOL child_created, DWORD exit_code)
{
    HANDLE event = INVALID_HANDLE_VALUE;
    DWORD ignored;
    if (child == NULL) return;
    if (child->job_token != 0u) {
        (void)runtime_host_handle_manager_release(handles, child->job_token, &ignored);
        child->job_token = 0u;
    }
    child->exit_code = exit_code;
    child->error = exit_code;
    child->state = child_created ? OPENNT_HOST_CHILD_COMPLETED :
        OPENNT_HOST_CHILD_FAILED;
    if (child->completion_event_token != 0u &&
        runtime_host_handle_manager_lookup_handle(handles,
            child->completion_event_token, &event))
        (void)SetEvent(event);
}

BOOL opennt_host_child_start(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, opennt_host_child_worker_proc worker,
    LPVOID context)
{
    HANDLE event, thread;
    DWORD error = ERROR_NOT_ENOUGH_MEMORY;
    if (child == NULL || worker == NULL || child->state == OPENNT_HOST_CHILD_PENDING)
        return FALSE;
    event = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (event == NULL || !runtime_host_handle_manager_publish(handles, event,
            RUNTIME_HOST_HANDLE_OWNED, &child->completion_event_token, &error)) {
        if (event != NULL) CloseHandle(event);
        SetLastError(error);
        return FALSE;
    }
    child->state = OPENNT_HOST_CHILD_PENDING;
    child->error = ERROR_SUCCESS;
    thread = CreateThread(NULL, 0u, worker, context, 0u, NULL);
    if (thread != NULL && runtime_host_handle_manager_publish(handles, thread,
            RUNTIME_HOST_HANDLE_OWNED, &child->worker_token, &error)) return TRUE;
    if (thread != NULL) {
        (void)WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    (void)runtime_host_handle_manager_release(handles,
        child->completion_event_token, &error);
    child->completion_event_token = 0u;
    child->state = OPENNT_HOST_CHILD_FAILED;
    child->error = error;
    SetLastError(error);
    return FALSE;
}

BOOL opennt_host_child_complete(opennt_host_child_record *child,
    runtime_host_handle_manager *handles)
{
    HANDLE worker;
    DWORD ignored;
    if (child == NULL ||
        (child->state != OPENNT_HOST_CHILD_PENDING &&
         child->state != OPENNT_HOST_CHILD_COMPLETED &&
         child->state != OPENNT_HOST_CHILD_FAILED) || child->worker_token == 0u ||
        !runtime_host_handle_manager_lookup_handle(handles, child->worker_token, &worker)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (WaitForSingleObject(worker, 0u) != WAIT_OBJECT_0) {
        SetLastError(ERROR_IO_INCOMPLETE);
        return FALSE;
    }
    (void)runtime_host_handle_manager_release(handles, child->worker_token, &ignored);
    child->worker_token = 0u;
    if (child->completion_event_token != 0u) {
        (void)runtime_host_handle_manager_release(handles,
            child->completion_event_token, &ignored);
        child->completion_event_token = 0u;
    }
    return child->state == OPENNT_HOST_CHILD_COMPLETED ||
        child->state == OPENNT_HOST_CHILD_FAILED;
}

BOOL opennt_host_child_reentry_pending(const opennt_host_child_record *child)
{
    return child != NULL && child->worker_token != 0u &&
        (child->state == OPENNT_HOST_CHILD_PENDING ||
         child->state == OPENNT_HOST_CHILD_COMPLETED ||
         child->state == OPENNT_HOST_CHILD_FAILED);
}

void opennt_host_child_dispose(opennt_host_child_record *child,
    runtime_host_handle_manager *handles, BOOL cancel)
{
    HANDLE worker = INVALID_HANDLE_VALUE;
    HANDLE job = INVALID_HANDLE_VALUE;
    DWORD ignored;
    if (child == NULL) return;
    if (cancel && child->state == OPENNT_HOST_CHILD_PENDING) {
        child->cancel_requested = 1u;
        if (child->job_token != 0u && runtime_host_handle_manager_lookup_handle(
                handles, child->job_token, &job))
            (void)TerminateJobObject(job, ERROR_CANCELLED);
    }
    if (child->worker_token != 0u && runtime_host_handle_manager_lookup_handle(
            handles, child->worker_token, &worker))
        (void)WaitForSingleObject(worker, INFINITE);
    if (cancel) {
        child->state = OPENNT_HOST_CHILD_CANCELLED;
        child->exit_code = ERROR_CANCELLED;
        child->error = ERROR_CANCELLED;
    }
    if (child->worker_token != 0u) {
        (void)runtime_host_handle_manager_release(handles, child->worker_token, &ignored);
        child->worker_token = 0u;
    }
    if (child->completion_event_token != 0u) {
        (void)runtime_host_handle_manager_release(handles,
            child->completion_event_token, &ignored);
        child->completion_event_token = 0u;
    }
}
