#include <nt.h>
#include <string.h>

#include <lmcons.h>
#include <lmerr.h>

#include "vrdlctab.h"
#include "vdmredir.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_redirector_pointer_scope.h"
#include "session/session.h"

VOID VrGetComputerName(VOID);

static USHORT fixture_ax;
static USHORT fixture_di;
static USHORT fixture_es;
static ULONG fixture_carry;

USHORT getAX(VOID) { return fixture_ax; }
USHORT getDI(VOID) { return fixture_di; }
USHORT getES(VOID) { return fixture_es; }
VOID setAX(USHORT value) { fixture_ax = value; }
VOID setCF(ULONG value) { fixture_carry = value; }
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    return mvdm_redirector_pointer_from_real_mode(segment, offset);
}

typedef struct fixture_memory {
    unsigned char bytes[0x20000];
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

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
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
    fixture_memory memory;
    session instance;
    char expected[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD expected_length = MAX_COMPUTERNAME_LENGTH;
    int expect_success;

    memset(&memory, 0, sizeof(memory));
    if (!GetComputerNameA(expected, &expected_length)) return 1;
    expect_success = expected_length <= LM20_CNLEN;
    session_initialize(&instance, 0x2905u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance)) return 2;

    fixture_es = 0u;
    fixture_di = 0x1000u;
    fixture_ax = 0xffffu;
    fixture_carry = 1u;
    if (expect_success) {
        if (!mvdm_redirector_pointer_scope_begin() ||
            !mvdm_redirector_pointer_scope_prepare(fixture_es, fixture_di,
                LM20_CNLEN + 1u, GUEST_MEMORY_ACCESS_WRITE)) return 3;
    }
    VrGetComputerName();
    if (expect_success) {
        if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
            fixture_ax != 0u || strcmp((char *)(memory.bytes + 0x1000u), expected) != 0)
            return 4;
    } else if (fixture_carry != 1u || fixture_ax != NERR_BufTooSmall) {
        return 5;
    }

    if (!session_thread_unbind(&instance)) return 6;
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) return 7;
    return 0;
}
