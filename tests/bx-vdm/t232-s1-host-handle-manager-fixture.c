#include <stdint.h>
#include <windows.h>

#include "bop/shim/bx_ntvdm_host_handle_manager.h"

int main(void)
{
    bx_ntvdm_host_handle_manager manager;
    HANDLE owned, borrowed, returned;
    uint32_t owned_id, owned_again, borrowed_id, found_id;
    DWORD error;
    if (!bx_ntvdm_host_handle_manager_initialize(&manager) ||
        !bx_ntvdm_host_handle_manager_valid(&manager)) return 1;
    owned = CreateEventW(NULL, FALSE, FALSE, NULL);
    borrowed = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (owned == NULL || borrowed == NULL) return 2;
    if (!bx_ntvdm_host_handle_manager_publish(&manager, owned,
            BX_NTVDM_HOST_HANDLE_OWNED, &owned_id, &error) || owned_id != 1u ||
        error != ERROR_SUCCESS) return 3;
    if (!bx_ntvdm_host_handle_manager_publish(&manager, owned,
            BX_NTVDM_HOST_HANDLE_BORROWED, &owned_again, &error) ||
        owned_again != owned_id || manager.entry_count != 1u) return 4;
    if (!bx_ntvdm_host_handle_manager_lookup_handle(&manager, owned_id, &returned) ||
        returned != owned || !bx_ntvdm_host_handle_manager_lookup_guest(&manager,
            owned, &found_id) || found_id != owned_id) return 5;
    if (bx_ntvdm_host_handle_manager_lookup_handle(&manager, 0u, &returned) ||
        bx_ntvdm_host_handle_manager_release(&manager, 0u, &error) ||
        bx_ntvdm_host_handle_manager_lookup_handle(&manager, UINT32_MAX, &returned) ||
        bx_ntvdm_host_handle_manager_release(&manager, UINT32_MAX, &error) ||
        error != ERROR_INVALID_HANDLE) return 6;
    if (!bx_ntvdm_host_handle_manager_publish(&manager, borrowed,
            BX_NTVDM_HOST_HANDLE_BORROWED, &borrowed_id, &error) || borrowed_id != 2u ||
        !bx_ntvdm_host_handle_manager_release(&manager, borrowed_id, &error) ||
        WaitForSingleObject(borrowed, 0u) != WAIT_TIMEOUT) return 7;
    if (!CloseHandle(borrowed)) return 8;
    if (!bx_ntvdm_host_handle_manager_release(&manager, owned_id, &error) ||
        WaitForSingleObject(owned, 0u) != WAIT_FAILED ||
        bx_ntvdm_host_handle_manager_release(&manager, owned_id, &error)) return 9;
    /* Synthetic borrowed values reach the high-word and sentinel boundaries
     * without requiring billions of kernel handles. */
    manager.next_guest_handle = 0x10000u;
    if (!bx_ntvdm_host_handle_manager_publish(&manager, (HANDLE)(uintptr_t)3u,
            BX_NTVDM_HOST_HANDLE_BORROWED, &found_id, &error) ||
        found_id != 0x10000u) return 10;
    manager.next_guest_handle = UINT32_MAX;
    if (bx_ntvdm_host_handle_manager_publish(&manager, (HANDLE)(uintptr_t)4u,
            BX_NTVDM_HOST_HANDLE_BORROWED, &found_id, &error) ||
        error != ERROR_TOO_MANY_OPEN_FILES) return 11;
    bx_ntvdm_host_handle_manager_reset(&manager);
    return manager.entry_count == 0u && manager.next_guest_handle == 1u ? 0 : 12;
}
