#ifndef SESSION_H
#define SESSION_H

#include <setjmp.h>
#include <stdint.h>

#include "compat/include/guest_memory_lease.h"
#include "compat/include/host_context.h"

#define SESSION_MAGIC UINT32_C(0x53455353)
#define SESSION_ABI_VERSION UINT32_C(15)
#define SESSION_PRESENTATION_PALETTE_ENTRIES 256u
#define SESSION_FIRMWARE_ROOT_BYTES COMPAT_MVDM_SYSTEM_ROOT_BYTES

/* A fixed-width composition choice.  The numeric value is session-local
 * state only: it is never copied to guest or MVDM storage. */
enum session_machine_backend {
    SESSION_MACHINE_BACKEND_NONE = 0u,
    SESSION_MACHINE_BACKEND_SOFTPC = 1u
};

enum session_state {
    SESSION_STATE_READY = COMPAT_HOST_CONTEXT_READY,
    SESSION_STATE_ACTIVE = COMPAT_HOST_CONTEXT_ACTIVE,
    SESSION_STATE_CANCELLED = COMPAT_HOST_CONTEXT_CANCELLED,
    SESSION_STATE_COMPLETED = COMPAT_HOST_CONTEXT_COMPLETED
};

enum session_cancellation_reason {
    SESSION_CANCELLATION_NONE = 0u,
    SESSION_CANCELLATION_REQUESTED = 1u
};

/* A session-owned explanation of the existing dispose precondition.  These
 * values are app-local diagnostics; they are never copied into guest/MVDM
 * state and do not alter dispose ordering or failure behavior. */
enum session_dispose_reason {
    SESSION_DISPOSE_REASON_NONE = 0u,
    SESSION_DISPOSE_REASON_INVALID = 1u,
    SESSION_DISPOSE_REASON_BINDING_COUNT = 2u,
    SESSION_DISPOSE_REASON_TERMINATION_ARMED = 3u
};

/* Source-owned categories for the existing per-thread session binding.  They
 * are host-local diagnostics only, never guest/MVDM values. */
enum session_thread_binding_owner {
    SESSION_THREAD_BINDING_UNSPECIFIED = COMPAT_HOST_THREAD_UNSPECIFIED,
    SESSION_THREAD_BINDING_SOFTPC_ENTRY = COMPAT_HOST_THREAD_MACHINE_ENTRY,
    SESSION_THREAD_BINDING_ORIGINAL_WORKER = COMPAT_HOST_THREAD_ORIGINAL_WORKER
};

#define SESSION_WORKER_SOURCE_MAX COMPAT_HOST_THREAD_SOURCE_BYTES

typedef struct session_binding_diagnostic {
    uint32_t total;
    uint32_t softpc_entry;
    uint32_t original_worker;
    uint32_t unspecified;
    char original_worker_source[SESSION_WORKER_SOURCE_MAX];
} session_binding_diagnostic;

typedef compat_host_thread_hook_fn session_thread_bind_fn;
typedef compat_host_thread_hook_fn session_thread_unbind_fn;

typedef struct session {
    jmp_buf termination_escape;
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t identity;
    uint32_t completion_code;
    uint32_t cancellation_reason;
    uint32_t machine_backend;
    compat_host_context host_context;
    /* Worker-local endpoint of the authenticated run16 I/O channel. */
    void *console_client;
    guest_memory_lease_context guest_memory_lease;
    char firmware_root[SESSION_FIRMWARE_ROOT_BYTES];
} session;

#ifdef __cplusplus
extern "C" {
#endif

void session_initialize(session *instance, uint32_t identity);
int session_valid(const session *instance);
int session_activate(session *instance);
int session_select_machine_backend(session *instance, uint32_t backend);
uint32_t session_machine_backend(const session *instance);
int session_register_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context);
int session_unregister_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context);
int session_request_cancellation(session *instance, uint32_t reason);
void session_complete(session *instance, uint32_t completion_code);
int session_dispose(session *instance);
/* The worker supplies its own Console close operation when a client is
 * attached. It is used synchronously after registered compatibility cleanup. */
int session_dispose_with_reason(session *instance, uint32_t *reason_out,
    void (*close_console)(session *));
int session_set_firmware_root(session *instance, const char *path);
const char *session_firmware_root(const session *instance);
/* Immutable MVDM system root selected by app for imported COMMAND, DOS and
 * WOW media.  It is not a guest-drive or a replacement for host SystemRoot. */
int session_set_mvdm_system_root(session *instance, const char *path);
const char *session_mvdm_system_root(const session *instance);
int session_guest_memory_begin(session *instance);
void session_guest_memory_end(session *instance);
int session_guest_memory_acquire(session *instance, uint32_t address,
    uint32_t byte_count, uint32_t access, guest_memory_lease **lease_out,
    uint8_t **bytes_out);
int session_guest_memory_release(session *instance, guest_memory_lease *lease,
    int commit);

int session_thread_bind(session *instance);
int session_thread_bind_owned(session *instance, uint32_t binding_owner);
int session_thread_bind_owned_source(session *instance, uint32_t binding_owner,
    const char *source_name);
int session_thread_unbind(session *instance);
/* Terminal intent is visible only while the existing reverse hooks run.
 * Ordinary detach and bind rollback remain retryable operations. */
int session_thread_finish(session *instance);
int session_thread_is_finishing(void);
/* This worker-private accessor is consumed by the /Gz WOW32 provider, while
 * session.c deliberately retains the original C ABI.  State it explicitly so
 * the import remains cdecl rather than inheriting the provider's default. */
session *__cdecl session_thread_current(void);
int session_binding_diagnostic_snapshot(const session *instance,
    session_binding_diagnostic *diagnostic_out);
int session_arm_termination_escape(session *instance);
void session_disarm_termination_escape(session *instance);

#ifdef __cplusplus
}
#endif

#endif
