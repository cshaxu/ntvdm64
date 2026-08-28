#include "dialog_context.h"

/*
 * DIVERGENCE ADAPTER-WIN32-013: the NT4 source stores a host pointer through
 * SetWindowLong(..., DWL_USER, (LONG)pointer).  Keep that pointer entirely
 * within the host GUI object and use the public pointer-width API.  It is not
 * an opaque component/guest identity, so it must not enter a mapping manager.
 */
void opennt_dialog_set_context(HWND dialog, void *context)
{
    (void)SetWindowLongPtr(dialog, DWLP_USER, (LONG_PTR)context);
}

void *opennt_dialog_get_context(HWND dialog)
{
    return (void *)GetWindowLongPtr(dialog, DWLP_USER);
}
