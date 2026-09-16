#include "mvdm_softpc_presentation_font.h"

#define MVDM_SOFTPC_PRESENTATION_HIFONT_ADDRESS UINT32_C(0x000c3990)

extern void c_sas_loads(uint32_t address, uint8_t *bytes, uint32_t byte_count);

int mvdm_softpc_presentation_font_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || bytes == NULL ||
        capacity != MVDM_SOFTPC_PRESENTATION_FONT_BYTES) return 0;
    /* DIVERGENCE(ADAPTER-SOFTPC-042): V7 VGA loads at 0xc0000 and its
     * original v7_video.c default 8x16 selection is EGA_HIFONT (0xc3990).
     * Original host presentation exposes Console Server buffers, not font
     * copies. Keep SoftPC's selected source and SAS read contract; make one
     * bounded copy so app cannot retain a ROM/VGA alias or modify the font. */
    c_sas_loads(MVDM_SOFTPC_PRESENTATION_HIFONT_ADDRESS, bytes,
        MVDM_SOFTPC_PRESENTATION_FONT_BYTES);
    return 1;
}
