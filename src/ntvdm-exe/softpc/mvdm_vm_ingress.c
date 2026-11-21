#include "mvdm_vm_ingress.h"
#include "compat/include/mvdm_command_launch.h"
#include "mvdm_vm_command.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"

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
