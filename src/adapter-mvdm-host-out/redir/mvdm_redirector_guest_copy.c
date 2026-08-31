#include "mvdm_redirector_guest_copy.h"

#include <windows.h>

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
