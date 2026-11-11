/*
 * Link-diagnostic trace guards for two source-unrecovered CCPU SAS vector
 * slots. Their behavior is not inferred here. A normal runtime must never
 * link this file. If a bounded diagnostic reaches either slot, record only
 * the ABI boundary and terminate before any guest-visible result exists.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

typedef enum runner_ccpu_sas_vector_slot {
    RUNNER_CCPU_SAS_VECTOR_TOUCH = 1,
    RUNNER_CCPU_SAS_VECTOR_VIRTUALISE_INSTRUCTION = 2
} runner_ccpu_sas_vector_slot;

typedef struct runner_ccpu_sas_vector_trace {
    uint32_t sequence;
    uint32_t slot;
    uint32_t first;
    uint32_t second;
    uint32_t third;
    uint32_t fourth;
    uintptr_t return_address;
} runner_ccpu_sas_vector_trace;

static runner_ccpu_sas_vector_trace last_trace;
static uint32_t trace_sequence;

static uintptr_t ccpu_sas_vector_return_address(void) {
#if defined(_MSC_VER)
    return (uintptr_t)_ReturnAddress();
#elif defined(__GNUC__) || defined(__clang__)
    return (uintptr_t)__builtin_return_address(0);
#else
    return 0;
#endif
}

static void ccpu_sas_vector_stop(runner_ccpu_sas_vector_slot slot,
                                 uint32_t first, uint32_t second,
                                 uint32_t third, uint32_t fourth,
                                 uintptr_t return_address) {
    last_trace.sequence = ++trace_sequence;
    last_trace.slot = (uint32_t)slot;
    last_trace.first = first;
    last_trace.second = second;
    last_trace.third = third;
    last_trace.fourth = fourth;
    last_trace.return_address = return_address;

    fprintf(stderr,
        "CCPU SAS trace: slot=%lu sequence=%lu return=%p args=%08lx,%08lx,%08lx,%08lx\n",
        (unsigned long)last_trace.slot, (unsigned long)last_trace.sequence,
        (void *)last_trace.return_address, (unsigned long)last_trace.first,
        (unsigned long)last_trace.second, (unsigned long)last_trace.third,
        (unsigned long)last_trace.fourth);
    abort();
}

uint8_t *c_sas_touch(uint32_t address, uint32_t length) {
    ccpu_sas_vector_stop(RUNNER_CCPU_SAS_VECTOR_TOUCH, address, length, 0, 0,
                         ccpu_sas_vector_return_address());
    return 0;
}

uint32_t c_VirtualiseInstruction(uint32_t eip_in_rom, uint16_t size,
                                 uint32_t linear_address_or_port,
                                 uint32_t data_in) {
    ccpu_sas_vector_stop(RUNNER_CCPU_SAS_VECTOR_VIRTUALISE_INSTRUCTION,
                         eip_in_rom, size, linear_address_or_port, data_in,
                         ccpu_sas_vector_return_address());
    return 0;
}
