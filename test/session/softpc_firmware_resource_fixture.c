#include "mvdm_softpc_firmware.h"

int main(void)
{
    unsigned char bytes[4];

    return mvdm_softpc_firmware_read_embedded_rom("not-a-product-rom", bytes,
        (uint32_t)sizeof(bytes)) != 0;
}
