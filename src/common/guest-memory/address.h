#ifndef COMMON_GUEST_MEMORY_ADDRESS_H
#define COMMON_GUEST_MEMORY_ADDRESS_H
#include <stdint.h>

/* Resolve a guest selector and offset using the worker's current CPU state. */
#if defined(_MSC_VER)
#if defined(COMMON_GUEST_MEMORY_IMPORTS)
__declspec(dllimport)
#endif
int __cdecl mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);
#else
int mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);
#endif
#endif
