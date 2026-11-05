#include <stdint.h>
#include <windows.h>

#include "adapter-mvdm-host-out/softpc/host_handle_manager.h"

int main(void)
{
    runtime_host_handle_manager *manager;
    HANDLE owned, borrowed, returned;
    uint32_t owned_id, owned_again, borrowed_id, found_id;
    DWORD error;
    manager = runtime_host_handle_manager_session();
    if (!runtime_host_handle_manager_initialize(manager) ||
        !runtime_host_handle_manager_valid(manager)) return 1;
    owned = CreateEventW(NULL, FALSE, FALSE, NULL);
    borrowed = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (owned == NULL || borrowed == NULL) return 2;
    if (!runtime_host_handle_manager_publish(manager, owned,
            RUNTIME_HOST_HANDLE_OWNED, &owned_id, &error) || owned_id != 1u ||
        error != ERROR_SUCCESS) return 3;
    if (!runtime_host_handle_manager_publish(manager, owned,
            RUNTIME_HOST_HANDLE_BORROWED, &owned_again, &error) ||
        owned_again != owned_id || manager->entry_count != 1u) return 4;
    if (!runtime_host_handle_manager_lookup_handle(manager, owned_id, &returned) ||
        returned != owned || !runtime_host_handle_manager_lookup_guest(manager,
            owned, &found_id) || found_id != owned_id) return 5;
    if (runtime_host_handle_manager_lookup_handle(manager, 0u, &returned) ||
        runtime_host_handle_manager_release(manager, 0u, &error) ||
        runtime_host_handle_manager_lookup_handle(manager, UINT32_MAX, &returned) ||
        runtime_host_handle_manager_release(manager, UINT32_MAX, &error) ||
        error != ERROR_INVALID_HANDLE) return 6;
    if (!runtime_host_handle_manager_publish(manager, borrowed,
            RUNTIME_HOST_HANDLE_BORROWED, &borrowed_id, &error) || borrowed_id != 2u ||
        !runtime_host_handle_manager_release(manager, borrowed_id, &error) ||
        WaitForSingleObject(borrowed, 0u) != WAIT_TIMEOUT) return 7;
    if (!CloseHandle(borrowed)) return 8;
    if (!runtime_host_handle_manager_release(manager, owned_id, &error) ||
        WaitForSingleObject(owned, 0u) != WAIT_FAILED ||
        runtime_host_handle_manager_release(manager, owned_id, &error)) return 9;
    /* Synthetic borrowed values reach the high-word and sentinel boundaries
     * without requiring billions of kernel handles. */
    manager->next_guest_handle = 0x10000u;
    if (!runtime_host_handle_manager_publish(manager, (HANDLE)(uintptr_t)3u,
            RUNTIME_HOST_HANDLE_BORROWED, &found_id, &error) ||
        found_id != 0x10000u) return 10;
    manager->next_guest_handle = UINT32_MAX;
    if (runtime_host_handle_manager_publish(manager, (HANDLE)(uintptr_t)4u,
            RUNTIME_HOST_HANDLE_BORROWED, &found_id, &error) ||
        error != ERROR_TOO_MANY_OPEN_FILES) return 11;
    runtime_host_handle_manager_reset(manager);
    return manager->entry_count == 0u && manager->next_guest_handle == 1u ? 0 : 12;
}
