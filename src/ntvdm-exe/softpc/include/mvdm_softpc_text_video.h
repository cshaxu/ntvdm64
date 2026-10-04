#ifndef MVDM_SOFTPC_TEXT_VIDEO_H
#define MVDM_SOFTPC_TEXT_VIDEO_H

#include <stdint.h>

/* Copy on the original video-update owner under its existing synchronization.
 * No EGA-plane or PCDisplay pointer crosses to the presentation thread. */
typedef struct mvdm_softpc_text_video {
    uint8_t fonts[2][256][32];
    unsigned font_height;
    unsigned columns, rows;
    int attribute_font_select;
    int cursor_column, cursor_row;
    int cursor_start, cursor_height, cursor_start1, cursor_height1;
    int cursor_visible;
} mvdm_softpc_text_video;

int mvdm_softpc_text_video_copy(mvdm_softpc_text_video *copy);
/* Original video owner only; existing frontend policy, no hardware switch. */
int mvdm_softpc_text_video_sync_route(void);

#endif
