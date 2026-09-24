#include "mvdm_softpc_fast_bop.h"
#include "mvdm_softpc_termination.h"

#include <nt.h>
#include <vdm.h>
#include <mvdm.h>
#include "mvdm/inc/dpmi.h"
#include <dpmidata.h>
#include <dpmi32p.h>
#include "mvdm/inc/vint.h"

extern uint32_t c_getCR0(void);
extern uint32_t c_sas_dw_at(uint32_t address);
extern void c_sas_storedw(uint32_t address, uint32_t value);

/* DIVERGENCE(MVDM-HOST-DIV-310): CPU40 cannot expose the original kernel
 * monitor's host CS:IP. Reserve one LDT identity for that endpoint, then let
 * the original CALLF owner redirect it to the existing BOP dispatcher. */
extern NTSTATUS DpmiSetWowLdtEntry(ULONG selector, ULONG low, ULONG high);

#define FAST_BOP_DISPATCH_OFFSET 0x10u
#define FAST_WOW_DISPATCH_OFFSET 0x20u
#define FAST_WOW_RETURN_OFFSET 0x30u

static USHORT fast_bop_selector;

void mvdm_softpc_fast_bop_sync_mode(void)
{
    uint32_t state;
    if (fast_bop_selector == 0u) return;
    /* Original dpmi386.c switch_to_{real,protected}_mode publishes RM_BIT
     * for BOP.INC's FBOP gate. Bind that same word through guest SAS, never
     * through the native process's absolute address 0714h. */
    state = c_sas_dw_at(FIXED_NTVDMSTATE_LINEAR);
    state = (state & ~RM_BIT_MASK) |
        ((c_getCR0() & 1u) ? 0u : RM_BIT_MASK);
    c_sas_storedw(FIXED_NTVDMSTATE_LINEAR, state);
}

static USHORT fast_bop_allocate_selector(void)
{
    ULONG index;
    PULONG ldt;

    if (Cpu40LdtShadowAddress == 0u) return 0u;
    ldt = (PULONG)(IntelBase + Cpu40LdtShadowAddress);
    for (index = LDT_SIZE; index-- > 1u;) {
        if ((ldt[index * 2u + 1u] & 0x00008000u) == 0u)
            return (USHORT)((index << 3) | 7u);
    }
    return 0u;
}

int mvdm_softpc_fast_bop_prepare(uint16_t *offset, uint16_t *selector)
{
    ULONG low;
    ULONG high;
    USHORT selected;

    if (offset == NULL || selector == NULL) return 0;
    if (fast_bop_selector == 0u) {
        selected = fast_bop_allocate_selector();
        if (selected == 0u) return 0;
        /* The native monitor endpoint is host code outside guest paging.
         * This present record reserves the selected identity only; CPU40's
         * CALLF carrier recognizes it before descriptor fetch. */
        low = 0u;
        high = 0x0000fa00u;
        if (!NT_SUCCESS(DpmiSetWowLdtEntry(selected, low, high))) {
            return 0;
        }
        /* Default-off witness: the selector filter is supplied by the
         * integration test, and the record samples the descriptor just
         * published through the same original CPU40 carrier. */
        mvdm_softpc_report_cpu40_descriptor_publish(selected, 0u, 0u, 0u,
            0u, ((PULONG)(IntelBase + Cpu40LdtShadowAddress))[
                (selected >> 3) * 2u],
            ((PULONG)(IntelBase + Cpu40LdtShadowAddress))[
                (selected >> 3) * 2u + 1u]);
        fast_bop_selector = selected;
    }
    mvdm_softpc_fast_bop_sync_mode();
    *offset = FAST_BOP_DISPATCH_OFFSET;
    *selector = fast_bop_selector;
    return 1;
}

int mvdm_softpc_fast_bop_redirect(uint16_t selector, uint32_t offset)
{
    if (fast_bop_selector == 0u || selector != fast_bop_selector) return 0;
    mvdm_softpc_report_wow_fast_callback_binding("fast-call-target", offset);
    switch (offset) {
    case FAST_BOP_DISPATCH_OFFSET: return MVDM_FAST_BOP_INLINE;
    case FAST_WOW_DISPATCH_OFFSET: return MVDM_FAST_BOP_WOW;
    case FAST_WOW_RETURN_OFFSET: return MVDM_FAST_BOP_RETURN;
    default: return 0;
    }
}

uint32_t mvdm_softpc_fast_bop_dispatch_offset(void)
{
    return fast_bop_selector ? FAST_WOW_DISPATCH_OFFSET : 0u;
}

uint32_t mvdm_softpc_fast_bop_callback_offset(void)
{
    return fast_bop_selector ? FAST_WOW_RETURN_OFFSET : 0u;
}
