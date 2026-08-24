/*
 * OpenNT BOP composition route.
 *
 * The generic ingress knows only a fixed mechanical ABI.  BOP selector
 * recognition remains here.  A valid C4 C4 50 xx or C4 C4 54 xx window is terminally
 * owned by its imported OpenNT owner session: a missing or failing session
 * declines to the CPU exception path and must never enter a legacy provider.
 * Other
 * selectors decline until their own OpenNT-shaped package is composed here.
 */

#include "opennt_bop_route.h"
#include "dem_v2_generic_ud_bridge.h"
#include "command_v2_generic_ud_bridge.h"
#include "softpc_machine_interrupt_v2_generic_ud_bridge.h"
#include "softpc_memory_size_v2_generic_ud_bridge.h"
#include "softpc_tape_io_v2_generic_ud_bridge.h"
#include "softpc_emm_unavailable_v2_generic_ud_bridge.h"
#include "softpc_mouse_vector_v2_generic_ud_bridge.h"
#include "softpc_printer_openclose_v2_generic_ud_bridge.h"
#include "config_complete_v2_generic_ud_bridge.h"
#include "spckbd_handoff_v2_generic_ud_bridge.h"
#include "top_level_nosupport_v2_generic_ud_bridge.h"
#include "xms_v2_generic_ud_bridge.h"
#include "redir_v2_generic_ud_bridge.h"
#include "opennt-bop/observation/bx_ntvdm_bop_sequence_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_command_bootstrap_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_command_current_dir_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_dem_open_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_ntdos_exec_entry_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"
#include "adapter-softpc/mechanical_action.h"

/* This adapter-owned observation calls the existing opaque, checked RAM
 * mechanism.  It neither changes RAM nor gives the mantle BOP vocabulary. */
static int command_bootstrap_read(void *state, uint64_t physical_address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)state;
    return runtime_mantle_checked_ram_read_v1(physical_address, bytes, byte_count);
}

int runtime_opennt_bop_route_dispatch_v1(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome, void *context)
{
    struct runtime_generic_ud_outcome_v1 declined = {0};
    int accepted;
    (void)context;
    if (!runtime_softpc_machine_interrupt_v2_generic_ud_recognizes(event) &&
        !runtime_softpc_memory_size_v2_generic_ud_recognizes(event) &&
        !runtime_softpc_tape_io_v2_generic_ud_recognizes(event) &&
        !runtime_softpc_emm_unavailable_v2_generic_ud_recognizes(event) &&
        !runtime_softpc_mouse_vector_v2_generic_ud_recognizes(event) &&
        !runtime_softpc_printer_openclose_v2_generic_ud_recognizes(event) &&
        !runtime_config_complete_v2_generic_ud_recognizes(event) &&
        !runtime_spckbd_handoff_v2_generic_ud_recognizes(event) &&
        !runtime_dem_v2_generic_ud_recognizes(event) &&
        !runtime_command_v2_generic_ud_recognizes(event) &&
        !runtime_xms_v2_generic_ud_recognizes(event) &&
        !runtime_redir_v2_generic_ud_recognizes(event) &&
        !runtime_top_level_nosupport_v2_generic_ud_recognizes(event)) {
        declined.abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = RUNTIME_GENERIC_UD_PASS_THROUGH;
        runtime_bop_sequence_observation_v1_consider(event, &declined);
        runtime_generic_ud_sequence_observation_v1_consider(event, &declined);
        return 0;
    }
    /* Selector recognition and owner dispatch remain OpenNT BOP work.  The
     * generic ingress calls one opaque route callback and never learns either
     * selector or provider identity. */
    accepted = runtime_softpc_machine_interrupt_v2_generic_ud_recognizes(event) ?
        runtime_softpc_machine_interrupt_v2_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_memory_size_v2_generic_ud_recognizes(event) ?
        runtime_softpc_memory_size_v2_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_tape_io_v2_generic_ud_recognizes(event) ?
        runtime_softpc_tape_io_v2_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_emm_unavailable_v2_generic_ud_recognizes(event) ?
        runtime_softpc_emm_unavailable_v2_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_mouse_vector_v2_generic_ud_recognizes(event) ?
        runtime_softpc_mouse_vector_v2_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_printer_openclose_v2_generic_ud_recognizes(event) ?
        runtime_softpc_printer_openclose_v2_generic_ud_dispatch(event, outcome) :
        (runtime_config_complete_v2_generic_ud_recognizes(event) ?
        runtime_config_complete_v2_generic_ud_dispatch(event, outcome) :
        (runtime_spckbd_handoff_v2_generic_ud_recognizes(event) ?
        runtime_spckbd_handoff_v2_generic_ud_dispatch(event, outcome) :
        (runtime_dem_v2_generic_ud_recognizes(event) ?
        runtime_dem_v2_generic_ud_dispatch(event, outcome) :
        (runtime_command_v2_generic_ud_recognizes(event) ?
            runtime_command_v2_generic_ud_dispatch(event, outcome) :
            (runtime_xms_v2_generic_ud_recognizes(event) ?
                runtime_xms_v2_generic_ud_dispatch(event, outcome) :
                (runtime_redir_v2_generic_ud_recognizes(event) ?
                    runtime_redir_v2_generic_ud_dispatch(event, outcome) :
                    runtime_top_level_nosupport_v2_generic_ud_dispatch(event, outcome))))))))))));
    if (accepted) {
        runtime_command_bootstrap_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        runtime_command_current_dir_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        runtime_dem_open_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        runtime_ntdos_exec_entry_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        runtime_bop_sequence_observation_v1_consider(event, outcome);
        runtime_generic_ud_sequence_observation_v1_consider(event, outcome);
    } else {
        declined.abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = RUNTIME_GENERIC_UD_PASS_THROUGH;
        runtime_bop_sequence_observation_v1_consider(event, &declined);
        runtime_generic_ud_sequence_observation_v1_consider(event, &declined);
    }
    return accepted;
}
