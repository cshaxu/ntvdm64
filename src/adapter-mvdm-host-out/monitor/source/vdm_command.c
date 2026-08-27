#include "vdm_command.h"

typedef struct adapter_vdm_monitor_command_binding {
    session *owner;
    adapter_vdm_monitor_command_provider_fn provider;
    void *context;
} adapter_vdm_monitor_command_binding;

static __declspec(thread) adapter_vdm_monitor_command_binding current_binding;

static void adapter_vdm_monitor_command_teardown(void *context)
{
    if (current_binding.owner != (session *)context) return;
    current_binding.owner = NULL;
    current_binding.provider = NULL;
    current_binding.context = NULL;
}

int adapter_vdm_monitor_bind_command_provider(session *owner,
    adapter_vdm_monitor_command_provider_fn provider, void *context)
{
    if (owner == NULL || provider == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE ||
        current_binding.owner != NULL) return 0;
    current_binding.owner = owner;
    current_binding.provider = provider;
    current_binding.context = context;
    if (!session_register_teardown(owner, adapter_vdm_monitor_command_teardown,
        owner)) {
        adapter_vdm_monitor_command_teardown(owner);
        return 0;
    }
    return 1;
}

int adapter_vdm_monitor_unbind_command_provider(session *owner)
{
    if (owner == NULL || current_binding.owner != owner) return 0;
    adapter_vdm_monitor_command_teardown(owner);
    return 1;
}

static DWORD vdm_command_error_from_status(NTSTATUS status)
{
    if (status == STATUS_INVALID_PARAMETER) return ERROR_INVALID_PARAMETER;
    if (status == STATUS_NO_MEMORY) return ERROR_NOT_ENOUGH_MEMORY;
    if (status == STATUS_NOT_IMPLEMENTED) return ERROR_CALL_NOT_IMPLEMENTED;
    return ERROR_NOT_READY;
}

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information)
{
    session *instance;
    NTSTATUS status;

    /* The NULL first-VDM query remains owned by the future broker. */
    if (information == NULL) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    instance = session_thread_current();
    if (instance == NULL) {
        SetLastError(ERROR_NOT_READY);
        return FALSE;
    }
    if (current_binding.owner != instance || current_binding.provider == NULL) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    status = current_binding.provider(current_binding.context, information);
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError(vdm_command_error_from_status(status));
    return FALSE;
}
