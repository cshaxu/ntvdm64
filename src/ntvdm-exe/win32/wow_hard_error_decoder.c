#include <stdint.h>

#include "wow_hard_error_dialog.h"

#define MVDM_SEB_DEFAULT UINT16_C(0x8000)
#define MVDM_SEB_CANCEL UINT16_C(2)
#define MVDM_SEB_MAXIMUM UINT16_C(8)

/* The original server ignores an empty or unknown source slot, compacts only
 * accepted controls for display, and keeps the source slot as the returned
 * identity. Its default begins with the first displayed control; later valid
 * SEB_DEFBUTTON markers replace that default. */
BOOL mvdm_wow_hard_error_decode_buttons(const WORD descriptors[
    MVDM_WOW_HARD_ERROR_MAX_BUTTONS], mvdm_wow_hard_error_buttons *buttons)
{
    UINT source_slot;

    if (descriptors == NULL || buttons == NULL) return FALSE;
    ZeroMemory(buttons, sizeof(*buttons));
    for (source_slot = 0u; source_slot < MVDM_WOW_HARD_ERROR_MAX_BUTTONS;
        ++source_slot) {
        WORD descriptor = descriptors[source_slot];
        WORD style = descriptor & ~MVDM_SEB_DEFAULT;
        UINT display_slot;

        if (style == 0u || style > MVDM_SEB_MAXIMUM) continue;
        display_slot = buttons->count++;
        buttons->descriptors[display_slot] = descriptor;
        buttons->source_slots[display_slot] = source_slot + 1u;
        if (display_slot == 0u || (descriptor & MVDM_SEB_DEFAULT) != 0u)
            buttons->default_slot = source_slot + 1u;
        if (style == MVDM_SEB_CANCEL)
            buttons->cancel_slot = source_slot + 1u;
    }
    return buttons->count != 0u;
}
