#include <stdint.h>
#include <string.h>

#include "adapter-softpc/guest_pointer_manager.h"
#include "adapter-softpc/host_handle_manager.h"

typedef struct fixture_guest { uint8_t memory[0x100000u]; } fixture_guest;
static int data_release_count;
static int data_marker;

static void release_data(void *value)
{
    if (value == &data_marker) ++data_release_count;
}

static int read_guest(void *state, uint32_t address, uint8_t *bytes, uint32_t count)
{
    fixture_guest *guest = (fixture_guest *)state;
    if (guest == NULL || bytes == NULL || address > 0x100000u || count > 0x100000u - address)
        return 0;
    memcpy(bytes, guest->memory + address, count);
    return 1;
}

static int write_guest(void *state, uint32_t address, const uint8_t *bytes, uint32_t count)
{
    fixture_guest *guest = (fixture_guest *)state;
    if (guest == NULL || bytes == NULL || address > 0x100000u || count > 0x100000u - address)
        return 0;
    memcpy(guest->memory + address, bytes, count);
    return 1;
}

int main(void)
{
    static fixture_guest guest;
    runtime_guest_pointer_manager *manager;
    runtime_guest_pointer_manager *handles;
    runtime_guest_pointer_manager *data;
    runtime_guest_pointer_manager private_memory_manager;
    runtime_guest_pointer_lease *lease;
    uint8_t *pointer;
    uint32_t data_id;
    void *data_value;
    HANDLE owned_handle;
    uint32_t handle_id;
    DWORD handle_error;
    memset(&guest, 0, sizeof(guest));
    guest.memory[0x12350u] = 0x11u;
    guest.memory[0x12351u] = 0x22u;
    manager = runtime_guest_pointer_manager_session();
    handles = runtime_host_handle_manager_session();
    data = runtime_guest_pointer_manager_session_data();
    runtime_guest_pointer_manager_initialize(manager);
    memset(&private_memory_manager, 0, sizeof(private_memory_manager));
    private_memory_manager.instance_kind = RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY;
    if (runtime_guest_pointer_manager_begin(&private_memory_manager, &guest,
            read_guest, write_guest)) return 12;
    if (manager != runtime_guest_pointer_manager_session() || manager == handles ||
        manager == data || handles == data ||
        manager->instance_kind != RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY ||
        handles->instance_kind != RUNTIME_MAPPING_INSTANCE_HOST_HANDLE ||
        data->instance_kind != RUNTIME_MAPPING_INSTANCE_SESSION_DATA ||
        !runtime_guest_pointer_manager_begin(manager, &guest, read_guest, write_guest) ||
        runtime_guest_pointer_manager_begin(manager, &guest, read_guest, write_guest)) return 1;
    if (!runtime_session_data_publish(data, &data_marker, release_data, &data_id) ||
        data_id != 1u || !runtime_session_data_lookup(data, data_id, &data_value) ||
        data_value != &data_marker || !runtime_session_data_release(data, data_id) ||
        data_release_count != 1 || runtime_session_data_lookup(data, data_id, &data_value)) return 11;
    if (!runtime_guest_pointer_manager_acquire_real_mode(manager, 0x1234u, 0x0010u,
            2u, RUNTIME_GUEST_POINTER_READ | RUNTIME_GUEST_POINTER_WRITE,
            &lease, (void **)&pointer) || pointer[0] != 0x11u || pointer[1] != 0x22u) return 2;
    pointer[0] = 0xaau; pointer[1] = 0xbbu;
    if (!runtime_guest_pointer_manager_release(manager, lease, 1) ||
        guest.memory[0x12350u] != 0xaau || guest.memory[0x12351u] != 0xbbu) return 3;
    if (runtime_guest_pointer_manager_acquire_real_mode(manager, 0xffffu, 0xffffu,
            2u, RUNTIME_GUEST_POINTER_READ, &lease, (void **)&pointer)) return 4;
    if (!runtime_guest_pointer_manager_acquire_real_mode(manager, 0x0000u, 0x0010u,
            1u, RUNTIME_GUEST_POINTER_READ, &lease, (void **)&pointer)) return 5;
    if (!runtime_guest_pointer_manager_release(manager, lease, 0)) return 6;
    if (!runtime_guest_pointer_manager_acquire_linear(manager, 0x80000u, 2u,
            RUNTIME_GUEST_POINTER_READ | RUNTIME_GUEST_POINTER_WRITE,
            &lease, (void **)&pointer)) return 7;
    pointer[0] = 0x5au; pointer[1] = 0xa5u;
    if (!runtime_guest_pointer_manager_release(manager, lease, 1) ||
        guest.memory[0x80000u] != 0x5au || guest.memory[0x80001u] != 0xa5u) return 8;
    if (runtime_guest_pointer_manager_acquire_linear(manager, UINT32_MAX, 2u,
            RUNTIME_GUEST_POINTER_READ, &lease, (void **)&pointer)) return 9;
    runtime_guest_pointer_manager_end(manager);
    if (runtime_guest_pointer_manager_release(manager, lease, 0)) return 10;
    if (!runtime_session_data_publish(data, &data_marker, release_data, &data_id) ||
        data_id != 2u) return 13;
    owned_handle = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (owned_handle == NULL || !runtime_host_handle_manager_publish(handles,
            owned_handle, RUNTIME_HOST_HANDLE_OWNED, &handle_id, &handle_error) ||
        handle_id != 1u || handle_error != ERROR_SUCCESS) return 15;
    runtime_session_mapping_registry_reset();
    if (data_release_count != 2 || runtime_session_data_lookup(data, data_id, &data_value) ||
        WaitForSingleObject(owned_handle, 0u) != WAIT_FAILED)
        return 14;
    return 0;
}
