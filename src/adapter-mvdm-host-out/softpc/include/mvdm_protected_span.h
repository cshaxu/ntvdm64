#ifndef MVDM_PROTECTED_SPAN_H
#define MVDM_PROTECTED_SPAN_H

#include <stdint.h>

/* Same-width, selector-blind transfer carrier for source bodies that used a
 * transient protected-mode Sim32 pointer.  The caller owns the source
 * structure interpretation; this facade only copies a bounded byte span. */
#define MVDM_PROTECTED_SPAN_READ 1u
#define MVDM_PROTECTED_SPAN_WRITE 2u

int mvdm_protected_span_copy(uint32_t kind, uint16_t selector,
    uint32_t offset, uint32_t byte_count, uint8_t *bytes);

#endif
