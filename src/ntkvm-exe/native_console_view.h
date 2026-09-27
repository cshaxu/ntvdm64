#ifndef RUN16_NATIVE_CONSOLE_VIEW_H
#define RUN16_NATIVE_CONSOLE_VIEW_H
#include "native_console_backend.h"

/* Root-local visible Console binding. One active I/O owner calls these; inner
 * launchers neither open this view nor read its input queue. */
typedef struct run16_native_console_view {
    HANDLE input,output;
    DWORD input_mode;
    DWORD presentation_error;
    BOOL mode_saved;
    BOOL geometry_saved;
    run16_native_geometry geometry;
    /* Optional Window presentation of the same complete hidden snapshot.
     * Set/clear only under the frontend I/O lock. The callback copies what it
     * retains and must not reenter this view. NULL preserves visible Console. */
    void *window_context;
    DWORD (*window_frame)(void *,const run16_native_frame_info *,const CHAR_INFO *,SIZE_T);
    /* Owner-local product hotkeys. Called once per consumed Console record;
     * returning keep=FALSE never changes native stream/geometry semantics. */
    DWORD (*console_input)(void *,const INPUT_RECORD *,BOOL *keep);
} run16_native_console_view;
DWORD run16_native_view_begin(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_begin_on(run16_native_backend *,run16_native_console_view *,HANDLE,HANDLE);
DWORD run16_native_view_seed(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_forward_input(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_present(run16_native_backend *,run16_native_console_view *);
/* Handoff/final drain synchronizes canonical storage without invoking the
 * Window owner's callbacks. Caller still holds the shared frontend I/O lock. */
DWORD run16_native_view_sync_console(run16_native_backend *,run16_native_console_view *);
/* Caller has stopped native forwarding and excludes other visible readers.
 * Rejoin unread hidden records before newer visible records, without parsing
 * or synthesizing guest keys. Empty native input is a successful no-op. */
DWORD run16_native_view_reclaim_input(run16_native_backend *,run16_native_console_view *,DWORD *);
/* Same unread-record transfer, to the frontend-owned DOS queue instead of
 * inactive physical CONIN$. The sink prepends the complete ordered batch. */
DWORD run16_native_view_reclaim_input_to(run16_native_backend *,run16_native_console_view *,DWORD *,
    DWORD (*)(void *,const INPUT_RECORD *,DWORD),void *);
DWORD run16_native_view_wait(run16_native_backend *,run16_native_console_view *,HANDLE,DWORD *);
void run16_native_view_end(run16_native_console_view *);
#endif
