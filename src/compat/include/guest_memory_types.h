#ifndef COMPAT_GUEST_MEMORY_TYPES_H
#define COMPAT_GUEST_MEMORY_TYPES_H
#include <stdint.h>

/* Provider-owned borrowed storage. Callers return it without inspecting it. */
typedef struct guest_memory_lease guest_memory_lease;
enum guest_memory_access {
    GUEST_MEMORY_ACCESS_READ = 1u,
    GUEST_MEMORY_ACCESS_WRITE = 2u
};
#endif
