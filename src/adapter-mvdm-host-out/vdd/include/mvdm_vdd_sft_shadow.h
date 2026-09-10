#ifndef MVDM_VDD_SFT_SHADOW_H
#define MVDM_VDD_SFT_SHADOW_H

#include <windows.h>

#include "mvdm_guest_location.h"

/* The original caller supplies doswow.h before this source-shaped surface.
 * DOSWOW.INC has no include guard and must not be re-included from here. */

/*
 * Source-shaped VDD SFT/JFT pointer bridge.  Returned PDOSSFT/PBYTE values
 * address host-owned mutable shadows, never guest RAM.  The caller must call
 * mvdm_vdd_sft_shadow_commit or mvdm_vdd_sft_shadow_discard with the returned
 * SFT pointer before it leaves the synchronous VDD operation.
 */
SHORT mvdm_vdd_sft_shadow_allocate(ULONG pdb_far,
    const mvdm_guest_location *sft_head, PDOSSFT *sft_out, PBYTE *jft_out);
HANDLE mvdm_vdd_sft_shadow_retrieve(ULONG pdb_far,
    const mvdm_guest_location *sft_head, SHORT dos_handle,
    PDOSSFT *sft_out, PBYTE *jft_out);
int mvdm_vdd_sft_shadow_associate(PDOSSFT sft, HANDLE file_handle,
    WORD access);
int mvdm_vdd_sft_shadow_commit(PDOSSFT sft);
void mvdm_vdd_sft_shadow_discard(PDOSSFT sft);

#endif
