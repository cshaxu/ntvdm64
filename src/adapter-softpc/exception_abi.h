#ifndef RUNTIME_EXCEPTION_ABI_H
#define RUNTIME_EXCEPTION_ABI_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "cpu_delta_abi.h"
#include "instruction_window_abi.h"

#define RUNTIME_EXCEPTION_ABI_MAGIC 0x42584e41u
#define RUNTIME_EXCEPTION_ABI_VERSION 1u

enum runtime_exception_event_kind {
    RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION = 1u
};

enum runtime_exception_result_disposition {
    RUNTIME_EXCEPTION_RESULT_PASS_THROUGH = 0u,
    RUNTIME_EXCEPTION_RESULT_RESUME = 1u,
    RUNTIME_EXCEPTION_RESULT_STOP = 2u
};

typedef struct runtime_exception_event_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t kind;
    uint32_t cpu_id;
    uint32_t vector;
    uint32_t error_code;
    uint32_t reserved0;
    uint64_t fault_rip;
} runtime_exception_event_v1;

typedef struct runtime_exception_result_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t disposition;
    uint64_t resume_rip;
    runtime_cpu_delta_v1 cpu_delta;
} runtime_exception_result_v1;

#ifdef __cplusplus
extern "C" {
#endif

int runtime_exception_event_v1_valid(const runtime_exception_event_v1 *event);
void runtime_exception_result_v1_pass_through(runtime_exception_result_v1 *result);
int runtime_exception_result_v1_resume(runtime_exception_result_v1 *result,
    uint64_t resume_rip);
int runtime_exception_dispatch_v1(const runtime_exception_event_v1 *event,
    runtime_exception_result_v1 *result);
int runtime_exception_dispatch_state_v1(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *state,
    runtime_exception_result_v1 *result);
int runtime_exception_dispatch_state_window_v1(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *state,
    const runtime_instruction_window_v1 *window,
    runtime_exception_result_v1 *result);

#ifdef __cplusplus
}
#endif

#endif
