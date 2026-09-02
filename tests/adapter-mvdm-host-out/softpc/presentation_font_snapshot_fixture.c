#include <stdint.h>
#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_presentation_font.h"

static uint32_t observed_address;
static uint32_t observed_bytes;

void c_sas_loads(uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    uint32_t index;
    observed_address = address;
    observed_bytes = byte_count;
    for (index = 0u; index < byte_count; ++index)
        bytes[index] = (uint8_t)(index & 0xffu);
}

int main(void)
{
    session owner;
    uint8_t font[MVDM_SOFTPC_PRESENTATION_FONT_BYTES];

    session_initialize(&owner, 361u);
    if (!session_valid(&owner) || !session_select_machine_backend(&owner,
            SESSION_MACHINE_BACKEND_SOFTPC) || !session_activate(&owner) ||
        mvdm_softpc_presentation_font_snapshot(&owner, font,
            MVDM_SOFTPC_PRESENTATION_FONT_BYTES - 1u) ||
        !mvdm_softpc_presentation_font_snapshot(&owner, font,
            MVDM_SOFTPC_PRESENTATION_FONT_BYTES) ||
        observed_address != UINT32_C(0x000c3990) ||
        observed_bytes != MVDM_SOFTPC_PRESENTATION_FONT_BYTES ||
        font[0] != 0u || font[1] != 1u || font[255] != 255u ||
        font[256] != 0u || !session_dispose(&owner)) return 1;
    return 0;
}
