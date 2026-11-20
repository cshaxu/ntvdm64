#ifndef MVDM_CVIDC_VECTOR_BINDING_H
#define MVDM_CVIDC_VECTOR_BINDING_H

/* Selects the original generated C-VID table for the selected CCPU40 state
 * carrier.  This is private to the paired SoftPC mirror/overlay composition. */
void mvdm_cvidc_bind_video_vector(void);
void mvdm_cvidc_bind_vectors(void);

#endif
