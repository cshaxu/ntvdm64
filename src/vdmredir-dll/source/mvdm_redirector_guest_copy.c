#include "mvdm_redirector_guest_copy.h"

#include <windows.h>
#include <lmerr.h>
#include <string.h>

#ifndef MVDM_GUEST_MEMORY_FIXTURE
#define COMMON_GUEST_MEMORY_IMPORTS
#endif
#include "common/guest-memory/access.h"
#include "common/guest-memory/address.h"

/* The worker owns the address space; Redir owns this bounded copy workflow.
 * Preserve the worker adapter's 4 KiB commit and partial-failure boundaries. */
static int transfer_guest(uint16_t segment, uint16_t offset, uint8_t *bytes,
    uint32_t byte_count, int write)
{
    uint32_t address, done = 0u;
    if (bytes == NULL || byte_count == 0u ||
        !mvdm_softpc_effective_address(segment, offset, &address) ||
        byte_count > UINT32_MAX - address) return 0;
    while (done < byte_count) {
        guest_memory_lease *lease;
        uint8_t *guest_bytes;
        uint32_t chunk = byte_count - done;
        if (chunk > 4096u) chunk = 4096u;
        if (!mvdm_softpc_guest_memory_acquire(address + done, chunk,
            write ? GUEST_MEMORY_ACCESS_WRITE : GUEST_MEMORY_ACCESS_READ,
            &lease, &guest_bytes)) return 0;
        if (write) memcpy(guest_bytes, bytes + done, chunk);
        else memcpy(bytes + done, guest_bytes, chunk);
        if (!mvdm_softpc_guest_memory_release(lease, write)) return 0;
        done += chunk;
    }
    return 1;
}

static int copy_guest(uint16_t segment, uint16_t offset, uint8_t const *bytes,
    uint32_t byte_count)
{
    return transfer_guest(segment, offset, (uint8_t *)bytes, byte_count, 1);
}

static int read_guest(uint16_t segment, uint16_t offset, uint8_t *bytes,
    uint32_t byte_count)
{
    return transfer_guest(segment, offset, bytes, byte_count, 0);
}

int mvdm_redirector_copy_ansi_to_guest(uint16_t segment, uint16_t offset,
    char const *bytes, uint32_t byte_count)
{
    return bytes != 0 && byte_count != 0u && copy_guest(segment, offset,
        (uint8_t const *)bytes, byte_count);
}

uint32_t mvdm_redirector_copy_wide_to_guest(uint16_t segment, uint16_t offset,
    wchar_t const *text, uint32_t capacity)
{
    char *bytes;
    int byte_count;
    int copied;

    if (text == 0) return ERROR_INVALID_ADDRESS;

    /*
     * DIVERGENCE(ADAPTER-REDIR-003): NetpCopyWStrToStr originally converted
     * directly into an unbounded VDM pointer through RtlUnicodeStringToOemString.
     * The lease boundary cannot expose such a pointer.  CP_OEMCP retains the
     * original default-LAN/OEM target encoding, first obtains its exact bounded
     * size, and commits the converted bytes through one write lease.
     */
    byte_count = WideCharToMultiByte(CP_OEMCP, 0, text, -1, 0, 0, 0, 0);
    if (byte_count <= 0) return ERROR_INVALID_ADDRESS;
    if ((uint32_t)byte_count > capacity) return NERR_BufTooSmall;
    bytes = (char *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)byte_count);
    if (bytes == 0) return ERROR_INVALID_ADDRESS;
    copied = WideCharToMultiByte(CP_OEMCP, 0, text, -1, bytes, byte_count,
        0, 0);
    if (copied != byte_count || !mvdm_redirector_copy_ansi_to_guest(segment,
        offset, bytes, (uint32_t)byte_count)) {
        HeapFree(GetProcessHeap(), 0, bytes);
        return ERROR_INVALID_ADDRESS;
    }
    HeapFree(GetProcessHeap(), 0, bytes);
    return NERR_Success;
}

static uint16_t read_u16(uint8_t const *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

int mvdm_redirector_cd_name_target(uint16_t segment, uint16_t offset,
    uint32_t index, uint32_t *target)
{
    uint8_t fields[12];
    uint8_t const *field;

    if (target == NULL || index >= 3u ||
        !read_guest(segment, offset, fields, (index + 1u) * 4u)) return 0;
    field = fields + index * 4u;
    *target = (uint32_t)read_u16(field) |
        ((uint32_t)read_u16(field + 2u) << 16);
    return 1;
}
