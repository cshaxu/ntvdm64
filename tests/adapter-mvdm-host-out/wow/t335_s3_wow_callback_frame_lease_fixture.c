#include "wow_callback_frame_lease.h"

#include <string.h>

#include <nt.h>
#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[128];
    uint32_t reads;
    uint32_t writes;
} fixture_memory;

int mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out)
{
    if (address_out == NULL || selector != 0x1234u) return 0;
    *address_out = offset;
    return 1;
}

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    memory->reads++;
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    memory->writes++;
    return 1;
}

int main(void)
{
    session owner;
    fixture_memory memory;
    wow_callback_frame_lease view;
    POPENNT_SUPPORT_TEB teb;

    memset(&memory, 0, sizeof(memory));
    teb = opennt_support_current_teb();
    if (teb == NULL || teb->WOW32Reserved != NULL) return 7;
    teb->WOW32Reserved = &owner;
    if (opennt_support_current_teb()->WOW32Reserved != &owner) return 8;
    teb->WOW32Reserved = NULL;
    memory.bytes[8] = 0x42u;
    session_initialize(&owner, 335u);
    if (!session_activate(&owner) ||
        !session_guest_memory_begin(&owner, &memory, read_memory, write_memory) ||
        !session_thread_bind(&owner) ||
        !wow_callback_frame_acquire_vp(0x12340008u, 4u,
        GUEST_MEMORY_ACCESS_READ, &view) ||
        view.bytes[0] != 0x42u || wow_callback_frame_release(&view, 1) ||
        !wow_callback_frame_release(&view, 0)) return 1;
    if (!wow_callback_frame_acquire_linear(16u, 4u,
        GUEST_MEMORY_ACCESS_WRITE, &view))
        return 2;
    view.bytes[0] = 0x99u;
    if (!wow_callback_frame_release(&view, 1) || memory.bytes[16] != 0x99u ||
        memory.writes != 1u || !wow_callback_frame_acquire_linear(20u, 4u,
        GUEST_MEMORY_ACCESS_WRITE, &view)) return 3;
    view.bytes[0] = 0x11u;
    if (!wow_callback_frame_release(&view, 0) || memory.bytes[20] != 0u ||
        memory.writes != 1u) return 4;
    if (!session_thread_unbind(&owner)) return 5;
    session_guest_memory_end(&owner);
    return session_dispose(&owner) ? 0 : 6;
}
