#include <nt.h>
#include <string.h>

#include "vrmslot.h"
#include "apistruc.h"

#include "adapter-mvdm-host-out/softpc/include/mvdm_host_identity.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_redirector_pointer_scope.h"
#include "session/session.h"

VOID WaitIfIdle(VOID) {}
static USHORT ax, bx, cx, dx, ds, si, di, es;
static ULONG carry;
USHORT getAX(VOID) { return ax; }
USHORT getBX(VOID) { return bx; }
USHORT getCX(VOID) { return cx; }
USHORT getDX(VOID) { return dx; }
USHORT getSI(VOID) { return si; }
USHORT getDI(VOID) { return di; }
USHORT getDS(VOID) { return ds; }
USHORT getES(VOID) { return es; }
VOID setAX(USHORT value) { ax = value; }
VOID setBX(USHORT value) { bx = value; }
VOID setCX(USHORT value) { cx = value; }
VOID setDX(USHORT value) { dx = value; }
VOID setSI(USHORT value) { si = value; }
VOID setDI(USHORT value) { di = value; }
VOID setES(USHORT value) { es = value; }
VOID setCF(ULONG value) { carry = value; }
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    return mvdm_redirector_pointer_from_real_mode(segment, offset);
}

typedef struct fixture_memory { unsigned char bytes[0x20000]; } fixture_memory;
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
    char *name = (char *)(memory.bytes + 0x1000u);
    struct DosWriteMailslotStruct *write_request =
        (struct DosWriteMailslotStruct *)(memory.bytes + 0x1200u);
    WORD handle;

    memset(&memory, 0, sizeof(memory));
    strcpy_s(name, 64u, "\\MAILSLOT\\t290-s4-local");
    session_initialize(&instance, 0x2904u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance)) return 1;

    ax = 0x3456u; bx = 64u; cx = 64u; dx = 0x1234u; ds = 0u; si = 0x1000u;
    es = 0u; di = 0x1100u; carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(ds, si, (uint32_t)strlen(name) + 1u,
            GUEST_MEMORY_ACCESS_READ)) return 2;
    VrMakeMailslot();
    if (!mvdm_redirector_pointer_scope_end(0) || carry != 0u || ax == 0u)
        return 3;
    handle = ax;

    bx = handle; carry = 1u; dx = si = 0xffffu;
    VrGetMailslotInfo();
    if (carry != 0u || ax != 64u || bx != 64u || cx != 0u || dx != 0u ||
        si != 0u)
        return 4;

    memcpy(memory.bytes + 0x1300u, "abc", 3u);
    write_request->DWMS_Timeout = 0u;
    write_request->DWMS_Buffer = 0x1300u;
    ds = 0u; si = 0x1000u; es = 0u; di = 0x1200u; cx = 3u; carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(ds, si,
            (uint32_t)strlen(name) + 1u, GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(es, di, sizeof(*write_request),
            GUEST_MEMORY_ACCESS_READ) ||
        !mvdm_redirector_pointer_scope_prepare(0u, 0x1300u, 3u,
            GUEST_MEMORY_ACCESS_READ)) return 5;
    VrWriteMailslot();
    if (!mvdm_redirector_pointer_scope_end(0) || carry != 0u) return 6;

    bx = handle; cx = 0u; dx = 0u; es = 0u; di = 0x1400u; carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(es, di, 64u,
            GUEST_MEMORY_ACCESS_WRITE)) return 7;
    VrReadMailslot();
    if (!mvdm_redirector_pointer_scope_end(1) || carry != 0u || ax != 3u ||
        memcmp(memory.bytes + 0x1400u, "abc", 3u) != 0) return 8;

    carry = 0u;
    VrPeekMailslot();
    if (carry == 0u || ax != ERROR_NOT_SUPPORTED) return 9;

    ax = 0x3456u; bx = handle; carry = 1u;
    VrDeleteMailslot();
    if (carry != 0u) return 10;
    bx = handle; carry = 0u;
    VrGetMailslotInfo();
    if (carry == 0u || ax != ERROR_INVALID_HANDLE) return 11;

    strcpy_s(name, 64u, "\\MAILSLOT\\t290-s4-terminate");
    ax = 0x3456u; bx = 64u; cx = 64u; dx = 0x1234u; ds = 0u; si = 0x1000u;
    es = 0u; di = 0x1100u; carry = 1u;
    if (!mvdm_redirector_pointer_scope_begin() ||
        !mvdm_redirector_pointer_scope_prepare(ds, si, (uint32_t)strlen(name) + 1u,
            GUEST_MEMORY_ACCESS_READ)) return 12;
    VrMakeMailslot();
    if (!mvdm_redirector_pointer_scope_end(0) || carry != 0u || ax == 0u)
        return 13;
    handle = ax;
    VrTerminateMailslots(0x3456u);
    bx = handle; carry = 0u;
    VrGetMailslotInfo();
    if (carry == 0u || ax != ERROR_INVALID_HANDLE) return 14;

    if (!session_thread_unbind(&instance)) return 15;
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) return 16;
    return 0;
}
