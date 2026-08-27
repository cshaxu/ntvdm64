#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <windows.h>

#include "doswow.h"
#include "mvdm_vdd_sft_shadow.h"
#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[0x10000];
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || byte_count > sizeof(memory->bytes) -
        address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || byte_count > sizeof(memory->bytes) -
        address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

static void put_u16(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

static void put_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

int main(void)
{
    fixture_memory memory;
    session instance;
    mvdm_guest_location head;
    PDOSSFT sft;
    PBYTE jft;
    HANDLE returned;
    int host_file;
    SHORT handle;

    memset(&memory, 0, sizeof(memory));
    /* DOSPDB: JFT length and 16:16 pointer. */
    put_u16(memory.bytes + 0x100u + offsetof(DOSPDB, PDB_JFN_Length), 3u);
    put_u32(memory.bytes + 0x100u + offsetof(DOSPDB, PDB_JFN_Pointer),
        0x00000200u);
    memset(memory.bytes + 0x200u, 0xff, 3u);
    /* One SFT group at 0000:0300.  SFTable begins at byte six. */
    put_u32(memory.bytes + 0x300u, 0x0000ffffu);
    put_u16(memory.bytes + 0x304u, 1u);

    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory,
            write_memory) || !mvdm_guest_location_set_real_mode(&head, 0u,
            0x300u)) return 1;
    handle = mvdm_vdd_sft_shadow_allocate(0x00000100u, &head, &sft, &jft);
    if (handle != 0 || sft == NULL || jft == NULL || jft[0] != 0u ||
        sft->SFT_Ref_Count != 1u) return 2;
    if (!mvdm_vdd_sft_shadow_associate(sft, (HANDLE)&host_file, 2u) ||
        !mvdm_vdd_sft_shadow_commit(sft) ||
        memory.bytes[0x200u] != 0u ||
        memory.bytes[0x300u + 6u] != 1u) return 3;
    /* The 32-bit SFT field must not resolve a session-one HANDLE in session
     * two.  First confirm same-session retrieval, then dispose session one. */
    returned = mvdm_vdd_sft_shadow_retrieve(0x00000100u, &head, 0, &sft,
        &jft);
    if (returned != (HANDLE)&host_file || sft == NULL || jft == NULL ||
        sft->SFT_Ref_Count != 1u) return 4;
    mvdm_vdd_sft_shadow_discard(sft);
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 5;
    session_initialize(&instance, 2u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory,
            write_memory) || mvdm_vdd_sft_shadow_retrieve(0x00000100u, &head,
                0, NULL, NULL) != NULL) return 6;
    memset(memory.bytes + 0x200u, 0xff, 3u);
    memset(memory.bytes + 0x300u + 6u, 0, sizeof(DOSSFT));
    handle = mvdm_vdd_sft_shadow_allocate(0x00000100u, &head, &sft, &jft);
    if (handle != 0 || sft == NULL || jft == NULL ||
        !mvdm_vdd_sft_shadow_associate(sft, (HANDLE)&host_file, 2u) ||
        !mvdm_vdd_sft_shadow_commit(sft)) return 7;
    returned = mvdm_vdd_sft_shadow_retrieve(0x00000100u, &head, 0, &sft,
        &jft);
    if (returned != (HANDLE)&host_file || sft == NULL || jft == NULL ||
        sft->SFT_Ref_Count != 1u) return 8;
    jft[0] = 0xffu;
    sft->SFT_Ref_Count = 0u;
    if (!mvdm_vdd_sft_shadow_commit(sft) ||
        memory.bytes[0x200u] != 0xffu ||
        memory.bytes[0x300u + 6u] != 0u ||
        mvdm_vdd_sft_shadow_commit(sft) ||
        mvdm_vdd_sft_shadow_retrieve(0x00000100u, &head, 0, NULL, NULL) !=
            NULL) return 9;
    /* An uncommitted borrow is owned by the session registry and must not
     * prevent teardown or retain an identity beyond session disposal. */
    handle = mvdm_vdd_sft_shadow_allocate(0x00000100u, &head, &sft, &jft);
    if (handle != 0 || sft == NULL || jft == NULL) return 10;
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 11;
    return 0;
}
