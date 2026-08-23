#ifndef BX_NTVDM_EXCEPTION_ABI_H
#define BX_NTVDM_EXCEPTION_ABI_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_cpu_delta_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

#define BX_NTVDM_EXCEPTION_ABI_MAGIC 0x42584e41u
#define BX_NTVDM_EXCEPTION_ABI_VERSION 1u

enum bx_ntvdm_exception_event_kind {
    BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION = 1u
};

enum bx_ntvdm_exception_result_disposition {
    BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH = 0u,
    BX_NTVDM_EXCEPTION_RESULT_RESUME = 1u,
    BX_NTVDM_EXCEPTION_RESULT_STOP = 2u
};

typedef struct bx_ntvdm_exception_event_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t kind;
    uint32_t cpu_id;
    uint32_t vector;
    uint32_t error_code;
    uint32_t reserved0;
    uint64_t fault_rip;
} bx_ntvdm_exception_event_v1;

typedef struct bx_ntvdm_exception_result_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t disposition;
    uint64_t resume_rip;
    bx_ntvdm_cpu_delta_v1 cpu_delta;
} bx_ntvdm_exception_result_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_exception_event_v1_valid(const bx_ntvdm_exception_event_v1 *event);
void bx_ntvdm_exception_result_v1_pass_through(bx_ntvdm_exception_result_v1 *result);
int bx_ntvdm_exception_result_v1_resume(bx_ntvdm_exception_result_v1 *result,
    uint64_t resume_rip);
int bx_ntvdm_exception_dispatch_v1(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_exception_result_v1 *result);
int bx_ntvdm_exception_dispatch_state_v1(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *state,
    bx_ntvdm_exception_result_v1 *result);
int bx_ntvdm_exception_dispatch_state_window_v1(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *state,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);

#ifdef __cplusplus
}
#endif

#endif
