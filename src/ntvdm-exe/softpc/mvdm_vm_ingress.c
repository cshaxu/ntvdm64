#include "mvdm_vm_ingress.h"
#include "mvdm_vm_notification.h"
#include "mvdm_vm_command.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"

extern void host_ica_lock(void), host_ica_unlock(void);
extern uint32_t c_getVM(void);
extern uint16_t c_getIP(void);
extern void c_setIP(uint16_t);
extern void CALLF(uint32_t target[2]);
static uint32_t command_callback;

DWORD mvdm_vm_ingress_register(uint32_t callback)
{
    DWORD error;
    uint64_t serial;
    if (!callback || !mvdm_standalone_worker_executor()) return ERROR_NOT_READY;
    host_ica_lock();
    error = mvdm_vm_notification_snapshot(&serial);
    if (!error && command_callback) error = ERROR_ALREADY_EXISTS;
    if (!error) command_callback = callback;
    host_ica_unlock();
    return error;
}
void mvdm_vm_ingress_reset(void)
{
    host_ica_lock();
    command_callback = 0;
    host_ica_unlock();
}
int mvdm_vm_command_enter(uint32_t service)
{
    uint32_t callback, target[2];
    if ((service != 8 && service != 10) || !c_getVM()) return 0;
    host_ica_lock();
    callback = command_callback;
    host_ica_unlock();
    if (!callback) return 0;
    /* CCPU owns segment/stack/paging checks and the real FAR return frame.
     * There is no request yet: VMM must first establish the calling VM. */
    c_setIP((uint16_t)(c_getIP() + 1));
    target[0] = (uint16_t)callback;
    target[1] = callback >> 16;
    CALLF(target);
    return 1;
}
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t descriptor, uint32_t bytes,
    uint64_t *token)
{
    mvdm_command_capture *capture = NULL;
    mvdm_command_capture_view view;
    ntvdm_vm_text *text = NULL;
    ntvdm_command_launch *launch = NULL;
    uint32_t service;
    DWORD error;
    HANDLE defaults[3] = {GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE),
        GetStdHandle(STD_ERROR_HANDLE)};
    if (!token) return ERROR_INVALID_PARAMETER;
    *token = 0;
    error = mvdm_vm_command_service(descriptor, bytes, &service);
    if (!error) error = mvdm_vm_command_snapshot(service, descriptor, bytes, &capture, &text);
    if (!error && !mvdm_command_capture_read(capture, &view)) error = ERROR_INVALID_DATA;
    if (!error) error = ntvdm_command_launch_prepare_guest(&view, defaults, &launch);
    if (!error) error = mvdm_vm_executor_submit(mvdm_standalone_worker_executor(),
        vm, launch, text, token);
    if (error) {
        ntvdm_command_launch_destroy(launch);
        ntvdm_vm_text_destroy(text);
    }
    mvdm_command_capture_destroy(capture);
    return error;
}
