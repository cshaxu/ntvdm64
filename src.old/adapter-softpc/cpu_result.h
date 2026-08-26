#ifndef RUNTIME_CPU_RESULT_H
#define RUNTIME_CPU_RESULT_H

#include <stdint.h>

#include "cpu_delta_abi.h"

#define RUNTIME_CPU_RESULT_MAGIC 0x42584352u
#define RUNTIME_CPU_RESULT_VERSION 3u
#define RUNTIME_CPU_RESULT_EFLAGS_CF 0x00000001u
#define RUNTIME_CPU_RESULT_EFLAGS_ZF 0x00000040u

enum runtime_cpu_result_disposition {
    RUNTIME_CPU_RESULT_PASS_THROUGH = 0u,
    RUNTIME_CPU_RESULT_RESUME = 1u,
    RUNTIME_CPU_RESULT_STOP = 2u,
    /* A selector-blind suspension request.  It carries no CPU delta and
     * deliberately leaves the faulting instruction unadvanced. */
    RUNTIME_CPU_RESULT_PENDING = 3u
};

/* A separately accepted-resume result retaining the GPR
 * delta and permits only CF as an explicit FLAGS result. */
typedef struct runtime_cpu_result {
    uint32_t magic, abi_version, struct_bytes, disposition;
    uint64_t resume_rip;
    runtime_cpu_delta cpu_delta;
    uint32_t eflags_write_mask;
    uint32_t eflags_values;
} runtime_cpu_result;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_cpu_result_pass_through(runtime_cpu_result *result);
int runtime_cpu_result_resume(runtime_cpu_result *result,
    uint64_t resume_rip);
int runtime_cpu_result_stop(runtime_cpu_result *result);
int runtime_cpu_result_pending(runtime_cpu_result *result);
int runtime_cpu_result_valid(const runtime_cpu_result *result);
int runtime_cpu_result_set_cf(runtime_cpu_result *result, int value);
int runtime_cpu_result_set_zf(runtime_cpu_result *result, int value);
#ifdef __cplusplus
}
#endif

#endif
