#include "mvdm_standalone_worker.h"
#include "worker-base/connection.h"
#include "compat/include/thread_lifecycle.h"
#include "compat/include/mvdm_softpc_worker_exit.h"
#include "compat/include/mvdm_softpc_termination.h"
#include "compat/include/mvdm_softpc_text_video.h"
extern const mvdm_text_video_provider mvdm_original_text_video_provider;
extern void host_applClose(void);
extern void (*BIOS[])(void);
#include "ntvdm-exe/command/include/command_launch.h"

#include "ntvdm-exe/package_layout.h"
#include "common/base_srv/base_rpc_client.h"
#include "compat/include/console_io.h"
#include "compat/include/base_capture_heap.h"
#include "mvdm_softpc_execution.h"
#include "compat/include/mvdm_softpc_ccpu_wait.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_vm_executor.h"
#include "compat/include/mvdm_vm_bridge.h"
#include "compat/include/mvdm_vm_ingress.h"
#include "ntvdm_dos_observer.h"
#include "ntvdm-exe/session/session.h"
#include "compat/include/monitor_context.h"
#include "compat/include/wow_user_session_binding.h"
#include "compat/include/mvdm_shadow_registry.h"
#include "ntvdm-exe/win32/console_client.h"
#include "compat/include/guest_location.h"
#include "ntvdm-exe/win32/worker_lifecycle.h"
#include <stdlib.h>
#include <ctype.h>

/* Original nt_reset.c switch, also respected by subsequent DOS reentry. */
extern BOOL StreamIoSwitchOn;

static session worker_session;
static ntvdm_dos_observer worker_observer;
static BOOL worker_character_io;
static BOOL worker_thread;
static BOOL worker_memory;
static BOOL worker_escape;
static BOOL worker_started;
static BOOL worker_session_initialized;
static wow_user_runtime worker_wow_runtime = WOW_USER_RUNTIME_INITIALIZER;
static wow_user_session_binding worker_wow_binding;
static BOOL worker_wow_attached;
static ntvdm_vm_host *worker_frontends;
static mvdm_vm_executor *worker_vm_executor;

static DWORD submit_vm_command(void *executor, uint32_t vm,
    ntvdm_command_launch *launch, ntvdm_vm_text *text, uint64_t *token)
{
    return mvdm_vm_executor_submit(executor, vm, launch, text, token);
}

mvdm_vm_executor *mvdm_standalone_worker_executor(void)
{
    return worker_started && worker_character_io ? worker_vm_executor : NULL;
}

ntvdm_vm_host *mvdm_standalone_worker_frontends(void)
{
    return worker_started && worker_character_io ? worker_frontends : NULL;
}
DWORD mvdm_standalone_worker_close_frontends(void)
{
    return ntvdm_vm_host_close(worker_frontends);
}
static DWORD begin_character_io(void *owner,HANDLE stop)
{
    LONG presentation[4]={0};
    DWORD error;
    int available;
    (void)stop;
    error=ntvdm_console_client_begin(owner);
    if(error)return error;
    /* Resolve the authenticated frontend requirement before original startup
     * can enable stream I/O. A short DOS task can finish before the first
     * video tick; deferring this choice loses writes on frame-only frontends.
     * A native Console shown in a Window still owns stream/restore state.
     * Only presentation-only frontends must disable the startup option. */
    available=ntvdm_console_window_query(CONSOLE_WINDOW_PRESENTATION_ONLY,presentation);
    if(available<=0)return available<0 ? ERROR_INVALID_STATE : GetLastError();
    if(presentation[0])StreamIoSwitchOn=FALSE;
    return ERROR_SUCCESS;
}
static BOOL worker_shadow_registry;
static BOOL worker_ccpu_wait;
extern void mvdm_softpc_v7_pointer_attach(void);

static int mvdm_standalone_worker_cleanup(int result)
{
    uint32_t dispose_reason;

    ntvdm_dos_observer_stop(&worker_observer);
    ntvdm_worker_lifecycle_end();
    mvdm_vm_command_bind(NULL, NULL);
    mvdm_vm_executor_destroy(worker_vm_executor);
    worker_vm_executor=NULL;
    /* Authorization already joined VM I/O before original host cleanup.
     * The registry object remained alive for concurrent rejected callers. */
    if (ntvdm_vm_host_destroy(worker_frontends)) return ERROR_BUSY;
    worker_frontends=NULL;

    /* The original host has already stopped its timer/event producers before
     * this process-level worker cleanup runs. */
    if (worker_ccpu_wait) {
        mvdm_softpc_ccpu_wait_end();
        worker_ccpu_wait=FALSE;
    }

    if (worker_escape) {
        session_disarm_termination_escape(&worker_session);
        worker_escape=FALSE;
    }
    /* USER retirement may still need the guest-memory service. Do not drop
     * its registration, or free backing state under a live thread, when the
     * bounded provider rejects release. The process-exit caller receives
     * failure; a same-owner cleanup retry retains the necessary state. */
    if (worker_wow_attached) {
        if (!wow_user_session_detach(&worker_wow_binding))
            return ERROR_BUSY;
        worker_wow_attached=FALSE;
    }
    if (worker_memory) {
        mvdm_softpc_guest_memory_end(&worker_session);
        worker_memory=FALSE;
    }
    if (worker_shadow_registry) {
        ntvdm_shadow_registry_shutdown();
        worker_shadow_registry=FALSE;
    }
    if (worker_thread) {
        (void)session_thread_unbind(&worker_session);
        worker_thread=FALSE;
    }
    worker_base_disconnect();
    if (worker_session_initialized && !session_dispose_with_reason(&worker_session,
            &dispose_reason, ntvdm_console_client_end)) result=ERROR_BUSY;
    if (!opennt_base_capture_heap_end()) result=ERROR_BUSY;
    worker_started=FALSE;
    worker_session_initialized=FALSE;
    return result;
}

static DWORD mvdm_standalone_worker_begin(void)
{
    DWORD error=ERROR_INVALID_STATE;
    int index;
    BOOL character_io=TRUE;

    if (worker_started) return ERROR_ALREADY_EXISTS;
    if (!mvdm_softpc_text_video_bind(&mvdm_original_text_video_provider) ||
            !mvdm_softpc_termination_bind(host_applClose) ||
            !opennt_thread_initialize() || !ntvdm_command_initialize() ||
            !ntvdm_dpmi_initialize() || !ntvdm_vm_services_initialize(&BIOS[MVDM_VM_DEVICE_BOP])) return GetLastError();
    session_initialize(&worker_session,1u);
    worker_session_initialized=TRUE;
    mvdm_softpc_v7_pointer_attach();
    if (!mvdm_softpc_ccpu_wait_begin()) {
        error=GetLastError(); goto fail;
    }
    worker_ccpu_wait=TRUE;
    if (!opennt_base_capture_heap_begin()) {
        error=ERROR_NOT_ENOUGH_MEMORY; goto fail;
    }
    if (!app_package_layout_set_process_media_roots(&worker_session) ||
        !app_package_layout_validate_command_configuration_root(&worker_session)) {
        error=ERROR_BAD_PATHNAME; goto fail;
    }
    if (!ntvdm_shadow_registry_initialize()) {
        error=GetLastError(); goto fail;
    }
    worker_shadow_registry=TRUE;
    error=NtvdmBaseConnectExecution(INFINITE);
    if (error!=ERROR_SUCCESS) goto fail;
    /* Classify only the original startup switch; no guest/global WOW state
     * is changed before original initialization. Retain controls first. */
    for(index=1;index<__argc;++index) {
        const char *argument=__argv[index];
        if((argument[0]=='-' || argument[0]=='/') &&
            tolower((unsigned char)argument[1])=='w') {
            character_io=FALSE;break;
        }
    }
    error=ntvdm_worker_lifecycle_begin(&worker_session,character_io);
    if(error)goto fail;
    if(character_io) {
        error=ntvdm_vm_host_create(&worker_frontends);
        if(error)goto fail;
        error=mvdm_vm_executor_create(worker_frontends,&worker_vm_executor);
        if(error)goto fail;
        mvdm_softpc_exit_rundown=mvdm_vm_executor_rundown(worker_vm_executor);
    }
    if (!session_select_machine_backend(&worker_session,
            SESSION_MACHINE_BACKEND_SOFTPC) || !session_activate(&worker_session) ||
        !session_thread_bind_owned(&worker_session,
            SESSION_THREAD_BINDING_SOFTPC_ENTRY)) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    worker_thread=TRUE;
    error=worker_base_start_character_io(character_io,begin_character_io,&worker_session,NULL);
    worker_character_io=character_io;
    if(error)goto fail;
    if (!wow_user_session_attach(&worker_wow_binding, &worker_session.host_context,
            &worker_wow_runtime)) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    worker_wow_attached=TRUE;
    if (!mvdm_monitor_bind_current_thread()) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    if (!mvdm_softpc_guest_memory_begin(&worker_session)) {
        error=ERROR_NOT_ENOUGH_MEMORY; goto fail;
    }
    worker_memory=TRUE;
    if(worker_character_io)(void)ntvdm_dos_observer_start(&worker_observer,&worker_session);
    if (!session_arm_termination_escape(&worker_session)) {
        error=ERROR_INVALID_STATE; goto fail;
    }
    worker_escape=TRUE;
    worker_started=TRUE;
    if (worker_character_io) mvdm_vm_command_bind(submit_vm_command, worker_vm_executor);
    return ERROR_SUCCESS;
fail:
    error=ntvdm_worker_lifecycle_exit_result(error);
    (void)mvdm_standalone_worker_cleanup((int)error);
    return error;
}

int __cdecl mvdm_softpc_prepare(int *argc, char **argv);
int softpc_run_initialized(void);
LONG VdmUnhandledExceptionFilter(PEXCEPTION_POINTERS exception);

static int run_softpc(int *argc, char **argv)
{
    int result = mvdm_softpc_prepare(argc, argv);
    if (result != 0)
        return result;

    result = -1;
    __try {
        NtvdmActivateLifecycle();
        result = softpc_run_initialized();
    }
    __except (VdmUnhandledExceptionFilter(GetExceptionInformation())) {
    }
    return result;
}

int main(int argc, char **argv)
{
    DWORD error = mvdm_standalone_worker_begin();
    int result;

    if (error != ERROR_SUCCESS)
        return (int)error;

    if (setjmp(worker_session.termination_escape) == 0)
        result = run_softpc(&argc, argv);
    else
        result = (int)worker_session.completion_code;

    result = (int)ntvdm_worker_lifecycle_exit_result((DWORD)result);
    return mvdm_standalone_worker_cleanup(result);
}
