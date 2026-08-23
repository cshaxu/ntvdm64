#include "bx_ntvdm_cpu_result_v2.h"

void bx_ntvdm_cpu_result_v2_pass_through(bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0) return;
    result->magic = BX_NTVDM_CPU_RESULT_V2_MAGIC;
    result->abi_version = BX_NTVDM_CPU_RESULT_V2_VERSION;
    result->struct_bytes = sizeof(*result);
    result->disposition = BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH;
    result->resume_rip = 0u;
    bx_ntvdm_cpu_delta_v1_initialize(&result->cpu_delta);
    result->eflags_write_mask = 0u;
    result->eflags_values = 0u;
}

int bx_ntvdm_cpu_result_v2_resume(bx_ntvdm_cpu_result_v2 *result,
    uint64_t resume_rip)
{
    if (result == 0) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    result->disposition = BX_NTVDM_CPU_RESULT_V2_RESUME;
    result->resume_rip = resume_rip;
    return 1;
}

int bx_ntvdm_cpu_result_v2_stop(bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    result->disposition = BX_NTVDM_CPU_RESULT_V2_STOP;
    return 1;
}

int bx_ntvdm_cpu_result_v2_pending(bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    result->disposition = BX_NTVDM_CPU_RESULT_V2_PENDING;
    return 1;
}

int bx_ntvdm_cpu_result_v2_valid(const bx_ntvdm_cpu_result_v2 *result)
{
    return result != 0 && result->magic == BX_NTVDM_CPU_RESULT_V2_MAGIC &&
        result->abi_version == BX_NTVDM_CPU_RESULT_V2_VERSION &&
        result->struct_bytes == sizeof(*result) &&
        (result->disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
         result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME ||
         result->disposition == BX_NTVDM_CPU_RESULT_V2_STOP ||
         result->disposition == BX_NTVDM_CPU_RESULT_V2_PENDING) &&
        bx_ntvdm_cpu_delta_v1_valid(&result->cpu_delta) &&
        (result->eflags_write_mask & ~(BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF |
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF)) == 0u &&
        (result->eflags_values & ~(BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF |
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF)) == 0u &&
        ((result->disposition != BX_NTVDM_CPU_RESULT_V2_STOP &&
          result->disposition != BX_NTVDM_CPU_RESULT_V2_PENDING) ||
         (result->resume_rip == 0u && result->cpu_delta.gpr16_write_mask == 0u &&
          result->cpu_delta.segment_write_mask == 0u &&
          result->eflags_write_mask == 0u && result->eflags_values == 0u));
}

int bx_ntvdm_cpu_result_v2_set_cf(bx_ntvdm_cpu_result_v2 *result, int value)
{
    if (!bx_ntvdm_cpu_result_v2_valid(result)) return 0;
    result->eflags_write_mask |= BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
    if (value) result->eflags_values |= BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
    else result->eflags_values &= ~BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
    return 1;
}

int bx_ntvdm_cpu_result_v2_set_zf(bx_ntvdm_cpu_result_v2 *result, int value)
{
    if (!bx_ntvdm_cpu_result_v2_valid(result)) return 0;
    result->eflags_write_mask |= BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF;
    if (value) result->eflags_values |= BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF;
    else result->eflags_values &= ~BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF;
    return 1;
}
