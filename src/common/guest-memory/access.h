#ifndef COMMON_GUEST_MEMORY_ACCESS_H
#define COMMON_GUEST_MEMORY_ACCESS_H
#include <stdint.h>

/* The lease is borrowed from the current worker's single memory context.
 * Consumers must return it to that provider and must not inspect its fields. */
typedef struct guest_memory_lease guest_memory_lease;
enum guest_memory_access {
    GUEST_MEMORY_ACCESS_READ = 1u,
    GUEST_MEMORY_ACCESS_WRITE = 2u
};

#if defined(_MSC_VER)
#define COMMON_GUEST_CDECL __cdecl
#else
#define COMMON_GUEST_CDECL
#endif
#if defined(_MSC_VER) && defined(COMMON_GUEST_MEMORY_IMPORTS)
#define COMMON_GUEST_API __declspec(dllimport)
#else
#define COMMON_GUEST_API
#endif
#ifdef __cplusplus
extern "C" {
#endif
COMMON_GUEST_API int COMMON_GUEST_CDECL mvdm_softpc_guest_memory_acquire(uint32_t address,
    uint32_t byte_count,uint32_t access,guest_memory_lease **lease_out,uint8_t **bytes_out);
COMMON_GUEST_API int COMMON_GUEST_CDECL mvdm_softpc_guest_memory_release(guest_memory_lease *lease,int commit);
#ifdef __cplusplus
}
#endif
#undef COMMON_GUEST_CDECL
#undef COMMON_GUEST_API
#endif
