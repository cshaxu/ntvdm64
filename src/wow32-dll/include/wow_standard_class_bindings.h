#ifndef WOW_STANDARD_CLASS_BINDINGS_H
#define WOW_STANDARD_CLASS_BINDINGS_H
#include "wow_class_words_binding.h"
#include "wow_cleanup_bindings.h"
typedef struct wow_standard_class_view {
    ATOM atom;
    WORD fnid;
    WNDCLASSEXA native;
} wow_standard_class_view;
/* Caller holds the existing USER class-domain lock. Original walias.c owns
 * classification and the single standard-class table. Returned strings and
 * procedures are borrowed identities, not owned registrations or guest data. */
BOOL WINAPI wow_standard_class_describe(LPCSTR, wow_standard_class_view *);
/* One-time process initialization of the nine original public controls.
 * Requires an empty class context; no lazy reimport after UnregisterClass.
 * Paired teardown is wow_class_words_forget_native. No HWND or guest-page
 * publication occurs until the creation owner attaches the shared class. */
BOOL WINAPI wow_standard_classes_prepare(wow_class_lookup_context *,
    wow_window_callback);
/* Initialize the original cleanup restore table before its first call.
 * This covers the same nine controls, not desktop/menu/switch/icon-title.
 * No native registration or window mutation. Failure leaves output intact. */
BOOL WINAPI wow_standard_classes_bind_cleanup(wow_cleanup_context *);
#endif
