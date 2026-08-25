#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/spckbd_handoff_generic_ud_bridge.h"
#include "adapter-softpc/spckbd_handoff_shim.h"
#include "cpu_state_abi.h"

static unsigned char ram[0x100000];
static int readable = 1, writable = 1;

int runtime_machine_checked_ram_read(uint64_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    if (!readable || bytes == 0 || address > sizeof(ram) ||
        byte_count > sizeof(ram) - (size_t)address) return 0;
    memcpy(bytes, ram + (size_t)address, byte_count);
    return 1;
}

int runtime_machine_checked_ram_write(uint64_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    if (!writable || bytes == 0 || address > sizeof(ram) ||
        byte_count > sizeof(ram) - (size_t)address) return 0;
    memcpy(ram + (size_t)address, bytes, byte_count);
    return 1;
}

static void word_at(uint32_t address, uint16_t value)
{
    ram[address] = (uint8_t)value;
    ram[address + 1u] = (uint8_t)(value >> 8);
}

static void initialize(struct runtime_generic_ud_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event); event->vector = 6u;
    event->execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip = 0x765u; event->eax = 0xbeefu;
    event->cs = 0x200u; event->ds = 0x300u; event->esi = 0x100u;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x5fu;
    word_at(0x2100u + 17u * 2u, 0x0040u);
    word_at(0x2100u + 32u * 2u, 0xbeefu);
}

int main(void)
{
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    initialize(&event);
    runtime_spckbd_handoff_display_state_set(2u);
    if (!runtime_spckbd_handoff_generic_ud_recognizes(&event) ||
        !runtime_spckbd_handoff_generic_ud_dispatch(&event, &outcome) ||
        ram[0x3040u] != 2u || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x768u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 1u || outcome.eflags_values != 1u) return 1;
    ram[0x3040u] = 0x44u; word_at(0x2100u + 32u * 2u, 0u);
    if (runtime_spckbd_handoff_generic_ud_dispatch(&event, &outcome) ||
        ram[0x3040u] != 0x44u) return 2;
    word_at(0x2100u + 32u * 2u, 0xbeefu); writable = 0;
    if (runtime_spckbd_handoff_generic_ud_dispatch(&event, &outcome)) return 3;
    writable = 1; event.execution_mode = RUNTIME_CPU_EXECUTION_PROTECTED;
    if (runtime_spckbd_handoff_generic_ud_recognizes(&event)) return 4;
    event.execution_mode = RUNTIME_CPU_EXECUTION_REAL; event.eax = 0xbeeu;
    if (runtime_spckbd_handoff_generic_ud_recognizes(&event)) return 5;
    runtime_spckbd_handoff_reset();
    puts("T243 S2 source-shaped BOP 5F STREAM_IO handoff passes");
    return 0;
}
