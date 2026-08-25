#ifndef RUNTIME_CONTROLLED_STOP_SERVICE_H
#define RUNTIME_CONTROLLED_STOP_SERVICE_H

#include "cpu_result.h"
#include "exception_abi.h"

int runtime_controlled_stop_service_dispatch(
    const runtime_exception_event *event,
    const runtime_cpu_state *cpu,
    const runtime_instruction_window *window,
    runtime_cpu_result *result);

#endif
