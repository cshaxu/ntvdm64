#include "wow_edit_entry_bindings.h"

/* Shared fixture-owned font context; GDI calls use the production provider. */
__declspec(thread) wow_edit_context *wow_edit_current;
