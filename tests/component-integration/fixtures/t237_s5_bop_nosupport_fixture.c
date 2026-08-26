#include "adapter-bop/generic_ud_bridge.h"
#include "app/bop/route.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

#include <string.h>

static int invoke(unsigned ax, unsigned selector,
    struct runtime_generic_ud_outcome *outcome)
{
    struct runtime_generic_ud_event event;
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
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
    return runtime_machine_generic_ud_bridge(&event, outcome);
}

int main(void)
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    static const unsigned reasons[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u };
    struct runtime_generic_ud_outcome outcome;
    unsigned index;
    runtime_opennt_direct_access_reset_thread();
    runtime_opennt_direct_access_fixture_reply_set(IDIGNORE);
    for (index = 0u; index < sizeof(reasons) / sizeof(reasons[0]); ++index) {
        if (!invoke(reasons[index] | UINT32_C(0xabcd0000), 0x59u, &outcome) ||
            outcome.abi_version != RUNTIME_GENERIC_UD_EVENT_VERSION ||
            outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
            outcome.resume_rip != UINT64_C(0x4103) ||
            outcome.gpr16_write_mask != 0u || outcome.segment_write_mask != 0u ||
            outcome.eflags_write_mask != 0u) return 1;
    }
    if (runtime_opennt_direct_access_prompt_count() != 7u) return 2;
    if (!invoke(2u, 0x59u, &outcome) ||
        runtime_opennt_direct_access_prompt_count() != 7u) return 3;
    runtime_opennt_direct_access_reset_thread();
    runtime_opennt_direct_access_fixture_reply_set(IDABORT);
    if (!invoke(4u, 0x59u, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_STOP ||
        outcome.gpr16_write_mask != 0u || outcome.segment_write_mask != 0u ||
        outcome.eflags_write_mask != 0u ||
        runtime_opennt_direct_access_prompt_count() != 1u) return 4;
    if (invoke(2u, 0x5au, &outcome)) return 5;
    return 0;
}
