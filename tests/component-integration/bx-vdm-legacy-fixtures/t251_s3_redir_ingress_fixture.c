#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "opennt-bop/ingress/dem_direct_session.h"
#include "adapter-softpc/bx_ntvdm_host_handle_manager.h"
#include "opennt-bop/ingress/redir_native_session.h"
#include "opennt-host/vdmredir/vrnmpipe_compat.h"
#include "opennt-bop/ingress/redir_v2_generic_ud_bridge.h"

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

static void put32(uint8_t *bytes, uint32_t value)
{ bytes[0] = (uint8_t)value; bytes[1] = (uint8_t)(value >> 8); bytes[2] = (uint8_t)(value >> 16); bytes[3] = (uint8_t)(value >> 24); }
static uint16_t get16(const uint8_t *bytes)
{ return (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8)); }

typedef struct mailslot_fixture_state {
    bx_ntvdm_host_handle_manager *handles;
    uint8_t memory[2048];
} mailslot_fixture_state;

static int fixture_publish(void *state, HANDLE handle, uint32_t *token, DWORD *error)
{ return bx_ntvdm_host_handle_manager_publish(((mailslot_fixture_state *)state)->handles, handle, BX_NTVDM_HOST_HANDLE_OWNED, token, error); }
static int fixture_lookup(void *state, uint32_t token, HANDLE *handle)
{ return bx_ntvdm_host_handle_manager_lookup_handle(((mailslot_fixture_state *)state)->handles, token, handle); }
static int fixture_release(void *state, uint32_t token, DWORD *error)
{ return bx_ntvdm_host_handle_manager_release(((mailslot_fixture_state *)state)->handles, token, error); }
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
    state.handles = bx_ntvdm_host_handle_manager_session();
    if (!bx_ntvdm_host_handle_manager_initialize(state.handles)) return 0;
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
    /* `namepipe.asm` passes BP:BX as an opaque 32-bit token and DS:SI as the
     * packed descriptor.  The original `int5c.asm` later consumes 57:26;
     * this fixture proves the source-shaped queue writes results before that
     * guest-owned continuation receives only copied 16:16 values. */
    {
        HANDLE server = INVALID_HANDLE_VALUE, client = INVALID_HANDLE_VALUE;
        DWORD error = ERROR_SUCCESS, wrote = 0u;
        uint32_t token = 0u;
        uint32_t spins;
        server = CreateNamedPipeW(L"\\\\.\\pipe\\ntdos64-t253-async", PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1u, 64u, 64u, 0u, NULL);
        if (server == INVALID_HANDLE_VALUE) return 0;
        client = CreateFileW(L"\\\\.\\pipe\\ntdos64-t253-async", GENERIC_READ, 0u,
            NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (client == INVALID_HANDLE_VALUE ||
            (ConnectNamedPipe(server, NULL) == FALSE && GetLastError() != ERROR_PIPE_CONNECTED) ||
            !direct.publish_handle(direct.state, client, &token, &error) || token == 0u ||
            !WriteFile(server, "go", 2u, &wrote, NULL) || wrote != 2u) {
            if (client != INVALID_HANDLE_VALUE) CloseHandle(client);
            CloseHandle(server); return 0;
        }
        memset(state.memory + 0x300u, 0, 24u);
        put32(state.memory + 0x300u, 0x00100300u); /* bytes -> linear 400 */
        state.memory[0x304u] = 2u;
        put32(state.memory + 0x306u, 0x00100320u); /* buffer -> linear 420 */
        put32(state.memory + 0x30au, 0x00100302u); /* error -> linear 402 */
        put32(state.memory + 0x30eu, 0x00200400u); /* ANR is guest-owned only */
        make_event(&event, 0x23u);
        event.eax = 0x0086u; event.ebx = token & 0xffffu; event.ebp = token >> 16;
        event.ds = 0x0010u; event.esi = 0x0200u;
        if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) || !expect(&outcome, 0, 0u)) {
            CloseHandle(server); return 0;
        }
        for (spins = 0u; spins < 100u; ++spins) {
            make_event(&event, 0x26u);
            if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome)) { CloseHandle(server); return 0; }
            if ((outcome.eflags_values & 1u) != 0u) break;
            Sleep(1u);
        }
        if (spins == 100u || (outcome.eflags_values & 0x40u) != 0u ||
            outcome.gpr16_values[0] != 0u || outcome.gpr16_values[1] != 0x0400u ||
            outcome.gpr16_values[2] != 0x0020u || outcome.gpr16_values[4] != 0x0320u ||
            ((outcome.segment_write_mask & ((1u << 0) | (1u << 3))) != ((1u << 0) | (1u << 3)) ||
             outcome.segment_values[3] != 0x0010u) ||
            memcmp(state.memory + 0x420u, "go", 2u) != 0 ||
            get16(state.memory + 0x400u) != 2u || get16(state.memory + 0x402u) != ERROR_SUCCESS) {
            CloseHandle(server); return 0;
        }
        CloseHandle(server);
    }
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
    bx_ntvdm_host_handle_manager_reset(state.handles);
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
    char pipe_name[] = "\\\\localhost\\PIPE\\ntdos64-t251-s3";

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
        !expect(&outcome, 0, 0u) || !VrInitialized()) return 3;
    server = CreateNamedPipeW(L"\\\\.\\pipe\\ntdos64-t251-s3", PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1u, 64u, 64u, 0u, NULL);
    if (server == INVALID_HANDLE_VALUE) return 4;
    client = CreateFileW(L"\\\\.\\pipe\\ntdos64-t251-s3", GENERIC_READ | GENERIC_WRITE,
        0u, NULL, OPEN_EXISTING, 0u, NULL);
    if (client == INVALID_HANDLE_VALUE ||
        (ConnectNamedPipe(server, NULL) == FALSE && GetLastError() != ERROR_PIPE_CONNECTED) ||
        !VrAddOpenNamedPipeInfo(client, pipe_name) ||
        !VrIsNamedPipeHandle(client) ||
        !VrWriteNamedPipe(client, (LPBYTE)"ok", 2u, &transferred) || transferred != 2u ||
        !ReadFile(server, bytes, 2u, &transferred, NULL) || transferred != 2u ||
        memcmp(bytes, "ok", 2u) != 0 ||
        !WriteFile(server, "go", 2u, &transferred, NULL) || transferred != 2u ||
        !VrReadNamedPipe(client, (LPBYTE)bytes, 2u, &transferred, &error) ||
        transferred != 2u || error != ERROR_SUCCESS || memcmp(bytes, "go", 2u) != 0) return 5;
    if (!VrRemoveOpenNamedPipeInfo(client)) return 5;
    CloseHandle(client); client = INVALID_HANDLE_VALUE;
    CloseHandle(server); server = INVALID_HANDLE_VALUE;
    make_event(&event, 0x20u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 1, ERROR_INVALID_FUNCTION)) return 6;
    make_event(&event, 0x23u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 1, ERROR_INVALID_HANDLE)) return 9;
    make_event(&event, 0x24u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 1, ERROR_INVALID_HANDLE)) return 10;
    make_event(&event, 0x01u);
    if (!bx_ntvdm_redir_v2_generic_ud_dispatch(&event, &outcome) ||
        !expect(&outcome, 0, 0u) || VrInitialized()) return 7;
    bx_ntvdm_redir_native_session_unbind(&session);
    bx_ntvdm_dem_direct_host_session_reset(&host);
    if (!mailslot_regression()) return 8;
    puts("T251 S4 Redirector: typed selector-57 lifecycle and mailslot owner group pass");
    return 0;
}
