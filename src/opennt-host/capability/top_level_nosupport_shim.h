#ifndef BX_NTVDM_BOP_SHIM_TOP_LEVEL_NOSUPPORT_SHIM_H
#define BX_NTVDM_BOP_SHIM_TOP_LEVEL_NOSUPPORT_SHIM_H

/*
 * Source-derived seam for OpenNT softpc.new/host/src/nt_error.c:
 * host_direct_access_error.  The source function is void; its host dialog
 * terminates the VDM or returns to the source caller.  This seam retains the
 * per-thread reason suppression and exposes the selected result to the
 * typed BOP boundary without exposing any Win32 handle to the guest.
 */

#include <stdint.h>

enum bx_ntvdm_top_level_nosupport_choice_v1 {
    BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE = 1,
    BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_TERMINATE = 2
};

void bx_ntvdm_top_level_nosupport_v2_direct_access_error(uint32_t reason);
enum bx_ntvdm_top_level_nosupport_choice_v1
bx_ntvdm_top_level_nosupport_v2_last_choice(void);
void bx_ntvdm_top_level_nosupport_v2_reset_thread(void);

/* Fixture-only response control.  Production leaves this unset and reaches
 * the public Win32 dialog below. */
void bx_ntvdm_top_level_nosupport_v2_fixture_choice_set(
    enum bx_ntvdm_top_level_nosupport_choice_v1 choice);
uint32_t bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count(void);

#endif
