#ifndef COMPAT_WOW_WINDOW_CAPTURE_H
#define COMPAT_WOW_WINDOW_CAPTURE_H

#include <windows.h>
#include "compat/include/wow_window_graph.h"

/* Call-local public-USER relationship sample. No WW, task association or
 * native ownership is created. Identities remain native until publication;
 * the desktop must be translated by the existing guest-view owner. */
typedef struct wow_user_window_links {
    HWND window, next, parent, child, owner;
} wow_user_window_links;
BOOL WINAPI wow_user_window_sample_links(HWND, wow_user_window_links *);
/* Native SDK message operands only; original WOW retains all conversion.
 * Caller supplies four slots. No window queries or guest allocation occur. */
ULONG WINAPI wow_user_message_window_roots(UINT, WPARAM, LPARAM, HWND [4]);
ULONG WINAPI wow_user_queue_window_roots(const MSG *, HWND [4]);
ULONG WINAPI wow_user_message_return_roots(UINT, WPARAM, LPARAM, LRESULT, HWND [4]);

/* Caller-owned transaction scratch. No guest mapping, task selection, retained
 * observation, or publication callback is owned by these native operations. */
BOOL WINAPI wow_native_window_capture(HWND, mvdm_softpc_wow_window_observation *);
BOOL WINAPI wow_native_window_observation_put(mvdm_softpc_wow_window_observation **,
    ULONG *, ULONG *, ULONG *, const mvdm_softpc_wow_window_observation *);
ULONG * WINAPI wow_native_window_observation_edge(mvdm_softpc_wow_window_observation *, unsigned);
BOOL WINAPI wow_native_window_normalize_chains(mvdm_softpc_wow_window_observation *,
    ULONG, const ULONG *, BOOL *);

#endif
