#include "wow_edit_native_bindings.h"
#include "wow_native_text_descriptor.h"

C_ASSERT(sizeof(CHWIDTHINFO) == 12);
C_ASSERT(sizeof(POLYPATBLT) == 20);
C_ASSERT(offsetof(POLYPATBLT, BrClr) == 16);

BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND window, const LARGE_STRING *text)
{
    wow_native_text_descriptor native_text;
    if (!wow_edit_native_ready()) return FALSE;
    if (text) wow_native_text_capture(&native_text, text);
    return wow_native_set_window_text(window, text ? &native_text : NULL);
}
