#include "reconstructed_monitor.h"

#include <string.h>

static reconstructed_monitor_run_result reconstructed_monitor_physical_address(
    const reconstructed_monitor_monitor *monitor,
    uint16_t segment,
    uint16_t offset,
    uint32_t *out_address)
{
    uint32_t address;

    if (monitor == NULL || out_address == NULL || monitor->ram == NULL) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    address = ((uint32_t)segment << 4) + offset;
    if ((size_t)address >= monitor->ram_bytes) {
        return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
    }
    *out_address = address;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_fetch_byte(
    const reconstructed_monitor_monitor *monitor,
    uint16_t ip,
    uint8_t *out_byte,
    uint32_t *out_physical)
{
    uint32_t physical;
    reconstructed_monitor_run_result result = reconstructed_monitor_physical_address(
        monitor, monitor->state.cs, ip, &physical);

    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return result;
    }
    *out_byte = monitor->ram[physical];
    if (out_physical != NULL) {
        *out_physical = physical;
    }
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_read_word(
    const reconstructed_monitor_monitor *monitor,
    uint16_t segment,
    uint16_t offset,
    uint16_t *out_value)
{
    uint32_t first;
    uint32_t second;

    if (out_value == NULL ||
        reconstructed_monitor_physical_address(monitor, segment, offset, &first) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        reconstructed_monitor_physical_address(monitor, segment, (uint16_t)(offset + 1u), &second) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
    }
    *out_value = (uint16_t)(monitor->ram[first] | ((uint16_t)monitor->ram[second] << 8));
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_read_byte(
    const reconstructed_monitor_monitor *monitor,
    uint16_t segment,
    uint16_t offset,
    uint8_t *out_value)
{
    uint32_t physical;
    reconstructed_monitor_run_result result = reconstructed_monitor_physical_address(monitor, segment, offset, &physical);

    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
    *out_value = monitor->ram[physical];
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_write_byte(
    reconstructed_monitor_monitor *monitor,
    uint16_t segment,
    uint16_t offset,
    uint8_t value)
{
    uint32_t physical;
    reconstructed_monitor_run_result result = reconstructed_monitor_physical_address(monitor, segment, offset, &physical);

    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
    monitor->ram[physical] = value;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_write_word(
    reconstructed_monitor_monitor *monitor,
    uint16_t segment,
    uint16_t offset,
    uint16_t value)
{
    uint32_t first;
    uint32_t second;

    if (reconstructed_monitor_physical_address(monitor, segment, offset, &first) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        reconstructed_monitor_physical_address(monitor, segment, (uint16_t)(offset + 1u), &second) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
    }
    monitor->ram[first] = (uint8_t)value;
    monitor->ram[second] = (uint8_t)(value >> 8);
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static uint16_t *reconstructed_monitor_register16(reconstructed_monitor_monitor *monitor, uint8_t index)
{
    switch (index & 7u) {
    case 0u: return (uint16_t *)&monitor->state.eax;
    case 1u: return (uint16_t *)&monitor->state.ecx;
    case 2u: return (uint16_t *)&monitor->state.edx;
    case 3u: return (uint16_t *)&monitor->state.ebx;
    case 4u: return (uint16_t *)&monitor->state.esp;
    case 5u: return (uint16_t *)&monitor->state.ebp;
    case 6u: return (uint16_t *)&monitor->state.esi;
    default: return (uint16_t *)&monitor->state.edi;
    }
}

static uint16_t *reconstructed_monitor_segment_register(reconstructed_monitor_monitor *monitor, uint8_t index)
{
    switch (index) {
    case 0u: return &monitor->state.es;
    case 2u: return &monitor->state.ss;
    case 3u: return &monitor->state.ds;
    default: return NULL;
    }
}

static int reconstructed_monitor_segment_value(
    const reconstructed_monitor_monitor *monitor,
    uint8_t index,
    uint16_t *out_value)
{
    if (out_value == NULL) return 0;
    switch (index & 7u) {
    case 0u: *out_value = monitor->state.es; return 1;
    case 1u: *out_value = monitor->state.cs; return 1;
    case 2u: *out_value = monitor->state.ss; return 1;
    case 3u: *out_value = monitor->state.ds; return 1;
    default: return 0;
    }
}

static void reconstructed_monitor_set_logic_flags(reconstructed_monitor_monitor *monitor, uint16_t value)
{
    uint8_t byte = (uint8_t)value;
    unsigned parity = 0u;
    unsigned bit;

    monitor->state.flags &= (uint16_t)~(RECONSTRUCTED_MONITOR_FLAG_CF | RECONSTRUCTED_MONITOR_FLAG_PF |
        RECONSTRUCTED_MONITOR_FLAG_ZF | RECONSTRUCTED_MONITOR_FLAG_SF);
    if (value == 0u) monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_ZF;
    if ((value & 0x8000u) != 0u) monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_SF;
    for (bit = 0u; bit < 8u; ++bit) parity += (byte >> bit) & 1u;
    if ((parity & 1u) == 0u) monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_PF;
}

static void reconstructed_monitor_set_add_flags(
    reconstructed_monitor_monitor *monitor,
    uint16_t left,
    uint16_t right,
    uint16_t result)
{
    uint32_t sum = (uint32_t)left + right;

    reconstructed_monitor_set_logic_flags(monitor, result);
    monitor->state.flags &= (uint16_t)~(RECONSTRUCTED_MONITOR_FLAG_CF | RECONSTRUCTED_MONITOR_FLAG_AF);
    if (sum > 0xffffu) monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_CF;
    if (((left ^ right ^ result) & 0x0010u) != 0u) monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_AF;
}

static void reconstructed_monitor_advance_ip(reconstructed_monitor_monitor *monitor, uint16_t count)
{
    monitor->state.ip = (uint16_t)(monitor->state.ip + count);
}

static reconstructed_monitor_bridge_disposition reconstructed_monitor_bridge_disposition_for_result(
    reconstructed_monitor_run_result result)
{
    if (result == RECONSTRUCTED_MONITOR_BRIDGE_REFUSED) {
        return RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_REFUSED;
    }
    return RECONSTRUCTED_MONITOR_BRIDGE_DISPOSITION_NOT_ATTEMPTED;
}

static void reconstructed_monitor_record_stop(
    reconstructed_monitor_monitor *monitor,
    reconstructed_monitor_run_result result,
    const reconstructed_monitor_state *origin)
{
    reconstructed_monitor_stop_event *event;
    uint16_t offset;
    uint32_t physical;
    unsigned index;

    event = &monitor->last_stop;
    memset(event, 0, sizeof(*event));
    event->reason = result;
    event->bridge_disposition = reconstructed_monitor_bridge_disposition_for_result(result);
    event->cs = origin->cs;
    event->ip = origin->ip;
    if (reconstructed_monitor_physical_address(monitor, origin->cs, origin->ip, &physical) ==
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        event->physical_fetch = physical;
        event->physical_fetch_valid = 1u;
    }
    for (index = 0u; index < sizeof(event->opcode_window); ++index) {
        offset = (uint16_t)(origin->ip + index);
        if (reconstructed_monitor_physical_address(monitor, origin->cs, offset, &physical) !=
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
            break;
        }
        event->opcode_window[index] = monitor->ram[physical];
        ++event->opcode_bytes;
    }
    monitor->last_stop_valid = 1u;
}

static reconstructed_monitor_run_result reconstructed_monitor_call_bridge(
    reconstructed_monitor_monitor *monitor,
    uint8_t selector,
    uint32_t physical_fetch)
{
    reconstructed_monitor_bridge_request request;
    reconstructed_monitor_bridge_response response;
    uint8_t next_byte;
    uint32_t physical_next;

    if (monitor->bridge_transport == NULL) {
        return RECONSTRUCTED_MONITOR_BRIDGE_REFUSED;
    }
    if (reconstructed_monitor_fetch_byte(monitor, monitor->state.ip, &next_byte, &physical_next) !=
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
    }
    memset(&request, 0, sizeof(request));
    memset(&response, 0, sizeof(response));
    request.selector = selector;
    request.next_byte = next_byte;
    request.physical_fetch = physical_fetch;
    request.physical_next = physical_next;
    request.state = monitor->state;

    if (!monitor->bridge_transport(monitor->bridge_context, &request, &response) ||
        (response.write_mask & ~RECONSTRUCTED_MONITOR_BRIDGE_WRITE_ALL) != 0u ||
        ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF) != 0u &&
            response.carry > 1u)) {
        return RECONSTRUCTED_MONITOR_BRIDGE_REFUSED;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX) != 0u) {
        monitor->state.eax = (monitor->state.eax & 0xffff0000u) | response.ax;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CX) != 0u) {
        monitor->state.ecx = (monitor->state.ecx & 0xffff0000u) | response.cx;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_DX) != 0u) {
        monitor->state.edx = (monitor->state.edx & 0xffff0000u) | response.dx;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CS) != 0u) {
        monitor->state.cs = response.cs;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_IP) != 0u) {
        monitor->state.ip = response.ip;
    }
    if ((response.write_mask & RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CF) != 0u) {
        monitor->state.flags = (uint16_t)((monitor->state.flags & ~RECONSTRUCTED_MONITOR_FLAG_CF) |
            (response.carry != 0u ? RECONSTRUCTED_MONITOR_FLAG_CF : 0u));
    }
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

static reconstructed_monitor_run_result reconstructed_monitor_step(reconstructed_monitor_monitor *monitor)
{
    uint8_t opcode;
    uint8_t modrm;
    uint8_t selector;
    uint8_t extension;
    uint32_t physical_fetch;
    uint32_t immediate;
    uint16_t index;
    uint16_t instruction_bytes;
    uint16_t word_value;
    uint16_t displacement;
    uint16_t *register_value;
    uint16_t *segment_value;
    reconstructed_monitor_run_result result;

    result = reconstructed_monitor_fetch_byte(monitor, monitor->state.ip, &opcode, &physical_fetch);
    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return result;
    }
    if (opcode == 0x90u) {
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xfcu) {
        monitor->state.flags &= (uint16_t)~RECONSTRUCTED_MONITOR_FLAG_DF;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode >= 0x50u && opcode <= 0x57u) {
        uint16_t sp = (uint16_t)monitor->state.esp;
        register_value = reconstructed_monitor_register16(monitor, (uint8_t)(opcode - 0x50u));
        sp = (uint16_t)(sp - 2u);
        result = reconstructed_monitor_write_word(monitor, monitor->state.ss, sp, *register_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | sp;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode >= 0x58u && opcode <= 0x5fu) {
        uint16_t sp = (uint16_t)monitor->state.esp;
        register_value = reconstructed_monitor_register16(monitor, (uint8_t)(opcode - 0x58u));
        result = reconstructed_monitor_read_word(monitor, monitor->state.ss, sp, &word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        *register_value = word_value;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | (uint16_t)(sp + 2u);
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x1eu || opcode == 0x0eu || opcode == 0x06u) {
        uint16_t sp = (uint16_t)monitor->state.esp;
        word_value = opcode == 0x1eu ? monitor->state.ds :
            (opcode == 0x0eu ? monitor->state.cs : monitor->state.es);
        sp = (uint16_t)(sp - 2u);
        result = reconstructed_monitor_write_word(monitor, monitor->state.ss, sp, word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | sp;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x1fu || opcode == 0x07u) {
        uint16_t sp = (uint16_t)monitor->state.esp;
        segment_value = opcode == 0x1fu ? &monitor->state.ds : &monitor->state.es;
        result = reconstructed_monitor_read_word(monitor, monitor->state.ss, sp, &word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        *segment_value = word_value;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | (uint16_t)(sp + 2u);
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode >= 0xb8u && opcode <= 0xbfu) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        *reconstructed_monitor_register16(monitor, (uint8_t)(opcode - 0xb8u)) = (uint16_t)(selector | ((uint16_t)extension << 8));
        reconstructed_monitor_advance_ip(monitor, 3u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x8eu) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc0u) != 0xc0u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        segment_value = reconstructed_monitor_segment_register(monitor, (uint8_t)((modrm >> 3) & 7u));
        if (segment_value == NULL) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        *segment_value = *reconstructed_monitor_register16(monitor, modrm);
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x8cu) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
            !reconstructed_monitor_segment_value(monitor, (uint8_t)((modrm >> 3) & 7u), &word_value)) {
            return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        }
        if ((modrm & 0xc0u) == 0xc0u) {
            *reconstructed_monitor_register16(monitor, modrm & 7u) = word_value;
            reconstructed_monitor_advance_ip(monitor, 2u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if ((modrm & 0xc7u) == 0x06u) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_write_word(monitor, monitor->state.ds,
                (uint16_t)(selector | ((uint16_t)extension << 8)), word_value);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            reconstructed_monitor_advance_ip(monitor, 4u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (modrm == 0x1cu) {
            result = reconstructed_monitor_write_word(monitor, monitor->state.ds,
                (uint16_t)monitor->state.esi, word_value);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            reconstructed_monitor_advance_ip(monitor, 2u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
    }
    if (opcode == 0x05u || opcode == 0x25u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        word_value = (uint16_t)(selector | ((uint16_t)extension << 8));
        if (opcode == 0x05u) {
            uint16_t left = (uint16_t)monitor->state.eax;
            uint16_t sum = (uint16_t)(left + word_value);

            monitor->state.eax = (monitor->state.eax & 0xffff0000u) | sum;
            reconstructed_monitor_set_add_flags(monitor, left, word_value, sum);
        } else {
            word_value = (uint16_t)monitor->state.eax & word_value;
            monitor->state.eax = (monitor->state.eax & 0xffff0000u) | word_value;
            reconstructed_monitor_set_logic_flags(monitor, word_value);
        }
        reconstructed_monitor_advance_ip(monitor, 3u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x33u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if ((modrm & 0xc0u) != 0xc0u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        register_value = reconstructed_monitor_register16(monitor, (uint8_t)((modrm >> 3) & 7u));
        word_value = (uint16_t)(*register_value ^ *reconstructed_monitor_register16(monitor, modrm & 7u));
        *register_value = word_value;
        reconstructed_monitor_set_logic_flags(monitor, word_value);
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xf7u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        displacement = (uint16_t)(selector | ((uint16_t)extension << 8));
        result = reconstructed_monitor_read_word(monitor, monitor->state.ds, displacement, &word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 5u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_set_logic_flags(monitor, (uint16_t)(word_value &
            (uint16_t)(selector | ((uint16_t)extension << 8))));
        reconstructed_monitor_advance_ip(monitor, 6u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xf0u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm != 0x81u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm != 0x26u && modrm != 0x0eu) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        displacement = (uint16_t)(selector | ((uint16_t)extension << 8));
        result = reconstructed_monitor_read_word(monitor, monitor->state.ds, displacement, &word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 5u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 6u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm == 0x26u) {
            word_value = (uint16_t)(word_value & (uint16_t)(selector | ((uint16_t)extension << 8)));
        } else {
            word_value = (uint16_t)(word_value | (uint16_t)(selector | ((uint16_t)extension << 8)));
        }
        result = reconstructed_monitor_write_word(monitor, monitor->state.ds, displacement, word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_set_logic_flags(monitor, word_value);
        reconstructed_monitor_advance_ip(monitor, 7u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x2eu || opcode == 0x26u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (opcode == 0x2eu && modrm == 0x89u) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &modrm, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc7u) != 0x06u) {
                return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
            }
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_write_word(monitor, monitor->state.cs,
                (uint16_t)(selector | ((uint16_t)extension << 8)),
                *reconstructed_monitor_register16(monitor, (uint8_t)((modrm >> 3) & 7u)));
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            reconstructed_monitor_advance_ip(monitor, 5u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (opcode == 0x2eu && modrm == 0x8cu) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &modrm, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc7u) != 0x06u ||
                !reconstructed_monitor_segment_value(monitor, (uint8_t)((modrm >> 3) & 7u), &word_value)) {
                return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
            }
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_write_word(monitor, monitor->state.cs,
                (uint16_t)(selector | ((uint16_t)extension << 8)), word_value);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            reconstructed_monitor_advance_ip(monitor, 5u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (opcode == 0x2eu && modrm == 0xacu) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)monitor->state.esi, &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            monitor->state.eax = (monitor->state.eax & 0xffffff00u) | selector;
            monitor->state.esi = (monitor->state.esi & 0xffff0000u) |
                (uint16_t)(monitor->state.esi + ((monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) != 0u ? -1 : 1));
            reconstructed_monitor_advance_ip(monitor, 2u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (modrm == 0x8eu) {
            uint16_t source_segment = opcode == 0x2eu ? monitor->state.cs : monitor->state.es;

            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            if (selector != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            displacement = extension;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            displacement |= (uint16_t)extension << 8;
            segment_value = &monitor->state.es;
            result = reconstructed_monitor_read_word(monitor, source_segment, displacement, &word_value);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            *segment_value = word_value;
            reconstructed_monitor_advance_ip(monitor, 5u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (opcode == 0x26u && modrm == 0x8au) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            if (selector != 0x45u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_read_byte(monitor, monitor->state.es,
                (uint16_t)((uint16_t)monitor->state.edi + (int8_t)extension), &selector);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            monitor->state.eax = (monitor->state.eax & 0xffffff00u) | selector;
            reconstructed_monitor_advance_ip(monitor, 4u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
    }
    if (opcode == 0xf3u) {
        uint32_t source;
        uint32_t destination;

        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || modrm != 0xa4u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        for (index = 0u; index < (uint16_t)monitor->state.ecx; ++index) {
            if (reconstructed_monitor_physical_address(monitor, monitor->state.ds,
                    (uint16_t)((uint16_t)monitor->state.esi + index), &source) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
                reconstructed_monitor_physical_address(monitor, monitor->state.es,
                    (uint16_t)((uint16_t)monitor->state.edi + index), &destination) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
                return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
            }
        }
        while ((uint16_t)monitor->state.ecx != 0u) {
            result = reconstructed_monitor_read_byte(monitor, monitor->state.ds, (uint16_t)monitor->state.esi, &selector);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            result = reconstructed_monitor_write_byte(monitor, monitor->state.es, (uint16_t)monitor->state.edi, selector);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            monitor->state.esi = (monitor->state.esi & 0xffff0000u) |
                (uint16_t)(monitor->state.esi + ((monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) != 0u ? -1 : 1));
            monitor->state.edi = (monitor->state.edi & 0xffff0000u) |
                (uint16_t)(monitor->state.edi + ((monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) != 0u ? -1 : 1));
            monitor->state.ecx = (monitor->state.ecx & 0xffff0000u) | (uint16_t)(monitor->state.ecx - 1u);
        }
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xa2u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_write_byte(monitor, monitor->state.ds,
            (uint16_t)(selector | ((uint16_t)extension << 8)), (uint8_t)monitor->state.eax);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_advance_ip(monitor, 3u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x98u) {
        monitor->state.eax = (monitor->state.eax & 0xffff0000u) |
            (uint16_t)(int16_t)(int8_t)monitor->state.eax;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xd1u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm == 0xe0u) {
            word_value = (uint16_t)(monitor->state.eax << 1u);
            monitor->state.eax = (monitor->state.eax & 0xffff0000u) | word_value;
        } else if (modrm == 0xefu) {
            word_value = (uint16_t)(monitor->state.edi >> 1u);
            monitor->state.edi = (monitor->state.edi & 0xffff0000u) | word_value;
        } else {
            return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        }
        reconstructed_monitor_set_logic_flags(monitor, word_value);
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x8bu) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc0u) != 0xc0u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        *reconstructed_monitor_register16(monitor, (uint8_t)((modrm >> 3) & 7u)) =
            *reconstructed_monitor_register16(monitor, modrm & 7u);
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x8au) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || modrm != 0x0eu) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_read_byte(monitor, monitor->state.ds,
            (uint16_t)(selector | ((uint16_t)extension << 8)), &selector);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.ecx = (monitor->state.ecx & 0xffff00ffu) | ((uint32_t)selector << 8);
        reconstructed_monitor_advance_ip(monitor, 4u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x89u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc7u) != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        word_value = *reconstructed_monitor_register16(monitor, (uint8_t)((modrm >> 3) & 7u));
        result = reconstructed_monitor_write_word(monitor, monitor->state.ds,
            (uint16_t)(selector | ((uint16_t)extension << 8)), word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_advance_ip(monitor, 4u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xa3u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_write_word(monitor, monitor->state.ds,
            (uint16_t)(selector | ((uint16_t)extension << 8)), (uint16_t)monitor->state.eax);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_advance_ip(monitor, 3u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xc6u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || modrm != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_write_byte(monitor, monitor->state.ds,
            (uint16_t)(selector | ((uint16_t)extension << 8)), modrm);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_advance_ip(monitor, 5u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xc7u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        displacement = (uint16_t)(selector | ((uint16_t)extension << 8));
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 5u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_write_word(monitor, monitor->state.ds, displacement,
            (uint16_t)(selector | ((uint16_t)extension << 8)));
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_advance_ip(monitor, 6u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x87u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || (modrm & 0xc0u) != 0xc0u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        register_value = reconstructed_monitor_register16(monitor, (uint8_t)((modrm >> 3) & 7u));
        word_value = *register_value;
        *register_value = *reconstructed_monitor_register16(monitor, modrm & 7u);
        *reconstructed_monitor_register16(monitor, modrm & 7u) = word_value;
        reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xadu) {
        result = reconstructed_monitor_read_word(monitor, monitor->state.ds, (uint16_t)monitor->state.esi, &word_value);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.eax = (monitor->state.eax & 0xffff0000u) | word_value;
        monitor->state.esi = (monitor->state.esi & 0xffff0000u) |
            (uint16_t)(monitor->state.esi + ((monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) != 0u ? -2 : 2));
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x93u) {
        word_value = (uint16_t)monitor->state.eax;
        monitor->state.eax = (monitor->state.eax & 0xffff0000u) | (uint16_t)monitor->state.ebx;
        monitor->state.ebx = (monitor->state.ebx & 0xffff0000u) | word_value;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x47u) {
        monitor->state.edi = (monitor->state.edi & 0xffff0000u) | (uint16_t)(monitor->state.edi + 1u);
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x83u || opcode == 0x81u) {
        uint16_t immediate_word;

        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if ((modrm & 0xf8u) != 0xc0u) {
            return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        }
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (opcode == 0x83u) {
            immediate_word = (uint16_t)(int16_t)(int8_t)selector;
            reconstructed_monitor_advance_ip(monitor, 3u);
        } else {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            immediate_word = (uint16_t)(selector | ((uint16_t)extension << 8));
            reconstructed_monitor_advance_ip(monitor, 4u);
        }
        register_value = reconstructed_monitor_register16(monitor, modrm & 7u);
        *register_value = (uint16_t)(*register_value + immediate_word);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xf6u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (modrm == 0xc1u) {
            result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
            if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
            reconstructed_monitor_set_logic_flags(monitor, (uint16_t)((uint8_t)monitor->state.ecx & selector));
            reconstructed_monitor_advance_ip(monitor, 3u);
            return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
        }
        if (modrm != 0x06u) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        displacement = (uint16_t)(selector | ((uint16_t)extension << 8));
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 4u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_read_byte(monitor, monitor->state.ds, displacement, &extension);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        reconstructed_monitor_set_logic_flags(monitor, (uint16_t)(extension & selector));
        reconstructed_monitor_advance_ip(monitor, 5u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xabu) {
        result = reconstructed_monitor_write_word(monitor, monitor->state.es, (uint16_t)monitor->state.edi,
            (uint16_t)monitor->state.eax);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.edi = (monitor->state.edi & 0xffff0000u) |
            (uint16_t)(monitor->state.edi + ((monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_DF) != 0u ? -2 : 2));
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x9fu) {
        monitor->state.eax = (monitor->state.eax & 0xffff00ffu) |
            ((uint32_t)((monitor->state.flags & (RECONSTRUCTED_MONITOR_FLAG_SF | RECONSTRUCTED_MONITOR_FLAG_ZF |
                RECONSTRUCTED_MONITOR_FLAG_AF | RECONSTRUCTED_MONITOR_FLAG_PF | RECONSTRUCTED_MONITOR_FLAG_CF)) | 0x02u) << 8);
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0x9eu) {
        uint16_t flags = (uint16_t)(monitor->state.eax >> 8);
        monitor->state.flags = (monitor->state.flags & (uint16_t)~(RECONSTRUCTED_MONITOR_FLAG_SF | RECONSTRUCTED_MONITOR_FLAG_ZF |
            RECONSTRUCTED_MONITOR_FLAG_AF | RECONSTRUCTED_MONITOR_FLAG_PF | RECONSTRUCTED_MONITOR_FLAG_CF)) |
            (flags & (RECONSTRUCTED_MONITOR_FLAG_SF | RECONSTRUCTED_MONITOR_FLAG_ZF | RECONSTRUCTED_MONITOR_FLAG_AF |
                RECONSTRUCTED_MONITOR_FLAG_PF | RECONSTRUCTED_MONITOR_FLAG_CF));
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xfau || opcode == 0xfbu) {
        if (opcode == 0xfau) monitor->state.flags &= (uint16_t)~RECONSTRUCTED_MONITOR_FLAG_IF;
        else monitor->state.flags |= RECONSTRUCTED_MONITOR_FLAG_IF;
        reconstructed_monitor_advance_ip(monitor, 1u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xcbu) {
        uint16_t sp = (uint16_t)monitor->state.esp;
        uint16_t return_ip;
        uint16_t return_cs;

        /* DEVIOCALL2 calls device entry points through a far pointer. */
        result = reconstructed_monitor_read_word(monitor, monitor->state.ss, sp, &return_ip);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_read_word(monitor, monitor->state.ss, (uint16_t)(sp + 2u), &return_cs);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | (uint16_t)(sp + 4u);
        monitor->state.ip = return_ip;
        monitor->state.cs = return_cs;
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xffu) {
        uint16_t target_ip;
        uint16_t target_cs;
        uint16_t return_ip;
        uint16_t sp;
        uint32_t ignored_address;

        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED || modrm != 0x1eu) return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 3u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        displacement = (uint16_t)(selector | ((uint16_t)extension << 8));
        result = reconstructed_monitor_read_word(monitor, monitor->state.ds, displacement, &target_ip);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_read_word(monitor, monitor->state.ds, (uint16_t)(displacement + 2u), &target_cs);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (reconstructed_monitor_physical_address(monitor, target_cs, target_ip, &ignored_address) !=
            RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
        sp = (uint16_t)monitor->state.esp;
        if (reconstructed_monitor_physical_address(monitor, monitor->state.ss, (uint16_t)(sp - 2u), &ignored_address) !=
                RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
            reconstructed_monitor_physical_address(monitor, monitor->state.ss, (uint16_t)(sp - 1u), &ignored_address) !=
                RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
            reconstructed_monitor_physical_address(monitor, monitor->state.ss, (uint16_t)(sp - 4u), &ignored_address) !=
                RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
            reconstructed_monitor_physical_address(monitor, monitor->state.ss, (uint16_t)(sp - 3u), &ignored_address) !=
                RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return RECONSTRUCTED_MONITOR_MEMORY_FAULT;
        return_ip = (uint16_t)(monitor->state.ip + 4u);
        result = reconstructed_monitor_write_word(monitor, monitor->state.ss, (uint16_t)(sp - 2u), monitor->state.cs);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_write_word(monitor, monitor->state.ss, (uint16_t)(sp - 4u), return_ip);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | (uint16_t)(sp - 4u);
        monitor->state.cs = target_cs;
        monitor->state.ip = target_ip;
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xe9u) {
        uint16_t target;

        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        target = (uint16_t)(monitor->state.ip + 3u +
            (int16_t)(uint16_t)(selector | ((uint16_t)extension << 8)));
        result = reconstructed_monitor_fetch_byte(monitor, target, &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.ip = target;
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xe8u) {
        uint16_t target;
        uint16_t return_ip;
        uint16_t sp;

        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        return_ip = (uint16_t)(monitor->state.ip + 3u);
        target = (uint16_t)(return_ip + (int16_t)(uint16_t)(selector | ((uint16_t)extension << 8)));
        result = reconstructed_monitor_fetch_byte(monitor, target, &modrm, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        sp = (uint16_t)(monitor->state.esp - 2u);
        result = reconstructed_monitor_write_word(monitor, monitor->state.ss, sp, return_ip);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        monitor->state.esp = (monitor->state.esp & 0xffff0000u) | sp;
        monitor->state.ip = target;
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xebu || opcode == 0x74u || opcode == 0x75u || opcode == 0xe2u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) return result;
        if (opcode == 0xebu || (opcode == 0x75u && (monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_ZF) == 0u) ||
            (opcode == 0x74u && (monitor->state.flags & RECONSTRUCTED_MONITOR_FLAG_ZF) != 0u)) {
            monitor->state.ip = (uint16_t)(monitor->state.ip + 2u + (int8_t)selector);
        } else if (opcode == 0xe2u) {
            monitor->state.ecx = (monitor->state.ecx & 0xffff0000u) | (uint16_t)(monitor->state.ecx - 1u);
            monitor->state.ip = (uint16_t)(monitor->state.ip + 2u +
                (((uint16_t)monitor->state.ecx != 0u) ? (int8_t)selector : 0));
        } else reconstructed_monitor_advance_ip(monitor, 2u);
        return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
    }
    if (opcode == 0xd6u) {
        result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &selector, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
            return result;
        }
        reconstructed_monitor_advance_ip(monitor, 2u);
        if (selector == 0xfeu) {
            return RECONSTRUCTED_MONITOR_BOP_EXIT;
        }
        return reconstructed_monitor_call_bridge(monitor, selector, physical_fetch);
    }
    if (opcode != 0xc4u) {
        return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
    }
    result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 1u), &modrm, NULL);
    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return result;
    }
    if ((modrm & 0xfcu) != 0xc4u) {
        return RECONSTRUCTED_MONITOR_UNKNOWN_OPCODE;
    }
    result = reconstructed_monitor_fetch_byte(monitor, (uint16_t)(monitor->state.ip + 2u), &selector, NULL);
    if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        return result;
    }
    immediate = selector;
    instruction_bytes = (uint16_t)(3u + (modrm & 3u));
    for (index = 0u; index < (uint16_t)(modrm & 3u); ++index) {
        result = reconstructed_monitor_fetch_byte(monitor,
            (uint16_t)(monitor->state.ip + 3u + index), &extension, NULL);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
            return result;
        }
        immediate |= (uint32_t)extension << (8u * (index + 1u));
    }
    reconstructed_monitor_advance_ip(monitor, instruction_bytes);
    if (immediate == 0xfeu) {
        return RECONSTRUCTED_MONITOR_BOP_EXIT;
    }
    if ((immediate & 0xffu) == 0xfeu) {
        return RECONSTRUCTED_MONITOR_UNSUPPORTED_BOP;
    }
    return reconstructed_monitor_call_bridge(monitor, selector, physical_fetch);
}

reconstructed_monitor_run_result reconstructed_monitor_monitor_initialize(
    reconstructed_monitor_monitor *monitor,
    uint8_t *ram,
    size_t ram_bytes,
    reconstructed_monitor_bridge_transport bridge_transport,
    void *bridge_context)
{
    if (monitor == NULL || ram == NULL || ram_bytes == 0u) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    memset(monitor, 0, sizeof(*monitor));
    monitor->ram = ram;
    monitor->ram_bytes = ram_bytes;
    monitor->state.flags = 0x0002u;
    monitor->bridge_transport = bridge_transport;
    monitor->bridge_context = bridge_context;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

reconstructed_monitor_run_result reconstructed_monitor_monitor_run(
    reconstructed_monitor_monitor *monitor,
    size_t instruction_budget)
{
    reconstructed_monitor_run_result result;
    reconstructed_monitor_state origin;

    if (monitor == NULL || monitor->ram == NULL) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    monitor->last_stop_valid = 0u;
    while (instruction_budget-- != 0u) {
        origin = monitor->state;
        result = reconstructed_monitor_step(monitor);
        if (result != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
            reconstructed_monitor_record_stop(monitor, result, &origin);
            return result;
        }
    }
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

reconstructed_monitor_run_result reconstructed_monitor_monitor_get_state(
    const reconstructed_monitor_monitor *monitor,
    reconstructed_monitor_state *out_state)
{
    if (monitor == NULL || out_state == NULL) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    *out_state = monitor->state;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

reconstructed_monitor_run_result reconstructed_monitor_monitor_set_state(
    reconstructed_monitor_monitor *monitor,
    const reconstructed_monitor_state *state)
{
    if (monitor == NULL || state == NULL) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    monitor->state = *state;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}

reconstructed_monitor_run_result reconstructed_monitor_monitor_get_last_stop(
    const reconstructed_monitor_monitor *monitor,
    reconstructed_monitor_stop_event *out_event)
{
    if (monitor == NULL || out_event == NULL) {
        return RECONSTRUCTED_MONITOR_INVALID_ARGUMENT;
    }
    if (!monitor->last_stop_valid) {
        return RECONSTRUCTED_MONITOR_NO_STOP_EVENT;
    }
    *out_event = monitor->last_stop;
    return RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED;
}
