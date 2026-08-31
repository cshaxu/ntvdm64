#include <string.h>

#include "session/session.h"
#include "vrnmpipe.h"
#include "mvdm_redirector_async.h"

typedef struct fixture_memory {
    uint8_t bytes[0x20000];
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

static void write_u16(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

static void write_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static uint16_t read_u16(uint8_t const *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static void initialize_request(fixture_memory *memory, uint16_t request_offset,
    uint16_t buffer_offset, uint16_t bytes_offset, uint16_t error_offset,
    uint16_t length)
{
    uint8_t *request = memory->bytes + request_offset;
    memset(request, 0, 24u);
    write_u32(request, bytes_offset);
    write_u16(request + 4u, length);
    write_u32(request + 6u, buffer_offset);
    write_u32(request + 10u, error_offset);
}

int main(void)
{
    fixture_memory memory;
    session instance;
    DOS_ASYNC_NAMED_PIPE_INFO request;
    LPBYTE buffer;
    WORD length;

    memset(&memory, 0, sizeof(memory));
    memset(&request, 0, sizeof(request));
    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_guest_memory_begin(&instance,
        &memory, read_memory, write_memory) || !session_thread_bind(&instance))
        return 1;

    initialize_request(&memory, 0x100u, 0x140u, 0x180u, 0x182u, 3u);
    memory.bytes[0x140u] = 0xa1u;
    memory.bytes[0x141u] = 0xb2u;
    memory.bytes[0x142u] = 0xc3u;
    if (!mvdm_redirector_async_prepare(&request, 0u, 0x100u, 0x91u,
        &buffer, &length) || length != 3u || buffer == NULL ||
        memcmp(buffer, memory.bytes + 0x140u, 3u) != 0) return 2;
    memset(memory.bytes + 0x140u, 0, 3u);
    if (!mvdm_redirector_async_complete(&request, 2u, 5u) ||
        read_u16(memory.bytes + 0x180u) != 2u ||
        read_u16(memory.bytes + 0x182u) != 5u) return 3;
    mvdm_redirector_async_release(&request);

    initialize_request(&memory, 0x100u, 0x140u, 0x180u, 0x182u, 3u);
    if (!mvdm_redirector_async_prepare(&request, 0u, 0x100u, 0x86u,
        &buffer, &length) || length != 3u || buffer == NULL) return 4;
    buffer[0] = 0xd1u;
    buffer[1] = 0xe2u;
    buffer[2] = 0xf3u;
    if (!mvdm_redirector_async_complete(&request, 3u, 0u) ||
        memcmp(memory.bytes + 0x140u, buffer, 3u) != 0) return 5;
    mvdm_redirector_async_release(&request);

    initialize_request(&memory, 0x100u, 0x140u, 0x180u, 0x182u, 1u);
    if (!mvdm_redirector_async_prepare(&request, 0u, 0x100u, 0x86u,
        &buffer, &length)) return 6;
    session_guest_memory_end(&instance);
    if (mvdm_redirector_async_complete(&request, 1u, 0u)) return 7;
    mvdm_redirector_async_release(&request);

    mvdm_redirector_async_worker_begin();
    if (mvdm_redirector_async_worker_stop_requested()) return 8;
    mvdm_redirector_async_worker_request_stop();
    if (!mvdm_redirector_async_worker_stop_requested()) return 9;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 10;
    return 0;
}
