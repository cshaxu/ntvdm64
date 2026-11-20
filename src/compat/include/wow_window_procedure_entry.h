#ifndef WOW_WINDOW_PROCEDURE_ENTRY_H
#define WOW_WINDOW_PROCEDURE_ENTRY_H
#include "wow_window_dispatch_bindings.h"

/* ADAPTER-WOW-072: a native ABI carrier for one immutable original WOW
 * procedure/callback pair. The actual native WND/CLS slot owns selection.
 * Entries share the worker/provider lifetime, like original saved procedure
 * values; replacing or destroying one window must not invalidate an entry. */
WNDPROC WINAPI wow_window_entry_for_guest(const wow_window_dispatch_target *);
/* Decode only entries allocated by this owner. Foreign native addresses and
 * USER CPDs remain opaque and are passed to the native procedure APIs. */
BOOL WINAPI wow_window_entry_guest_target(WNDPROC, wow_window_dispatch_target *);
/* Native registration carrier. Admitted creation replaces it with the real
 * immutable/native WND procedure before WM_NCCREATE. Never selects behavior. */
LRESULT CALLBACK wow_window_class_creation_proc(HWND, UINT, WPARAM, LPARAM);

#endif
