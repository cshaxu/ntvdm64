#ifndef COMPAT_HOST_CONTEXT_H
#define COMPAT_HOST_CONTEXT_H

#include <stdint.h>

struct guest_memory_lease_context;
struct mvdm_mouse_bridge;

#define COMPAT_HOST_CLEANUP_LIMIT 8u

typedef void (*compat_host_cleanup_fn)(void *);
typedef struct compat_host_cleanup {
    compat_host_cleanup_fn function;
    void *context;
} compat_host_cleanup;

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
    uint32_t cleanup_count;
    uint32_t state;
    /* Borrowed B resource; the composing owner controls its begin/end lifetime. */
    struct guest_memory_lease_context *guest_memory;
    struct mvdm_mouse_bridge *mouse; /* Borrowed until the composing owner detaches. */
    compat_host_cleanup cleanups[COMPAT_HOST_CLEANUP_LIMIT];
} compat_host_context;

compat_host_context *__cdecl compat_host_context_current(void);
int compat_host_context_valid(const compat_host_context *context);
int compat_host_context_bind(compat_host_context *context);
int compat_host_context_unbind(compat_host_context *context);
int compat_host_context_register(compat_host_context *context,
    compat_host_cleanup_fn function, void *argument);
void compat_host_context_dispose(compat_host_context *context);

#endif
