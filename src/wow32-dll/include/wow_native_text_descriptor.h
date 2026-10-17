#ifndef WOW_NATIVE_TEXT_DESCRIPTOR_H
#define WOW_NATIVE_TEXT_DESCRIPTOR_H
#include <windows.h>
#include <stddef.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT

/* OPENNT-HOST-118: inspected x86 USER counted-input layout, not NT4
 * LARGE_STRING. Borrowed text remains live only through the native call. */
typedef struct wow_native_text_descriptor {
    ULONG length;
    ULONG maximum_length_and_ansi;
    ULONG buffer;
    ULONG buffer_upper;
} wow_native_text_descriptor;
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(LARGE_STRING) == 12);
C_ASSERT(sizeof(wow_native_text_descriptor) == 16);
C_ASSERT(offsetof(wow_native_text_descriptor, buffer) == 8);
C_ASSERT(offsetof(wow_native_text_descriptor, buffer_upper) == 12);

static __inline void wow_native_text_capture(wow_native_text_descriptor *target,
    const LARGE_STRING *source)
{
    target->length = source->Length;
    target->maximum_length_and_ansi = source->MaximumLength |
        (source->bAnsi ? 0x80000000u : 0);
    target->buffer = (ULONG)(ULONG_PTR)source->Buffer;
    target->buffer_upper = 0;
}
#endif
