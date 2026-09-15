/* Pure worker composition. run16 owns classification, command construction
 * and the inherited VDM environment; BaseSrv owns command availability.
 * Compile-only until the independent link selects authenticated BaseClient
 * transport. Never link this entry against the legacy local queue provider. */
#include "app/package_layout.h"
#include "adapter-opennt-host/basesrv/include/base_rpc_client.h"
#include "adapter-mvdm-host-out/basesrv/include/mvdm_command_native_child.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_execution.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_termination.h"
#include <windows.h>
#include <stdio.h>

/* Original BaseClient capture storage is private to one worker process.  The
 * broker protocol never observes this heap or pointers allocated from it. */
PVOID CsrPortHeap;

/* Default-off integration evidence; it never alters worker control flow. */
static void worker_trace(const char *phase,DWORD status)
{
    char path[MAX_PATH];
    FILE *file;
    if (!GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path))) return;
    file=fopen(path,"a");
    if (!file) return;
    fprintf(file,"NTVDM-S3 phase=%s status=%08lx\n",phase,status);
    fclose(file);
}

int main(int argc,char **argv)
{
    session owner;
    uint32_t dispose_reason;
    int result=ERROR_INVALID_STATE;
    BOOL capture_heap_started=FALSE;
    session_initialize(&owner,1u);
    /* Retain the existing default-off reports before original cmdenv.c imports
     * inherited variables into guest state.  They record only copied host
     * observations and do not enter the BaseSrv wire or alter control flow. */
    mvdm_softpc_capture_command_continuation_report_path();
    mvdm_command_native_child_capture_report_path();
    CsrPortHeap=HeapCreate(0,0,0);
    if (!CsrPortHeap) { worker_trace("capture-heap",ERROR_NOT_ENOUGH_MEMORY); return ERROR_NOT_ENOUGH_MEMORY; }
    capture_heap_started=TRUE;
    if (!app_package_layout_set_process_media_roots(&owner) ||
        !app_package_layout_validate_command_configuration_root(&owner)) {
        result=ERROR_BAD_PATHNAME;
        worker_trace("package",result);
        goto finish;
    }
    if (OpenNtBaseClientConnectCurrent()!=ERROR_SUCCESS) {
        result=ERROR_SERVICE_NOT_ACTIVE;
        worker_trace("connect",result);
        goto finish;
    }
    worker_trace("connect",ERROR_SUCCESS);
    if (!session_select_machine_backend(&owner,SESSION_MACHINE_BACKEND_SOFTPC) ||
        !session_activate(&owner)) { worker_trace("session",result); goto finish; }
    /* No private argv shadow: original ntvdm.c/host_main see the launcher's
     * actual command line, with the original -w/-a/-i contract intact. */
    if (!mvdm_softpc_execution_run_original_entry(&owner,argc,argv,&result))
        result=ERROR_PROCESS_ABORTED;
    worker_trace("run",result);
finish:
    OpenNtBaseClientDisconnectCurrent();
    if (!session_dispose_with_reason(&owner,&dispose_reason)) result=ERROR_BUSY;
    if (capture_heap_started && !HeapDestroy(CsrPortHeap)) result=ERROR_BUSY;
    CsrPortHeap=NULL;
    return result;
}
