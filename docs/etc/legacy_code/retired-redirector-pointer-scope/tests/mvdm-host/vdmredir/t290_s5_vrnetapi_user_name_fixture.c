#include <nt.h>
#include <string.h>

#include <lmerr.h>
#include <lmwksta.h>
#include <lmapibuf.h>

#include "vrdlctab.h"
#include "vdmredir.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_redirector_pointer_scope.h"
#include "session/session.h"

VOID VrGetUserName(VOID);

static USHORT fixture_ax;
static USHORT fixture_bx;
static USHORT fixture_cx;
static USHORT fixture_di;
static USHORT fixture_es;
static ULONG fixture_carry;

USHORT getAX(VOID) { return fixture_ax; }
USHORT getBX(VOID) { return fixture_bx; }
USHORT getCX(VOID) { return fixture_cx; }
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
    LPBYTE native_buffer = NULL;
    LPWKSTA_USER_INFO_0 user_info;
    NET_API_STATUS status;
    size_t length;
    USHORT capacity;

    memset(&memory, 0, sizeof(memory));
    status = NetWkstaUserGetInfo(NULL, 0u, &native_buffer);
    if (status != NERR_Success || native_buffer == NULL) return 1;
    user_info = (LPWKSTA_USER_INFO_0)native_buffer;
    if (user_info->wkui0_username == NULL) {
        NetApiBufferFree(native_buffer);
        return 2;
    }
    length = strlen(user_info->wkui0_username);
    if (length >= UINT16_MAX) {
        NetApiBufferFree(native_buffer);
        return 3;
    }
    capacity = (USHORT)(length + 1u);
    if (capacity == 0u) {
        NetApiBufferFree(native_buffer);
        return 4;
    }

    session_initialize(&instance, 0x2905u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance)) {
        NetApiBufferFree(native_buffer);
        return 5;
    }

    fixture_ax = 0xffffu;
    fixture_bx = 1u;
    fixture_cx = capacity;
    fixture_di = 0x1000u;
    fixture_es = 0u;
    fixture_carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(fixture_es, fixture_di,
            capacity, GUEST_MEMORY_ACCESS_WRITE)) return 6;
    VrGetUserName();
    if (!mvdm_redirector_pointer_scope_end(1) || fixture_carry != 0u ||
        fixture_ax != 0u ||
        strcmp((char *)(memory.bytes + fixture_di), user_info->wkui0_username) != 0)
        return 7;

    if (length != 0u) {
        fixture_ax = 0xffffu;
        fixture_bx = 1u;
        fixture_cx = (USHORT)length;
        fixture_carry = 0u;
        VrGetUserName();
        if (fixture_carry != 1u || fixture_ax != NERR_BufTooSmall) return 8;
    }

    if (!session_thread_unbind(&instance)) return 9;
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) return 10;
    NetApiBufferFree(native_buffer);
    return 0;
}
