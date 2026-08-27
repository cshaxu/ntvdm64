#include "dpmi_interrupt_registration.h"

#include <stdint.h>

#include "mvdm_command_registers.h"
#include "mvdm_protected_span.h"
#include "vdm.h"

static uint16_t read16(const uint8_t *bytes, uint32_t offset)
{
    return (uint16_t)((uint16_t)bytes[offset] |
        ((uint16_t)bytes[offset + 1u] << 8));
}

static uint32_t read32(const uint8_t *bytes, uint32_t offset)
{
    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
        ((uint32_t)bytes[offset + 2u] << 16) |
        ((uint32_t)bytes[offset + 3u] << 24);
}

int mvdm_dpmi_register_protected_interrupt(void)
{
    uint8_t frame[10];
    uint16_t number;

    if (!mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_READ, getSS(), getSP(),
            (uint32_t)sizeof(frame), frame)) return 0;
    number = read16(frame, 6u);
    /* DIVERGENCE(MVDM-HOST-DIV-016): The original trusts a DOSX-owned
     * interrupt number before indexing a 256-entry table. The bounded modern
     * carrier refuses an out-of-range frame rather than corrupting its
     * session-local TIB. A conforming source frame retains the exact order. */
    if (number >= 256u) return 0;
    VdmTib.VdmInterruptHandlers[number].Flags = read16(frame, 8u);
    VdmTib.VdmInterruptHandlers[number].CsSelector = read16(frame, 4u);
    VdmTib.VdmInterruptHandlers[number].Eip = read32(frame, 0u);
    if (number == 0x21u) {
        VDMSET_INT21_HANDLER_DATA data;
        data.Selector = VdmTib.VdmInterruptHandlers[number].CsSelector;
        data.Offset = VdmTib.VdmInterruptHandlers[number].Eip;
        data.Gate32 = (BOOLEAN)(VdmTib.VdmInterruptHandlers[number].Flags & VDM_INT_32);
        (void)NtVdmControl(VdmSetInt21Handler, &data);
    }
    setAX(0u);
    return 1;
}

int mvdm_dpmi_register_fault_handler(void)
{
    uint8_t frame[18];
    uint16_t number;

    if (!mvdm_protected_span_copy(MVDM_PROTECTED_SPAN_READ, getSS(), getSP(),
            (uint32_t)sizeof(frame), frame)) return 0;
    number = read16(frame, 12u);
    /* DIVERGENCE(MVDM-HOST-DIV-016): see the matching interrupt-registration
     * guard. The original 32-entry table remains its source-defined owner. */
    if (number >= 32u) return 0;
    VdmTib.VdmFaultHandlers[number].Flags = read32(frame, 14u);
    VdmTib.VdmFaultHandlers[number].CsSelector = read16(frame, 10u);
    VdmTib.VdmFaultHandlers[number].Eip = read32(frame, 6u);
    VdmTib.VdmFaultHandlers[number].SsSelector = read16(frame, 4u);
    VdmTib.VdmFaultHandlers[number].Esp = read32(frame, 0u);
    setAX(0u);
    return 1;
}
