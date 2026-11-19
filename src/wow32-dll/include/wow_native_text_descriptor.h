#ifndef WOW_NATIVE_TEXT_DESCRIPTOR_H
#define WOW_NATIVE_TEXT_DESCRIPTOR_H
#include <windows.h>
#include <stddef.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "wow32-dll/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT

#include "compat/include/wow_native_controls.h"
C_ASSERT(sizeof(LARGE_STRING) == 12);

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
