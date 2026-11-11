/* Observer-only unit test: link the actual worker diagnostic implementation.
 * No guest, CCPU execution, or replacement historical provider is involved. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
void *Ldt;
unsigned long IntelBase, Cpu40GdtShadowAddress, Cpu40LdtShadowAddress;
/* Diagnostic-only coverage must never terminate a session or access a CPU. */
void host_applClose(void) { abort(); }
int session_terminate_current(uint32_t code) { (void)code; abort(); }
int xtrn2phy(unsigned long linear, unsigned char access, unsigned long *physical)
{ (void)linear; (void)access; (void)physical; abort(); }
void c_sas_loads(unsigned long source, unsigned char *destination, unsigned long count)
{ (void)source; (void)destination; (void)count; abort(); }
static unsigned checks;
#define CHECK(call) do { SetLastError(10004); call; ++checks; \
    if (GetLastError() != 10004) { fprintf(stderr, "%s corrupted error: %lu\n", \
        #call, GetLastError()); return 1; } } while (0)
int main(int argc, char **argv)
{
    unsigned mode;
    char long_path[MAX_PATH + 32];
    if (argc != 3 && argc != 4) return 2;
    SetEnvironmentVariableA("MVDM_WOW_NT_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_TERMINATE_VDM_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_HARDWARE_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_MODE_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_XMS_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_ILLEGAL_TRACE_PATH", argc == 4 ? argv[1] : NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_REAL_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WIN31_SETUP_COMMAND_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S41_WOW_BOP_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S41_DPMI_MODE_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_DESCRIPTOR_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_SET_DESCRIPTOR_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_SET_DESCRIPTOR_ENTRY_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_BOP_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_VXD_BOP_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_SWINT_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_ALLOCSEL_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_SETDESCRIPTOR_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_KRNL_GETSEL_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_WOW_LONGPTRADD_TRACE_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_IRET16_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_S42_DPMI_IRET16_STATE_REPORT_PATH", NULL);
    CHECK(mvdm_softpc_report_nt_transition(0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_termination(NULL, NULL));
    CHECK(mvdm_softpc_report_virtual_memory_failure(0));
    CHECK(mvdm_softpc_report_setup_hardware_probe(NULL, 0, 0));
    CHECK(mvdm_softpc_report_setup_mode_switch(NULL, 0, 0));
    CHECK(mvdm_softpc_report_xms_service(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_xms_move(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_setup_illegal_opcode(0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_setup_real_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_setup_command_classification(NULL, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_table_address(NULL, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_bop_state(0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_dispatch_return_state(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_stage(NULL, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_fast_callback_binding(NULL, 0));
    CHECK(mvdm_softpc_report_dpmi_mode_state(NULL, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_descriptor(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_cpu40_descriptor_publish(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_set_descriptor(0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_set_descriptor_entry(0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_bop(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_fast_bop_result(0, 0, 0));
    CHECK(mvdm_softpc_report_virtual_device_control(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_swint(0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_swint_target(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_allocsel_instruction(0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_setdescriptor_instruction(0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_getsel_instruction(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_longptradd_instruction(0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_iret16(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_iret16_state(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_code_segment_load(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_far_call_attempt(0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_far_call_validation(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_int2a_ldt(0, 0, 0));
    CHECK(mvdm_softpc_report_wow_int2a(0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_exception_hook(0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_fault_dispatch(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_dpmi_fault_stack(NULL, 0, 0, 0, 0, NULL, 0));
    CHECK(mvdm_softpc_initialize_default_off_traces());
    CHECK(mvdm_softpc_report_wow_source_descriptor_store(0, 0, 0, 0, 0, 0, 0));
    memset(long_path, 'x', sizeof(long_path) - 1);
    long_path[sizeof(long_path) - 1] = 0;
    for (mode = 0; mode < 3; ++mode) {
        const char *path = mode == 0 ? long_path : (mode == 1 ? argv[2] : argv[1]);
        SetEnvironmentVariableA("MVDM_S41_WOW_BOP_REPORT_PATH", path);
        SetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", path);
        SetEnvironmentVariableA("MVDM_S42_DPMI_SWINT_REPORT_PATH", path);
        CHECK(mvdm_softpc_report_wow_bop_state(0, 0, 0, 0, 0, 0));
        CHECK(mvdm_softpc_report_wow_dispatch_return_state(0, 0, 0, 0));
        CHECK(mvdm_softpc_report_wow_stage("fixture", 0, 0, 0, 0));
        CHECK(mvdm_softpc_report_dpmi_swint_target(0x31, 0x1234, 0x5678, 0x9abc));
    }
    printf("OBSERVER_LAST_ERROR checks=%u result=0\n", checks);
    return 0;
}
