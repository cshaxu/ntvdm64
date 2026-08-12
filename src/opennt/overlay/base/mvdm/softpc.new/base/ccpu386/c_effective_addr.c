/*
 * Local reconstruction of a source-missing CCPU effective-address vector.
 *
 * The original CCPU callers own selector validation, faults, paging and the
 * later SAS access. This function preserves the narrower vector contract:
 * real/V86 segment arithmetic, or a protected-mode descriptor-base lookup.
 */
#include <stdint.h>

typedef struct _NTDOS64_CCPU_DESCRIPTOR {
    uint32_t base;
    uint32_t limit;
    uint16_t ar;
} NTDOS64_CCPU_DESCRIPTOR;

extern int c_getPE(void);
extern int c_getVM(void);
#ifndef NTDOS64_CCPU_EFFECTIVE_ADDR_REALMODE_ONLY
extern int selector_outside_table(uint16_t selector, uint32_t *descriptor);
extern void read_descriptor_linear(uint32_t address,
                                   NTDOS64_CCPU_DESCRIPTOR *descriptor);
#endif

uint32_t c_effective_addr(uint16_t segment, uint32_t offset) {
    uint32_t descriptor_address;
    NTDOS64_CCPU_DESCRIPTOR descriptor;

    if (!c_getPE() || c_getVM()) {
        return ((uint32_t)segment << 4) + offset;
    }

#ifdef NTDOS64_CCPU_EFFECTIVE_ADDR_REALMODE_ONLY
    /* This SAS lifecycle fixture does not admit protected-mode translation. */
    return UINT32_MAX;
#else
    if (selector_outside_table(segment, &descriptor_address)) {
        return UINT32_MAX;
    }

    read_descriptor_linear(descriptor_address, &descriptor);
    return descriptor.base + offset;
#endif
}

/* xt.h exposes this companion ABI with a 16-bit offset. */
uint32_t effective_addr(uint16_t segment, uint16_t offset) {
    return c_effective_addr(segment, offset);
}
