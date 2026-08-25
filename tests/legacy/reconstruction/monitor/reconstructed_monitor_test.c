#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "reconstructed_monitor.h"

typedef struct refused_bridge_observation {
    unsigned calls;
    uint8_t selector;
    uint8_t next_byte;
    uint32_t physical_fetch;
    uint32_t physical_next;
} refused_bridge_observation;

static int refused_bridge(
    void *context,
    const reconstructed_monitor_bridge_request *request,
    reconstructed_monitor_bridge_response *response)
{
    refused_bridge_observation *observation = context;

    (void)response;
    ++observation->calls;
    observation->selector = request->selector;
    observation->next_byte = request->next_byte;
    observation->physical_fetch = request->physical_fetch;
    observation->physical_next = request->physical_next;
    return 0;
}

static int demgetdrives_shape_bridge(
    void *context,
    const reconstructed_monitor_bridge_request *request,
    reconstructed_monitor_bridge_response *response)
{
    unsigned *calls = context;

    assert(request->selector == 0x50u);
    assert(request->next_byte == 0x0fu);
    assert(request->state.ip == 3u);
    ++*calls;
    response->write_mask = RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_IP | RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF;
    response->ax = 5u;
    response->ip = 4u;
    response->carry = 0u;
    return 1;
}

static int invalid_carry_bridge(
    void *context,
    const reconstructed_monitor_bridge_request *request,
    reconstructed_monitor_bridge_response *response)
{
    (void)context;
    assert(request->selector == 0x50u);
    assert(request->next_byte == 0x0fu);
    response->write_mask = RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF;
    response->ax = 5u;
    response->carry = 2u;
    return 1;
}

int main(void)
{
    uint8_t ram[] = {0x90u, 0xd6u, 0x27u, 0xaau};
    uint8_t exit_ram[] = {0xc4u, 0xc4u, 0xfeu};
    uint8_t fast_bop_ram[] = {0xc4u, 0xc5u, 0xfeu, 0x03u};
    uint8_t unknown_ram[] = {0x91u};
    uint8_t xor_ram[] = {0x33u, 0xc0u};
    const uint8_t ntio_entry_bytes[] = {0xe9u, 0x6du, 0x03u};
    const uint8_t ntio_init_prefix_bytes[] = {
        0x1eu, 0x50u, 0xb8u, 0x40u, 0x00u, 0x8eu, 0xd8u, 0x9fu,
        0xf7u, 0x06u, 0x14u, 0x03u, 0x00u, 0x24u, 0x75u, 0x0cu,
        0xf0u, 0x81u, 0x26u, 0x14u, 0x03u, 0xffu, 0xfdu, 0x9eu,
        0x58u, 0x1fu, 0xebu, 0x03u, 0xfau, 0xebu, 0xf8u, 0x33u,
        0xc0u, 0x8eu, 0xd8u, 0x0eu, 0x07u, 0xb9u, 0x04u, 0x00u,
        0xbeu, 0xeau, 0x00u, 0x2eu, 0xacu, 0x98u, 0xd1u, 0xe0u,
        0xd1u, 0xe0u, 0x8bu, 0xf8u, 0x87u, 0xf7u, 0xadu, 0xabu,
        0xadu, 0xabu, 0x87u, 0xf7u, 0xe2u, 0xedu, 0xc7u, 0x06u,
        0x54u, 0x00u, 0xedu, 0x02u, 0x8cu, 0x0eu, 0x56u, 0x00u,
        0xc7u, 0x06u, 0x64u, 0x00u, 0xa8u, 0x02u, 0x8cu, 0x0eu,
        0x66u, 0x00u, 0x33u, 0xd2u, 0x8eu, 0xd2u, 0xbcu, 0x00u,
        0x07u, 0x1eu, 0x50u, 0xb8u, 0x40u, 0x00u, 0x8eu, 0xd8u,
        0x9fu, 0xf7u, 0x06u, 0x14u, 0x03u, 0x03u, 0x00u, 0x75u,
        0x14u, 0xf7u, 0x06u, 0x14u, 0x03u, 0x00u, 0x24u, 0x75u,
        0x0cu, 0xf0u, 0x81u, 0x0eu, 0x14u, 0x03u, 0x00u, 0x02u,
        0x9eu, 0x58u, 0x1fu, 0xebu, 0x03u, 0xfbu, 0xebu, 0xf8u,
        0x33u, 0xd2u, 0x8eu, 0xdau, 0x8eu, 0xc2u, 0x33u, 0xc0u,
        0xbfu, 0x34u, 0x05u, 0xabu, 0xabu, 0x8cu, 0xc8u, 0xc7u,
        0x06u, 0x6cu, 0x00u, 0x85u, 0x01u, 0xa3u, 0x6eu, 0x00u,
        0xc7u, 0x06u, 0xa4u, 0x00u, 0x54u, 0x02u, 0xa3u, 0xa6u,
        0x00u, 0xbfu, 0x04u, 0x00u, 0xbbu, 0x8bu, 0x01u, 0x93u,
        0xabu, 0x93u, 0xabu, 0x83u, 0xc7u, 0x04u, 0x93u, 0xabu,
        0x93u, 0xabu, 0x93u, 0xabu, 0x93u, 0xabu, 0x89u, 0x16u,
        0x00u, 0x05u, 0x89u, 0x16u, 0x04u, 0x05u, 0xbau, 0x0du,
        0x01u, 0x8eu, 0xdau, 0xc7u, 0x06u, 0x05u, 0x03u, 0x24u,
        0x00u, 0x8cu, 0x0eu, 0x07u, 0x03u, 0xc7u, 0x06u, 0x03u,
        0x03u, 0xb0u, 0x08u, 0x33u, 0xc9u, 0x8eu, 0xd9u, 0x8au,
        0x0eu, 0x96u, 0x04u, 0xf6u, 0xc1u, 0x10u, 0x74u, 0x0au,
        0xc6u, 0x06u, 0x06u, 0x01u, 0x10u, 0xc6u, 0x06u, 0x07u,
        0x01u, 0x11u, 0x0eu, 0x1fu, 0x0eu, 0x07u, 0xbfu, 0x66u,
        0x03u, 0xd1u, 0xefu, 0xd1u, 0xefu, 0xd1u, 0xefu, 0xd1u,
        0xefu, 0x47u, 0x81u, 0xc7u, 0x70u, 0x00u, 0x89u, 0x3eu,
        0x03u, 0x00u, 0xbfu, 0xb0u, 0x08u, 0xc4u, 0xc4u, 0x50u,
        0x11u
    };
    uint8_t ntio_prefix_ram[0x2000] = {0};
    reconstructed_monitor_monitor monitor;
    reconstructed_monitor_state state;
    reconstructed_monitor_stop_event stop;
    refused_bridge_observation observation = {0};

    {
        uint8_t demgetdrives_ram[] = {0xc4u, 0xc4u, 0x50u, 0x0fu};
        unsigned bridge_calls = 0u;

        assert(reconstructed_monitor_monitor_initialize(&monitor, demgetdrives_ram,
            sizeof(demgetdrives_ram), demgetdrives_shape_bridge, &bridge_calls) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a50000u;
        state.flags = RECONSTRUCTED_MONITOR_FLAG_CF | RECONSTRUCTED_MONITOR_FLAG_IF | 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(bridge_calls == 1u);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.eax == 0xa5a50005u && state.ip == 4u);
        assert((state.flags & RECONSTRUCTED_MONITOR_FLAG_CF) == 0u);
        assert((state.flags & (RECONSTRUCTED_MONITOR_FLAG_IF | 0x0002u)) ==
            (RECONSTRUCTED_MONITOR_FLAG_IF | 0x0002u));
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_NO_STOP_EVENT);
    }

    {
        uint8_t invalid_carry_ram[] = {0xc4u, 0xc4u, 0x50u, 0x0fu};

        assert(reconstructed_monitor_monitor_initialize(&monitor, invalid_carry_ram,
            sizeof(invalid_carry_ram), invalid_carry_bridge, NULL) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a50000u;
        state.flags = RECONSTRUCTED_MONITOR_FLAG_CF | RECONSTRUCTED_MONITOR_FLAG_IF | 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.eax == 0xa5a50000u && state.ip == 3u);
        assert(state.flags == (RECONSTRUCTED_MONITOR_FLAG_CF | RECONSTRUCTED_MONITOR_FLAG_IF | 0x0002u));
    }

    assert(reconstructed_monitor_monitor_initialize(&monitor, ram, sizeof(ram), refused_bridge, &observation) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_NO_STOP_EVENT);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 1u && state.flags == 0x0002u);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
    assert(observation.calls == 1u && observation.selector == 0x27u && observation.next_byte == 0xaau);
    assert(observation.physical_fetch == 1u && observation.physical_next == 3u);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 3u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
    assert(stop.bridge_disposition == RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_REFUSED);
    assert(stop.cs == 0u && stop.ip == 1u && stop.physical_fetch_valid == 1u);
    assert(stop.physical_fetch == 1u && stop.opcode_bytes == 3u);
    assert(stop.opcode_window[0] == 0xd6u && stop.opcode_window[1] == 0x27u &&
        stop.opcode_window[2] == 0xaau);

    assert(reconstructed_monitor_monitor_initialize(&monitor, exit_ram, sizeof(exit_ram), NULL, NULL) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BOP_EXIT);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 3u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(stop.reason == RECONSTRUCTED_MONITOR_BOP_EXIT);
    assert(stop.bridge_disposition == RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_NOT_ATTEMPTED);
    assert(stop.cs == 0u && stop.ip == 0u && stop.physical_fetch == 0u);
    assert(stop.opcode_bytes == 3u && stop.opcode_window[0] == 0xc4u &&
        stop.opcode_window[1] == 0xc4u && stop.opcode_window[2] == 0xfeu);

    assert(reconstructed_monitor_monitor_initialize(&monitor, fast_bop_ram, sizeof(fast_bop_ram), NULL, NULL) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_UNSUPPORTED_BOP);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 4u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(stop.reason == RECONSTRUCTED_MONITOR_UNSUPPORTED_BOP && stop.opcode_bytes == 4u);

    assert(reconstructed_monitor_monitor_initialize(&monitor, unknown_ram, sizeof(unknown_ram), NULL, NULL) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(stop.reason == RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE && stop.opcode_bytes == 1u);

    assert(reconstructed_monitor_monitor_initialize(&monitor, xor_ram, sizeof(xor_ram), NULL, NULL) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    state.eax = 0xa5a51234u;
    state.ebx = state.ecx = state.edx = state.esi = state.edi = state.ebp = state.esp = 0u;
    state.cs = state.ds = state.es = state.ss = state.ip = 0u;
    state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF);
    assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 2u && state.eax == 0xa5a50000u);
    assert((state.flags & RECONSTRUCTED_MONITOR_FLAG_ZF) != 0u);

    memcpy(ntio_prefix_ram + 0x700u, ntio_entry_bytes, sizeof(ntio_entry_bytes));
    memcpy(ntio_prefix_ram + 0xa70u, ntio_init_prefix_bytes, sizeof(ntio_init_prefix_bytes));
    ntio_prefix_ram[0x714u] = 0x00u;
    ntio_prefix_ram[0x715u] = 0x02u;
    ntio_prefix_ram[0x7eau] = 0x10u;
    ntio_prefix_ram[0x7efu] = 0x15u;
    ntio_prefix_ram[0x7f4u] = 0x19u;
    ntio_prefix_ram[0x7f9u] = 0x1bu;
    ntio_prefix_ram[0x40u] = 0xa0u; ntio_prefix_ram[0x41u] = 0x10u;
    ntio_prefix_ram[0x42u] = 0xc0u; ntio_prefix_ram[0x43u] = 0xf0u;
    ntio_prefix_ram[0x54u] = 0xa5u; ntio_prefix_ram[0x55u] = 0x15u;
    ntio_prefix_ram[0x56u] = 0xc5u; ntio_prefix_ram[0x57u] = 0xf5u;
    ntio_prefix_ram[0x64u] = 0xa9u; ntio_prefix_ram[0x65u] = 0x19u;
    ntio_prefix_ram[0x66u] = 0xc9u; ntio_prefix_ram[0x67u] = 0xf9u;
    ntio_prefix_ram[0x6cu] = 0xadu; ntio_prefix_ram[0x6du] = 0x1bu;
    ntio_prefix_ram[0x6eu] = 0xddu; ntio_prefix_ram[0x6fu] = 0xfbu;
    assert(reconstructed_monitor_monitor_initialize(&monitor, ntio_prefix_ram, sizeof(ntio_prefix_ram), NULL, NULL) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    state.eax = 0xbeef1234u;
    state.ebx = state.ecx = state.edx = state.esi = state.edi = state.ebp = 0u;
    state.esp = 0x00000080u;
    state.cs = 0x0070u;
    state.ds = 0x1234u;
    state.es = 0x5678u;
    state.ss = 0u;
    state.ip = 0u;
    state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF);
    assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 6u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x0378u && state.ds == 0x0040u && state.es == 0x5678u && state.cs == 0x0070u);
    assert(state.esp == 0x0000007cu && state.eax == 0xbeef0240u);
    assert((state.flags & RECONSTRUCTED_MONITOR_FLAG_IF) != 0u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_NO_STOP_EVENT);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x037eu && (state.flags & RECONSTRUCTED_MONITOR_FLAG_ZF) != 0u);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x0380u);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(ntio_prefix_ram[0x714u] == 0u && ntio_prefix_ram[0x715u] == 0u);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x039bu && state.eax == 0xbeef0000u && state.ecx == 4u && state.esi == 0xeau);
    assert(state.ds == 0u && state.es == 0x0070u && state.esp == 0x00000080u);
    assert((state.flags & (RECONSTRUCTED_MONITOR_FLAG_IF | RECONSTRUCTED_MONITOR_FLAG_ZF)) ==
        (RECONSTRUCTED_MONITOR_FLAG_IF | RECONSTRUCTED_MONITOR_FLAG_ZF));
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x039du && state.eax == 0xbeef0010u && state.esi == 0x00ebu);
    assert(reconstructed_monitor_monitor_run(&monitor, 47u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x03aeu && state.ecx == 0u && state.esi == 0x00feu && state.edi == 0x0070u);
    assert(ntio_prefix_ram[0x7ebu] == 0xa0u && ntio_prefix_ram[0x7ecu] == 0x10u &&
        ntio_prefix_ram[0x7edu] == 0xc0u && ntio_prefix_ram[0x7eeu] == 0xf0u);
    assert(ntio_prefix_ram[0x7f0u] == 0xa5u && ntio_prefix_ram[0x7f1u] == 0x15u &&
        ntio_prefix_ram[0x7f2u] == 0xc5u && ntio_prefix_ram[0x7f3u] == 0xf5u);
    assert(ntio_prefix_ram[0x7f5u] == 0xa9u && ntio_prefix_ram[0x7f6u] == 0x19u &&
        ntio_prefix_ram[0x7f7u] == 0xc9u && ntio_prefix_ram[0x7f8u] == 0xf9u);
    assert(ntio_prefix_ram[0x7fau] == 0xadu && ntio_prefix_ram[0x7fbu] == 0x1bu &&
        ntio_prefix_ram[0x7fcu] == 0xddu && ntio_prefix_ram[0x7fdu] == 0xfbu);
    assert(reconstructed_monitor_monitor_run(&monitor, 7u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x03c9u && state.edx == 0u && state.ss == 0u && state.esp == 0x00000700u);
    assert(ntio_prefix_ram[0x54u] == 0xedu && ntio_prefix_ram[0x55u] == 0x02u &&
        ntio_prefix_ram[0x56u] == 0x70u && ntio_prefix_ram[0x57u] == 0u);
    assert(ntio_prefix_ram[0x64u] == 0xa8u && ntio_prefix_ram[0x65u] == 0x02u &&
        ntio_prefix_ram[0x66u] == 0x70u && ntio_prefix_ram[0x67u] == 0u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_NO_STOP_EVENT);
    assert(reconstructed_monitor_monitor_run(&monitor, 14u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x03f0u && state.ds == 0u && state.esp == 0x00000700u &&
        state.eax == 0xbeeffbddu);
    assert(ntio_prefix_ram[0x714u] == 0u && ntio_prefix_ram[0x715u] == 0x02u);
    assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x03f2u && state.edx == 0u);
    assert(reconstructed_monitor_monitor_run(&monitor, 52u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
    assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(state.ip == 0x0478u && state.ds == 0x0070u && state.es == 0x0070u &&
        state.edi == 0x08b0u && state.eax == 0xbeef0070u);
    assert(ntio_prefix_ram[0x6cu] == 0x85u && ntio_prefix_ram[0x6du] == 0x01u &&
        ntio_prefix_ram[0x6eu] == 0x70u && ntio_prefix_ram[0x6fu] == 0u);
    assert(ntio_prefix_ram[0xa4u] == 0x54u && ntio_prefix_ram[0xa5u] == 0x02u &&
        ntio_prefix_ram[0xa6u] == 0x70u && ntio_prefix_ram[0xa7u] == 0u);
    assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
    assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED && stop.bridge_disposition ==
        RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_REFUSED);
    assert(stop.cs == 0x0070u && stop.ip == 0x0475u && stop.physical_fetch == 0x0b75u);
    assert(stop.opcode_bytes == 4u && stop.opcode_window[0] == 0xc4u &&
        stop.opcode_window[1] == 0xc4u && stop.opcode_window[2] == 0x50u &&
        stop.opcode_window[3] == 0x11u);

    {
        /* Fixed-source NTDOS DosInit prefix through its first SVC, 0x50/0x0f. */
        static const uint8_t dosinit_prefix[] = {
            0xfcu, 0x52u, 0x56u, 0x1eu, 0x57u, 0x8cu, 0xc3u, 0xb8u,
            0x39u, 0x79u, 0x05u, 0x0fu, 0x00u, 0x25u, 0xf0u, 0xffu,
            0x8bu, 0xf0u, 0x8cu, 0xc8u, 0x8eu, 0xd8u, 0x2eu, 0x8eu,
            0x06u, 0xeeu, 0x73u, 0x26u, 0x8eu, 0x06u, 0x03u, 0x00u,
            0x33u, 0xffu, 0xb9u, 0xe2u, 0x15u, 0xf3u, 0xa4u, 0x5fu,
            0x1fu, 0x5eu, 0x5au, 0x06u, 0x1eu, 0x07u, 0x1fu, 0x06u,
            0x8eu, 0xc3u, 0x26u, 0x8au, 0x45u, 0x01u, 0xa2u, 0x80u,
            0x14u, 0x07u, 0xc4u, 0xc4u, 0x50u, 0x0fu
        };
        uint8_t dosinit_ram[0x20000] = {0};
        refused_bridge_observation dosinit_observation = {0};
        size_t index;

        memcpy(dosinit_ram + 0x10000u, dosinit_prefix, sizeof(dosinit_prefix));
        dosinit_ram[0x173eeu] = 0x00u;
        dosinit_ram[0x173efu] = 0x11u;
        dosinit_ram[0x11003u] = 0x00u;
        dosinit_ram[0x11004u] = 0x12u;
        for (index = 0u; index < 0x15e2u; ++index) {
            dosinit_ram[0x17940u + index] = (uint8_t)(index * 7u);
        }
        dosinit_ram[0x14021u] = 0x4du;

        assert(reconstructed_monitor_monitor_initialize(&monitor, dosinit_ram, sizeof(dosinit_ram),
            refused_bridge, &dosinit_observation) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a50000u;
        state.ebx = state.ecx = 0u;
        state.edx = 0x0280u;
        state.esi = 0x0010u;
        state.edi = 0x0020u;
        state.ebp = 0u;
        state.esp = 0x0100u;
        state.cs = 0x1000u;
        state.ds = 0x1300u;
        state.es = 0x1400u;
        state.ss = 0x1500u;
        state.ip = 0u;
        state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_DF);
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 31u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
        assert(dosinit_observation.calls == 1u && dosinit_observation.selector == 0x50u &&
            dosinit_observation.next_byte == 0x0fu);
        assert(dosinit_observation.physical_fetch == 0x10000u + sizeof(dosinit_prefix) - 4u &&
            dosinit_observation.physical_next == 0x10000u + sizeof(dosinit_prefix) - 1u);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.cs == 0x1000u && state.ds == 0x1200u && state.es == 0x1300u &&
            state.ss == 0x1500u && state.ip == sizeof(dosinit_prefix) - 1u);
        assert(state.edx == 0x0280u && state.esi == 0x0010u && state.edi == 0x0020u &&
            state.esp == 0x0100u && (uint16_t)state.ecx == 0u);
        assert((state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) == 0u);
        assert(dosinit_ram[0x13480u] == 0x4du);
        assert(memcmp(dosinit_ram + 0x12000u, dosinit_ram + 0x17940u, 0x1480u) == 0);
        assert(memcmp(dosinit_ram + 0x13481u, dosinit_ram + 0x18dc1u, 0x15e2u - 0x1481u) == 0);
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED && stop.bridge_disposition ==
            RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_REFUSED);
        assert(stop.cs == 0x1000u && stop.ip == sizeof(dosinit_prefix) - 4u &&
            stop.physical_fetch == 0x10000u + sizeof(dosinit_prefix) - 4u);
        assert(stop.opcode_bytes == 4u && stop.opcode_window[0] == 0xc4u &&
            stop.opcode_window[1] == 0xc4u && stop.opcode_window[2] == 0x50u &&
            stop.opcode_window[3] == 0x0fu);
    }

    {
        /* Fixed-source normal DosInit bytes after 50h/0fh through 50h/1bh. */
        uint8_t dosinit_after_drives[0x20000] = {0};
        static const uint8_t prefix[] = {
            0xa2u, 0x46u, 0x00u, 0xf6u, 0x06u, 0x80u, 0x14u, 0x01u,
            0x74u, 0x2eu
        };
        static const uint8_t normal_path[] = {
            0x52u, 0xb8u, 0x2eu, 0x03u, 0xbau, 0x32u, 0x03u, 0x51u,
            0xb9u, 0x25u, 0x03u, 0x56u, 0xbeu, 0x82u, 0x14u,
            0xc4u, 0xc4u, 0x50u, 0x1bu
        };
        refused_bridge_observation after_drives_observation = {0};

        memcpy(dosinit_after_drives + 0x10000u, prefix, sizeof(prefix));
        memcpy(dosinit_after_drives + 0x10000u + sizeof(prefix) + 0x2eu, normal_path,
            sizeof(normal_path));
        assert(reconstructed_monitor_monitor_initialize(&monitor, dosinit_after_drives,
            sizeof(dosinit_after_drives), refused_bridge, &after_drives_observation) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a50005u; /* AL is the original demGetDrives result. */
        state.ebx = 0u;
        state.ecx = 0x00001234u;
        state.edx = 0x00005678u;
        state.esi = 0x00009abcu;
        state.edi = state.ebp = 0u;
        state.esp = 0x00000100u;
        state.cs = 0x1000u;
        state.ds = state.es = state.ss = state.ip = 0u;
        state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF);
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 11u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
        assert(after_drives_observation.calls == 1u &&
            after_drives_observation.selector == 0x50u &&
            after_drives_observation.next_byte == 0x1bu);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.ip == (uint16_t)(sizeof(prefix) + 0x2eu + sizeof(normal_path) - 1u));
        assert((uint16_t)state.eax == 0x032eu);
        assert((uint16_t)state.edx == 0x0332u);
        assert((uint16_t)state.ecx == 0x0325u);
        assert((uint16_t)state.esi == 0x1482u);
        assert((uint16_t)state.esp == 0x00fau);
        assert(dosinit_after_drives[0x0046u] == 5u &&
            (state.flags & RECONSTRUCTED_MONITOR_FLAG_ZF) != 0u);
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED && stop.cs == 0x1000u &&
            stop.ip == (uint16_t)(sizeof(prefix) + 0x2eu + sizeof(normal_path) - 4u));
    }

    {
        /* Fixed-source normal DosInit bytes after completed 50h/1bh through 50h/32h. */
        static const uint8_t after_dta[] = {
            0x5eu, 0xbau, 0xf9u, 0x12u, 0x53u, 0xbbu, 0x48u, 0x00u,
            0xc4u, 0xc4u, 0x50u, 0x32u
        };
        uint8_t after_dta_ram[0x20000] = {0};
        refused_bridge_observation after_dta_observation = {0};

        memcpy(after_dta_ram + 0x10000u, after_dta, sizeof(after_dta));
        after_dta_ram[0x00fau] = 0xbcu;
        after_dta_ram[0x00fbu] = 0x9au;
        assert(reconstructed_monitor_monitor_initialize(&monitor, after_dta_ram, sizeof(after_dta_ram),
            refused_bridge, &after_dta_observation) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a5032eu;
        state.ebx = 0xface4567u;
        state.ecx = 0xbeef0325u;
        state.edx = 0xabcd0332u;
        state.esi = 0x12341482u;
        state.edi = state.ebp = 0u;
        state.esp = 0x000000fau;
        state.cs = 0x1000u;
        state.ds = state.es = state.ss = state.ip = 0u;
        state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF | RECONSTRUCTED_MONITOR_FLAG_ZF);
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 5u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
        assert(after_dta_observation.calls == 1u && after_dta_observation.selector == 0x50u &&
            after_dta_observation.next_byte == 0x32u);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.ip == sizeof(after_dta) - 1u && (uint16_t)state.esi == 0x9abcu &&
            (uint16_t)state.edx == 0x12f9u && (uint16_t)state.ebx == 0x0048u &&
            (uint16_t)state.esp == 0x00fau);
        assert(after_dta_ram[0x00fau] == 0x67u && after_dta_ram[0x00fbu] == 0x45u);
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED && stop.cs == 0x1000u && stop.ip == 8u &&
            stop.opcode_bytes == 4u && stop.opcode_window[0] == 0xc4u &&
            stop.opcode_window[1] == 0xc4u && stop.opcode_window[2] == 0x50u &&
            stop.opcode_window[3] == 0x32u);
    }

    {
        /* Fixed-source normal DosInit bytes after completed 50h/32h through 54h/05h. */
        static const uint8_t after_hard_error[] = {
            0xbau, 0xfcu, 0x12u, 0xbbu, 0xccu, 0x13u, 0xb9u, 0xd0u, 0x13u,
            0xc4u, 0xc4u, 0x54u, 0x05u
        };
        uint8_t after_hard_error_ram[0x20000] = {0};
        refused_bridge_observation after_hard_error_observation = {0};

        memcpy(after_hard_error_ram + 0x10000u, after_hard_error, sizeof(after_hard_error));
        assert(reconstructed_monitor_monitor_initialize(&monitor, after_hard_error_ram,
            sizeof(after_hard_error_ram), refused_bridge, &after_hard_error_observation) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        state.eax = 0xa5a5032eu;
        state.ebx = 0xface0048u;
        state.ecx = 0xbeef0325u;
        state.edx = 0xabcd12f9u;
        state.esi = 0x12349abcu;
        state.edi = state.ebp = state.esp = 0u;
        state.cs = 0x1000u;
        state.ds = state.es = state.ss = state.ip = 0u;
        state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF);
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 4u) == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED);
        assert(after_hard_error_observation.calls == 1u &&
            after_hard_error_observation.selector == 0x54u &&
            after_hard_error_observation.next_byte == 0x05u);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.ip == sizeof(after_hard_error) - 1u && (uint16_t)state.edx == 0x12fcu &&
            (uint16_t)state.ebx == 0x13ccu && (uint16_t)state.ecx == 0x13d0u);
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(stop.reason == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED && stop.cs == 0x1000u && stop.ip == 9u &&
            stop.opcode_bytes == 4u && stop.opcode_window[0] == 0xc4u &&
            stop.opcode_window[1] == 0xc4u && stop.opcode_window[2] == 0x54u &&
            stop.opcode_window[3] == 0x05u);
    }

    {
        uint8_t far_call_ram[64] = {0xffu, 0x1eu, 0x10u, 0x00u, 0xc4u, 0xc4u, 0xfeu};

        far_call_ram[0x10u] = 4u;
        far_call_ram[0x11u] = 0u;
        far_call_ram[0x12u] = 0u;
        far_call_ram[0x13u] = 0u;
        assert(reconstructed_monitor_monitor_initialize(&monitor, far_call_ram, sizeof(far_call_ram), NULL, NULL) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        memset(&state, 0, sizeof(state));
        state.ss = 0u;
        state.esp = 0x20u;
        state.flags = 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 2u) == RECONSTRUCTED_MONITOR_BOP_EXIT);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.cs == 0u && state.ip == 7u && (uint16_t)state.esp == 0x1cu);
        assert(far_call_ram[0x1cu] == 4u && far_call_ram[0x1du] == 0u &&
            far_call_ram[0x1eu] == 0u && far_call_ram[0x1fu] == 0u);
    }

    {
        uint8_t far_call_fault_ram[32] = {0xffu, 0x1eu, 0x10u, 0x00u, 0x90u};

        far_call_fault_ram[0x10u] = 4u;
        assert(reconstructed_monitor_monitor_initialize(&monitor, far_call_fault_ram,
            sizeof(far_call_fault_ram), NULL, NULL) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        memset(&state, 0, sizeof(state));
        state.ss = 0u;
        state.esp = 2u;
        state.cs = 0u;
        state.ip = 0u;
        state.flags = 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_MEMORY_FAULT);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.cs == 0u && state.ip == 0u && (uint16_t)state.esp == 2u);
    }

    {
        uint8_t retf_ram[] = {0xcbu, 0x90u, 0x90u, 0xc4u, 0xc4u, 0xfeu,
            0u, 0u, 3u, 0u, 0u, 0u};

        assert(reconstructed_monitor_monitor_initialize(&monitor, retf_ram, sizeof(retf_ram), NULL, NULL) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        memset(&state, 0, sizeof(state));
        state.ss = 0u;
        state.esp = 8u;
        state.flags = 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 2u) == RECONSTRUCTED_MONITOR_BOP_EXIT);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.cs == 0u && state.ip == 6u && (uint16_t)state.esp == 12u);
    }

    {
        uint8_t retf_fault_ram[] = {0xcbu, 0x90u, 0x90u};

        assert(reconstructed_monitor_monitor_initialize(&monitor, retf_fault_ram, sizeof(retf_fault_ram), NULL, NULL) ==
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        memset(&state, 0, sizeof(state));
        state.ss = 0u;
        state.esp = 2u;
        state.cs = 0u;
        state.ip = 0u;
        state.flags = 0x0002u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 1u) == RECONSTRUCTED_MONITOR_MEMORY_FAULT);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.cs == 0u && state.ip == 0u && (uint16_t)state.esp == 2u);
    }

    {
        /* Exact NTDOS.SYS bytes from the first instruction after 54h/05h to call charinit. */
        static const uint8_t after_cmdsetinfo[] = {
            0x5bu, 0x59u, 0x5au, 0x89u, 0x3eu, 0x1eu, 0x10u, 0x89u, 0x1eu, 0x20u, 0x10u,
            0x2eu, 0x8cu, 0x1eu, 0x57u, 0x22u, 0x2eu, 0x8cu, 0x1eu, 0x47u, 0x4fu,
            0x2eu, 0x8cu, 0x1eu, 0x4bu, 0x4fu, 0x2eu, 0x8cu, 0x1eu, 0x4fu, 0x4fu,
            0x89u, 0x16u, 0x48u, 0x03u, 0x89u, 0x26u, 0x86u, 0x05u, 0x8cu, 0x16u,
            0x88u, 0x05u, 0x8cu, 0xd8u, 0x8eu, 0xd0u, 0xbcu, 0x22u, 0x09u, 0xc7u,
            0x06u, 0xdau, 0x11u, 0xf4u, 0x5bu, 0xc7u, 0x06u, 0xdcu, 0x11u, 0xf4u,
            0x5bu, 0xc7u, 0x06u, 0x61u, 0x00u, 0xf4u, 0x5bu, 0x8cu, 0xc8u, 0xa3u,
            0xacu, 0x0au, 0x8cu, 0x06u, 0x4au, 0x00u, 0x89u, 0x36u, 0x48u, 0x00u,
            0x8cu, 0x1eu, 0xd0u, 0x0fu, 0x56u, 0xb9u, 0x07u, 0x00u, 0xbeu, 0xd4u,
            0x0fu, 0x8cu, 0x1cu, 0x83u, 0xc6u, 0x06u, 0xe2u, 0xf9u, 0xb9u, 0x05u,
            0x00u, 0xbeu, 0xbeu, 0x12u, 0x8cu, 0x1cu, 0x83u, 0xc6u, 0x06u, 0xe2u,
            0xf9u, 0x5eu, 0x06u, 0x1fu, 0x1eu, 0x33u, 0xc0u, 0x8eu, 0xd8u, 0xb8u,
            0xedu, 0x73u, 0xa3u, 0xa8u, 0x00u, 0x8cu, 0xc8u, 0xa3u, 0xaau, 0x00u,
            0x1fu, 0xe8u, 0xaau, 0x01u
        };
        uint8_t after_cmdsetinfo_ram[0x20000] = {0};
        uint16_t charinit_ip = (uint16_t)(sizeof(after_cmdsetinfo) + 0x01aau);

        memcpy(after_cmdsetinfo_ram + 0x10000u, after_cmdsetinfo, sizeof(after_cmdsetinfo));
        after_cmdsetinfo_ram[0x10000u + charinit_ip] = 0x90u;
        assert(reconstructed_monitor_monitor_initialize(&monitor, after_cmdsetinfo_ram,
            sizeof(after_cmdsetinfo_ram), NULL, NULL) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        memset(&state, 0, sizeof(state));
        state.cs = 0x1000u;
        state.es = 0x0200u;
        state.ss = 0u;
        state.esp = 0x0100u;
        state.esi = 0x2222u;
        state.edi = 0x1357u;
        state.flags = (uint16_t)(0x0002u | RECONSTRUCTED_MONITOR_FLAG_IF);
        after_cmdsetinfo_ram[0x0100u] = 0x56u;
        after_cmdsetinfo_ram[0x0101u] = 0x34u;
        after_cmdsetinfo_ram[0x0102u] = 0x67u;
        after_cmdsetinfo_ram[0x0103u] = 0x45u;
        after_cmdsetinfo_ram[0x0104u] = 0x78u;
        after_cmdsetinfo_ram[0x0105u] = 0x56u;
        assert(reconstructed_monitor_monitor_set_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_run(&monitor, 76u) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(reconstructed_monitor_monitor_get_state(&monitor, &state) == RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED);
        assert(state.ip == charinit_ip && state.ss == 0u && (uint16_t)state.esp == 0x0920u &&
            after_cmdsetinfo_ram[0x0920u] == (uint8_t)sizeof(after_cmdsetinfo) &&
            after_cmdsetinfo_ram[0x0921u] == (uint8_t)(sizeof(after_cmdsetinfo) >> 8));
        assert(after_cmdsetinfo_ram[0x101eu] == 0x57u && after_cmdsetinfo_ram[0x101fu] == 0x13u &&
            after_cmdsetinfo_ram[0x1020u] == 0x56u && after_cmdsetinfo_ram[0x1021u] == 0x34u);
        assert(reconstructed_monitor_monitor_get_last_stop(&monitor, &stop) == RECONSTRUCTED_MONITOR_NO_STOP_EVENT);
    }
    return 0;
}
