#include "guest_pointer_manager.h"
#include "host_handle_manager.h"

#include <stdlib.h>
#include <string.h>

static runtime_session_mapping_registry g_session_registry;
static int g_session_registry_ready;
static uint32_t g_session_identity;

static void runtime_guest_pointer_manager_configure(
    runtime_guest_pointer_manager *manager, uint32_t instance_kind)
{
    memset(manager, 0, sizeof(*manager));
    manager->instance_kind = instance_kind;
    manager->next_guest_handle = 1u;
}

static void runtime_guest_pointer_manager_ensure_session(void)
{
    if (g_session_registry_ready) return;
    runtime_guest_pointer_manager_configure(&g_session_registry.guest_memory_manager,
        RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY);
    runtime_guest_pointer_manager_configure(&g_session_registry.host_handle_manager,
        RUNTIME_MAPPING_INSTANCE_HOST_HANDLE);
    runtime_guest_pointer_manager_configure(&g_session_registry.session_data_manager,
        RUNTIME_MAPPING_INSTANCE_SESSION_DATA);
    g_session_registry_ready = 1;
}

static int runtime_guest_pointer_manager_memory_valid(
    const runtime_guest_pointer_manager *manager)
{
    return manager == runtime_guest_pointer_manager_session() &&
        manager->instance_kind == RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY;
}

void runtime_guest_pointer_manager_initialize(runtime_guest_pointer_manager *manager)
{
    if (manager == NULL || manager != runtime_guest_pointer_manager_session()) return;
    runtime_guest_pointer_manager_configure(manager,
        RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY);
}

runtime_guest_pointer_manager *runtime_guest_pointer_manager_session(void)
{
    runtime_guest_pointer_manager_ensure_session();
    return &g_session_registry.guest_memory_manager;
}

runtime_guest_pointer_manager *runtime_guest_pointer_manager_session_host_handle(void)
{
    runtime_guest_pointer_manager_ensure_session();
    return &g_session_registry.host_handle_manager;
}

runtime_guest_pointer_manager *runtime_guest_pointer_manager_session_data(void)
{
    runtime_guest_pointer_manager_ensure_session();
    return &g_session_registry.session_data_manager;
}

void runtime_session_mapping_registry_reset(void)
{
    runtime_guest_pointer_manager_ensure_session();
    runtime_guest_pointer_manager_end(&g_session_registry.guest_memory_manager);
    runtime_host_handle_manager_reset(&g_session_registry.host_handle_manager);
    runtime_session_data_reset(&g_session_registry.session_data_manager);
    g_session_identity = 0u;
}

int runtime_session_mapping_registry_bind(
    const app_session *session)
{
    if (!app_session_valid(session) ||
        session->state != APP_SESSION_ACTIVE || session->identity == 0u)
        return 0;
    runtime_guest_pointer_manager_ensure_session();
    if (g_session_identity != 0u &&
        g_session_identity != session->identity) return 0;
    g_session_identity = session->identity;
    return 1;
}

int runtime_guest_pointer_manager_begin(runtime_guest_pointer_manager *manager,
    void *guest_state, runtime_guest_pointer_read_fn read, runtime_guest_pointer_write_fn write)
{
    if (!runtime_guest_pointer_manager_memory_valid(manager) || manager->active ||
        read == NULL || write == NULL) return 0;
    if (++manager->epoch == 0u) manager->epoch = 1u;
    manager->guest_state = guest_state; manager->read = read; manager->write = write;
    manager->active = 1u;
    return 1;
}

void runtime_guest_pointer_manager_end(runtime_guest_pointer_manager *manager)
{
    uint32_t index;
    if (!runtime_guest_pointer_manager_memory_valid(manager)) return;
    for (index = 0u; index < RUNTIME_GUEST_POINTER_MANAGER_MAX_LEASES; ++index) {
        free(manager->leases[index].bounce);
        memset(&manager->leases[index], 0, sizeof(manager->leases[index]));
    }
    manager->guest_state = NULL; manager->read = NULL; manager->write = NULL; manager->active = 0u;
}

int runtime_guest_pointer_manager_acquire_linear(runtime_guest_pointer_manager *manager,
    uint32_t address, uint32_t bytes, uint32_t access,
    runtime_guest_pointer_lease **lease_out, void **pointer_out)
{
    uint32_t index;
    runtime_guest_pointer_lease *lease = NULL;
    if (lease_out == NULL || pointer_out == NULL ||
        !runtime_guest_pointer_manager_memory_valid(manager) || !manager->active ||
        manager->read == NULL || manager->write == NULL || access == 0u ||
        (access & ~(RUNTIME_GUEST_POINTER_READ | RUNTIME_GUEST_POINTER_WRITE)) != 0u ||
        bytes > UINT32_MAX - address) return 0;
    for (index = 0u; index < RUNTIME_GUEST_POINTER_MANAGER_MAX_LEASES; ++index)
        if (!manager->leases[index].active) { lease = &manager->leases[index]; break; }
    if (lease == NULL || (lease->bounce = (uint8_t *)malloc(bytes == 0u ? 1u : bytes)) == NULL)
        return 0;
    /* A write lease is seeded too: SAS left unrelated guest bytes intact after a
     * short host operation. Direct mapping needs a separately admitted stable-RAM seam. */
    if (bytes != 0u && !manager->read(manager->guest_state, address, lease->bounce, bytes)) {
        free(lease->bounce); memset(lease, 0, sizeof(*lease)); return 0;
    }
    lease->epoch = manager->epoch; lease->address = address; lease->bytes = bytes;
    lease->access = access; lease->active = 1u;
    *lease_out = lease; *pointer_out = lease->bounce;
    return 1;
}

int runtime_guest_pointer_manager_acquire_real_mode(runtime_guest_pointer_manager *manager,
    uint16_t segment, uint16_t offset, uint32_t bytes, uint32_t access,
    runtime_guest_pointer_lease **lease_out, void **pointer_out)
{
    uint32_t address = ((uint32_t)segment << 4) + (uint32_t)offset;
    if (address > 0x100000u || bytes > 0x100000u - address) return 0;
    return runtime_guest_pointer_manager_acquire_linear(manager, address, bytes,
        access, lease_out, pointer_out);
}

int runtime_guest_pointer_manager_release(runtime_guest_pointer_manager *manager,
    runtime_guest_pointer_lease *lease, int write_back)
{
    int result;
    if (!runtime_guest_pointer_manager_memory_valid(manager) || lease == NULL ||
        !manager->active || !lease->active ||
        lease->epoch != manager->epoch) return 0;
    result = !write_back || (lease->access & RUNTIME_GUEST_POINTER_WRITE) == 0u ||
        lease->bytes == 0u || manager->write(manager->guest_state, lease->address,
            lease->bounce, lease->bytes);
    free(lease->bounce); memset(lease, 0, sizeof(*lease));
    return result;
}
