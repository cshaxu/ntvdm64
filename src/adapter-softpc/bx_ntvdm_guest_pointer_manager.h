#ifndef BX_NTVDM_GUEST_POINTER_MANAGER_H
#define BX_NTVDM_GUEST_POINTER_MANAGER_H

/* Session-scoped SAS-pointer compatibility. Its native pointer is private to
 * an active OpenNT call: never guest state, an ABI field, a HANDLE or a
 * bx-core/bx-mantle object. */
#include <stdint.h>

#include "session/session_v1.h"
#include <windows.h>

#define BX_NTVDM_GUEST_POINTER_MANAGER_MAX_LEASES 8u
#define BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS 257u

/* The three session instances are deliberately typed.  Guest linear
 * addresses are transient mapping keys; opaque guest IDs identify host
 * handles or future bounded session data.  A numeric key never crosses an
 * instance boundary. */
enum bx_ntvdm_session_mapping_instance {
    BX_NTVDM_MAPPING_INSTANCE_GUEST_MEMORY = 1u,
    BX_NTVDM_MAPPING_INSTANCE_HOST_HANDLE = 2u,
    BX_NTVDM_MAPPING_INSTANCE_SESSION_DATA = 3u
};

enum bx_ntvdm_host_handle_ownership {
    BX_NTVDM_HOST_HANDLE_BORROWED = 0u,
    BX_NTVDM_HOST_HANDLE_OWNED = 1u
};

typedef int (*bx_ntvdm_guest_pointer_read_fn)(void *state, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
typedef int (*bx_ntvdm_guest_pointer_write_fn)(void *state, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);
typedef void (*bx_ntvdm_session_data_release_fn)(void *value);
enum bx_ntvdm_guest_pointer_access {
    BX_NTVDM_GUEST_POINTER_READ = 1u,
    BX_NTVDM_GUEST_POINTER_WRITE = 2u
};
typedef struct bx_ntvdm_guest_pointer_lease {
    uint32_t epoch, address, bytes, access, active;
    uint8_t *bounce;
} bx_ntvdm_guest_pointer_lease;
typedef struct bx_ntvdm_host_handle_entry bx_ntvdm_host_handle_entry;
typedef struct bx_ntvdm_guest_pointer_manager {
    uint32_t instance_kind;
    uint32_t epoch, active;
    void *guest_state;
    bx_ntvdm_guest_pointer_read_fn read;
    bx_ntvdm_guest_pointer_write_fn write;
    bx_ntvdm_guest_pointer_lease leases[BX_NTVDM_GUEST_POINTER_MANAGER_MAX_LEASES];
    /* HOST_HANDLE instance storage.  These are session-owned host-private entries;
 * their guest keys are never confused with addresses or SESSION_DATA. */
    uint32_t entry_count;
    uint32_t next_guest_handle;
    bx_ntvdm_host_handle_entry *by_host[BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS];
    bx_ntvdm_host_handle_entry *by_guest[BX_NTVDM_HOST_HANDLE_MANAGER_BUCKETS];
} bx_ntvdm_guest_pointer_manager;

/* One session owns exactly these three instances of the same manager
 * implementation.  Families obtain an accessor; they never embed one. */
typedef struct bx_ntvdm_session_mapping_registry {
    bx_ntvdm_guest_pointer_manager guest_memory_manager;
    bx_ntvdm_guest_pointer_manager host_handle_manager;
    bx_ntvdm_guest_pointer_manager session_data_manager;
} bx_ntvdm_session_mapping_registry;

void bx_ntvdm_guest_pointer_manager_initialize(bx_ntvdm_guest_pointer_manager *manager);
/* The session's GUEST_MEMORY manager instance. Family shims must obtain this
 * accessor rather than embedding or allocating a manager of their own. */
bx_ntvdm_guest_pointer_manager *bx_ntvdm_guest_pointer_manager_session(void);
bx_ntvdm_guest_pointer_manager *bx_ntvdm_guest_pointer_manager_session_host_handle(void);
bx_ntvdm_guest_pointer_manager *bx_ntvdm_guest_pointer_manager_session_data(void);
/* Active session lifecycle only: retire every lease and release the
 * session's host-handle and opaque-data entries.  Family shims must not
 * manufacture a replacement registry or use this to reset another owner
 * mid-call.  `session` owns the registration identity; this component owns
 * only the three mapping-manager implementations. */
void bx_ntvdm_session_mapping_registry_reset(void);
int bx_ntvdm_session_mapping_registry_bind(
    const ntdos64_session_v1 *session);
int bx_ntvdm_guest_pointer_manager_begin(bx_ntvdm_guest_pointer_manager *manager,
    void *guest_state, bx_ntvdm_guest_pointer_read_fn read, bx_ntvdm_guest_pointer_write_fn write);
void bx_ntvdm_guest_pointer_manager_end(bx_ntvdm_guest_pointer_manager *manager);
int bx_ntvdm_guest_pointer_manager_acquire_linear(bx_ntvdm_guest_pointer_manager *manager,
    uint32_t address, uint32_t bytes, uint32_t access,
    bx_ntvdm_guest_pointer_lease **lease_out, void **pointer_out);
int bx_ntvdm_guest_pointer_manager_acquire_real_mode(bx_ntvdm_guest_pointer_manager *manager,
    uint16_t segment, uint16_t offset, uint32_t bytes, uint32_t access,
    bx_ntvdm_guest_pointer_lease **lease_out, void **pointer_out);
int bx_ntvdm_guest_pointer_manager_release(bx_ntvdm_guest_pointer_manager *manager,
    bx_ntvdm_guest_pointer_lease *lease, int write_back);

/* SESSION_DATA is the third fixed instance.  It maps bounded host-private
 * objects to opaque 32-bit guest IDs and never accepts a native pointer as a
 * guest value. */
int bx_ntvdm_session_data_publish(bx_ntvdm_guest_pointer_manager *manager,
    void *value, bx_ntvdm_session_data_release_fn release, uint32_t *guest_id_out);
int bx_ntvdm_session_data_lookup(const bx_ntvdm_guest_pointer_manager *manager,
    uint32_t guest_id, void **value_out);
int bx_ntvdm_session_data_release(bx_ntvdm_guest_pointer_manager *manager,
    uint32_t guest_id);
void bx_ntvdm_session_data_reset(bx_ntvdm_guest_pointer_manager *manager);

#endif
