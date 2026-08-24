#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "opennt-host/softpc.new/host/inc/nt_error_compat.h"

#include <string.h>

static int invoke(unsigned ax, unsigned selector,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.fault_rip = UINT64_C(0x4100);
    event.eax = ax;
    event.ebx = UINT32_C(0xdeadbeef);
    event.window_bytes = 3u;
    event.window[0] = 0xc4u;
    event.window[1] = 0xc4u;
    event.window[2] = (uint8_t)selector;
    memset(outcome, 0, sizeof(*outcome));
    return bx_ntvdm_mantle_generic_ud_bridge_v1(&event, outcome);
}

int main(void)
{
    if (!bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_opennt_bop_route_dispatch_v1, 0)) return 90;
    static const unsigned reasons[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u };
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    unsigned index;
    bx_ntvdm_top_level_nosupport_v2_reset_thread();
    bx_ntvdm_top_level_nosupport_v2_fixture_choice_set(
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE);
    for (index = 0u; index < sizeof(reasons) / sizeof(reasons[0]); ++index) {
        if (!invoke(reasons[index] | UINT32_C(0xabcd0000), 0x59u, &outcome) ||
            outcome.abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            outcome.resume_rip != UINT64_C(0x4103) ||
            outcome.gpr16_write_mask != 0u || outcome.segment_write_mask != 0u ||
            outcome.eflags_write_mask != 0u) return 1;
    }
    if (bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count() != 7u) return 2;
    if (!invoke(2u, 0x59u, &outcome) ||
        bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count() != 7u) return 3;
    bx_ntvdm_top_level_nosupport_v2_reset_thread();
    bx_ntvdm_top_level_nosupport_v2_fixture_choice_set(
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_TERMINATE);
    if (!invoke(4u, 0x59u, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        outcome.gpr16_write_mask != 0u || outcome.segment_write_mask != 0u ||
        outcome.eflags_write_mask != 0u ||
        bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count() != 1u) return 4;
    if (invoke(2u, 0x5au, &outcome)) return 5;
    return 0;
}
