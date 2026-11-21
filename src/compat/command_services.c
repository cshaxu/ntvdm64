#include "compat/include/command_services.h"
#include "include/command_process.h"

static const compat_command_services *binding;

typedef struct opennt_command_standard_handles {
    unsigned int initialized;
    unsigned int overridden[3];
    HANDLE values[3];
} opennt_command_standard_handles;

static __declspec(thread) opennt_command_standard_handles current_handles;

BOOL compat_command_install(const compat_command_services *services)
{
    const compat_command_services *previous;
    if (!services || !services->capture_guest || !services->capture_host_command ||
            !services->std_handles || !services->command || !services->environment ||
            !services->replace_command || !services->select_service || !services->finish ||
            !services->clear_doswow || !services->register_doswow) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)services, NULL);
    if (previous && previous != services) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

const compat_command_services *compat_command_current_services(void)
{
    /* Missing product composition is fatal, never an implicit native launch. */
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}

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

BOOL __cdecl opennt_command_set_std_handle(DWORD standard_handle, HANDLE handle)
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

LPSTARTUPINFOA __cdecl opennt_command_child_startup(LPSTARTUPINFOA startup,
    LPSTARTUPINFOA child)
{
    opennt_command_initialize_standard_handles();
    if (!current_handles.overridden[0] && !current_handles.overridden[1] &&
        !current_handles.overridden[2])
        return startup;

    *child = *startup;
    child->dwFlags |= STARTF_USESTDHANDLES;
    child->hStdInput = current_handles.values[0];
    child->hStdOutput = current_handles.values[1];
    child->hStdError = current_handles.values[2];
    return child;
}

static compat_command_start start_child;
static void *start_context;

void compat_command_bind_start(compat_command_start start, void *context)
{
    start_child = start;
    start_context = start ? context : NULL;
}

DWORD compat_command_start_child(compat_command_child_request *request)
{
    if (!start_child) return ERROR_INVALID_STATE;
    start_child(start_context, request);
    return request->error;
}
