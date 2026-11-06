#include "mvdm_vm_ingress.h"
#include "mvdm_vm_executor.h"
#include "mvdm_vm_command.h"
#include <stdio.h>
#include <stdlib.h>

static unsigned checks, calls, stage, destroyed, texts, captured;
static uint32_t vm_mode, far_target[2];
static uint16_t ip, return_ip;
static BOOL available = TRUE, open = TRUE;
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr,"FAIL %u: %s\n",__LINE__,#x); exit(1); } } while (0)
void host_ica_lock(void) {}
void host_ica_unlock(void) {}
uint32_t c_getVM(void) { return vm_mode; }
uint16_t c_getIP(void) { return ip; }
void c_setIP(uint16_t value) { ip = value; }
void CALLF(uint32_t target[2])
{ ++calls; return_ip = ip; far_target[0] = target[0]; far_target[1] = target[1]; ip = (uint16_t)target[0]; }
mvdm_vm_executor *mvdm_standalone_worker_executor(void)
{ return available ? (mvdm_vm_executor *)1 : NULL; }
DWORD mvdm_vm_notification_snapshot(uint64_t *serial)
{ *serial = 0; return open ? ERROR_SUCCESS : ERROR_NOT_READY; }
DWORD mvdm_vm_command_service(uint32_t address, uint32_t bytes, uint32_t *service)
{ CHECK(address == 100 && bytes == 20); *service = 8; return stage == 1 ? ERROR_INVALID_ADDRESS : 0; }
DWORD mvdm_vm_command_snapshot(uint32_t service, uint32_t address, uint32_t bytes,
    mvdm_command_capture **capture, ntvdm_vm_text **text)
{
    CHECK(service == 8 && address == 100 && bytes == 20);
    if (stage == 2) return ERROR_INVALID_DATA;
    *capture = (mvdm_command_capture *)1; *text = (ntvdm_vm_text *)1; return 0;
}
BOOL mvdm_command_capture_read(const mvdm_command_capture *capture, mvdm_command_capture_view *view)
{ (void)view; CHECK(capture == (mvdm_command_capture *)1); return stage != 3; }
DWORD ntvdm_command_launch_prepare_guest(const mvdm_command_capture_view *view,
    const HANDLE defaults[3], ntvdm_command_launch **launch)
{
    CHECK(view && defaults);
    if (stage == 4) return ERROR_ACCESS_DENIED;
    *launch = (ntvdm_command_launch *)1; return 0;
}
DWORD mvdm_vm_executor_submit(mvdm_vm_executor *executor, uint32_t vm,
    ntvdm_command_launch *launch, ntvdm_vm_text *text, uint64_t *token)
{
    CHECK(executor == (mvdm_vm_executor *)1 && vm == 7);
    CHECK(launch == (ntvdm_command_launch *)1 && text == (ntvdm_vm_text *)1);
    if (stage == 5) return ERROR_OPERATION_ABORTED;
    *token = UINT64_C(0x100000002); return 0;
}
void ntvdm_command_launch_destroy(ntvdm_command_launch *launch) { if (launch) ++destroyed; }
void ntvdm_vm_text_destroy(ntvdm_vm_text *text) { if (text) ++texts; }
void mvdm_command_capture_destroy(mvdm_command_capture *capture) { if (capture) ++captured; }
int main(void)
{
    uint64_t token;
    DWORD error;
    vm_mode = 1; ip = 0xffff;
    CHECK(!mvdm_vm_command_enter(8) && !calls && ip == 0xffff);
    CHECK(mvdm_vm_ingress_register(0) == ERROR_NOT_READY);
    open = FALSE;
    CHECK(mvdm_vm_ingress_register(0x12345678) == ERROR_NOT_READY);
    open = TRUE; available = FALSE;
    CHECK(mvdm_vm_ingress_register(0x12345678) == ERROR_NOT_READY);
    available = TRUE;
    CHECK(!mvdm_vm_ingress_register(0x12345678));
    CHECK(mvdm_vm_ingress_register(0x12345678) == ERROR_ALREADY_EXISTS);
    CHECK(!mvdm_vm_command_enter(9) && !calls && ip == 0xffff);
    vm_mode = 0;
    CHECK(!mvdm_vm_command_enter(8) && !calls && ip == 0xffff);
    vm_mode = 1;
    CHECK(mvdm_vm_command_enter(8) && calls == 1);
    CHECK(return_ip == 0 && far_target[0] == 0x5678 && far_target[1] == 0x1234 && ip == 0x5678);
    ip = 17;
    CHECK(mvdm_vm_command_enter(10) && calls == 2 && return_ip == 18);
    mvdm_vm_ingress_reset();
    CHECK(!mvdm_vm_command_enter(8) && calls == 2 && ip == 0x5678);
    for (stage = 0; stage < 6; ++stage) {
        destroyed = texts = captured = 0;
        token = UINT64_MAX;
        error = mvdm_vm_command_admit(7, 100, 20, &token);
        CHECK(error == (DWORD)(stage == 1 ? ERROR_INVALID_ADDRESS : stage == 2 || stage == 3 ?
            ERROR_INVALID_DATA : stage == 4 ? ERROR_ACCESS_DENIED : stage == 5 ? ERROR_OPERATION_ABORTED : 0));
        CHECK(token == (stage == 0 ? UINT64_C(0x100000002) : 0));
        CHECK(captured == (stage == 0 || stage >= 3 ? 1u : 0u));
        CHECK(texts == (stage >= 3 ? 1u : 0u));
        CHECK(destroyed == (stage == 5 ? 1u : 0u));
    }
    printf("PASS %u ingress selection/ownership assertions; CCPU/broker boundaries mocked\n", checks);
    return 0;
}
