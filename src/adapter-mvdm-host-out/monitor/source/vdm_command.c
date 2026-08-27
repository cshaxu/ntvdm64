#include "vdm_command.h"
#include "session.h"

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
    adapter_vdm_monitor_command_request request;
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
    request.version = ADAPTER_VDM_MONITOR_COMMAND_REQUEST_VERSION;
    request.struct_bytes = (uint32_t)sizeof(request);
    request.information = information;
    status = (NTSTATUS)session_dispatch_control(instance,
        ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND, &request,
        (int32_t)STATUS_NOT_IMPLEMENTED);
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError(vdm_command_error_from_status(status));
    return FALSE;
}
