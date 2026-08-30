#include "mvdm_int15.h"

/* The original CCPU40 SAS owner supplies the IVT access forms. */
#include "sas.h"

static uint16_t sp_int15_handler_segment;
static uint16_t sp_int15_handler_offset;

static void current_int15_vector(uint16_t *offset, uint16_t *segment)
{
    if (offset == 0 || segment == 0) return;
    sas_loadw(UINT32_C(0x15) * 4u, offset);
    sas_loadw(UINT32_C(0x15) * 4u + 2u, segment);
}

void mvdm_int15_capture_vector(void)
{
    current_int15_vector(&sp_int15_handler_offset, &sp_int15_handler_segment);
}

void mvdm_int15_reset_vector(void)
{
    sp_int15_handler_offset = 0u;
    sp_int15_handler_segment = 0u;
}

int mvdm_int15_matches_current_vector(void)
{
    uint16_t int15_offset;
    uint16_t int15_segment;

    current_int15_vector(&int15_offset, &int15_segment);
    return int15_offset == sp_int15_handler_offset &&
        int15_segment == sp_int15_handler_segment;
}

/* DIVERGENCE: the source body lives in the exact keybd_io.c firmware mirror,
 * whose complete keyboard controller/IRQ composition is deliberately not a
 * host runtime input. This same-shaped extraction retains only the reached
 * IVT cache contract and uses the source-shaped SAS memory facade. */
void UpdateKbdInt15(uint16_t segment, uint16_t offset)
{
    if (!mvdm_int15_matches_current_vector()) {
        mvdm_int15_reset_vector();
        return;
    }

    sp_int15_handler_offset = offset;
    sp_int15_handler_segment = segment;
}
