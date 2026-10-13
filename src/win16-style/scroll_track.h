#ifndef WIN16_STYLE_SCROLL_TRACK_H
#define WIN16_STYLE_SCROLL_TRACK_H
#include "scroll_layout.h"
#include <limits.h>
#define WIN16_SCROLL_KEEP_TIMER UINT_MAX

/* No HWND/callback ownership here. The adapter applies capture/timer/paint and
 * dispatches notices, revalidating its attachment after EVERY notification. */
typedef struct win16_scroll_notice { UINT command; LONG position; } win16_scroll_notice;
typedef struct win16_scroll_actions {
    win16_scroll_notice notices[2];
    UINT count,timer_ms; /* KEEP_TIMER: unchanged; 0: stop; otherwise rearm. */
    BOOL repaint;
} win16_scroll_actions;
typedef struct win16_scroll_track {
    win16_scroll_part part;
    RECT drag_limit;
    int grab_offset,initial_pixel,pixel;
    LONG position;
    UINT repeat_ms;
    BOOL active,inside;
} win16_scroll_track;

BOOL win16_scroll_begin(win16_scroll_track *track,const win16_scroll_layout *layout,
    UINT disabled,POINT point,const RECT *drag_limit,win16_scroll_actions *actions);
void win16_scroll_motion(win16_scroll_track *track,const win16_scroll_layout *layout,
    UINT disabled,POINT point,BOOL timer,win16_scroll_actions *actions);
void win16_scroll_end(win16_scroll_track *track,BOOL cancel,win16_scroll_actions *actions);
#endif
