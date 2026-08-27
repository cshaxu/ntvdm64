#include "adapter-mvdm-host-out/softpc/include/mvdm_int15.h"
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

static void set_vector(uint16_t segment, uint16_t offset)
{
    sas_storew(UINT32_C(0x15) * 4u, offset);
    sas_storew(UINT32_C(0x15) * 4u + 2u, segment);
}

int main(void)
{
    sas_init(sizeof(memory));
    set_vector(UINT16_C(0xf000), UINT16_C(0x0100));
    mvdm_int15_capture_vector();
    if (!mvdm_int15_matches_current_vector()) return 1;
    UpdateKbdInt15(UINT16_C(0x1234), UINT16_C(0x5678));

    if (mvdm_int15_matches_current_vector()) return 2;

    /* The original unchanged-vector path accepts the new callout. */
    set_vector(UINT16_C(0x1234), UINT16_C(0x5678));
    UpdateKbdInt15(UINT16_C(0x2222), UINT16_C(0x3333));
    set_vector(UINT16_C(0x2222), UINT16_C(0x3333));
    if (!mvdm_int15_matches_current_vector()) return 3;

    /* A hook invalidates the cache. With a zero IVT, the next call captures
     * only its supplied vector, exactly as the source reset branch requires. */
    set_vector(UINT16_C(0x4444), UINT16_C(0x5555));
    UpdateKbdInt15(UINT16_C(0x6666), UINT16_C(0x7777));
    if (mvdm_int15_matches_current_vector()) return 4;
    set_vector(0u, 0u);
    UpdateKbdInt15(UINT16_C(0x8888), UINT16_C(0x9999));
    set_vector(UINT16_C(0x8888), UINT16_C(0x9999));
    if (!mvdm_int15_matches_current_vector()) return 5;
    set_vector(UINT16_C(0x8888), UINT16_C(0x9999));
    UpdateKbdInt15(UINT16_C(0xaaaau), UINT16_C(0xbbbbu));
    set_vector(UINT16_C(0xaaaau), UINT16_C(0xbbbbu));
    if (!mvdm_int15_matches_current_vector()) return 6;

    sas_term();
    return mvdm_int15_matches_current_vector() ? 7 : 0;
}
