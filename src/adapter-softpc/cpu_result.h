#ifndef RUNTIME_CPU_RESULT_V2_H
#define RUNTIME_CPU_RESULT_V2_H

#include <stdint.h>

#include "cpu_delta_abi.h"

#define RUNTIME_CPU_RESULT_V2_MAGIC 0x42584352u
#define RUNTIME_CPU_RESULT_V2_VERSION 3u
#define RUNTIME_CPU_RESULT_V2_EFLAGS_CF 0x00000001u
#define RUNTIME_CPU_RESULT_V2_EFLAGS_ZF 0x00000040u

enum runtime_cpu_result_v2_disposition {
    RUNTIME_CPU_RESULT_V2_PASS_THROUGH = 0u,
    RUNTIME_CPU_RESULT_V2_RESUME = 1u,
    RUNTIME_CPU_RESULT_V2_STOP = 2u,
    /* A selector-blind suspension request.  It carries no CPU delta and
     * deliberately leaves the faulting instruction unadvanced. */
    RUNTIME_CPU_RESULT_V2_PENDING = 3u
};

/* A separately versioned accepted-resume result. v2 retains the v1 GPR
 * delta and permits only CF as an explicit FLAGS result. */
typedef struct runtime_cpu_result_v2 {
    uint32_t magic, abi_version, struct_bytes, disposition;
    uint64_t resume_rip;
    runtime_cpu_delta_v1 cpu_delta;
    uint32_t eflags_write_mask;
    uint32_t eflags_values;
} runtime_cpu_result_v2;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_cpu_result_v2_pass_through(runtime_cpu_result_v2 *result);
int runtime_cpu_result_v2_resume(runtime_cpu_result_v2 *result,
    uint64_t resume_rip);
int runtime_cpu_result_v2_stop(runtime_cpu_result_v2 *result);
int runtime_cpu_result_v2_pending(runtime_cpu_result_v2 *result);
int runtime_cpu_result_v2_valid(const runtime_cpu_result_v2 *result);
int runtime_cpu_result_v2_set_cf(runtime_cpu_result_v2 *result, int value);
int runtime_cpu_result_v2_set_zf(runtime_cpu_result_v2 *result, int value);
#ifdef __cplusplus
}
#endif

#endif
