#include "dem_direct_session.h"

#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

static int valid(const bx_ntvdm_dem_direct_host_session *session)
{
    return session != NULL && session->magic == BX_NTVDM_DEM_DIRECT_HOST_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_DEM_DIRECT_HOST_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        bx_ntvdm_host_handle_manager_valid(session->handles);
}

static int publish(void *state, HANDLE handle, uint32_t *token, DWORD *error)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    uint32_t guest_handle;
    if (token) *token = 0u;
    if (!valid(session) || !bx_ntvdm_host_handle_manager_publish(session->handles,
            handle, BX_NTVDM_HOST_HANDLE_OWNED, &guest_handle, error)) return 0;
    if (token) *token = guest_handle;
    return 1;
}

static int lookup(void *state, uint32_t token, HANDLE *handle)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    if (!valid(session) || token == 0u || token == UINT32_MAX) {
        if (handle) *handle = INVALID_HANDLE_VALUE;
        return 0;
    }
    return bx_ntvdm_host_handle_manager_lookup_handle(session->handles, token,
        handle);
}

static int release(void *state, uint32_t token, DWORD *error)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    if (!valid(session) || token == 0u || token == UINT32_MAX) {
        if (error) *error = ERROR_INVALID_HANDLE;
        return 0;
    }
    return bx_ntvdm_host_handle_manager_release(session->handles, token, error);
}

static int query_attributes(void *state, uint8_t drive, const wchar_t *path,
    DWORD *attributes, DWORD *error)
{
    DWORD value;
    (void)state; (void)drive;
    if (attributes) *attributes = INVALID_FILE_ATTRIBUTES;
    if (error) *error = ERROR_FILE_NOT_FOUND;
    if (path == NULL) return 0;
    value = GetFileAttributesW(path);
    if (value == INVALID_FILE_ATTRIBUTES) { if (error) *error = GetLastError(); return 0; }
    if (attributes) *attributes = value;
    if (error) *error = ERROR_SUCCESS;
    return 1;
}

static int set_attributes(void *state, uint8_t drive, const wchar_t *path,
    DWORD attributes, DWORD *error)
{
    (void)state; (void)drive;
    if (error) *error = ERROR_FILE_NOT_FOUND;
    if (path == NULL || !SetFileAttributesW(path, attributes)) {
        if (error) *error = GetLastError();
        return 0;
    }
    if (error) *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_direct_host_session_guest_read(void *state,
    uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    return valid((bx_ntvdm_dem_direct_host_session *)state) &&
        bx_ntvdm_mantle_checked_ram_read_v1(address, bytes, byte_count);
}

int bx_ntvdm_dem_direct_host_session_guest_write(void *state,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count)
{
    return valid((bx_ntvdm_dem_direct_host_session *)state) &&
        bx_ntvdm_mantle_checked_ram_write_v1(address, bytes, byte_count);
}

int bx_ntvdm_dem_direct_host_session_initialize(
    bx_ntvdm_dem_direct_host_session *session)
{
    if (session == NULL) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_DEM_DIRECT_HOST_SESSION_MAGIC;
    session->abi_version = BX_NTVDM_DEM_DIRECT_HOST_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session->handles = bx_ntvdm_host_handle_manager_session();
    if (!bx_ntvdm_host_handle_manager_initialize(session->handles)) return 0;
    session->context.magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    session->context.abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    session->context.struct_bytes = sizeof(session->context);
    session->context.state = session;
    session->context.publish_handle = publish;
    session->context.lookup_handle = lookup;
    session->context.release_handle = release;
    session->context.query_attributes = query_attributes;
    session->context.set_attributes = set_attributes;
    return valid(session) && bx_ntvdm_dem_direct_context_valid(&session->context);
}

void bx_ntvdm_dem_direct_host_session_reset(
    bx_ntvdm_dem_direct_host_session *session)
{
    if (!valid(session)) return;
    bx_ntvdm_host_handle_manager_reset(session->handles);
}

bx_ntvdm_dem_direct_context *bx_ntvdm_dem_direct_host_session_context(
    bx_ntvdm_dem_direct_host_session *session)
{
    return valid(session) ? &session->context : NULL;
}
