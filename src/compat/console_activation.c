#include "include/console_activation.h"
#include "include/console_endpoint.h"
#include "include/console_geometry.h"
#include "include/host_context.h"
#include "include/mvdm_softpc_mouse_bridge.h"
#include <limits.h>

static DWORD exchange_activation(compat_console_activation *activation,
    compat_console_activation_operation operation)
{
    activation->operation = operation;
    if (!SetEvent(activation->request) ||
        WaitForSingleObject(activation->completed, INFINITE) != WAIT_OBJECT_0)
        return GetLastError();
    return activation->status;
}

DWORD compat_console_activate(compat_console_activation *activation, BOOL active)
{
    compat_console_endpoint *endpoint = activation->endpoint;
    DWORD error=ERROR_SUCCESS;
    EnterCriticalSection(&endpoint->lock);
    if(active && !endpoint->channel.pipe) {
        error=exchange_activation(activation, CONSOLE_ACTIVATION_OPEN);
        if(!error)error=ntcon_worker_client_init(&endpoint->channel,endpoint->channel.pipe,
            endpoint->channel.peer,activation->stop,endpoint->channel.generation);
        if(!error)endpoint->channel.failure_event=activation->failure_event;
    }
    if(!active && endpoint->channel.pipe) {
        console_io_request barrier={0};console_io_reply reply;
        error=exchange_activation(activation, CONSOLE_ACTIVATION_CHECK_PAUSE);
        if(error || !activation->release_pending) {
            LeaveCriticalSection(&endpoint->lock);return error;
        }
        barrier.operation=CONSOLE_IO_BARRIER;
        error=ntcon_worker_call(&endpoint->channel,&barrier,&reply);
        if(!error) {
            activation->released_pipe=endpoint->channel.pipe;
            activation->released_peer=endpoint->channel.peer;
            error=exchange_activation(activation, CONSOLE_ACTIVATION_CLOSE);
            /* A rejected release leaves the usable transport intact. Only
             * after physical closure discard its local operation state. */
            if(!activation->released_pipe) {
                ntcon_worker_client_dispose(&endpoint->channel);
                DWORD disabled=exchange_activation(activation, CONSOLE_ACTIVATION_UNBIND_INPUT);
                if(!error)error=disabled;
            }
        }
    }
    if(!error && active) {
        console_io_reply reply;
        int64_t height;
        ZeroMemory(&endpoint->request,sizeof(endpoint->request));
        endpoint->request.operation=CONSOLE_IO_SCREEN_INFO;
        error=ntcon_worker_call(&endpoint->channel,&endpoint->request,&reply);
        if(!error) {
            height=(int64_t)reply.state.bottom-reply.state.top+1;
            if(height<=0 || height>SHRT_MAX)error=ERROR_INVALID_DATA;
            else error=ntcon_worker_prepare_text(&endpoint->channel,
                (COORD){80,ntvdm_console_return_height((SHORT)height)});
        }
        /* Conversion is part of acquisition, before guest producers resume.
         * A failed conversion must not strand the frontend's active owner. */
        if(error) {
            DWORD release=compat_console_activate(activation,FALSE);
            if(!release)mvdm_softpc_mouse_reset(activation->mouse);
            else (void)ntcon_worker_client_fail(&endpoint->channel,release);
        }
    }
    /* Unlike IRQ cancellation, successful DOS ownership handoff retires the
     * frontend's DOS mouse route and discards its copied relative records.
     * Original nt_block_event_thread has quiesced the event/timer producers
     * before this call. Match that acknowledged retirement locally; the next
     * DOS activation will receive a fresh ENTER. Never reset on failed RPC. */
    if(!error && active)error=exchange_activation(activation, CONSOLE_ACTIVATION_BIND_INPUT);
    if(!error && !active)mvdm_softpc_mouse_reset(activation->mouse);
    LeaveCriticalSection(&endpoint->lock);
    return error;
}
BOOL ntvdm_console_set_active(BOOL active)
{
    compat_host_context *context = compat_host_context_current();
    compat_console_endpoint *endpoint = context ? context->console_endpoint : NULL;
    DWORD error = endpoint && endpoint->activation ?
        compat_console_activate(endpoint->activation, active) : ERROR_SUCCESS;
    SetLastError(error);
    return error == ERROR_SUCCESS;
}
