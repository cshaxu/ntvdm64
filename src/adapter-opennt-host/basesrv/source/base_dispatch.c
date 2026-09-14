/* Finite routing below the retained BaseSrv owners. Mapping evidence:
 * OpenNT base/win32/server/srvinit.c BaseServerApiDispatchTable.
 * The full CSR dispatcher and non-VDM Base APIs are excluded. No command,
 * capacity, queue or retry policy is implemented here. */
#include "basesrv.h"
#include <base_dispatch.h>

NTSTATUS OpenNtBaseDispatch(PCSR_API_MSG message, CSR_API_NUMBER number,
    ULONG length)
{
    PCSR_THREAD caller = OpenNtBaseServerRequestThread();
    PCSR_API_ROUTINE routine;
    ULONG expected;
    CSR_REPLY_STATUS reply = 0;
    if (!message) return STATUS_INVALID_PARAMETER;
    if (!caller || !caller->Process) {
        message->ReturnValue = STATUS_ACCESS_DENIED;
        return STATUS_ACCESS_DENIED;
    }
#define VDM_ROUTE(api, handler, type) \
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, api): \
        routine = handler; expected = sizeof(type); break
    switch (number) {
    VDM_ROUTE(BasepCheckVDM, BaseSrvCheckVDM, BASE_CHECKVDM_MSG);
    VDM_ROUTE(BasepUpdateVDMEntry, BaseSrvUpdateVDMEntry, BASE_UPDATE_VDM_ENTRY_MSG);
    VDM_ROUTE(BasepGetNextVDMCommand, BaseSrvGetNextVDMCommand, BASE_GET_NEXT_VDM_COMMAND_MSG);
    VDM_ROUTE(BasepExitVDM, BaseSrvExitVDM, BASE_EXIT_VDM_MSG);
    VDM_ROUTE(BasepIsFirstVDM, BaseSrvIsFirstVDM, BASE_IS_FIRST_VDM_MSG);
    VDM_ROUTE(BasepGetVDMExitCode, BaseSrvGetVDMExitCode, BASE_GET_VDM_EXIT_CODE_MSG);
    VDM_ROUTE(BasepSetReenterCount, BaseSrvSetReenterCount, BASE_SET_REENTER_COUNT_MSG);
    VDM_ROUTE(BasepSetVDMCurDirs, BaseSrvSetVDMCurDirs, BASE_GET_SET_VDM_CUR_DIRS_MSG);
    VDM_ROUTE(BasepGetVDMCurDirs, BaseSrvGetVDMCurDirs, BASE_GET_SET_VDM_CUR_DIRS_MSG);
    VDM_ROUTE(BasepBatNotification, BaseSrvBatNotification, BASE_BAT_NOTIFICATION_MSG);
    VDM_ROUTE(BasepRegisterWowExec, BaseSrvRegisterWowExec, BASE_REGISTER_WOWEXEC_MSG);
    default:
        message->ReturnValue = STATUS_INVALID_PARAMETER;
        return STATUS_INVALID_PARAMETER;
    }
#undef VDM_ROUTE
    if (length != expected) {
        message->ReturnValue = STATUS_INVALID_PARAMETER;
        return STATUS_INVALID_PARAMETER;
    }
    /* Never trust a caller identity carried in a decoded message. */
    message->h.ClientId = caller->ClientId;
    message->ReturnValue = routine(message, &reply);
    return message->ReturnValue;
}
