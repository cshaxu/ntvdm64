#ifndef MVDM_REDIRECTOR_GUEST_COPY_H
#define MVDM_REDIRECTOR_GUEST_COPY_H

#include <stdint.h>
#include <wchar.h>

/* Copies one already-bounded ANSI result into an exact real-mode guest span. */
int mvdm_redirector_copy_ansi_to_guest(uint16_t segment, uint16_t offset,
    char const *bytes, uint32_t byte_count);
/* Returns a Win32/LAN status. Capacity is OEM bytes including the terminator,
 * not Unicode characters; no guest write occurs if it is too small. */
uint32_t mvdm_redirector_copy_wide_to_guest(uint16_t segment, uint16_t offset,
    wchar_t const *text, uint32_t capacity);

/* Read one packed I_CDNames target immediately before its original write.
 * Prefix reads preserve the contiguous structure address across offset 0xffff. */
int mvdm_redirector_cd_name_target(uint16_t segment, uint16_t offset,
    uint32_t index, uint32_t *target);

#endif
