#include <string.h>

#include "app/observation/command_bootstrap_observation.h"

typedef struct fixture_ram { uint8_t bytes[0x100000u]; } fixture_ram;

static int read_guest(void *state, uint64_t address, uint8_t *bytes, uint32_t count)
{
    fixture_ram *ram = (fixture_ram *)state;
    if (ram == NULL || bytes == NULL || address > sizeof(ram->bytes) ||
        count > sizeof(ram->bytes) - (uint32_t)address) return 0;
    memcpy(bytes, ram->bytes + (uint32_t)address, count);
    return 1;
}

int main(void)
{
    static fixture_ram ram;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    struct runtime_command_bootstrap_observation observation;
    uint32_t address = (uint32_t)((uint32_t)0x8e08u << 4) + 0x3466u;
    memset(&ram, 0, sizeof(ram));
    memcpy(ram.bytes + address, "O:\\DOS\\COMMAND.COM", 19u);
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event);
    event.execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event.cs = 0x8e08u; event.eip = 0x08afu;
    event.window_bytes = 3u; event.window[0] = 0xc4u;
    event.window[1] = 0xc4u; event.window[2] = 0x5eu;
    outcome.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome.disposition = RUNTIME_GENERIC_UD_RESUME;
    runtime_command_bootstrap_observation_enable(1u);
    runtime_command_bootstrap_observation_consider(&event, &outcome,
        read_guest, &ram);
    if (!runtime_command_bootstrap_observation_copy(&observation) ||
        observation.observed != 1u || observation.read_failed != 0u ||
        observation.physical_address != address ||
        strcmp((const char *)observation.command, "O:\\DOS\\COMMAND.COM") != 0)
        return 1;
    event.eip = 0x08b0u;
    memset(&observation, 0, sizeof(observation));
    runtime_command_bootstrap_observation_enable(1u);
    runtime_command_bootstrap_observation_consider(&event, &outcome,
        read_guest, &ram);
    if (!runtime_command_bootstrap_observation_copy(&observation) ||
        observation.observed != 0u || observation.read_failed != 0u) return 2;
    runtime_command_bootstrap_observation_enable(0u);
    return 0;
}
