#include <stdint.h>
#include <string.h>

#include "adapter-mvdm-host-in/bop_ingress.h"

int main(void)
{
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome expected;
    struct runtime_generic_ud_outcome outcome;

    runtime_bop_ingress_unbind();
    if (runtime_bop_ingress_bound()) return 1;

    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.window_bytes = 4u;
    event.window[0] = 0xc4u;
    event.window[1] = 0xc4u;
    event.window[2] = 0x57u;
    event.window[3] = 0x1cu;

    memset(&expected, 0xa5, sizeof(expected));
    outcome = expected;
    if (runtime_machine_generic_ud_bridge(&event, &outcome) != 0) return 2;
    if (memcmp(&outcome, &expected, sizeof(outcome)) != 0) return 3;
    return 0;
}
