#include "cpu_result.h"

void runtime_cpu_result_pass_through(runtime_cpu_result *result)
{
    if (result == 0) return;
    result->magic = RUNTIME_CPU_RESULT_MAGIC;
    result->abi_version = RUNTIME_CPU_RESULT_VERSION;
    result->struct_bytes = sizeof(*result);
    result->disposition = RUNTIME_CPU_RESULT_PASS_THROUGH;
    result->resume_rip = 0u;
    runtime_cpu_delta_initialize(&result->cpu_delta);
    result->eflags_write_mask = 0u;
    result->eflags_values = 0u;
}

int runtime_cpu_result_resume(runtime_cpu_result *result,
    uint64_t resume_rip)
{
    if (result == 0) return 0;
    runtime_cpu_result_pass_through(result);
    result->disposition = RUNTIME_CPU_RESULT_RESUME;
    result->resume_rip = resume_rip;
    return 1;
}

int runtime_cpu_result_stop(runtime_cpu_result *result)
{
    if (result == 0) return 0;
    runtime_cpu_result_pass_through(result);
    result->disposition = RUNTIME_CPU_RESULT_STOP;
    return 1;
}

int runtime_cpu_result_pending(runtime_cpu_result *result)
{
    if (result == 0) return 0;
    runtime_cpu_result_pass_through(result);
    result->disposition = RUNTIME_CPU_RESULT_PENDING;
    return 1;
}

int runtime_cpu_result_valid(const runtime_cpu_result *result)
{
    return result != 0 && result->magic == RUNTIME_CPU_RESULT_MAGIC &&
        result->abi_version == RUNTIME_CPU_RESULT_VERSION &&
        result->struct_bytes == sizeof(*result) &&
        (result->disposition == RUNTIME_CPU_RESULT_PASS_THROUGH ||
         result->disposition == RUNTIME_CPU_RESULT_RESUME ||
         result->disposition == RUNTIME_CPU_RESULT_STOP ||
         result->disposition == RUNTIME_CPU_RESULT_PENDING) &&
        runtime_cpu_delta_valid(&result->cpu_delta) &&
        (result->eflags_write_mask & ~(RUNTIME_CPU_RESULT_EFLAGS_CF |
            RUNTIME_CPU_RESULT_EFLAGS_ZF)) == 0u &&
        (result->eflags_values & ~(RUNTIME_CPU_RESULT_EFLAGS_CF |
            RUNTIME_CPU_RESULT_EFLAGS_ZF)) == 0u &&
        ((result->disposition != RUNTIME_CPU_RESULT_STOP &&
          result->disposition != RUNTIME_CPU_RESULT_PENDING) ||
         (result->resume_rip == 0u && result->cpu_delta.gpr16_write_mask == 0u &&
          result->cpu_delta.segment_write_mask == 0u &&
          result->eflags_write_mask == 0u && result->eflags_values == 0u));
}

int runtime_cpu_result_set_cf(runtime_cpu_result *result, int value)
{
    if (!runtime_cpu_result_valid(result)) return 0;
    result->eflags_write_mask |= RUNTIME_CPU_RESULT_EFLAGS_CF;
    if (value) result->eflags_values |= RUNTIME_CPU_RESULT_EFLAGS_CF;
    else result->eflags_values &= ~RUNTIME_CPU_RESULT_EFLAGS_CF;
    return 1;
}

int runtime_cpu_result_set_zf(runtime_cpu_result *result, int value)
{
    if (!runtime_cpu_result_valid(result)) return 0;
    result->eflags_write_mask |= RUNTIME_CPU_RESULT_EFLAGS_ZF;
    if (value) result->eflags_values |= RUNTIME_CPU_RESULT_EFLAGS_ZF;
    else result->eflags_values &= ~RUNTIME_CPU_RESULT_EFLAGS_ZF;
    return 1;
}
