#ifndef RUNTIME_CONTROLLED_STOP_SERVICE_H
#define RUNTIME_CONTROLLED_STOP_SERVICE_H

#include "cpu_result.h"
#include "exception_abi.h"

int runtime_controlled_stop_service_v1_dispatch(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *cpu,
    const runtime_instruction_window_v1 *window,
    runtime_cpu_result_v2 *result);

#endif
