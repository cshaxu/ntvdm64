#include "mvdm_standalone_worker.h"

#include "product-package/package_layout.h"
#include "basesrv/opennt/include/base_rpc_client.h"
#include "adapter-mvdm-host-out/basesrv/include/mvdm_command_native_child.h"
#include "mvdm_softpc_execution.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_softpc_termination.h"
#include "session/session.h"
#include "adapter-mvdm-host-out/monitor/include/monitor_context.h"

#include <stdio.h>

/* Original BaseClient capture storage is private to this worker process. */
PVOID CsrPortHeap;

static session worker_session;
static BOOL worker_heap;
static BOOL worker_thread;
static BOOL worker_memory;
static BOOL worker_escape;
static BOOL worker_started;
static BOOL worker_session_initialized;

static void mvdm_standalone_worker_trace(const char *phase,DWORD status)
{
    char path[MAX_PATH];
    FILE *file;

    if (!GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path)))
        return;
    file=fopen(path,"a");
    if (!file) return;
    fprintf(file,"NTVDM-S3 phase=%s status=%08lx\n",phase,status);
    fclose(file);
}

static int mvdm_standalone_worker_cleanup(int result)
{
    uint32_t dispose_reason;

    if (worker_escape) {
        session_disarm_termination_escape(&worker_session);
        worker_escape=FALSE;
    }
    if (worker_memory) {
        mvdm_softpc_guest_memory_end(&worker_session);
        worker_memory=FALSE;
    }
    if (worker_thread) {
        (void)session_thread_unbind(&worker_session);
        worker_thread=FALSE;
    }
    OpenNtBaseClientDisconnectCurrent();
    if (worker_session_initialized && !session_dispose_with_reason(&worker_session,
            &dispose_reason)) result=ERROR_BUSY;
    if (worker_heap && !HeapDestroy(CsrPortHeap)) result=ERROR_BUSY;
    CsrPortHeap=NULL;
    worker_heap=FALSE;
    worker_started=FALSE;
    worker_session_initialized=FALSE;
    return result;
}

DWORD mvdm_standalone_worker_begin(void)
{
    DWORD error=ERROR_INVALID_STATE;

    if (worker_started) return ERROR_ALREADY_EXISTS;
    session_initialize(&worker_session,1u);
    worker_session_initialized=TRUE;
    mvdm_softpc_capture_command_continuation_report_path();
    mvdm_command_native_child_capture_report_path();
    CsrPortHeap=HeapCreate(0,0,0);
    if (!CsrPortHeap) { error=ERROR_NOT_ENOUGH_MEMORY; goto fail; }
    worker_heap=TRUE;
    if (!app_package_layout_set_process_media_roots(&worker_session) ||
        !app_package_layout_validate_command_configuration_root(&worker_session)) {
        error=ERROR_BAD_PATHNAME; goto fail;
    }
    error=OpenNtBaseClientConnectCurrent();
    if (error!=ERROR_SUCCESS) goto fail;
    error=OpenNtBaseClientWatchBroker();
    if (error!=ERROR_SUCCESS) goto fail;
    if (!session_select_machine_backend(&worker_session,
            SESSION_MACHINE_BACKEND_SOFTPC) || !session_activate(&worker_session) ||
        !session_thread_bind_owned(&worker_session,
            SESSION_THREAD_BINDING_SOFTPC_ENTRY)) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    worker_thread=TRUE;
    if (!mvdm_monitor_bind_current_thread()) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    if (!mvdm_softpc_guest_memory_begin(&worker_session)) {
        error=ERROR_NOT_ENOUGH_MEMORY; goto fail;
    }
    worker_memory=TRUE;
    if (!session_arm_termination_escape(&worker_session)) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    worker_escape=TRUE;
    worker_started=TRUE;
    mvdm_standalone_worker_trace("connect",ERROR_SUCCESS);
    return ERROR_SUCCESS;
fail:
    mvdm_standalone_worker_trace("bootstrap",error);
    (void)mvdm_standalone_worker_cleanup((int)error);
    return error;
}

jmp_buf *mvdm_standalone_worker_termination_escape(void)
{
    return worker_started && worker_escape ? &worker_session.termination_escape : NULL;
}

DWORD mvdm_standalone_worker_completion_code(void)
{
    return worker_started ? worker_session.completion_code : ERROR_INVALID_STATE;
}

int mvdm_standalone_worker_finish(int result)
{
    mvdm_standalone_worker_trace("run",(DWORD)result);
    return mvdm_standalone_worker_cleanup(result);
}

void mvdm_standalone_worker_record_phase(const char *phase)
{
    /* Default-off host-local witness for the original ntvdm.c entry.  This
     * does not participate in worker setup or change source ordering. */
    mvdm_standalone_worker_trace(phase,ERROR_SUCCESS);
}
