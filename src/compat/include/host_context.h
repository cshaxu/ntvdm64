#ifndef COMPAT_HOST_CONTEXT_H
#define COMPAT_HOST_CONTEXT_H

#include <stdint.h>

#define COMPAT_HOST_CLEANUP_LIMIT 8u

typedef void (*compat_host_cleanup_fn)(void *);
typedef struct compat_host_cleanup {
    compat_host_cleanup_fn function;
    void *context;
} compat_host_cleanup;

/* Embedded by the product owner. The owner opens/closes resource registration
 * and invokes disposal only after its threads have detached. There is no task
 * identity, scheduling, cancellation or completion policy here. */
typedef struct compat_host_context {
    uint32_t cleanup_count;
    int registration_open;
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
