#include "vdm_control.h"
#include "session.h"

typedef struct adapter_vdm_monitor_query_dir_binding {
    session *owner;
    adapter_vdm_monitor_query_dir_fn callback;
    void *context;
} adapter_vdm_monitor_query_dir_binding;

static __declspec(thread) adapter_vdm_monitor_query_dir_binding current_binding;

int adapter_vdm_monitor_bind_query_dir(session *owner,
    adapter_vdm_monitor_query_dir_fn callback, void *context)
{
    if (owner == NULL || callback == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || session_thread_current() != owner ||
        current_binding.owner != NULL) return 0;
    current_binding.owner = owner;
    current_binding.callback = callback;
    current_binding.context = context;
    return 1;
}

int adapter_vdm_monitor_unbind_query_dir(session *owner)
{
    if (owner == NULL || current_binding.owner != owner ||
        session_thread_current() != owner) return 0;
    current_binding.owner = NULL;
    current_binding.callback = NULL;
    current_binding.context = NULL;
    return 1;
}

/*
 * DIVERGENCE: NT4 exported NtVdmControl entered a kernel VDM product service.
 * The modern product has no kernel VDM API. Preserve the reached VdmQueryDir
 * spelling, structure and status contract through a separately bound
 * monitor-control capability. It never enters the BaseSrv-shaped command
 * route. Other service classes remain explicitly unavailable until their
 * original owner package is admitted.
 */
NTSTATUS NtVdmControl(VDMSERVICECLASS service, PVOID service_data)
{
    session *instance;

    if (service != VdmQueryDir || service_data == NULL) {
        return STATUS_NOT_IMPLEMENTED;
    }
    instance = session_thread_current();
    if (instance == NULL) {
        return STATUS_NOT_IMPLEMENTED;
    }
    if (current_binding.owner != instance || current_binding.callback == NULL) {
        return STATUS_NOT_IMPLEMENTED;
    }
    return current_binding.callback(current_binding.context,
        (PVDMQUERYDIRINFO)service_data);
}
