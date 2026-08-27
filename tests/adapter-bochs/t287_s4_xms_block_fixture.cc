#include "adapter-bochs/machine_facade.h"

extern "C" {
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_registers.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_xms_memory.h"
#include "session/session.h"

typedef int32_t (*commit_routine)(uint32_t, uint32_t);
typedef void (*move_routine)(uint32_t, uint32_t, uint32_t);
void *SAInitialize(uint32_t base, uint32_t size, commit_routine commit,
    commit_routine decommit, move_routine move);
void xmsAllocBlock(void);
void xmsFreeBlock(void);
void xmsReallocBlock(void);
void xmsMoveBlock(void);
void xmsQueryFreeExtMem(void);
void *ExtMemSA = 0;
unsigned long __cdecl DbgPrint(char *, ...) { return 0u; }
}

static int machine_read(void *, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_read(address, byte_count, bytes);
}

static int machine_write(void *, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_write(address, byte_count, bytes);
}

static int copy_ax(uint16_t expected)
{
    uint16_t value = 0u;
    return machine_facade_copy_ax16(&value) && value == expected;
}

int main()
{
    session instance;
    uint16_t base = 0u;
    uint16_t moved_base = 0u;
    uint8_t source[4] = { 0x11u, 0x22u, 0x33u, 0x44u };
    uint8_t destination[4] = { 0u, 0u, 0u, 0u };
    uint8_t stack[12] = {
        0x00u, 0x01u, 0x11u, 0x00u, /* destination 0x00110100 */
        0x00u, 0x00u, 0x11u, 0x00u, /* source      0x00110000 */
        0x02u, 0x00u, 0x00u, 0x00u  /* two words = four bytes */
    };

    if (!machine_facade_machine_begin(0x400000u, 0x400000u)) return 1;
    session_initialize(&instance, 2u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, 0, machine_read, machine_write))
        return 2;
    ExtMemSA = SAInitialize(0x110000u, 0x100000u, xmsCommitBlock,
        xmsDecommitBlock, xmsMoveMemory);
    if (ExtMemSA == 0) return 3;

    if (xmsCommitBlock(0x3ffff0u, 0x20u) == 0 ||
        mvdm_xms_memory_copy(0x3ffff0u, 0u, 0x20u)) return 4;

    if (!machine_facade_set_dx16(64u)) return 5;
    xmsAllocBlock();
    if (!machine_facade_copy_ax16(&base) || base == 0u) return 6;

    if (!machine_facade_memory_write(0x110000u, sizeof(source), source) ||
        !machine_facade_memory_write(4u, sizeof(stack), stack) ||
        !machine_facade_set_bp16(16u)) return 7;
    xmsMoveBlock();
    if (!copy_ax(1u) || !machine_facade_memory_read(0x110100u,
        sizeof(destination), destination) ||
        destination[0] != source[0] || destination[1] != source[1] ||
        destination[2] != source[2] || destination[3] != source[3]) return 8;

    if (!machine_facade_set_bp16(8u)) return 9;
    xmsMoveBlock();
    if (!copy_ax(0u)) return 10;

    if (!machine_facade_set_ax16(base) || !machine_facade_set_dx16(64u) ||
        !machine_facade_set_bx16(96u)) return 11;
    xmsReallocBlock();
    if (!machine_facade_copy_cx16(&moved_base) || moved_base == 0u) return 12;
    if (!machine_facade_set_ax16(moved_base) ||
        !machine_facade_set_dx16(96u)) return 13;
    xmsFreeBlock();
    if (!copy_ax(1u)) return 14;

    if (!machine_facade_set_dx16(0xffffu)) return 15;
    xmsAllocBlock();
    if (!copy_ax(0u)) return 16;

    xmsQueryFreeExtMem();
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) ||
        !session_dispose(&instance) || !machine_facade_machine_cleanup()) return 17;
    return 0;
}
