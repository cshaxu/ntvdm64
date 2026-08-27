#include "vdm.h"
#include "session.h"

/*
 * DIVERGENCE: NT4 exported NtVdmControl entered a kernel VDM product service.
 * The modern product has no kernel VDM API. Preserve the reached VdmQueryDir
 * spelling, structure and status contract by dispatching a host-local request
 * to the bound session capability. Other service classes remain explicitly
 * unavailable until their original owner package is admitted.
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
    return (NTSTATUS)session_dispatch_control(instance, (uint32_t)service,
        service_data, (int32_t)STATUS_NOT_IMPLEMENTED);
}
