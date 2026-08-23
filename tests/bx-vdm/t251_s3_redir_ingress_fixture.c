#include <stdio.h>
#include <string.h>

#include "bop/shim/dem_direct_host_session.h"
#include "bop/shim/bx_ntvdm_host_handle_manager.h"
#include "bop/shim/redir_session_shim.h"
#include "bop/redir_v2_generic_ud_bridge.h"

static void make_event(struct bx_ntvdm_generic_ud_event_v1 *event, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = 0x2400u;
    event->eip = 0x2400u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x57u;
    event->window[3] = service;
}

static int expect(struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int carry,
    uint16_t ax)
{
    return outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
        outcome->resume_rip == 0x2404u &&
        ((outcome->eflags_values & 1u) != 0u) == carry &&
        (outcome->gpr16_write_mask & 1u) != 0u &&
        outcome->gpr16_values[0] == ax;
}

typedef struct mailslot_fixture_state {
    bx_ntvdm_host_handle_manager handles;
    uint8_t memory[2048];
} mailslot_fixture_state;

static int fixture_publish(void *state, HANDLE handle, uint32_t *token, DWORD *error)
{ return bx_ntvdm_host_handle_manager_publish(&((mailslot_fixture_state *)state)->handles, handle, BX_NTVDM_HOST_HANDLE_OWNED, token, error); }
static int fixture_lookup(void *state, uint32_t token, HANDLE *handle)
{ return bx_ntvdm_host_handle_manager_lookup_handle(&((mailslot_fixture_state *)state)->handles, token, handle); }
static int fixture_release(void *state, uint32_t token, DWORD *error)
{ return bx_ntvdm_host_handle_manager_release(&((mailslot_fixture_state *)state)->handles, token, error); }
static int fixture_attr(void *state, uint8_t drive, const wchar_t *path, DWORD *value, DWORD *error)
{ (void)state; (void)drive; (void)path; if (value) *value = 0u; if (error) *error = ERROR_FILE_NOT_FOUND; return 0; }
static int fixture_set_attr(void *state, uint8_t drive, const wchar_t *path, DWORD value, DWORD *error)
{ (void)state; (void)drive; (void)path; (void)value; if (error) *error = ERROR_FILE_NOT_FOUND; return 0; }
static int fixture_read(void *state, uint32_t address, uint8_t *bytes, uint32_t count)
{ mailslot_fixture_state *s = state; if (s == NULL || address > sizeof(s->memory) || count > sizeof(s->memory) - address) return 0; memcpy(bytes, s->memory + address, count); return 1; }
static int fixture_write(void *state, uint32_t address, const uint8_t *bytes, uint32_t count)
{ mailslot_fixture_state *s = state; if (s == NULL || address > sizeof(s->memory) || count > sizeof(s->memory) - address) return 0; memcpy(s->memory + address, bytes, count); return 1; }

static int mailslot_regression(void)
{
    mailslot_fixture_state state;
    bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_redir_native_session session;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    const char name[] = "\\MAILSLOT\\ntdos64-t251-s4";
    static const uint8_t message[] = { 'b', 'x', '-', 'v', 'd', 'm' };
    memset(&state, 0, sizeof(state));
    if (!bx_ntvdm_host_handle_manager_initialize(&state.handles)) return 0;
    memcpy(state.memory + 0x100u, name, sizeof(name));
    memset(&direct, 0, sizeof(direct));
    direct.magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    direct.struct_bytes = sizeof(direct); direct.state = &state;
    direct.publish_handle = fixture_publish; direct.lookup_handle = fixture_lookup;
    direct.release_handle = fixture_release; direct.query_attributes = fixture_attr;
    direct.set_attributes = fixture_set_attr;
    if (!bx_ntvdm_redir_native_session_initialize(&session, &direct, &state,
            fixture_read, fixture_write) || !bx_ntvdm_redir_native_session_bind(&session)) return 0;
    make_event(&event, 0x00u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 0u)) return 0;
    make_event(&event, 0x0bu);
    event.eax = 0x0042u; event.ebx = 64u; event.ecx = 64u;
    event.ds = 0x0010u; event.esi = 0u; event.es = 0x0020u; event.edi = 0x0004u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 1u)) return 0;
    make_event(&event, 0x0au); event.ebx = 1u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 64u)) return 0;
    /* DosWriteMailslotStruct = timeout dword + 16:16 source buffer. */
    state.memory[0x200u] = 0u; state.memory[0x201u] = 0u;
    state.memory[0x202u] = 0u; state.memory[0x203u] = 0u;
    state.memory[0x204u] = 0u; state.memory[0x205u] = 2u; /* offset */
    state.memory[0x206u] = 0x10u; state.memory[0x207u] = 0u; /* segment */
    memcpy(state.memory + 0x300u, message, sizeof(message));
    make_event(&event, 0x0eu); event.ds = 0x0010u; event.esi = 0u;
    event.es = 0x0020u; event.edi = 0u; event.ecx = sizeof(message);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 0u)) return 0;
    make_event(&event, 0x0cu); event.ebx = 1u; event.es = 0x0010u; event.edi = 0x0400u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, sizeof(message)) ||
        memcmp(state.memory + 0x500u, message, sizeof(message)) != 0) return 0;
    make_event(&event, 0x0du); event.ebx = 1u; event.es = 0x0010u; event.edi = 0x0420u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, sizeof(message)) ||
        memcmp(state.memory + 0x520u, message, sizeof(message)) != 0) return 0;
    make_event(&event, 0x09u); event.eax = 0x0042u; event.ebx = 1u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 0u)) return 0;
    make_event(&event, 0x0bu);
    event.eax = 0x0042u; event.ebx = 64u; event.ecx = 64u;
    event.ds = 0x0010u; event.esi = 0u; event.es = 0x0020u; event.edi = 4u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 2u)) return 0;
    make_event(&event, 0x0fu); event.eax = 0x0042u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 0u)) return 0;
    make_event(&event, 0x0au); event.ebx = 2u;
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        (outcome.eflags_values & 1u) == 0u || outcome.gpr16_values[0] != ERROR_INVALID_HANDLE) return 0;
    bx_ntvdm_redir_native_session_unbind(&session);
    bx_ntvdm_host_handle_manager_reset(&state.handles);
    return 1;
}

int main(void)
{
    bx_ntvdm_dem_direct_host_session host;
    bx_ntvdm_redir_native_session session;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    HANDLE server = INVALID_HANDLE_VALUE, client = INVALID_HANDLE_VALUE;
    DWORD transferred = 0u, error = 0u;
    char bytes[8] = {0};

    if (!bx_ntvdm_dem_direct_host_session_initialize(&host) ||
        !bx_ntvdm_redir_native_session_initialize(&session,
            bx_ntvdm_dem_direct_host_session_context(&host), &host,
            bx_ntvdm_dem_direct_host_session_guest_read,
            bx_ntvdm_dem_direct_host_session_guest_write) ||
        !bx_ntvdm_redir_native_session_bind(&session)) return 1;
    make_event(&event, 0x02u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 1, ERROR_INVALID_FUNCTION)) return 2;
    make_event(&event, 0x00u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 0, 0u) || !bx_ntvdm_redir_loaded()) return 3;
    server = CreateNamedPipeW(L"\\\\.\\pipe\\ntdos64-t251-s3", PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1u, 64u, 64u, 0u, NULL);
    if (server == INVALID_HANDLE_VALUE) return 4;
    client = CreateFileW(L"\\\\.\\pipe\\ntdos64-t251-s3", GENERIC_READ | GENERIC_WRITE,
        0u, NULL, OPEN_EXISTING, 0u, NULL);
    if (client == INVALID_HANDLE_VALUE ||
        (ConnectNamedPipe(server, NULL) == FALSE && GetLastError() != ERROR_PIPE_CONNECTED) ||
        !bx_ntvdm_redir_is_named_pipe_handle(client) ||
        !bx_ntvdm_redir_write_named_pipe(client, "ok", 2u, &transferred) || transferred != 2u ||
        !ReadFile(server, bytes, 2u, &transferred, NULL) || transferred != 2u ||
        memcmp(bytes, "ok", 2u) != 0 ||
        !WriteFile(server, "go", 2u, &transferred, NULL) || transferred != 2u ||
        !bx_ntvdm_redir_read_named_pipe(client, bytes, 2u, &transferred, &error) ||
        transferred != 2u || error != ERROR_SUCCESS || memcmp(bytes, "go", 2u) != 0) return 5;
    CloseHandle(client); client = INVALID_HANDLE_VALUE;
    CloseHandle(server); server = INVALID_HANDLE_VALUE;
    make_event(&event, 0x20u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 1, ERROR_INVALID_FUNCTION)) return 6;
    make_event(&event, 0x01u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 0, 0u) || bx_ntvdm_redir_loaded()) return 7;
    bx_ntvdm_redir_native_session_unbind(&session);
    bx_ntvdm_dem_direct_host_session_reset(&host);
    if (!mailslot_regression()) return 8;
    puts("T251 S4 Redirector: typed selector-57 lifecycle and mailslot owner group pass");
    return 0;
}
