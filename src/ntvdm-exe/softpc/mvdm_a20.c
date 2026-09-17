#include "mvdm_a20.h"

/*
 * The original CCPU/SAS provider owns the A20 wrap mask.  This facade keeps
 * the historical public SAS spellings used by XMS while deliberately avoiding
 * a second machine implementation or a retired-machine dependency.
 */
extern void c_sas_enable_20_bit_wrapping(void);
extern void c_sas_disable_20_bit_wrapping(void);
extern int c_sas_twenty_bit_wrapping_enabled(void);

void sas_enable_20_bit_wrapping(void)
{
    c_sas_enable_20_bit_wrapping();
}

void sas_disable_20_bit_wrapping(void)
{
    c_sas_disable_20_bit_wrapping();
}

int sas_twenty_bit_wrapping_enabled(void)
{
    return c_sas_twenty_bit_wrapping_enabled() != 0;
}
