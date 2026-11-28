#ifndef WOW_BUTTON_DRAWING_PRECOMP_H
#define WOW_BUTTON_DRAWING_PRECOMP_H
#include "wow_button_lifetime.h"
#include "wow_edit_entry_bindings.h"
#define WOW_ORIGINAL_BUTTON_RESOURCES
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_BUTTON_RESOURCES
/* The original helper is separately composed for the Button invocation.
 * DrawState has its own atlas, even when it encloses this invocation. */
#define gpsi wow_button_current_context()
#define NtUserBitBltSysBmp wow_button_atlas_blit
#define BitBltSysBmp wow_button_BitBltSysBmp
#endif
