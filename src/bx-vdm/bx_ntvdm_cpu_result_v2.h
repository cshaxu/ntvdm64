#ifndef BX_NTVDM_CPU_RESULT_V2_H
#define BX_NTVDM_CPU_RESULT_V2_H

#include <stdint.h>

#include "bx_ntvdm_cpu_delta_abi.h"

#define BX_NTVDM_CPU_RESULT_V2_MAGIC 0x42584352u
#define BX_NTVDM_CPU_RESULT_V2_VERSION 2u
#define BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF 0x00000001u
#define BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF 0x00000040u

enum bx_ntvdm_cpu_result_v2_disposition {
    BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH = 0u,
    BX_NTVDM_CPU_RESULT_V2_RESUME = 1u,
    BX_NTVDM_CPU_RESULT_V2_STOP = 2u
};

/* A separately versioned accepted-resume result. v2 retains the v1 GPR
 * delta and permits only CF as an explicit FLAGS result. */
typedef struct bx_ntvdm_cpu_result_v2 {
    uint32_t magic, abi_version, struct_bytes, disposition;
    uint64_t resume_rip;
    bx_ntvdm_cpu_delta_v1 cpu_delta;
    uint32_t eflags_write_mask;
    uint32_t eflags_values;
} bx_ntvdm_cpu_result_v2;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_cpu_result_v2_pass_through(bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_cpu_result_v2_resume(bx_ntvdm_cpu_result_v2 *result,
    uint64_t resume_rip);
int bx_ntvdm_cpu_result_v2_stop(bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_cpu_result_v2_valid(const bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_cpu_result_v2_set_cf(bx_ntvdm_cpu_result_v2 *result, int value);
int bx_ntvdm_cpu_result_v2_set_zf(bx_ntvdm_cpu_result_v2 *result, int value);
#ifdef __cplusplus
}
#endif

#endif
