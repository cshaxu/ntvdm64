#ifndef MVDM_HOST_OVERLAY_ICA_EOI_BRIDGE_H
#define MVDM_HOST_OVERLAY_ICA_EOI_BRIDGE_H

/* Preserve the vdmredir-exported int* spelling while entering the original
 * host PIC interface, whose selected declaration uses LONG*. */
void mvdm_ica_eoi_bridge(int adapter, int *line);

#endif
