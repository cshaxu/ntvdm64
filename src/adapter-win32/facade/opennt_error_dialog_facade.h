#ifndef BX_NTVDM_BOP_SHIM_OPENNT_ERROR_DIALOG_FACADE_H
#define BX_NTVDM_BOP_SHIM_OPENNT_ERROR_DIALOG_FACADE_H

/* OpenNT source-shaped RcErrorDialogBox seam.  The historical spelling stays
 * at each imported call site; this header only gives the two mirrors one
 * bounded public-Win32 implementation. */
#include <windows.h>

void bx_ntvdm_opennt_rc_error_dialog(UINT error, CHAR *first, CHAR *second);
int bx_ntvdm_opennt_rc_message_box(UINT error, CHAR *first, CHAR *second,
    ULONG flags);

/* Same public-dialog role as nt_error.c:ErrorDialogBox for the reached
 * direct-access path.  Return values retain the historical IDIGNORE/IDABORT
 * choice; the host source decides what each means for its VDM session. */
int bx_ntvdm_opennt_direct_access_dialog(const CHAR *message);

/* The original host owner returns void.  The composition boundary observes
 * the public-dialog choice here, where the modern Win32 implementation owns
 * it, rather than extending an imported OpenNT header with project state. */
enum bx_ntvdm_opennt_direct_access_choice_v1 {
    BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE = 1,
    BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_TERMINATE = 2
};
enum bx_ntvdm_opennt_direct_access_choice_v1
bx_ntvdm_opennt_direct_access_last_choice(void);
int bx_ntvdm_opennt_direct_access_category_should_prompt(ULONG category);
void bx_ntvdm_opennt_direct_access_reset_thread(void);
UINT bx_ntvdm_opennt_direct_access_prompt_count(void);

/* Fixture-only observation controls.  They neither alter the historical
 * call shape nor participate in a product route. */
void bx_ntvdm_opennt_error_dialog_fixture_suppress(BOOL suppress);
UINT bx_ntvdm_opennt_error_dialog_fixture_last_error(void);
UINT bx_ntvdm_opennt_error_dialog_fixture_count(void);
void bx_ntvdm_opennt_direct_access_fixture_reply_set(int reply);

#endif
