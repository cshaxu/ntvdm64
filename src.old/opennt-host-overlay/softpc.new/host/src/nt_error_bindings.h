/* DIVERGENCE(HOST-DIV-012,HOST-DIV-014): private NT4 product bindings for
 * the otherwise original nt_error.c body.  The adapter preserves the reached
 * TLS/resource/dialog call shapes without importing the NTVDM product shell. */
#ifndef OPENNT_HOST_OVERLAY_NT_ERROR_BINDINGS_H
#define OPENNT_HOST_OVERLAY_NT_ERROR_BINDINGS_H

#include "adapter-win32/facade/opennt_error_dialog_facade.h"

#define TlsGetValue(index) ((LPVOID)(ULONG_PTR) \
    runtime_opennt_direct_access_category_bits_get())
#define TlsSetValue(index, value) \
    runtime_opennt_direct_access_category_bits_set((DWORD)(ULONG_PTR)(value))

#ifdef LoadString
#undef LoadString
#endif
#define LoadString(module, id, buffer, count) \
    runtime_opennt_direct_access_load_string((id), (buffer), (count))
#define ErrorDialogBox(message, edit, options) \
    runtime_opennt_direct_access_dialog((message))
#define szDoomMsg runtime_opennt_direct_access_fallback_message()

#endif
