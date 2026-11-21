#ifndef WOW_NATIVE_TEXT_DESCRIPTOR_H
#define WOW_NATIVE_TEXT_DESCRIPTOR_H
#include "compat/include/wow_native_controls.h"

/* The original record stays C-owned; B receives only a synchronous borrow. */
struct _LARGE_STRING;
typedef void (WINAPI *wow_text_capture_provider)(wow_native_text_descriptor *,
    const struct _LARGE_STRING *);
BOOL WINAPI wow_native_text_bind(wow_text_capture_provider);
void WINAPI wow_native_text_capture(wow_native_text_descriptor *,
    const struct _LARGE_STRING *);
#endif
