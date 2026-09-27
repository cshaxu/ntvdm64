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
} run16_native_console_view;
DWORD run16_native_view_begin(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_seed(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_forward_input(run16_native_backend *,run16_native_console_view *);
DWORD run16_native_view_present(run16_native_backend *,run16_native_console_view *);
/* Caller has stopped native forwarding and excludes other visible readers.
 * Rejoin unread hidden records before newer visible records, without parsing
 * or synthesizing guest keys. Empty native input is a successful no-op. */
DWORD run16_native_view_reclaim_input(run16_native_backend *,run16_native_console_view *,DWORD *);
DWORD run16_native_view_wait(run16_native_backend *,run16_native_console_view *,HANDLE,DWORD *);
void run16_native_view_end(run16_native_console_view *);
#endif
