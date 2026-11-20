#ifndef WOW_LIST_NATIVE_BINDINGS_H
#define WOW_LIST_NATIVE_BINDINGS_H
#include <windows.h>

/* ADAPTER-WOW-065: native queue operation, not control tracking policy.
 * Failure must be handled by the original-call binding, not ignored.
 * The caller remains on the control's current GUI thread. */
BOOL WINAPI wow_list_native_reset_double_click(void);
#endif
