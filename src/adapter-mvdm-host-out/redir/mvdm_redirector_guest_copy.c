#include "mvdm_redirector_guest_copy.h"

#include <windows.h>
#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_guest_location.h"

int mvdm_redirector_copy_ansi_to_guest(uint16_t segment, uint16_t offset,
    char const *bytes, uint32_t byte_count)
{
    mvdm_guest_location location;

    return bytes != 0 && byte_count != 0u &&
        mvdm_guest_location_set_real_mode(&location, segment, offset) &&
        mvdm_guest_location_copy_to_guest(&location, (uint8_t const *)bytes,
            byte_count);
}

int mvdm_redirector_copy_wide_to_guest(uint16_t segment, uint16_t offset,
    wchar_t const *text)
{
    char *bytes;
    int byte_count;
    int copied;

    if (text == 0) return 0;

    /*
     * DIVERGENCE(ADAPTER-REDIR-003): NetpCopyWStrToStr originally converted
     * directly into an unbounded VDM pointer through RtlUnicodeStringToOemString.
     * The lease boundary cannot expose such a pointer.  CP_OEMCP retains the
     * original default-LAN/OEM target encoding, first obtains its exact bounded
     * size, and commits the converted bytes through one write lease.
     */
    byte_count = WideCharToMultiByte(CP_OEMCP, 0, text, -1, 0, 0, 0, 0);
    if (byte_count <= 0) return 0;
    bytes = (char *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)byte_count);
    if (bytes == 0) return 0;
    copied = WideCharToMultiByte(CP_OEMCP, 0, text, -1, bytes, byte_count,
        0, 0);
    if (copied != byte_count || !mvdm_redirector_copy_ansi_to_guest(segment,
        offset, bytes, (uint32_t)byte_count)) {
        HeapFree(GetProcessHeap(), 0, bytes);
        return 0;
    }
    HeapFree(GetProcessHeap(), 0, bytes);
    return 1;
}

static uint16_t read_u16(uint8_t const *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static int write_cd_name(uint8_t const *field, char const *value)
{
    mvdm_guest_location location;
    uint32_t far_value = (uint32_t)read_u16(field) |
        ((uint32_t)read_u16(field + 2u) << 16);
    static char const empty[] = "";

    if (far_value == 0u) return 1;
    if (!mvdm_guest_location_from_far_value(&location, far_value) ||
        !mvdm_guest_location_copy_to_guest(&location,
            (uint8_t const *)empty, 1u)) return 0;
    return value == 0 || mvdm_guest_location_copy_to_guest(&location,
        (uint8_t const *)value, (uint32_t)strlen(value) + 1u);
}

int mvdm_redirector_write_cd_names(uint16_t segment, uint16_t offset,
    char const *computer, char const *primary_domain, char const *logon_domain)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    uint8_t fields[12];

    if (!mvdm_guest_location_set_real_mode(&location, segment, offset) ||
        !mvdm_guest_location_acquire(&location, sizeof(fields),
            GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
    memcpy(fields, lease.bytes, sizeof(fields));
    if (!mvdm_guest_location_release(&lease, 0)) return 0;
    return write_cd_name(fields, computer) && write_cd_name(fields + 4u,
        primary_domain) && write_cd_name(fields + 8u, logon_domain);
}
