#ifndef BX_NTVDM_OPENNT_HOST_NT_ERROR_COMPAT_H
#define BX_NTVDM_OPENNT_HOST_NT_ERROR_COMPAT_H

/*
 * Composition declaration for the directly re-rooted OpenNT
 * softpc.new/host/src/nt_error.c host_direct_access_error owner.  The source
 * function is void; this local compatibility declaration exposes its selected
 * result to the typed BOP boundary without exposing a Win32 handle to guest.
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
