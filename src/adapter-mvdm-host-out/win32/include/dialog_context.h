/* Same-shaped support for the reached NT4 dialog user-data contract. */
#ifndef MVDM_ADAPTER_DIALOG_CONTEXT_H
#define MVDM_ADAPTER_DIALOG_CONTEXT_H

#include <windows.h>

void opennt_dialog_set_context(HWND dialog, void *context);
void *opennt_dialog_get_context(HWND dialog);

#endif /* MVDM_ADAPTER_DIALOG_CONTEXT_H */
