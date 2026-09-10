#include "mvdm_command_guest_state.h"

#include <string.h>

#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[0x1000];
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

int main(void)
{
    fixture_memory first_memory;
    fixture_memory second_memory;
    session first;
    session second;
    uint16_t value;

    memset(&first_memory, 0, sizeof(first_memory));
    memset(&second_memory, 0, sizeof(second_memory));
    session_initialize(&first, 1u);
    session_initialize(&second, 2u);
    if (!session_activate(&first) || !session_activate(&second) ||
        !session_guest_memory_begin(&first, &first_memory, read_memory,
            write_memory) || !session_guest_memory_begin(&second,
            &second_memory, read_memory, write_memory) ||
        !session_thread_bind(&first) ||
        !mvdm_command_guest_state_set_scs_scalars(0u, 0x100u, 32u, 1u,
            0u, 0x200u, 0u, 0x202u)) return 1;

    if (!mvdm_command_guest_state_set_to_sync(0xffu) ||
        first_memory.bytes[0x101u] != 0xffu ||
        !mvdm_command_guest_state_write_is_dos_binary(1u) ||
        first_memory.bytes[0x200u] != 1u ||
        !mvdm_command_guest_state_write_fd_access(0xffffu) ||
        !mvdm_command_guest_state_add_fd_access(1u) ||
        !mvdm_command_guest_state_read_fd_access(&value) || value != 0u ||
        !mvdm_command_guest_state_add_fd_access(0xffffu) ||
        !mvdm_command_guest_state_read_fd_access(&value) || value != 0xffffu)
        return 2;

    if (!session_thread_unbind(&first) || !session_thread_bind(&second) ||
        mvdm_command_guest_state_write_is_dos_binary(0u) ||
        mvdm_command_guest_state_write_fd_access(0u)) return 3;
    if (!session_thread_unbind(&second) || !session_thread_bind(&first)) return 4;

    session_guest_memory_end(&first);
    if (mvdm_command_guest_state_write_is_dos_binary(0u) ||
        mvdm_command_guest_state_read_fd_access(&value)) return 5;
    if (!session_guest_memory_begin(&first, &first_memory, read_memory,
            write_memory) || mvdm_command_guest_state_write_is_dos_binary(0u) ||
        !mvdm_command_guest_state_set_scs_scalars(0u, 0x100u, 32u, 1u,
            0u, 0x200u, 0u, 0xfffu) ||
        mvdm_command_guest_state_write_fd_access(1u))
        return 6;

    if (!session_thread_unbind(&first)) return 7;
    session_guest_memory_end(&first);
    session_guest_memory_end(&second);
    if (!session_dispose(&first) || !session_dispose(&second)) return 8;
    return 0;
}
