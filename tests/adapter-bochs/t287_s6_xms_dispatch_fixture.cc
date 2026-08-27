#include "adapter-bochs/machine_facade.h"

extern "C" {
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_registers.h"
#include "session/session.h"

typedef int32_t (*commit_routine)(uint32_t, uint32_t);
typedef void (*move_routine)(uint32_t, uint32_t, uint32_t);
void *SAInitialize(uint32_t, uint32_t, commit_routine, commit_routine, move_routine);
int32_t xmsCommitBlock(uint32_t, uint32_t);
int32_t xmsDecommitBlock(uint32_t, uint32_t);
void xmsMoveMemory(uint32_t, uint32_t, uint32_t);
int XMSDispatch(unsigned long);
int ReserveUMB(unsigned short, void **, unsigned long *);
void *ExtMemSA = 0;
unsigned long xmsMemorySize = 640u;
unsigned long __cdecl DbgPrint(char *, ...) { return 0u; }
}

static unsigned short int15_segment;
static unsigned short int15_offset;
static unsigned int int15_calls;

extern "C" int ReserveUMB(unsigned short, void **address, unsigned long *size)
{
    if (address != 0) *address = 0;
    if (size != 0) *size = 0;
    return 0;
}

extern "C" void UpdateKbdInt15(unsigned short segment, unsigned short offset)
{
    int15_segment = segment;
    int15_offset = offset;
    ++int15_calls;
}

static int machine_read(void *, uint32_t address, uint8_t *bytes, uint32_t count)
{ return machine_facade_memory_read(address, count, bytes); }
static int machine_write(void *, uint32_t address, const uint8_t *bytes, uint32_t count)
{ return machine_facade_memory_write(address, count, bytes); }
static int ax(unsigned short expected) { unsigned short v = 0; return machine_facade_copy_ax16(&v) && v == expected; }
static int bx(unsigned short expected) { unsigned short v = 0; return machine_facade_copy_bx16(&v) && v == expected; }
static int cx(unsigned short expected) { unsigned short v = 0; return machine_facade_copy_cx16(&v) && v == expected; }

int main()
{
    session instance;
    unsigned short base = 0u;
    unsigned short temporary = 0u;
    unsigned short moved = 0u;
    const uint8_t source[4] = { 0x11u, 0x22u, 0x33u, 0x44u };
    uint8_t target[4] = { 0u, 0u, 0u, 0u };
    const uint8_t stack[12] = {
        0x00u, 0x01u, 0x11u, 0x00u, 0x00u, 0x00u, 0x11u, 0x00u,
        0x02u, 0x00u, 0x00u, 0x00u
    };

    if (!machine_facade_machine_begin(0x400000u, 0x400000u)) return 1;
    session_initialize(&instance, 3u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, 0, machine_read, machine_write)) return 2;
    ExtMemSA = SAInitialize(0x110000u, 0x100000u, xmsCommitBlock,
        xmsDecommitBlock, xmsMoveMemory);
    if (ExtMemSA == 0) return 3;

    if (!machine_facade_set_ax16(2u) || !machine_facade_set_bx16(0xff00u) ||
        !XMSDispatch(0u) || !ax(1u)) return 4;
    { unsigned short value = 0u; if (!machine_facade_copy_bx16(&value) ||
        (value & 0x00ffu) != 0u) return 4; }

    if (!machine_facade_memory_write(0x110000u, sizeof(source), source) ||
        !machine_facade_memory_write(4u, sizeof(stack), stack) ||
        !machine_facade_set_bp16(16u) || !XMSDispatch(1u) || !ax(1u) ||
        !machine_facade_memory_read(0x110100u, sizeof(target), target) ||
        target[0] != source[0] || target[1] != source[1] ||
        target[2] != source[2] || target[3] != source[3]) return 5;

    if (!machine_facade_set_dx16(64u) || !XMSDispatch(2u) ||
        !machine_facade_copy_ax16(&base) || base == 0u) return 6;
    if (!machine_facade_set_dx16(32u) || !XMSDispatch(2u) ||
        !machine_facade_copy_ax16(&temporary) || temporary == 0u) return 7;
    if (!machine_facade_set_ax16(temporary) || !machine_facade_set_dx16(32u) ||
        !XMSDispatch(3u) || !ax(1u)) return 7;
    if (!machine_facade_set_ax16(base) || !machine_facade_set_dx16(64u) ||
        !machine_facade_set_bx16(96u) || !XMSDispatch(11u) ||
        !machine_facade_copy_cx16(&moved) || moved == 0u) return 14;
    if (!XMSDispatch(4u) || !ax(4096u)) return 8;
    if (!XMSDispatch(5u) || !ax(640u)) return 9;

    if (!machine_facade_set_ax16(0x0200u) || !machine_facade_set_bx16(0x0010u) ||
        !XMSDispatch(6u) || !machine_facade_set_dx16(1u) || !XMSDispatch(7u) ||
        !ax(0u) || !bx(0x00b1u)) return 10;
    if (!machine_facade_set_dx16(0xe000u) || !XMSDispatch(8u) ||
        !ax(0u) || !bx(0x00b2u)) return 11;

    machine_facade_apply_real_mode_entry(0x1234u, 0u);
    if (!machine_facade_set_ax16(0x5678u) || !XMSDispatch(9u) ||
        int15_calls != 1u || int15_segment != 0x1234u ||
        int15_offset != 0x5678u || !cx(640u)) return 12;
    if (!XMSDispatch(10u)) return 13;

    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance) ||
        !machine_facade_machine_cleanup()) return 15;
    return 0;
}
