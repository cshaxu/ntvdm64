#include "mvdm_redirector_guest_copy.h"

#include <windows.h>
#include <lmerr.h>
#include <string.h>

#include "ntvdm-exe/redir/include/mvdm_redirector_worker_copy.h"

static int copy_guest(uint16_t segment, uint16_t offset, uint8_t const *bytes,
    uint32_t byte_count)
{
    return mvdm_redirector_worker_copy_to(segment, offset, bytes, byte_count);
}

static int read_guest(uint16_t segment, uint16_t offset, uint8_t *bytes,
    uint32_t byte_count)
{
    return mvdm_redirector_worker_copy_from(segment, offset, bytes, byte_count);
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

static int write_cd_name(uint8_t const *field, char const *value)
{
    uint32_t far_value = (uint32_t)read_u16(field) |
        ((uint32_t)read_u16(field + 2u) << 16);
    static char const empty[] = "";

    if (far_value == 0u || !copy_guest((uint16_t)(far_value >> 16),
        (uint16_t)far_value, (uint8_t const *)empty, 1u)) return far_value == 0u;
    return value == 0 || copy_guest((uint16_t)(far_value >> 16),
        (uint16_t)far_value, (uint8_t const *)value, (uint32_t)strlen(value) + 1u);
}

int mvdm_redirector_write_cd_names(uint16_t segment, uint16_t offset,
    char const *computer, char const *primary_domain, char const *logon_domain)
{
    uint8_t fields[12];

    /* Original VrGetCDNames reads each far field only after writing the prior
     * result. A valid destination may alias a later field, so do not freeze
     * all three pointers before those writes. Prefix reads keep the original
     * contiguous structure address without narrowing offset+8 to 16 bits. */
    return read_guest(segment, offset, fields, 4u) &&
        write_cd_name(fields, computer) &&
        read_guest(segment, offset, fields, 8u) &&
        write_cd_name(fields + 4u, primary_domain) &&
        read_guest(segment, offset, fields, sizeof(fields)) &&
        write_cd_name(fields + 8u, logon_domain);
}
