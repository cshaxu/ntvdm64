#include "adapter-softpc/ccpu_frame_context.h"

#include <string.h>

static int read_byte(void *state, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    uint8_t *memory = (uint8_t *)state;
    if (memory == 0 || bytes == 0 || byte_count != 1u || address != 7u) {
        return 0;
    }
    bytes[0] = memory[7];
    return 1;
}

static int write_byte(void *state, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    uint8_t *memory = (uint8_t *)state;
    if (memory == 0 || bytes == 0 || byte_count != 1u || address != 7u) {
        return 0;
    }
    memory[7] = bytes[0];
    return 1;
}

int main(void)
{
    uint8_t memory[8];
    uint8_t loaded = 0u;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_ccpu_frame_context context;

    memset(memory, 0, sizeof(memory));
    memory[7] = 0xa5u;
    runtime_cpu_state_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    runtime_cpu_result_pass_through(&result);
    cpu.eax = 0x11223344u;
    cpu.cs = 0x0070u;
    cpu.eip = 0x0100u;

    memset(&context, 0, sizeof(context));
    context.magic = RUNTIME_CCPU_FRAME_CONTEXT_MAGIC;
    context.abi_version = RUNTIME_CCPU_FRAME_CONTEXT_VERSION;
    context.struct_bytes = sizeof(context);
    context.cpu = &cpu;
    context.result = &result;
    context.guest_state = memory;
    context.guest_read = read_byte;
    context.guest_write = write_byte;

    if (!runtime_ccpu_frame_context_begin(&context) ||
        runtime_ccpu_get_ax() != 0x3344u || runtime_ccpu_get_cs() != 0x0070u ||
        runtime_ccpu_get_ip() != 0x0100u) return 1;
    runtime_ccpu_set_ax(0x55aau);
    runtime_ccpu_set_al(0x66u);
    runtime_ccpu_set_ds(0x0080u);
    runtime_ccpu_set_cf(1);
    runtime_ccpu_set_ip(0x0200u);
    runtime_ccpu_sas_load(7u, &loaded);
    if (loaded != 0xa5u || cpu.eip != 0x0200u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x5566u ||
        result.cpu_delta.segment_write_mask != (1u << 3u) ||
        result.cpu_delta.segment_values[3] != 0x0080u ||
        result.eflags_write_mask != RUNTIME_CPU_RESULT_EFLAGS_CF ||
        result.eflags_values != RUNTIME_CPU_RESULT_EFLAGS_CF ||
        !runtime_ccpu_set_pending() ||
        result.disposition != RUNTIME_CPU_RESULT_PENDING) return 2;
    if (!runtime_ccpu_set_controlled_stop() ||
        result.disposition != RUNTIME_CPU_RESULT_STOP) return 3;
    runtime_ccpu_frame_context_end();
    return runtime_ccpu_get_ax() == 0u ? 0 : 4;
}
