#include <nt.h>
#include <windows.h>
#include "include/base_client.h"
#include "include/base_transport.h"

static const ntvdm_base_transport *provider;
static HANDLE worker_wait;
static ULONGLONG observation_direct;
static ntvdm_base_activate activate_console;
static struct compat_console_activation *activation;

void ntvdm_base_bind_transport(const ntvdm_base_transport *transport)
{
    provider = transport;
    if (!transport) {
        if (worker_wait) CloseHandle(worker_wait);
        worker_wait = NULL;
        observation_direct = 0;
        activate_console = NULL;
        activation = NULL;
    }
}

void ntvdm_base_set_activation(ntvdm_base_activate activate, struct compat_console_activation *context)
{
    activate_console = activate;
    activation = context;
}

ULONGLONG ntvdm_base_observation_direct(void)
{
    return observation_direct;
}

static NTSTATUS get_command(const ntvdm_base_transport *transport, PBASE_GET_NEXT_VDM_COMMAND_MSG command)
{
    DWORD requested = command->VDMState;
    ULONGLONG direct = 0;
    HANDLE wait = NULL;
    NTSTATUS status = transport->get(command, &wait, &direct);
    if (wait) {
        /* Get can return a new duplicate of the same ConsoleRecord event. */
        if (worker_wait) CloseHandle(wait);
        else worker_wait = wait;
        command->WaitObjectForVDM = worker_wait;
    }
    if ((status == STATUS_SUCCESS || (status == STATUS_NO_MEMORY && (command->VDMState & RETURN_ON_NO_COMMAND))) &&
        !command->WaitObjectForVDM && (command->CmdLen || (command->VDMState & RETURN_ON_NO_COMMAND)) &&
        !(requested & (ASKING_FOR_ENVIRONMENT | ASKING_FOR_PIF | ASKING_FOR_WOW_BINARY)))
        observation_direct = direct;
    if (status == STATUS_SUCCESS && activate_console && !command->WaitObjectForVDM &&
        command->CmdLen && !(command->VDMState & ASKING_FOR_ENVIRONMENT)) {
        DWORD error = activate_console(activation, TRUE);
        if (error) {
            SetLastError(error);
            return STATUS_UNSUCCESSFUL;
        }
    }
    return status;
}

NTSTATUS NTAPI NtvdmBaseClientCallServer(PCSR_API_MSG message,
    PCSR_CAPTURE_HEADER capture, CSR_API_NUMBER number, ULONG length)
{
    PBASE_API_MSG base = (PBASE_API_MSG)message;
    const ntvdm_base_transport *transport = provider;
    NTSTATUS status;
    ULONG value;
    BOOL notify_started;
    BOOL wow;

    (void)capture;
    if (!base || !transport) return STATUS_UNSUCCESSFUL;
    status = transport->status(ERROR_SUCCESS);
    if (NT_SUCCESS(status)) {
        status = STATUS_UNSUCCESSFUL;
        switch (number) {
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepCheckVDM):
            status = length == sizeof(base->u.CheckVDM)
                ? transport->check(&base->u.CheckVDM) : transport->status(ERROR_INVALID_DATA);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepGetNextVDMCommand):
            status = length == sizeof(base->u.GetNextVDMCommand)
                ? get_command(transport, &base->u.GetNextVDMCommand) : transport->status(ERROR_INVALID_DATA);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepUpdateVDMEntry):
            status = length == sizeof(base->u.UpdateVDMEntry)
                ? transport->update(&base->u.UpdateVDMEntry) : transport->status(ERROR_INVALID_DATA);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepExitVDM):
            if (length != sizeof(base->u.ExitVDM)) {
                status = transport->status(ERROR_INVALID_PARAMETER);
                break;
            }
            notify_started = base->u.ExitVDM.ConsoleHandle == OPENNT_WOW_STARTUP_NOOP_CONSOLE;
            wow = notify_started || base->u.ExitVDM.ConsoleHandle == (HANDLE)-1;
            value = 0;
            status = transport->exit_vdm(wow, notify_started, wow ? base->u.ExitVDM.iWowTask : 0, &value);
            if (NT_SUCCESS(status)) {
                if (value > 1 || (value && !worker_wait)) {
                    status = transport->status(ERROR_INVALID_DATA);
                    break;
                }
                base->u.ExitVDM.WaitObjectForVDM = value ? worker_wait : NULL;
                if (value) worker_wait = NULL;
            }
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepGetVDMExitCode):
            status = length == sizeof(base->u.GetVDMExitCode)
                ? transport->exit_code(base->u.GetVDMExitCode.hParent, &base->u.GetVDMExitCode.ExitCode)
                : transport->status(ERROR_INVALID_PARAMETER);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepSetReenterCount):
            if (length == sizeof(base->u.SetReenterCount) &&
                (base->u.SetReenterCount.fIncDec == INCREMENT_REENTER_COUNT ||
                 base->u.SetReenterCount.fIncDec == DECREMENT_REENTER_COUNT))
                status = transport->reenter(base->u.SetReenterCount.fIncDec);
            else
                status = transport->status(ERROR_INVALID_PARAMETER);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepRegisterWowExec):
            if (length == sizeof(base->u.RegisterWowExec))
                status = transport->register_wow((ULONG)(ULONG_PTR)base->u.RegisterWowExec.hwndWowExec);
            break;
        case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX, BasepIsFirstVDM):
            if (length == sizeof(base->u.IsFirstVDM)) {
                value = 0;
                status = transport->first(&value);
                if (NT_SUCCESS(status)) base->u.IsFirstVDM.FirstVDM = value;
            }
            break;
        }
    }
    base->ReturnValue = status;
    return status;
}
