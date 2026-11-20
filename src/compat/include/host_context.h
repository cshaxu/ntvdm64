#ifndef COMPAT_HOST_CONTEXT_H
#define COMPAT_HOST_CONTEXT_H

#include <stdint.h>
#include <setjmp.h>

struct guest_memory_lease_context;
struct mvdm_mouse_bridge;

#define COMPAT_HOST_CLEANUP_LIMIT 8u
#define COMPAT_HOST_THREAD_HOOK_LIMIT 8u
#define COMPAT_HOST_THREAD_SOURCE_BYTES 64u
#define COMPAT_MVDM_SYSTEM_ROOT_BYTES 1024u

typedef void (*compat_host_cleanup_fn)(void *);
typedef struct compat_host_cleanup {
    compat_host_cleanup_fn function;
    void *context;
} compat_host_cleanup;

enum compat_host_thread_owner {
    COMPAT_HOST_THREAD_UNSPECIFIED,
    COMPAT_HOST_THREAD_MACHINE_ENTRY,
    COMPAT_HOST_THREAD_ORIGINAL_WORKER
};

typedef int (*compat_host_thread_hook_fn)(void *context);
typedef struct compat_host_thread_hook {
    compat_host_thread_hook_fn bind;
    compat_host_thread_hook_fn unbind;
    void *context;
} compat_host_thread_hook;

typedef struct compat_host_thread_bindings {
    uint32_t hook_count;
    volatile long count;
    volatile long owner_count[3];
    char original_worker_source[COMPAT_HOST_THREAD_SOURCE_BYTES];
    compat_host_thread_hook hooks[COMPAT_HOST_THREAD_HOOK_LIMIT];
} compat_host_thread_bindings;

/* Embedded by the product owner. The owner opens/closes resource registration
 * and invokes disposal only after its threads have detached. There is no task
 * identity, scheduling, cancellation or completion policy here. */
enum compat_host_context_state {
    COMPAT_HOST_CONTEXT_READY = 0,
    COMPAT_HOST_CONTEXT_ACTIVE = 1,
    COMPAT_HOST_CONTEXT_CANCELLED = 2,
    COMPAT_HOST_CONTEXT_COMPLETED = 3
};

/* The composing owner chooses transitions. One state gates resource use and
 * registration; cancellation reasons and completion results remain outside. */
typedef struct compat_host_context {
    struct compat_console_endpoint *console_endpoint; /* Borrowed while the route is bound. */
    uint32_t cleanup_count;
    uint32_t state;
    uint32_t epoch; /* Activation generation, advanced by the composing owner. */
    jmp_buf *escape_stack;
    uint32_t *escape_result;
    void *mvdm_command_native_child; /* Owned capture resource; registered cleanup. */
    void *mvdm_command_guest_state; /* Owned DOS directory registration. */
    /* Borrowed B resource; the composing owner controls its begin/end lifetime. */
    struct guest_memory_lease_context *guest_memory;
    struct mvdm_mouse_bridge *mouse; /* Borrowed until the composing owner detaches. */
    /* Internal storage for the original SoftPC 80x50 character/attribute
     * text plane.  Only the source-facing adapter may acquire a writable
     * host-local lease; app presentation receives copies through the snapshot
     * API and never this pointer. */
    uint8_t *presentation_text_storage;
    uint32_t presentation_text_columns;
    uint32_t presentation_text_rows;
    uint32_t presentation_text_bytes;
    compat_host_cleanup cleanups[COMPAT_HOST_CLEANUP_LIMIT];
    compat_host_thread_bindings threads;
    char mvdm_system_root[COMPAT_MVDM_SYSTEM_ROOT_BYTES];
} compat_host_context;

compat_host_context *__cdecl compat_host_context_current(void);
int compat_host_context_valid(const compat_host_context *context);
int compat_host_context_bind(compat_host_context *context);
int compat_host_context_unbind(compat_host_context *context);
uint32_t compat_host_thread_count(const compat_host_context *context);
uint32_t compat_host_thread_owner_count(const compat_host_context *context, uint32_t owner);
int compat_host_thread_register(compat_host_context *context,
    compat_host_thread_hook_fn bind, compat_host_thread_hook_fn unbind, void *argument);
int compat_host_thread_unregister(compat_host_context *context,
    compat_host_thread_hook_fn bind, compat_host_thread_hook_fn unbind, void *argument);
int compat_host_thread_bind(compat_host_context *context, uint32_t owner, const char *source);
int compat_host_thread_unbind(compat_host_context *context);
int compat_host_thread_finish(compat_host_context *context);
int compat_host_thread_is_finishing(void);
/* Borrow the initiating thread's stack and result until it disarms. */
int compat_host_escape_arm(compat_host_context *context, jmp_buf *stack, uint32_t *result);
void compat_host_escape_disarm(compat_host_context *context);
int compat_host_escape_terminate_current(uint32_t result);
int compat_host_context_register(compat_host_context *context,
    compat_host_cleanup_fn function, void *argument);
/* Release registered resources in reverse order before the owner closes its
 * own facilities. The context and its text storage remain valid until dispose. */
void compat_host_context_release_resources(compat_host_context *context);
void compat_host_context_dispose(compat_host_context *context);

int compat_host_text_acquire_writable(compat_host_context *instance,
    uint32_t columns, uint32_t rows, uint32_t cell_bytes, uint8_t **bytes_out);
int compat_host_text_describe(const compat_host_context *instance,
    uint32_t *columns_out, uint32_t *rows_out, uint32_t *bytes_out);
int compat_host_text_snapshot(const compat_host_context *instance,
    uint8_t *destination, uint32_t destination_bytes, uint32_t *columns_out,
    uint32_t *rows_out, uint32_t *bytes_out);
void compat_host_text_clear(compat_host_context *instance);

#endif
