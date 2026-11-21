#include "include/mvdm_vm_io.h"

static DWORD dispatch(void *context, const ntvdm_vm_cancel *cancel,
    const console_io_request *request, console_io_reply *reply, frontend_base_result *result)
{
    HANDLE waits[4];
    if (!context || !cancel || !request || !reply || !result) return ERROR_INVALID_PARAMETER;
    waits[0] = cancel->stop; waits[1] = cancel->worker;
    waits[2] = cancel->retired; waits[3] = cancel->broker;
    result->reply = TRUE; result->terminal_error = 0;
    return mvdm_vm_io_dispatch(context, waits, request, reply);
}
static DWORD input_pending(void *context, BOOL *pending)
{
    return mvdm_vm_input_pending(mvdm_vm_io_input(context), pending);
}
static void finish(void *context)
{
    mvdm_vm_io_finish(context);
}
const ntvdm_vm_io mvdm_vm_io_operations = {dispatch, input_pending, finish};
