#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#include "mapping_manager.h"
#include "guest_memory_lease.h"

#define SESSION_MAGIC UINT32_C(0x53455353)
#define SESSION_ABI_VERSION UINT32_C(1)
#define SESSION_MAX_TEARDOWNS 8u

enum session_state {
    SESSION_STATE_READY = 0u,
    SESSION_STATE_ACTIVE = 1u,
    SESSION_STATE_CANCELLED = 2u,
    SESSION_STATE_COMPLETED = 3u
};

enum session_cancellation_reason {
    SESSION_CANCELLATION_NONE = 0u,
    SESSION_CANCELLATION_REQUESTED = 1u
};

typedef void (*session_teardown_fn)(void *context);

typedef struct session_teardown {
    session_teardown_fn function;
    void *context;
} session_teardown;

typedef struct session {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t cancellation_reason;
    uint32_t teardown_count;
    volatile long binding_count;
    session_teardown teardowns[SESSION_MAX_TEARDOWNS];
    mapping_manager guest_memory_mappings;
    mapping_manager host_resource_mappings;
    mapping_manager completion_callback_mappings;
    guest_memory_lease_context guest_memory_lease;
} session;

#ifdef __cplusplus
extern "C" {
#endif

void session_initialize(session *instance, uint32_t identity);
int session_valid(const session *instance);
int session_activate(session *instance);
int session_register_teardown(session *instance, session_teardown_fn function,
    void *context);
int session_request_cancellation(session *instance, uint32_t reason);
void session_complete(session *instance, uint32_t completion_code);
int session_dispose(session *instance);

mapping_manager *session_guest_memory_mappings(session *instance);
mapping_manager *session_host_resource_mappings(session *instance);
mapping_manager *session_completion_callback_mappings(session *instance);

int session_guest_memory_begin(session *instance, void *context,
    guest_memory_read_fn read, guest_memory_write_fn write);
void session_guest_memory_end(session *instance);
int session_guest_memory_acquire(session *instance, uint32_t address,
    uint32_t byte_count, uint32_t access, guest_memory_lease **lease_out,
    uint8_t **bytes_out);
int session_guest_memory_release(session *instance, guest_memory_lease *lease,
    int commit);

int session_thread_bind(session *instance);
int session_thread_unbind(session *instance);
session *session_thread_current(void);

#ifdef __cplusplus
}
#endif

#endif
