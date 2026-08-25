#ifndef RUNTIME_GUEST_POINTER_MANAGER_H
#define RUNTIME_GUEST_POINTER_MANAGER_H

/* Session-scoped SAS-pointer compatibility. Its native pointer is private to
 * an active OpenNT call: never guest state, an ABI field, a HANDLE or a
 * bochs-core/adapter-bochs object. */
#include <stdint.h>

#include "session/session.h"
#include <windows.h>

#define RUNTIME_GUEST_POINTER_MANAGER_MAX_LEASES 8u
#define RUNTIME_HOST_HANDLE_MANAGER_BUCKETS 257u

/* The three session instances are deliberately typed.  Guest linear
 * addresses are transient mapping keys; opaque guest IDs identify host
 * handles or future bounded session data.  A numeric key never crosses an
 * instance boundary. */
enum runtime_session_mapping_instance {
    RUNTIME_MAPPING_INSTANCE_GUEST_MEMORY = 1u,
    RUNTIME_MAPPING_INSTANCE_HOST_HANDLE = 2u,
    RUNTIME_MAPPING_INSTANCE_SESSION_DATA = 3u
};

enum runtime_host_handle_ownership {
    RUNTIME_HOST_HANDLE_BORROWED = 0u,
    RUNTIME_HOST_HANDLE_OWNED = 1u
};

typedef int (*runtime_guest_pointer_read_fn)(void *state, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
typedef int (*runtime_guest_pointer_write_fn)(void *state, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);
typedef void (*runtime_session_data_release_fn)(void *value);
enum runtime_guest_pointer_access {
    RUNTIME_GUEST_POINTER_READ = 1u,
    RUNTIME_GUEST_POINTER_WRITE = 2u
};
typedef struct runtime_guest_pointer_lease {
    uint32_t epoch, address, bytes, access, active;
    uint8_t *bounce;
} runtime_guest_pointer_lease;
typedef struct runtime_host_handle_entry runtime_host_handle_entry;
typedef struct runtime_guest_pointer_manager {
    uint32_t instance_kind;
    uint32_t epoch, active;
    void *guest_state;
    runtime_guest_pointer_read_fn read;
    runtime_guest_pointer_write_fn write;
    runtime_guest_pointer_lease leases[RUNTIME_GUEST_POINTER_MANAGER_MAX_LEASES];
    /* HOST_HANDLE instance storage.  These are session-owned host-private entries;
 * their guest keys are never confused with addresses or SESSION_DATA. */
    uint32_t entry_count;
    uint32_t next_guest_handle;
    runtime_host_handle_entry *by_host[RUNTIME_HOST_HANDLE_MANAGER_BUCKETS];
    runtime_host_handle_entry *by_guest[RUNTIME_HOST_HANDLE_MANAGER_BUCKETS];
} runtime_guest_pointer_manager;

/* One session owns exactly these three instances of the same manager
 * implementation.  Families obtain an accessor; they never embed one. */
typedef struct runtime_session_mapping_registry {
    runtime_guest_pointer_manager guest_memory_manager;
    runtime_guest_pointer_manager host_handle_manager;
    runtime_guest_pointer_manager session_data_manager;
} runtime_session_mapping_registry;

void runtime_guest_pointer_manager_initialize(runtime_guest_pointer_manager *manager);
/* The session's GUEST_MEMORY manager instance. Family shims must obtain this
 * accessor rather than embedding or allocating a manager of their own. */
runtime_guest_pointer_manager *runtime_guest_pointer_manager_session(void);
runtime_guest_pointer_manager *runtime_guest_pointer_manager_session_host_handle(void);
runtime_guest_pointer_manager *runtime_guest_pointer_manager_session_data(void);
/* Active session lifecycle only: retire every lease and release the
 * session's host-handle and opaque-data entries.  Family shims must not
 * manufacture a replacement registry or use this to reset another owner
 * mid-call.  `session` owns the registration identity; this component owns
 * only the three mapping-manager implementations. */
void runtime_session_mapping_registry_reset(void);
int runtime_session_mapping_registry_bind(
    const app_session *session);
int runtime_guest_pointer_manager_begin(runtime_guest_pointer_manager *manager,
    void *guest_state, runtime_guest_pointer_read_fn read, runtime_guest_pointer_write_fn write);
void runtime_guest_pointer_manager_end(runtime_guest_pointer_manager *manager);
int runtime_guest_pointer_manager_acquire_linear(runtime_guest_pointer_manager *manager,
    uint32_t address, uint32_t bytes, uint32_t access,
    runtime_guest_pointer_lease **lease_out, void **pointer_out);
int runtime_guest_pointer_manager_acquire_real_mode(runtime_guest_pointer_manager *manager,
    uint16_t segment, uint16_t offset, uint32_t bytes, uint32_t access,
    runtime_guest_pointer_lease **lease_out, void **pointer_out);
int runtime_guest_pointer_manager_release(runtime_guest_pointer_manager *manager,
    runtime_guest_pointer_lease *lease, int write_back);

/* SESSION_DATA is the third fixed instance.  It maps bounded host-private
 * objects to opaque 32-bit guest IDs and never accepts a native pointer as a
 * guest value. */
int runtime_session_data_publish(runtime_guest_pointer_manager *manager,
    void *value, runtime_session_data_release_fn release, uint32_t *guest_id_out);
int runtime_session_data_lookup(const runtime_guest_pointer_manager *manager,
    uint32_t guest_id, void **value_out);
int runtime_session_data_release(runtime_guest_pointer_manager *manager,
    uint32_t guest_id);
void runtime_session_data_reset(runtime_guest_pointer_manager *manager);

#endif
