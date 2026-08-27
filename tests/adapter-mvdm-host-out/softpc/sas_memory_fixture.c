#include "adapter-mvdm-host-out/softpc/include/mvdm_sas.h"

#include <stdint.h>
#include <string.h>

static uint8_t memory[128];

int machine_facade_memory_readable(uint64_t address, uint64_t count)
{
    return address <= sizeof(memory) && count <= sizeof(memory) - address;
}

int machine_facade_memory_writable(uint64_t address, uint64_t count)
{
    return machine_facade_memory_readable(address, count);
}

int machine_facade_memory_read(uint64_t address, uint64_t count, uint8_t *bytes)
{
    if (bytes == 0 || !machine_facade_memory_readable(address, count)) return 0;
    memcpy(bytes, memory + (size_t)address, (size_t)count);
    return 1;
}

int machine_facade_memory_write(uint64_t address, uint64_t count,
    const uint8_t *bytes)
{
    if (bytes == 0 || !machine_facade_memory_writable(address, count)) return 0;
    memcpy(memory + (size_t)address, bytes, (size_t)count);
    return 1;
}

int main(void)
{
    uint8_t bytes[4] = { 1u, 2u, 3u, 4u };
    uint16_t word = 0u;

    sas_init(sizeof(memory));
    if (sas_memory_size() != sizeof(memory)) return 1;
    sas_storew(4u, UINT16_C(0x1234));
    sas_loadw(4u, &word);
    if (word != UINT16_C(0x1234) || sas_hw_at(128u) != 0xfeu) return 2;
    sas_stores(16u, bytes, sizeof(bytes));
    sas_move_bytes_forward(16u, 18u, sizeof(bytes));
    if (sas_dw_at(18u) != UINT32_C(0x04030201)) return 3;
    sas_move_bytes_backward(21u, 25u, sizeof(bytes));
    if (sas_dw_at(22u) != UINT32_C(0x04030201)) return 4;
    sas_fills(32u, 0xa5u, 3u);
    sas_fillsw(40u, UINT16_C(0xbeef), 2u);
    if (sas_hw_at(32u) != 0xa5u || sas_hw_at(34u) != 0xa5u ||
        sas_dw_at(40u) != UINT32_C(0xbeefbeef)) return 5;
    sas_store_no_check(127u, 0x5au);
    if (sas_hw_at_no_check(127u) != 0x5au ||
        sas_w_at_no_check(127u) != UINT16_C(0xfefe)) return 6;
    sas_term();
    return sas_memory_size() == 0u ? 0 : 7;
}
