#include "mvdm_sas.h"

#include "adapter-bochs/machine_facade.h"

#include <stdlib.h>

static uint32_t sas_size;

static int sas_range(uint32_t address, uint32_t count, int write)
{
    if (sas_size == 0u || count > sas_size || address > sas_size - count)
        return 0;
    return write ? machine_facade_memory_writable(address, count) :
        machine_facade_memory_readable(address, count);
}

static uint16_t read_word(uint32_t address)
{
    uint8_t bytes[2] = { 0xfeu, 0xfeu };
    (void)machine_facade_memory_read(address, sizeof(bytes), bytes);
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

void sas_init(uint32_t size)
{
    sas_size = size != 0u && machine_facade_memory_readable(size - 1u, 1u) ?
        size : 0u;
}

void sas_term(void)
{
    sas_size = 0u;
}

uint32_t sas_memory_size(void)
{
    return sas_size;
}

uint8_t sas_hw_at(uint32_t address)
{
    uint8_t value = 0xfeu;
    if (sas_range(address, 1u, 0))
        (void)machine_facade_memory_read(address, 1u, &value);
    return value;
}

uint16_t sas_w_at(uint32_t address)
{
    return sas_range(address, 2u, 0) ? read_word(address) : UINT16_C(0xfefe);
}

uint32_t sas_dw_at(uint32_t address)
{
    uint8_t bytes[4] = { 0xfeu, 0xfeu, 0xfeu, 0xfeu };
    if (sas_range(address, 4u, 0))
        (void)machine_facade_memory_read(address, sizeof(bytes), bytes);
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

void sas_load(uint32_t address, uint8_t *value)
{
    if (value == NULL) return;
    *value = sas_hw_at(address);
}

void sas_loadw(uint32_t address, uint16_t *value)
{
    if (value == NULL) return;
    *value = sas_w_at(address);
}

void sas_store(uint32_t address, uint8_t value)
{
    if (sas_range(address, 1u, 1))
        (void)machine_facade_memory_write(address, 1u, &value);
}

void sas_storew(uint32_t address, uint16_t value)
{
    uint8_t bytes[2];
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    if (sas_range(address, sizeof(bytes), 1))
        (void)machine_facade_memory_write(address, sizeof(bytes), bytes);
}

void sas_storedw(uint32_t address, uint32_t value)
{
    uint8_t bytes[4];
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
    if (sas_range(address, sizeof(bytes), 1))
        (void)machine_facade_memory_write(address, sizeof(bytes), bytes);
}

void sas_loads(uint32_t address, uint8_t *bytes, uint32_t count)
{
    if (bytes != NULL && sas_range(address, count, 0))
        (void)machine_facade_memory_read(address, count, bytes);
}

void sas_stores(uint32_t address, const uint8_t *bytes, uint32_t count)
{
    if (bytes != NULL && sas_range(address, count, 1))
        (void)machine_facade_memory_write(address, count, bytes);
}

static void sas_move(uint32_t source, uint32_t destination, uint32_t count)
{
    uint8_t *bytes;
    if (count == 0u || !sas_range(source, count, 0) ||
        !sas_range(destination, count, 1)) return;
    bytes = (uint8_t *)malloc(count);
    if (bytes == NULL) return;
    if (machine_facade_memory_read(source, count, bytes))
        (void)machine_facade_memory_write(destination, count, bytes);
    free(bytes);
}

static int sas_word_count(uint32_t count, uint32_t *bytes)
{
    if (bytes == NULL || count > UINT32_MAX / 2u) return 0;
    *bytes = count * 2u;
    return 1;
}

void sas_move_bytes_forward(uint32_t source, uint32_t destination,
    uint32_t count)
{
    sas_move(source, destination, count);
}

void sas_move_bytes_backward(uint32_t source, uint32_t destination,
    uint32_t count)
{
    if (count == 0u || source < count - 1u || destination < count - 1u)
        return;
    sas_move(source - count + 1u, destination - count + 1u, count);
}

void sas_move_words_forward(uint32_t source, uint32_t destination,
    uint32_t count)
{
    uint32_t bytes;
    if (sas_word_count(count, &bytes)) sas_move(source, destination, bytes);
}

void sas_move_words_backward(uint32_t source, uint32_t destination,
    uint32_t count)
{
    uint32_t bytes;
    if (sas_word_count(count, &bytes))
        sas_move_bytes_backward(source, destination, bytes);
}

void sas_fills(uint32_t address, uint8_t value, uint32_t count)
{
    uint8_t *bytes;
    if (count == 0u || !sas_range(address, count, 1)) return;
    bytes = (uint8_t *)malloc(count);
    if (bytes == NULL) return;
    for (uint32_t index = 0u; index < count; ++index) bytes[index] = value;
    (void)machine_facade_memory_write(address, count, bytes);
    free(bytes);
}

void sas_fillsw(uint32_t address, uint16_t value, uint32_t count)
{
    uint8_t *bytes;
    uint32_t byte_count;
    if (!sas_word_count(count, &byte_count) || byte_count == 0u ||
        !sas_range(address, byte_count, 1)) return;
    bytes = (uint8_t *)malloc(byte_count);
    if (bytes == NULL) return;
    for (uint32_t index = 0u; index < byte_count; index += 2u) {
        bytes[index] = (uint8_t)value;
        bytes[index + 1u] = (uint8_t)(value >> 8);
    }
    (void)machine_facade_memory_write(address, byte_count, bytes);
    free(bytes);
}

/* DIVERGENCE: the original unchecked forms dereference a guest numeric
 * address as a host pointer.  The adapter preserves their scalar contracts
 * but cannot disable Bochs range/ROM checks. */
uint8_t sas_hw_at_no_check(uint32_t address)
{
    return sas_hw_at(address);
}

uint16_t sas_w_at_no_check(uint32_t address)
{
    return sas_w_at(address);
}

uint32_t sas_dw_at_no_check(uint32_t address)
{
    return sas_dw_at(address);
}

void sas_store_no_check(uint32_t address, uint8_t value)
{
    sas_store(address, value);
}

void sas_storew_no_check(uint32_t address, uint16_t value)
{
    sas_storew(address, value);
}
