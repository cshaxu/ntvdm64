#ifndef RUNTIME_BOP_SHIM_HOST_HANDLE_MANAGER_H
#define RUNTIME_BOP_SHIM_HOST_HANDLE_MANAGER_H

/*
 * Session-owned host-handle representation for a modern composition.
 *
 * BX-VDM-001 is the registered adapter ABI divergence: the original NT4
 * service composition could place a 32-bit HANDLE in guest register pairs;
 * x86/x64 modern hosts instead use this opaque guest-ID representation.
 *
 * A legacy 16-bit caller may reserve two registers for a 32-bit native handle.
 * A contemporary x64 HANDLE cannot use that representation. This manager is
 * the only adapter-owned representation bridge: guest-visible values are
 * opaque 32-bit IDs, while the native HANDLE remains host-private. Zero is
 * invalid and UINT32_MAX is reserved for a caller-defined sentinel.
 * It does not interpret any guest file-handle or service protocol.
 */

#include "guest_pointer_manager.h"

/* Historical facade only.  It aliases the bx-vdm session's HOST_HANDLE
 * mapping-manager instance; no caller may allocate or embed a second
 * manager. */
typedef runtime_guest_pointer_manager runtime_host_handle_manager;

runtime_host_handle_manager *runtime_host_handle_manager_session(void);

int runtime_host_handle_manager_initialize(
    runtime_host_handle_manager *manager);
int runtime_host_handle_manager_valid(
    const runtime_host_handle_manager *manager);

/* Publishing an already mapped HANDLE is idempotent.  `ownership` applies
 * only to a new mapping; it never upgrades or downgrades an existing entry. */
int runtime_host_handle_manager_publish(
    runtime_host_handle_manager *manager, HANDLE host_handle,
    uint32_t ownership, uint32_t *guest_handle_out, DWORD *error_out);
int runtime_host_handle_manager_lookup_handle(
    const runtime_host_handle_manager *manager, uint32_t guest_handle,
    HANDLE *host_handle_out);
int runtime_host_handle_manager_lookup_guest(
    const runtime_host_handle_manager *manager, HANDLE host_handle,
    uint32_t *guest_handle_out);
int runtime_host_handle_manager_release(
    runtime_host_handle_manager *manager, uint32_t guest_handle,
    DWORD *error_out);
void runtime_host_handle_manager_reset(
    runtime_host_handle_manager *manager);

#endif
