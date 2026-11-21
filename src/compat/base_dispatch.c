#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include "include/base_transport.h"

static const ntvdm_base_transport *provider;
static HANDLE worker_wait;
static ULONGLONG observation_direct;
static ntvdm_base_activate activate_console;
static struct compat_console_activation *activation;

void ntvdm_base_bind_transport(const ntvdm_base_transport *transport)
{
    provider = transport;
    if (!transport) {
        if (worker_wait) CloseHandle(worker_wait);
        worker_wait = NULL;
        observation_direct = 0;
        activate_console = NULL;
        activation = NULL;
    }
}

void ntvdm_base_set_activation(ntvdm_base_activate activate, struct compat_console_activation *context)
{
    activate_console = activate;
    activation = context;
}

ULONGLONG ntvdm_base_observation_direct(void)
{
    return observation_direct;
}

static LONG get_command(const ntvdm_base_transport *transport, ntvdm_base_request *request)
{
    ULONGLONG direct = 0;
    HANDLE wait = NULL;
    ntvdm_base_command_state state;
    LONG status = transport->get(request->payload, &wait, &direct);
    if (wait) {
        /* Get can return a new duplicate of the same ConsoleRecord event. */
        if (worker_wait) CloseHandle(wait);
        else worker_wait = wait;
        *request->wait = worker_wait;
    }
    transport->command_state(request, status, &state);
    if (state.observe) observation_direct = direct;
    if (state.activate && activate_console) {
        DWORD error = activate_console(activation, TRUE);
        if (error) {
            SetLastError(error);
            return STATUS_UNSUCCESSFUL;
        }
    }
    return status;
}

/* Keep the original CSR-shaped entry without importing its private layout. */
struct _CSR_API_MSG;
struct _CSR_CAPTURE_HEADER;
LONG NTAPI NtvdmBaseClientCallServer(struct _CSR_API_MSG *message,
    struct _CSR_CAPTURE_HEADER *capture, ULONG number, ULONG length)
{
    const ntvdm_base_transport *transport = provider;
    ntvdm_base_request request = {0};
    LONG status;
    ULONG value;

    (void)capture;
    if (!message || !transport) return STATUS_UNSUCCESSFUL;
    status = transport->status(ERROR_SUCCESS);
    transport->decode(message, number, (status >= 0) ? length : 0, &request);
    if ((status >= 0)) {
        status = STATUS_UNSUCCESSFUL;
        if (request.error) status = transport->status(request.error);
        else switch (request.operation) {
        case ntvdm_base_check:
            status = transport->check(request.payload); break;
        case ntvdm_base_get:
            status = get_command(transport, &request); break;
        case ntvdm_base_update:
            status = transport->update(request.payload); break;
        case ntvdm_base_exit:
            value = 0;
            status = transport->exit_vdm(request.wow, request.notify_started, request.value, &value);
            if ((status >= 0)) {
                if (value > 1 || (value && !worker_wait)) {
                    status = transport->status(ERROR_INVALID_DATA); break;
                }
                *request.wait = value ? worker_wait : NULL;
                if (value) worker_wait = NULL;
            }
            break;
        case ntvdm_base_code:
            status = transport->exit_code(request.parent, request.exit_code); break;
        case ntvdm_base_reenter:
            status = transport->reenter(request.value); break;
        case ntvdm_base_register:
            status = transport->register_wow(request.value); break;
        case ntvdm_base_first:
            value = 0;
            status = transport->first(&value);
            if ((status >= 0)) *request.first = value;
            break;
        }
    }
    *request.status = status;
    return status;
}
