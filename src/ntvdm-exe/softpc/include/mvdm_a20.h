#ifndef MVDM_A20_H
#define MVDM_A20_H

/* Same-shaped historical SAS A20 imports. */
#ifdef __cplusplus
extern "C" {
#endif

void sas_enable_20_bit_wrapping(void);
void sas_disable_20_bit_wrapping(void);
int sas_twenty_bit_wrapping_enabled(void);

#ifdef __cplusplus
}
#endif

#endif
