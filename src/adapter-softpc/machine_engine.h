#ifndef RUNTIME_MACHINE_ENGINE_V1_H
#define RUNTIME_MACHINE_ENGINE_V1_H

#include <stdint.h>

#include "machine_stage.h"

/* Pure machine lifecycle runner.  Callers supply already assembled bytes and
 * real-mode entry state; this surface has no profile, provider, DOS, VDM or
 * terminal-meaning vocabulary. */
struct runtime_machine_engine_v1_result {
    uint32_t begin_status;
    uint32_t entry_status;
    uint32_t execution_status;
    uint32_t reset_status;
    uint32_t cancellation_reason;
};

#ifdef __cplusplus
extern "C" {
#endif

int runtime_machine_engine_v1_run(
    const struct runtime_machine_stage_v1_request *stage,
    const struct runtime_machine_stage_v1_entry *entry,
    const struct runtime_machine_stage_v1_execution_request *execution,
    struct runtime_machine_engine_v1_result *result);

#ifdef __cplusplus
}
#endif

#endif
