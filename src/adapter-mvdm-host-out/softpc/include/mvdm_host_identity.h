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

/* Preserve the historical 16-bit register-pair ABI while resolving the
 * session-owned 32-bit host-resource identity behind it. */
uintptr_t mvdm_host_identity_resolve_words(uint16_t high, uint16_t low);
int mvdm_host_identity_publish_words(uintptr_t native_value,
    uint16_t *high_out, uint16_t *low_out);

#endif
