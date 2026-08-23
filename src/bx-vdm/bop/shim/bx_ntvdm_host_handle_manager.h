#ifndef BX_NTVDM_BOP_SHIM_HOST_HANDLE_MANAGER_H
#define BX_NTVDM_BOP_SHIM_HOST_HANDLE_MANAGER_H

/*
 * Session-owned host-handle representation for the modern BOP composition.
 *
 * BX-VDM-001 is the registered adapter ABI divergence: the original NT4
 * service composition could place a 32-bit HANDLE in guest register pairs;
 * x86/x64 modern hosts instead use this opaque guest-ID representation.
 *
 * OpenNT's 16-bit callers reserve two registers for a 32-bit NT HANDLE.  A
 * contemporary x64 HANDLE cannot use that representation.  This manager is
 * the only adapter-owned representation bridge: guest-visible values are
 * opaque 32-bit IDs, while the native HANDLE remains host-private. Zero is
 * invalid and UINT32_MAX is reserved for the historical COMMAND default
 * standard-handle sentinel.
 * It is not a DOS JFN/SFT implementation and has no BOP/service knowledge.
 */

#include "bx_ntvdm_guest_pointer_manager.h"

/* Historical facade only.  It aliases the bx-vdm session's HOST_HANDLE
 * mapping-manager instance; no caller may allocate or embed a second
 * manager. */
typedef bx_ntvdm_guest_pointer_manager bx_ntvdm_host_handle_manager;

bx_ntvdm_host_handle_manager *bx_ntvdm_host_handle_manager_session(void);

int bx_ntvdm_host_handle_manager_initialize(
    bx_ntvdm_host_handle_manager *manager);
int bx_ntvdm_host_handle_manager_valid(
    const bx_ntvdm_host_handle_manager *manager);

/* Publishing an already mapped HANDLE is idempotent.  `ownership` applies
 * only to a new mapping; it never upgrades or downgrades an existing entry. */
int bx_ntvdm_host_handle_manager_publish(
    bx_ntvdm_host_handle_manager *manager, HANDLE host_handle,
    uint32_t ownership, uint32_t *guest_handle_out, DWORD *error_out);
int bx_ntvdm_host_handle_manager_lookup_handle(
    const bx_ntvdm_host_handle_manager *manager, uint32_t guest_handle,
    HANDLE *host_handle_out);
int bx_ntvdm_host_handle_manager_lookup_guest(
    const bx_ntvdm_host_handle_manager *manager, HANDLE host_handle,
    uint32_t *guest_handle_out);
int bx_ntvdm_host_handle_manager_release(
    bx_ntvdm_host_handle_manager *manager, uint32_t guest_handle,
    DWORD *error_out);
void bx_ntvdm_host_handle_manager_reset(
    bx_ntvdm_host_handle_manager *manager);

#endif
