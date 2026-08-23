#include <stdio.h>
#include <string.h>

#include "bop/shim/dem_direct_host_session.h"
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
    puts("T251 S3 Redirector: typed selector-57 lifecycle and unavailable pipe route pass");
    return 0;
}
