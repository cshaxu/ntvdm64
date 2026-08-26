#include "dem_host_session.h"

#include "adapter-softpc/mechanical_action.h"

#include <string.h>

static int valid(const runtime_dem_direct_host_session *session)
{
    return session != NULL && session->magic == RUNTIME_DEM_DIRECT_HOST_SESSION_MAGIC &&
        session->abi_version == RUNTIME_DEM_DIRECT_HOST_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        runtime_host_handle_manager_valid(session->handles);
}

static int publish(void *state, HANDLE handle, uint32_t *token, DWORD *error)
{
    runtime_dem_direct_host_session *session = state;
    uint32_t guest_handle;
    if (token) *token = 0u;
    if (!valid(session) || !runtime_host_handle_manager_publish(session->handles,
            handle, RUNTIME_HOST_HANDLE_OWNED, &guest_handle, error)) return 0;
    if (token) *token = guest_handle;
    return 1;
}

static int lookup(void *state, uint32_t token, HANDLE *handle)
{
    runtime_dem_direct_host_session *session = state;
    if (!valid(session) || token == 0u || token == UINT32_MAX) {
        if (handle) *handle = INVALID_HANDLE_VALUE;
        return 0;
    }
    return runtime_host_handle_manager_lookup_handle(session->handles, token,
        handle);
}

static int release(void *state, uint32_t token, DWORD *error)
{
    runtime_dem_direct_host_session *session = state;
    if (!valid(session) || token == 0u || token == UINT32_MAX) {
        if (error) *error = ERROR_INVALID_HANDLE;
        return 0;
    }
    return runtime_host_handle_manager_release(session->handles, token, error);
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

int runtime_dem_direct_host_session_guest_read(void *state,
    uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    return valid((runtime_dem_direct_host_session *)state) &&
        runtime_machine_checked_ram_read(address, bytes, byte_count);
}

int runtime_dem_direct_host_session_guest_write(void *state,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count)
{
    return valid((runtime_dem_direct_host_session *)state) &&
        runtime_machine_checked_ram_write(address, bytes, byte_count);
}

int runtime_dem_direct_host_session_initialize(
    runtime_dem_direct_host_session *session)
{
    if (session == NULL) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_DEM_DIRECT_HOST_SESSION_MAGIC;
    session->abi_version = RUNTIME_DEM_DIRECT_HOST_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session->handles = runtime_host_handle_manager_session();
    if (!runtime_host_handle_manager_initialize(session->handles)) return 0;
    session->context.magic = RUNTIME_DEM_DIRECT_CONTEXT_MAGIC;
    session->context.abi_version = RUNTIME_DEM_DIRECT_CONTEXT_VERSION;
    session->context.struct_bytes = sizeof(session->context);
    session->context.state = session;
    session->context.publish_handle = publish;
    session->context.lookup_handle = lookup;
    session->context.release_handle = release;
    session->context.query_attributes = query_attributes;
    session->context.set_attributes = set_attributes;
    return valid(session) && runtime_dem_direct_context_valid(&session->context);
}

void runtime_dem_direct_host_session_reset(
    runtime_dem_direct_host_session *session)
{
    if (!valid(session)) return;
    runtime_host_handle_manager_reset(session->handles);
}

runtime_dem_direct_context *runtime_dem_direct_host_session_context(
    runtime_dem_direct_host_session *session)
{
    return valid(session) ? &session->context : NULL;
}
