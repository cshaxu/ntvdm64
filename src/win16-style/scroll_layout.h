#ifndef WIN16_STYLE_SCROLL_LAYOUT_H
#define WIN16_STYLE_SCROLL_LAYOUT_H
#include <windows.h>

/* Internal Win3.1 scrollbar geometry. The native-state adapter must supply an
 * effective min/max; modern nPage is not a thumb-size input. Coordinates are
 * caller-relative, with the same model for horizontal and vertical bars. */
typedef struct win16_scroll_metrics { int arrow,thumb,border; } win16_scroll_metrics;
typedef struct win16_scroll_layout {
    RECT bar,first,last,thumb;
    int minimum_pixel,travel;
    LONG minimum,maximum,position;
    BOOL vertical,thumb_fits,thumb_draggable;
} win16_scroll_layout;
typedef enum win16_scroll_part {
    WIN16_SCROLL_NONE,WIN16_SCROLL_LINE_BEFORE,WIN16_SCROLL_LINE_AFTER,
    WIN16_SCROLL_PAGE_BEFORE,WIN16_SCROLL_PAGE_AFTER,WIN16_SCROLL_THUMB
} win16_scroll_part;

BOOL win16_scroll_measure(const RECT *bar,BOOL vertical,
    const win16_scroll_metrics *metrics,LONG minimum,LONG maximum,LONG position,
    win16_scroll_layout *layout);
/* Maps a thumb-origin pixel, not the pointer itself. Tracking must preserve
 * its initial grab offset and apply the original outside-track cancel rule. */
LONG win16_scroll_position(const win16_scroll_layout *layout,int thumb_origin);
win16_scroll_part win16_scroll_hit(const win16_scroll_layout *layout,POINT point,
    UINT disabled_arrows);
#endif
