#include "mvdm_protected_span.h"

#include "adapter-bochs/machine_facade.h"

int mvdm_protected_span_copy(uint32_t kind, uint16_t selector,
    uint32_t offset, uint32_t byte_count, uint8_t *bytes)
{
    uint32_t slot;
    struct machine_facade_protected_segment segment;

    /* SoftPC source expresses a current protected segment as its selector;
     * adapter-bochs deliberately accepts only neutral active-segment slots.
     * Resolve the selector from copied machine data rather than passing an
     * OpenNT/DPMI concept into Bochs. */
    for (slot = 0u; slot != 6u; ++slot) {
        if (machine_facade_copy_protected_segment(slot, &segment) ==
            MACHINE_FACADE_PROTECTED_FRAME_OK && segment.selector == selector) {
            return machine_facade_protected_span_transfer(kind, slot, offset,
                byte_count, bytes) == MACHINE_FACADE_PROTECTED_RANGE_OK;
        }
    }
    return 0;
}
