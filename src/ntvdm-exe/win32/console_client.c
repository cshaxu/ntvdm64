/* Worker-local transport only. No native Console presentation or guest policy. */
#include "console_client.h"
#include "worker_lifecycle.h"
#include "compat/include/console_text.h"
#include "compat/include/console_activation.h"
#include "worker-base/input_watch.h"
#include "compat/include/publication.h"
#include "compat/include/console_endpoint.h"
#include "common/protocol/frontend_protocol.h"
#include "common/base_srv/base_rpc_client.h"
#include "worker-base/connection.h"
#include "compat/include/mvdm_softpc_mouse_bridge.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>

typedef struct console_client {
    session *owner;
    compat_console_endpoint endpoint;
    compat_console_activation activation;
    PTP_WAIT activation_wait;
    HANDLE shutdown;
    worker_base_input_watch *input_watch;
    HANDLE capability;
    mvdm_mouse_bridge mouse;
} console_client;
static void CALLBACK service_activation(PTP_CALLBACK_INSTANCE instance,
    void *context, PTP_WAIT wait, TP_WAIT_RESULT result)
{
    console_client *client = context;
    compat_console_activation *activation = &client->activation;
    ntcon_worker_client *channel = &client->endpoint.channel;
    DWORD decision = WORKER_IO_KEEP;
    DWORD status = ERROR_INVALID_FUNCTION;
    (void)instance;
    (void)result;

    switch (activation->operation) {
    case CONSOLE_ACTIVATION_OPEN:
        status = worker_base_io_open(&channel->pipe, &channel->peer,
            &activation->ready, &channel->generation);
        break;
    case CONSOLE_ACTIVATION_CHECK_PAUSE:
        status = worker_base_io_checkpoint(WORKER_IO_CHECKPOINT_PAUSE, 0, &decision);
        activation->release_pending = decision != WORKER_IO_KEEP;
        break;
    case CONSOLE_ACTIVATION_CLOSE:
        status = worker_base_io_close(&activation->released_pipe,
            &activation->released_peer, &activation->ready);
        break;
    case CONSOLE_ACTIVATION_BIND_INPUT:
        status = worker_base_input_watch_bind(client->input_watch, activation->ready);
        break;
    case CONSOLE_ACTIVATION_UNBIND_INPUT:
        status = worker_base_input_watch_bind(client->input_watch, NULL);
        break;
    }

    activation->status = status;
    SetThreadpoolWait(wait, activation->request, NULL);
    SetEvent(activation->completed);
}

static DWORD start_activation(console_client *client)
{
    compat_console_activation *activation = &client->activation;
    activation->endpoint = &client->endpoint;
    activation->mouse = &client->mouse;
    activation->request = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!activation->request) return GetLastError();
    activation->completed = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!activation->completed) return GetLastError();
    client->activation_wait = CreateThreadpoolWait(service_activation, client, NULL);
    if (!client->activation_wait) return GetLastError();
    SetThreadpoolWait(client->activation_wait, activation->request, NULL);
    return ERROR_SUCCESS;
}
static void console_input_failed(void *context,DWORD error)
{
    console_client *client=context;
    EnterCriticalSection(&client->endpoint.lock);
    (void)ntcon_worker_client_fail(&client->endpoint.channel,error);
    LeaveCriticalSection(&client->endpoint.lock);
}
static void console_client_destroy(console_client *client)
{
    client->owner->host_context.mouse=NULL;
    client->owner->host_context.console_endpoint=NULL;
    client->owner->console_client=NULL;
    OpenNtBaseClientSetCommandBinding(NULL,NULL);
    if (client->activation_wait) {
        SetThreadpoolWait(client->activation_wait, NULL, NULL);
        WaitForThreadpoolWaitCallbacks(client->activation_wait, TRUE);
        CloseThreadpoolWait(client->activation_wait);
    }
    if (client->activation.completed) CloseHandle(client->activation.completed);
    if (client->activation.request) CloseHandle(client->activation.request);
    if (client->activation.stop) SetEvent(client->activation.stop);
    worker_base_publication_destroy(client->endpoint.publisher);
    worker_base_input_watch_destroy(client->input_watch);
    CloseHandle(client->endpoint.channel.pipe);CloseHandle(client->endpoint.channel.peer);
    ntcon_worker_client_dispose(&client->endpoint.channel);
    if(client->activation.failure_event)CloseHandle(client->activation.failure_event);
    CloseHandle(client->activation.ready);
    if (client->capability) CloseHandle(client->capability);
    if (client->activation.stop) CloseHandle(client->activation.stop);
    if (client->shutdown) CloseHandle(client->shutdown);
    if (client->endpoint.input_identity) CloseHandle(client->endpoint.input_identity);
    if (client->endpoint.output_identity) CloseHandle(client->endpoint.output_identity);
    DeleteCriticalSection(&client->endpoint.lock);
    DeleteCriticalSection(&client->endpoint.palette_lock);
    ntvdm_console_graphics_destroy(client->endpoint.graphics);
    HeapFree(GetProcessHeap(),0,client);
}

void ntvdm_console_client_end(session *owner)
{
    if (owner && owner->console_client) console_client_destroy(owner->console_client);
}

static DWORD console_command_ready(void *context)
{
    console_client *client=context;
    return compat_console_activate(&client->activation, TRUE);
}

DWORD ntvdm_console_client_begin(session *owner)
{
    console_client *client;
    DWORD error;
    if (!owner || owner->console_client || owner->host_context.state != SESSION_STATE_ACTIVE)
        return ERROR_INVALID_STATE;
    client=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*client));
    if (!client) return ERROR_NOT_ENOUGH_MEMORY;
    error=worker_base_io_open(&client->endpoint.channel.pipe,&client->endpoint.channel.peer,&client->activation.ready,
        &client->endpoint.channel.generation);
    if (error) { HeapFree(GetProcessHeap(),0,client);return error; }
    client->owner=owner;
    InitializeCriticalSection(&client->endpoint.lock);
    InitializeCriticalSection(&client->endpoint.palette_lock);
    client->endpoint.graphics=ntvdm_console_graphics_create();
    if (!client->endpoint.graphics) { error=GetLastError();console_client_destroy(client);return error; }
    error=OpenNtBaseClientWorkerFrontendCapability(&client->capability);
    if (error) { console_client_destroy(client);return error; }
    /* The authenticated broker grants both transport and shell-out capability.
     * This worker does not resolve or retain a separate frontend relationship. */
    error=ntcon_worker_client_init(&client->endpoint.channel,client->endpoint.channel.pipe,
        client->endpoint.channel.peer,NULL,client->endpoint.channel.generation);
    if(error){console_client_destroy(client);return error;}
    error=ntvdm_worker_lifecycle_fault_event(&client->activation.failure_event);
    if(error){console_client_destroy(client);return error;}
    client->endpoint.channel.failure_event=client->activation.failure_event;
    client->activation.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->endpoint.input_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->endpoint.output_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!client->endpoint.channel.event || !client->activation.stop ||
        !client->endpoint.input_identity || !client->endpoint.output_identity) {
        error=GetLastError();console_client_destroy(client);return error;
    }
    error=worker_base_shutdown_event(&client->shutdown);
    if(error){console_client_destroy(client);return error;}
    client->endpoint.publisher=worker_base_publication_create(compat_console_send_video_copy,&client->endpoint,client->shutdown);
    if(!client->endpoint.publisher){error=GetLastError();console_client_destroy(client);return error;}
    error=ntvdm_console_graphics_attach_publisher(client->endpoint.graphics,client->endpoint.publisher);
    if(error){console_client_destroy(client);return error;}
    client->input_watch=worker_base_input_watch_create(NULL,NULL,NULL,client->activation.stop,
        NULL,NULL,NULL,NULL,console_input_failed,client);
    client->mouse.input_ready=worker_base_input_watch_event(client->input_watch);
    error=client->input_watch ? start_activation(client) : GetLastError();
    if (!error) error=compat_console_activate(&client->activation, TRUE);
    if (error) {
        console_client_destroy(client);return error;
    }
    client->endpoint.input_readiness=worker_base_input_watch_readiness(client->input_watch);
    client->endpoint.activation=&client->activation;
    owner->host_context.mouse=&client->mouse;
    owner->host_context.console_endpoint=&client->endpoint;
    owner->console_client=client;
    OpenNtBaseClientSetCommandBinding(console_command_ready,client);
    return ERROR_SUCCESS;
}

BOOL ntvdm_console_inherit_launch_capabilities(session *owner,HANDLE *capability,HANDLE *execution)
{
    console_client *client=owner ? owner->console_client : NULL;
    BOOL result=TRUE;
    HANDLE retained=NULL;
    DWORD error;
    if (!capability || !execution) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *capability=NULL;*execution=NULL;
    if (!client) return TRUE;
    EnterCriticalSection(&client->endpoint.lock);
    if (!client->capability) { SetLastError(ERROR_INVALID_STATE);result=FALSE; }
    else result=DuplicateHandle(GetCurrentProcess(),client->capability,
        GetCurrentProcess(),capability,SYNCHRONIZE,TRUE,0);
    LeaveCriticalSection(&client->endpoint.lock);
    if (!result) return FALSE;
    /* A worker-created native child must receive this worker's verified
     * execution Console, never an old launcher-local handle from guest env. */
    error=OpenNtBaseClientAcquireConsoleContext(*capability,&retained);
    if (!error && !DuplicateHandle(GetCurrentProcess(),retained,GetCurrentProcess(),
        execution,SYNCHRONIZE,TRUE,0)) error=GetLastError();
    if (retained) CloseHandle(retained);
    if (error) {
        CloseHandle(*capability);*capability=NULL;
        SetLastError(error);return FALSE;
    }
    return result;
}
