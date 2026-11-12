/* Link the actual retained reporters; no guest execution or guest reads. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
void host_applClose(void) { abort(); }
int session_terminate_current(uint32_t code) { (void)code; abort(); }
int xtrn2phy(unsigned long linear, unsigned char access, unsigned long *physical)
{ (void)linear; (void)access; (void)physical; abort(); }
void c_sas_loads(unsigned long source, unsigned char *destination, unsigned long count)
{ (void)source; (void)destination; (void)count; abort(); }
static unsigned checks;
#define CHECK(call) do { SetLastError(10004); call; ++checks; \
    if (GetLastError() != 10004) return 1; } while (0)
int main(int argc, char **argv)
{
    static const char *names[] = {
        "MVDM_TERMINATE_VDM_REPORT_PATH", "MVDM_WIN31_SETUP_XMS_TRACE_PATH",
        "MVDM_S41_WOW_BOP_REPORT_PATH", "MVDM_S42_WOW_RETURN_REPORT_PATH",
        "MVDM_WOW_CALLBACK_CPU_TRACE_PATH", "MVDM_S41_DPMI_MODE_REPORT_PATH"
    };
    char long_path[MAX_PATH + 32];
    const char *path;
    unsigned i;
    if (argc != 3) return 2;
    memset(long_path, 'x', sizeof(long_path) - 1);
    long_path[sizeof(long_path) - 1] = 0;
    path = !strcmp(argv[2], "disabled") ? NULL :
        (!strcmp(argv[2], "long") ? long_path : argv[1]);
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        SetEnvironmentVariableA(names[i], path);
    SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", "8");
    CHECK(mvdm_softpc_report_termination("fixture", NULL));
    CHECK(mvdm_softpc_report_xms_move(1, 2, 3, 1));
    CHECK(mvdm_softpc_report_wow_bop_state(0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_stage("fixture", 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_wow_fast_callback_binding("fixture", 0));
    CHECK(mvdm_softpc_report_dpmi_mode_state("fixture", 0, 0, 0, 0, 0, 0));
    CHECK(mvdm_softpc_report_cpu40_descriptor_publish(8, 0, 0, 0, 0, 0, 0));
    printf("OBSERVER_LAST_ERROR checks=%u result=0\n", checks);
    return 0;
}
