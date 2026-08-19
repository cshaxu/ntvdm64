#include "dem_direct_host_session.h"

#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

static int valid(const bx_ntvdm_dem_direct_host_session *session)
{
    return session != NULL && session->magic == BX_NTVDM_DEM_DIRECT_HOST_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_DEM_DIRECT_HOST_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u;
}

static int publish(void *state, HANDLE handle, uint32_t *token, DWORD *error)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    uint32_t index;
    if (token) *token = 0u;
    if (error) *error = ERROR_TOO_MANY_OPEN_FILES;
    if (!valid(session) || handle == NULL || handle == INVALID_HANDLE_VALUE) return 0;
    for (index = 0u; index < BX_NTVDM_DEM_DIRECT_HOST_SESSION_HANDLES; ++index)
        if (session->handles[index] == NULL) {
            session->handles[index] = handle;
            if (token) *token = index + 1u;
            if (error) *error = ERROR_SUCCESS;
            return 1;
        }
    return 0;
}

static int lookup(void *state, uint32_t token, HANDLE *handle)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    if (handle) *handle = INVALID_HANDLE_VALUE;
    if (!valid(session) || token == 0u || token > BX_NTVDM_DEM_DIRECT_HOST_SESSION_HANDLES ||
        session->handles[token - 1u] == NULL) return 0;
    if (handle) *handle = session->handles[token - 1u];
    return 1;
}

static int release(void *state, uint32_t token, DWORD *error)
{
    bx_ntvdm_dem_direct_host_session *session = state;
    HANDLE handle;
    if (error) *error = ERROR_INVALID_HANDLE;
    if (!lookup(session, token, &handle) || !CloseHandle(handle)) {
        if (error && GetLastError() != ERROR_SUCCESS) *error = GetLastError();
        return 0;
    }
    session->handles[token - 1u] = NULL;
    if (error) *error = ERROR_SUCCESS;
    return 1;
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
    uint32_t index;
    if (!valid(session)) return;
    for (index = 0u; index < BX_NTVDM_DEM_DIRECT_HOST_SESSION_HANDLES; ++index)
        if (session->handles[index] != NULL) {
            CloseHandle(session->handles[index]);
            session->handles[index] = NULL;
        }
}

bx_ntvdm_dem_direct_context *bx_ntvdm_dem_direct_host_session_context(
    bx_ntvdm_dem_direct_host_session *session)
{
    return valid(session) ? &session->context : NULL;
}
