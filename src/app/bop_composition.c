#include "bop_composition.h"

#include <stdint.h>
#include <string.h>

#include "adapter-bop/bop_ingress.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "adapter-bochs/machine_facade.h"
#include "adapter-softpc/first_fault_observation.h"
#include "adapter-softpc/interrupt_return_observation.h"
#include "adapter-softpc/segment_access_observation.h"
#include "adapter-softpc/software_interrupt_observation.h"
#include "adapter-softpc/instruction_history.h"
#include "adapter-softpc/physical_write_observation.h"
#include "adapter-softpc/mechanical_action.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "app/observation/bop_sequence_observation.h"
#include "app/observation/command_bootstrap_observation.h"
#include "app/observation/command_current_dir_observation.h"
#include "app/observation/dem_open_observation.h"
#include "app/observation/generic_ud_sequence_observation.h"
#include "app/observation/ntdos_exec_entry_observation.h"

/* CLI diagnostics are composed outside the OpenNT BOP mirror.  This keeps
 * the original dispatcher/provider side free of an app reverse dependency. */
static int app_bop_observation_read(void *state, uint64_t physical_address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)state;
    return runtime_machine_checked_ram_read(physical_address, bytes, byte_count);
}

static void app_bop_observe(const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome)
{
    runtime_command_bootstrap_observation_consider(event, outcome,
        app_bop_observation_read, 0);
    runtime_command_current_dir_observation_consider(event, outcome,
        app_bop_observation_read, 0);
    runtime_dem_open_observation_consider(event, outcome,
        app_bop_observation_read, 0);
    runtime_ntdos_exec_entry_observation_consider(event, outcome,
        app_bop_observation_read, 0);
    runtime_bop_sequence_observation_consider(event, outcome);
    runtime_generic_ud_sequence_observation_consider(event, outcome);
}

static int app_bop_route_callback(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void *context)
{
    struct runtime_generic_ud_outcome declined = {0};
    int accepted;
    (void)context;
    accepted = runtime_opennt_bop_route_dispatch(event, outcome, 0);
    if (accepted) {
        app_bop_observe(event, outcome);
    } else {
        declined.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
        declined.disposition = RUNTIME_GENERIC_UD_PASS_THROUGH;
        app_bop_observe(event, &declined);
    }
    return accepted;
}

/* The CPU-facing callback sees bytes only.  App owns the composed selection
 * of the BOP route and the optional first-fault diagnostic; neither reaches
 * bochs-core or its private overlay. */
static int app_bop_composition_opaque_callback(void *context,
    const void *event, unsigned event_bytes, void *outcome,
    unsigned outcome_bytes)
{
    uint32_t magic;
    uint32_t opaque_tag = 0;
    (void)context;
    if (event == 0 || event_bytes < sizeof(magic)) return 0;
    memcpy(&magic, event, sizeof(magic));
    if (event_bytes >= sizeof(opaque_tag))
        memcpy(&opaque_tag, (const uint8_t *)event + event_bytes - sizeof(opaque_tag),
            sizeof(opaque_tag));
    if (magic == RUNTIME_GENERIC_UD_EVENT_MAGIC)
        return runtime_bop_ingress_opaque_callback(
            0, event, event_bytes, outcome, outcome_bytes);
    if (magic == RUNTIME_FIRST_FAULT_OBSERVATION_MAGIC &&
        event_bytes == sizeof(struct runtime_first_fault_observation) &&
        outcome == 0 && outcome_bytes == 0)
        return runtime_machine_first_fault_observation(
            (const struct runtime_first_fault_observation *)event);
    if (magic == RUNTIME_SEGMENT_ACCESS_OBSERVATION_MAGIC &&
        event_bytes == sizeof(struct runtime_segment_access_observation) &&
        outcome == 0 && outcome_bytes == 0)
        return runtime_machine_segment_access_observation(
            (const struct runtime_segment_access_observation *)event);
    if (event_bytes == sizeof(struct runtime_interrupt_return_observation_record) + sizeof(uint32_t) &&
        magic == RUNTIME_INTERRUPT_RETURN_OBSERVATION_VERSION && opaque_tag == 0x42584952u &&
        outcome == 0 && outcome_bytes == 0) {
        runtime_machine_interrupt_return_observation_record(
            (const struct runtime_interrupt_return_observation_record *)event);
        return 1;
    }
    if (event_bytes == sizeof(struct runtime_software_interrupt_observation_record) + sizeof(uint32_t) &&
        magic == RUNTIME_SOFTWARE_INTERRUPT_OBSERVATION_VERSION && opaque_tag == 0x42585349u &&
        outcome == 0 && outcome_bytes == 0) {
        runtime_machine_software_interrupt_observation_record(
            (const struct runtime_software_interrupt_observation_record *)event);
        return 1;
    }
    if (event_bytes == sizeof(struct runtime_instruction_history_record) + sizeof(uint32_t) &&
        magic == RUNTIME_INSTRUCTION_HISTORY_VERSION && opaque_tag == 0x42584948u &&
        outcome == 0 && outcome_bytes == 0) {
        runtime_machine_instruction_history_record(
            (const struct runtime_instruction_history_record *)event);
        return 1;
    }
    if (event_bytes == sizeof(struct runtime_physical_write_observation) + sizeof(uint32_t) &&
        magic == RUNTIME_PHYSICAL_WRITE_OBSERVATION_MAGIC && opaque_tag == 0x42585057u &&
        outcome == 0 && outcome_bytes == 0) {
        {
            const struct runtime_physical_write_observation *record =
                (const struct runtime_physical_write_observation *)event;
            runtime_physical_write_observation_record(record->physical_address,
                record->byte_count, record->bytes, record->sequence, record->rip,
                record->cs_base, record->cs, record->ss, record->sp);
        }
        return 1;
    }
    return 0;
}

int app_bop_composition_bind(void)
{
    if (!runtime_bop_ingress_bind(
            app_bop_route_callback, 0)) return 0;
    if (machine_facade_bind_opaque_callback(
            app_bop_composition_opaque_callback, 0)) return 1;
    runtime_bop_ingress_unbind();
    return 0;
}

void app_bop_composition_reset(void)
{
    machine_facade_unbind_opaque_callback();
    runtime_bop_ingress_unbind();
}
