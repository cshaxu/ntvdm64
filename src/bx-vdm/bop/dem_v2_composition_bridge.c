/*
 * Direct-only DEM composition entry.
 *
 * The mantle knows only this fixed mechanical ABI.  BOP selector recognition
 * remains in bx-vdm.  A valid C4 C4 50 xx or C4 C4 54 xx window is terminally
 * owned by its imported OpenNT owner session: a missing or failing session
 * declines to the CPU exception path and must never enter a legacy provider.
 * Other
 * selectors decline until their own OpenNT-shaped package is composed here.
 */

#include "dem_v2_generic_ud_bridge.h"
#include "command_v2_generic_ud_bridge.h"
#include "softpc_memory_size_v2_generic_ud_bridge.h"
#include "softpc_tape_io_v2_generic_ud_bridge.h"
#include "softpc_emm_unavailable_v2_generic_ud_bridge.h"
#include "softpc_mouse_vector_v2_generic_ud_bridge.h"
#include "softpc_printer_openclose_v2_generic_ud_bridge.h"
#include "config_complete_v2_generic_ud_bridge.h"
#include "spckbd_handoff_v2_generic_ud_bridge.h"
#include "top_level_nosupport_v2_generic_ud_bridge.h"
#include "xms_v2_generic_ud_bridge.h"
#include "bop/observation/bx_ntvdm_bop_sequence_observation_v1.h"
#include "bop/observation/bx_ntvdm_command_bootstrap_observation_v1.h"
#include "bop/observation/bx_ntvdm_command_current_dir_observation_v1.h"
#include "bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

/* This adapter-owned observation calls the existing opaque, checked RAM
 * mechanism.  It neither changes RAM nor gives the mantle BOP vocabulary. */
static int command_bootstrap_read(void *state, uint64_t physical_address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)state;
    return bx_ntvdm_mantle_checked_ram_read_v1(physical_address, bytes, byte_count);
}

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    struct bx_ntvdm_generic_ud_outcome_v1 declined = {0};
    int accepted;
    if (!bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_softpc_tape_io_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_softpc_mouse_vector_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_softpc_printer_openclose_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_config_complete_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_spckbd_handoff_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_dem_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_command_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_xms_v2_generic_ud_recognizes(event) &&
        !bx_ntvdm_top_level_nosupport_v2_generic_ud_recognizes(event)) {
        declined.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = BX_NTVDM_GENERIC_UD_PASS_THROUGH;
        bx_ntvdm_bop_sequence_observation_v1_consider(event, &declined);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, &declined);
        return 0;
    }
    /* Selector recognition and owner dispatch remain adapter work.  The
     * mantle calls one opaque mechanical entry and never learns either the
     * DEM or COMMAND selector. */
    accepted = bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_softpc_tape_io_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_softpc_tape_io_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_softpc_mouse_vector_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_softpc_mouse_vector_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_softpc_printer_openclose_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_softpc_printer_openclose_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_config_complete_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_config_complete_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_spckbd_handoff_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_spckbd_handoff_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_dem_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_dem_v2_generic_ud_dispatch(event, outcome) :
        (bx_ntvdm_command_v2_generic_ud_recognizes(event) ?
            bx_ntvdm_command_v2_generic_ud_dispatch(event, outcome) :
            (bx_ntvdm_xms_v2_generic_ud_recognizes(event) ?
                bx_ntvdm_xms_v2_generic_ud_dispatch(event, outcome) :
                bx_ntvdm_top_level_nosupport_v2_generic_ud_dispatch(event, outcome))))))))));
    if (accepted) {
        bx_ntvdm_command_bootstrap_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        bx_ntvdm_command_current_dir_observation_v1_consider(event, outcome,
            command_bootstrap_read, NULL);
        bx_ntvdm_bop_sequence_observation_v1_consider(event, outcome);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, outcome);
    } else {
        declined.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = BX_NTVDM_GENERIC_UD_PASS_THROUGH;
        bx_ntvdm_bop_sequence_observation_v1_consider(event, &declined);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, &declined);
    }
    return accepted;
}
