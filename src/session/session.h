#ifndef SESSION_H
#define SESSION_H

#include <setjmp.h>
#include <stdint.h>

#include "mapping_manager.h"
#include "guest_memory_lease.h"

#define SESSION_MAGIC UINT32_C(0x53455353)
#define SESSION_ABI_VERSION UINT32_C(2)
#define SESSION_MAX_TEARDOWNS 8u
#define SESSION_MAX_THREAD_HOOKS 8u
#define SESSION_MECHANICAL_STATUS_NONE UINT32_MAX
#define SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE UINT32_C(0xfffffffe)
#define SESSION_FIRMWARE_ROOT_BYTES 1024u

/* A fixed-width composition choice.  The numeric value is session-local
 * state only: it is never copied to guest or MVDM storage. */
enum session_machine_backend {
    SESSION_MACHINE_BACKEND_NONE = 0u,
    SESSION_MACHINE_BACKEND_SOFTPC = 1u
};

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

enum session_video_event_kind {
    SESSION_VIDEO_EVENT_INVALIDATE = 1u,
    SESSION_VIDEO_EVENT_PALETTE = 2u,
    SESSION_VIDEO_EVENT_ACTIVE = 3u
};

typedef struct session_video_event {
    uint32_t kind;
    uintptr_t output_handle;
    uintptr_t palette_handle;
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
    uint32_t flags;
} session_video_event;

typedef int (*session_video_event_fn)(void *context,
    const session_video_event *event);

typedef void (*session_teardown_fn)(void *context);
typedef int (*session_thread_bind_fn)(void *context);
typedef void (*session_thread_unbind_fn)(void *context);

typedef struct session_teardown {
    session_teardown_fn function;
    void *context;
} session_teardown;

typedef struct session_thread_hook {
    session_thread_bind_fn bind;
    session_thread_unbind_fn unbind;
    void *context;
} session_thread_hook;

typedef struct session {
    /* `jmp_buf` has a stricter x64 alignment than scalar session state.
     * Keeping it first expresses that requirement in the ABI rather than
     * relying on compiler-inserted interior padding. */
    jmp_buf termination_escape;
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t cancellation_reason;
    uint32_t machine_backend;
    uint64_t mechanical_resume_budget;
    uint32_t mechanical_resume_status;
    uint32_t video_event_active;
    uint32_t teardown_count;
    uint32_t thread_hook_count;
    uint32_t termination_armed;
    volatile long binding_count;
    session_teardown teardowns[SESSION_MAX_TEARDOWNS];
    session_thread_hook thread_hooks[SESSION_MAX_THREAD_HOOKS];
    mapping_manager guest_memory_mappings;
    mapping_manager host_resource_mappings;
    mapping_manager completion_callback_mappings;
    guest_memory_lease_context guest_memory_lease;
    char firmware_root[SESSION_FIRMWARE_ROOT_BYTES];
    char mvdm_system_root[SESSION_FIRMWARE_ROOT_BYTES];
    session_video_event_fn video_event_sink;
    void *video_event_context;
} session;

#ifdef __cplusplus
extern "C" {
#endif

void session_initialize(session *instance, uint32_t identity);
int session_valid(const session *instance);
int session_activate(session *instance);
int session_select_machine_backend(session *instance, uint32_t backend);
uint32_t session_machine_backend(const session *instance);
int session_register_teardown(session *instance, session_teardown_fn function,
    void *context);
int session_register_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context);
int session_unregister_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context);
int session_request_cancellation(session *instance, uint32_t reason);
void session_complete(session *instance, uint32_t completion_code);
int session_set_mechanical_resume_budget(session *instance, uint64_t budget);
uint64_t session_mechanical_resume_budget(const session *instance);
void session_record_mechanical_resume_status(session *instance,
    uint32_t status);
uint32_t session_mechanical_resume_status(const session *instance);
int session_set_video_event_sink(session *instance, session_video_event_fn sink,
    void *context);
int session_notify_video_event(session *instance,
    const session_video_event *event);
uint32_t session_video_event_active(const session *instance);
int session_dispose(session *instance);
mapping_manager *session_guest_memory_mappings(session *instance);
mapping_manager *session_host_resource_mappings(session *instance);
mapping_manager *session_completion_callback_mappings(session *instance);

int session_set_firmware_root(session *instance, const char *path);
const char *session_firmware_root(const session *instance);
/* Immutable MVDM system root selected by app for imported COMMAND, DOS and
 * WOW media.  It is not a guest-drive or a replacement for host SystemRoot. */
int session_set_mvdm_system_root(session *instance, const char *path);
const char *session_mvdm_system_root(const session *instance);

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
int session_arm_termination_escape(session *instance);
void session_disarm_termination_escape(session *instance);
int session_terminate_current(uint32_t completion_code);

#ifdef __cplusplus
}
#endif

#endif
