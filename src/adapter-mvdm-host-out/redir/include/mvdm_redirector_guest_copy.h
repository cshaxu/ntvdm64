#ifndef MVDM_REDIRECTOR_GUEST_COPY_H
#define MVDM_REDIRECTOR_GUEST_COPY_H

#include <stdint.h>
#include <wchar.h>

/* Copies one already-bounded ANSI result into an exact real-mode guest span. */
int mvdm_redirector_copy_ansi_to_guest(uint16_t segment, uint16_t offset,
    char const *bytes, uint32_t byte_count);
int mvdm_redirector_copy_wide_to_guest(uint16_t segment, uint16_t offset,
    wchar_t const *text);

#endif
