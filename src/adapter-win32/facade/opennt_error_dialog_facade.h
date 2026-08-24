#ifndef RUNTIME_BOP_SHIM_OPENNT_ERROR_DIALOG_FACADE_H
#define RUNTIME_BOP_SHIM_OPENNT_ERROR_DIALOG_FACADE_H

/* OpenNT source-shaped RcErrorDialogBox seam.  The historical spelling stays
 * at each imported call site; this header only gives the two mirrors one
 * bounded public-Win32 implementation. */
#include <windows.h>

void runtime_opennt_rc_error_dialog(UINT error, CHAR *first, CHAR *second);
int runtime_opennt_rc_message_box(UINT error, CHAR *first, CHAR *second,
    ULONG flags);

/* Same public-dialog role as nt_error.c:ErrorDialogBox for the reached
 * direct-access path.  Return values retain the historical IDIGNORE/IDABORT
 * choice; the host source decides what each means for its VDM session. */
int runtime_opennt_direct_access_dialog(const CHAR *message);

/* The original host owner returns void.  The composition boundary observes
 * the public-dialog choice here, where the modern Win32 implementation owns
 * it, rather than extending an imported OpenNT header with project state. */
enum runtime_opennt_direct_access_choice_v1 {
    RUNTIME_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE = 1,
    RUNTIME_OPENNT_DIRECT_ACCESS_CHOICE_V1_TERMINATE = 2
};
enum runtime_opennt_direct_access_choice_v1
runtime_opennt_direct_access_last_choice(void);
int runtime_opennt_direct_access_category_should_prompt(ULONG category);
DWORD runtime_opennt_direct_access_category_bits_get(void);
void runtime_opennt_direct_access_category_bits_set(DWORD bits);
int runtime_opennt_direct_access_load_string(UINT resource_id, CHAR *buffer,
    UINT capacity);
const CHAR *runtime_opennt_direct_access_fallback_message(void);
void runtime_opennt_direct_access_reset_thread(void);
UINT runtime_opennt_direct_access_prompt_count(void);

/* Fixture-only observation controls.  They neither alter the historical
 * call shape nor participate in a product route. */
void runtime_opennt_error_dialog_fixture_suppress(BOOL suppress);
UINT runtime_opennt_error_dialog_fixture_last_error(void);
UINT runtime_opennt_error_dialog_fixture_count(void);
void runtime_opennt_direct_access_fixture_reply_set(int reply);

#endif
