#include <string.h>

#include "opennt-bop/observation/dem_open_observation.h"

typedef struct { uint8_t bytes[0x100000u]; } ram_t;

static ram_t fixture_ram;

static int read_guest(void *state, uint64_t address, uint8_t *bytes,
    uint32_t count)
{
    ram_t *ram = state;
    if (!ram || !bytes || address > sizeof(ram->bytes) ||
        count > sizeof(ram->bytes) - (uint32_t)address) return 0;
    memcpy(bytes, ram->bytes + (uint32_t)address, count);
    return 1;
}

int main(void)
{
    ram_t *ram = &fixture_ram;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    struct runtime_dem_open_observation value;
    uint32_t address = ((uint32_t)0x00a7u << 4) + 0x03d2u;

    memset(ram, 0, sizeof(*ram));
    memcpy(ram->bytes + address, "O:\\X\\CONFIG.TMP", 16u);
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event);
    event.execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event.ds = 0x00a7u;
    event.esi = 0x03d2u;
    event.ebx = 2u;
    event.window_bytes = 4u;
    event.window[0] = 0xc4u;
    event.window[1] = 0xc4u;
    event.window[2] = 0x50u;
    event.window[3] = 0x12u;
    memset(&outcome, 0, sizeof(outcome));
    outcome.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome.disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome.gpr16_write_mask = 1u;
    outcome.gpr16_values[0] = 0x1234u;

    runtime_dem_open_observation_enable(1u);
    runtime_dem_open_observation_consider(&event, &outcome, read_guest, ram);
    memcpy(ram->bytes + address, "O:\\X\\COMMAND.COM", 17u);
    runtime_dem_open_observation_consider(&event, &outcome, read_guest, ram);
    if (!runtime_dem_open_observation_copy(&value) || !value.observed ||
        value.read_failed || value.observed_count != 2u ||
        value.physical_address != address || value.mode != 2u ||
        value.gpr16_values[0] != 0x1234u ||
        strcmp((char *)value.path, "O:\\X\\COMMAND.COM")) return 1;

    event.window[3] = 0x13u;
    runtime_dem_open_observation_enable(1u);
    runtime_dem_open_observation_consider(&event, &outcome, read_guest, ram);
    if (!runtime_dem_open_observation_copy(&value) || value.observed ||
        value.read_failed || value.observed_count) return 2;
    return 0;
}
