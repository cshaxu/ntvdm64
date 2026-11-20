#include "include/mvdm_softpc_worker_exit.h"
#include "include/mvdm_softpc_event_thread.h"
#include "include/mvdm_vm_command_entry.h"
#include "include/mvdm_vm_continuation_store.h"

mvdm_vm_rundown *mvdm_softpc_exit_rundown;

void ntvdm_worker_lifecycle_authorize_exit(void)
{
    mvdm_softpc_exit_permission permission = mvdm_softpc_wait_for_exit();
    DWORD error, reply;

    if (permission == MVDM_EXIT_UNMANAGED) return;
    if (permission == MVDM_EXIT_CONSOLE_GRANTED && mvdm_softpc_exit_original_ready) {
        error = mvdm_softpc_exchange_exit(MVDM_EXIT_CONSOLE_CONTROL, &reply);
        if (!error && !reply) WaitForSingleObject(GetCurrentProcess(), INFINITE);
        ExitProcess(error ? error : ERROR_INVALID_STATE);
    }
    if (permission == MVDM_EXIT_SERVICE_LOST) {
        error = mvdm_softpc_exchange_exit(MVDM_EXIT_STATUS_ON_LOSS, &reply);
        if (error) ExitProcess(error);
    }

    /* Close admission and retire guest wake/application state on the original
     * cleanup thread. These local operations use the recursive ICA lock.
     * Before original activation no guest callback can have registered. */
    if (mvdm_softpc_exit_rundown) {
        if (mvdm_softpc_exit_original_ready) mvdm_vm_ingress_reset();
        error = mvdm_vm_rundown_close(mvdm_softpc_exit_rundown,
            mvdm_softpc_exit_original_ready);
        if (error) ExitProcess(error);
    }

    /* All command producers have joined. The worker can now close its native
     * frontend objects without taking ICA or running original cleanup. */
    error = mvdm_softpc_exchange_exit(MVDM_EXIT_CLOSE_FRONTENDS, &reply);
    if (error || reply) ExitProcess(error ? error : reply);
}
