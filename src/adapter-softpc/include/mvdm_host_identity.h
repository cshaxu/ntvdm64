#ifndef MVDM_HOST_IDENTITY_H
#define MVDM_HOST_IDENTITY_H

#include <stdint.h>

/*
 * Source-derived replacement for the x86 MVDM convention of persisting a
 * host pointer in a 32-bit DOS-owned field.  The returned identity is opaque;
 * it is neither a guest address nor an OpenNT numeric data value.
 */
int mvdm_host_identity_publish(uintptr_t native_value,
    uint32_t *identity_out);
int mvdm_host_identity_resolve(uint32_t identity,
    uintptr_t *native_value_out);
int mvdm_host_identity_lookup(uintptr_t native_value,
    uint32_t *identity_out);
int mvdm_host_identity_release(uint32_t identity);

#endif
