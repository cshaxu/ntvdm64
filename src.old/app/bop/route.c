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

#include "route.h"
#include "app/bop/dem_entry.h"
#include "app/bop/command_entry.h"
#include "app/bop/softpc_machine_interrupt_generic_ud_bridge.h"
#include "app/bop/softpc_memory_size_generic_ud_bridge.h"
#include "app/bop/softpc_tape_io_generic_ud_bridge.h"
#include "app/bop/softpc_emm_unavailable_generic_ud_bridge.h"
#include "app/bop/softpc_mouse_vector_generic_ud_bridge.h"
#include "app/bop/softpc_printer_openclose_generic_ud_bridge.h"
#include "app/bop/config_complete_entry.h"
#include "app/bop/spckbd_handoff_generic_ud_bridge.h"
#include "app/bop/top_level_nosupport_generic_ud_bridge.h"
#include "app/bop/xms_entry.h"
#include "app/bop/redir_generic_ud_bridge.h"

int runtime_opennt_bop_route_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void *context)
{
    int accepted;
    (void)context;
    if (!runtime_softpc_machine_interrupt_generic_ud_recognizes(event) &&
        !runtime_softpc_memory_size_generic_ud_recognizes(event) &&
        !runtime_softpc_tape_io_generic_ud_recognizes(event) &&
        !runtime_softpc_emm_unavailable_generic_ud_recognizes(event) &&
        !runtime_softpc_mouse_vector_generic_ud_recognizes(event) &&
        !runtime_softpc_printer_openclose_generic_ud_recognizes(event) &&
        !runtime_config_complete_generic_ud_recognizes(event) &&
        !runtime_spckbd_handoff_generic_ud_recognizes(event) &&
        !runtime_dem_generic_ud_recognizes(event) &&
        !runtime_command_generic_ud_recognizes(event) &&
        !runtime_xms_generic_ud_recognizes(event) &&
        !runtime_redir_generic_ud_recognizes(event) &&
        !runtime_top_level_nosupport_generic_ud_recognizes(event)) {
        return 0;
    }
    /* Selector recognition and owner dispatch remain OpenNT BOP work.  The
     * generic ingress calls one opaque route callback and never learns either
     * selector or provider identity. */
    accepted = runtime_softpc_machine_interrupt_generic_ud_recognizes(event) ?
        runtime_softpc_machine_interrupt_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_memory_size_generic_ud_recognizes(event) ?
        runtime_softpc_memory_size_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_tape_io_generic_ud_recognizes(event) ?
        runtime_softpc_tape_io_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_emm_unavailable_generic_ud_recognizes(event) ?
        runtime_softpc_emm_unavailable_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_mouse_vector_generic_ud_recognizes(event) ?
        runtime_softpc_mouse_vector_generic_ud_dispatch(event, outcome) :
        (runtime_softpc_printer_openclose_generic_ud_recognizes(event) ?
        runtime_softpc_printer_openclose_generic_ud_dispatch(event, outcome) :
        (runtime_config_complete_generic_ud_recognizes(event) ?
        runtime_config_complete_generic_ud_dispatch(event, outcome) :
        (runtime_spckbd_handoff_generic_ud_recognizes(event) ?
        runtime_spckbd_handoff_generic_ud_dispatch(event, outcome) :
        (runtime_dem_generic_ud_recognizes(event) ?
        runtime_dem_generic_ud_dispatch(event, outcome) :
        (runtime_command_generic_ud_recognizes(event) ?
            runtime_command_generic_ud_dispatch(event, outcome) :
            (runtime_xms_generic_ud_recognizes(event) ?
                runtime_xms_generic_ud_dispatch(event, outcome) :
                (runtime_redir_generic_ud_recognizes(event) ?
                    runtime_redir_generic_ud_dispatch(event, outcome) :
                    runtime_top_level_nosupport_generic_ud_dispatch(event, outcome))))))))))));
    return accepted;
}
