#include <string.h>
#include "opennt-bop/observation/command_current_dir_observation.h"
typedef struct { uint8_t bytes[0x100000u]; } ram_t;
static int read_guest(void *state, uint64_t address, uint8_t *bytes, uint32_t count)
{ ram_t *ram = (ram_t *)state; if (!ram || !bytes || address > sizeof(ram->bytes) || count > sizeof(ram->bytes) - (uint32_t)address) return 0; memcpy(bytes, ram->bytes + (uint32_t)address, count); return 1; }
int main(void)
{
    ram_t ram; struct runtime_generic_ud_event event; struct runtime_generic_ud_outcome outcome; struct runtime_command_current_dir_observation value; uint32_t address = ((uint32_t)0x00a7u << 4) + 0x13f3u;
    memset(&ram, 0, sizeof(ram)); memcpy(ram.bytes + address, "O:\\repos.hobby", 15u);
    memset(&event, 0, sizeof(event)); event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC; event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION; event.struct_bytes = sizeof(event); event.execution_mode = RUNTIME_CPU_EXECUTION_REAL; event.ds = 0x00a7u; event.esi = 0x13f3u; event.window_bytes = 4u; event.window[0] = 0xc4u; event.window[1] = 0xc4u; event.window[2] = 0x54u; event.window[3] = 0x04u;
    memset(&outcome, 0, sizeof(outcome)); outcome.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION; outcome.disposition = RUNTIME_GENERIC_UD_RESUME;
    runtime_command_current_dir_observation_enable(1u); runtime_command_current_dir_observation_consider(&event, &outcome, read_guest, &ram);
    if (!runtime_command_current_dir_observation_copy(&value) || !value.observed || value.read_failed || value.physical_address != address || strcmp((char *)value.directory, "O:\\repos.hobby") != 0) return 1;
    event.window[3] = 0x05u; runtime_command_current_dir_observation_enable(1u); runtime_command_current_dir_observation_consider(&event, &outcome, read_guest, &ram); if (!runtime_command_current_dir_observation_copy(&value) || value.observed || value.read_failed) return 2;
    return 0;
}
