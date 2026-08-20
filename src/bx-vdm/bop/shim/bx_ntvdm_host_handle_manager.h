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
 * opaque nonzero uint16_t IDs, while the native HANDLE remains host-private.
 * It is not a DOS JFN/SFT implementation and has no BOP/service knowledge.
 */

#include <stdint.h>
#include <windows.h>

#define BX_NTVDM_HOST_HANDLE_MANAGER_MAGIC 0x4258484du
#define BX_NTVDM_HOST_HANDLE_MANAGER_VERSION 1u
#define BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS 257u

enum bx_ntvdm_host_handle_ownership {
    BX_NTVDM_HOST_HANDLE_BORROWED = 0u,
    BX_NTVDM_HOST_HANDLE_OWNED = 1u
};

typedef struct bx_ntvdm_host_handle_entry bx_ntvdm_host_handle_entry;

typedef struct bx_ntvdm_host_handle_manager {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t entry_count;
    uint32_t next_guest_handle;
    bx_ntvdm_host_handle_entry *by_host[BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS];
    bx_ntvdm_host_handle_entry *by_guest[BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS];
} bx_ntvdm_host_handle_manager;

int bx_ntvdm_host_handle_manager_initialize(
    bx_ntvdm_host_handle_manager *manager);
int bx_ntvdm_host_handle_manager_valid(
    const bx_ntvdm_host_handle_manager *manager);

/* Publishing an already mapped HANDLE is idempotent.  `ownership` applies
 * only to a new mapping; it never upgrades or downgrades an existing entry. */
int bx_ntvdm_host_handle_manager_publish(
    bx_ntvdm_host_handle_manager *manager, HANDLE host_handle,
    uint32_t ownership, uint16_t *guest_handle_out, DWORD *error_out);
int bx_ntvdm_host_handle_manager_lookup_handle(
    const bx_ntvdm_host_handle_manager *manager, uint16_t guest_handle,
    HANDLE *host_handle_out);
int bx_ntvdm_host_handle_manager_lookup_guest(
    const bx_ntvdm_host_handle_manager *manager, HANDLE host_handle,
    uint16_t *guest_handle_out);
int bx_ntvdm_host_handle_manager_release(
    bx_ntvdm_host_handle_manager *manager, uint16_t guest_handle,
    DWORD *error_out);
void bx_ntvdm_host_handle_manager_reset(
    bx_ntvdm_host_handle_manager *manager);

#endif
