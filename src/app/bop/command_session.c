#include "command_session.h"

#include "adapter-bop/frame_transaction.h"
void MS_bop_4(void);

#include <string.h>

static runtime_command_native_session *g_active_session;

static int session_valid(const runtime_command_native_session *session)
{
    return session != NULL &&
        session->magic == RUNTIME_COMMAND_NATIVE_SESSION_MAGIC &&
        session->abi_version == RUNTIME_COMMAND_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        runtime_command_misc_session_valid(&session->direct);
}

int runtime_command_native_session_initialize(
    runtime_command_native_session *session)
{
    if (session == NULL) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_COMMAND_NATIVE_SESSION_MAGIC;
    session->abi_version = RUNTIME_COMMAND_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    runtime_command_misc_session_initialize(&session->direct);
    return session_valid(session) && CMDInit(0, NULL);
}

int runtime_command_native_session_bind(
    runtime_command_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u ||
        g_active_session != NULL) return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

void runtime_command_native_session_unbind(
    runtime_command_native_session *session)
{
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) {
        runtime_host_handle_manager_reset(session->direct.handles);
        session->bound = 0u;
    }
}

static int event_valid(const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= RUNTIME_GENERIC_UD_WINDOW_BYTES;
}

int runtime_command_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    runtime_bop_frame_transaction frame;
    runtime_command_misc_call call;
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        !event_valid(event) || outcome == NULL) return 0;
    if (!runtime_bop_frame_transaction_begin(event, &frame)) return 0;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = RUNTIME_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    if (event->window_bytes < 4u || event->window[0] != 0xc4u ||
        event->window[1] != 0xc4u || event->window[2] != 0x54u ||
        event->eip > UINT64_MAX - 3u) return 0;
    /* MS_bop_4 reads the service byte at the staged CS:IP. This value only
     * describes the bounded callback/post-body record. */
    call.service = event->window[3];
    call.boundary = &frame.boundary; call.cpu = &frame.cpu; call.result = &frame.result;
    call.guest_state = &frame;
    call.guest_read = runtime_bop_frame_transaction_guest_read;
    call.guest_write = runtime_bop_frame_transaction_guest_write;
    call.session = &g_active_session->direct;
    call.first_call = 1u;
    frame.cpu.eip += 3u;
    return runtime_command_misc_invoke_body(&call, MS_bop_4) &&
        runtime_bop_frame_transaction_complete(&frame, outcome);
}
