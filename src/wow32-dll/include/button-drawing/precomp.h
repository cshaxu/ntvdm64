#ifndef WOW_BUTTON_DRAWING_PRECOMP_H
#define WOW_BUTTON_DRAWING_PRECOMP_H
#include "wow_button_lifetime.h"
/* The original helper is separately composed for the Button invocation.
 * DrawState has its own atlas, even when it encloses this invocation. */
#define oemInfo (*wow_button_current_resources())
#define NtUserBitBltSysBmp wow_button_atlas_blit
#define BitBltSysBmp wow_button_BitBltSysBmp
#endif
