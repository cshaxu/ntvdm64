#include "console_video.h"
#include <limits.h>
#include <string.h>

static void discard_pending(run16_console_video *video)
{
    if (video->pending) HeapFree(GetProcessHeap(), 0, video->pending);
    video->pending = NULL;
    video->pending_serial = video->received = 0;
    ZeroMemory(&video->pending_description, sizeof(video->pending_description));
}

void run16_console_video_dispose(run16_console_video *video)
{
    if (!video) return;
    discard_pending(video);
    if (video->pixels) HeapFree(GetProcessHeap(), 0, video->pixels);
    ZeroMemory(video, sizeof(*video));
}

DWORD run16_console_video_begin(run16_console_video *video, uint32_t serial,
    const console_video_description *description)
{
    uint64_t stride, bytes;
    BYTE *pending;
    unsigned int i;
    if (!video || !description) return ERROR_INVALID_PARAMETER;
    if (!serial || serial <= video->serial) return ERROR_INVALID_DATA;
    if (!description->width || !description->height ||
        description->width > SHRT_MAX || description->height > SHRT_MAX)
        return ERROR_INVALID_DATA;
    if(description->kind==CONSOLE_VIDEO_TEXT_FRAME) {
        if(description->depth || description->width>160 || description->height>96)
            return ERROR_INVALID_DATA;
        stride=(uint64_t)description->width*2;
        bytes=sizeof(console_text_style)+stride*description->height;
    } else if(description->kind==CONSOLE_VIDEO_DIB) {
        if(description->depth!=1 && description->depth!=8)return ERROR_INVALID_DATA;
        stride = (((uint64_t)description->width * description->depth + 31) / 32) * 4;
        bytes = stride * description->height;
    } else return ERROR_INVALID_DATA;
    if (stride != description->stride || bytes != description->bytes || bytes > SIZE_MAX)
        return ERROR_INVALID_DATA;
    for (i = 0; i < 256; ++i)
        if (description->palette[i] & 0xff000000u) return ERROR_INVALID_DATA;
    pending = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)bytes);
    if (!pending) return ERROR_NOT_ENOUGH_MEMORY;
    discard_pending(video);
    video->pending = pending;
    video->pending_description = *description;
    video->serial = video->pending_serial = serial;
    return ERROR_SUCCESS;
}

DWORD run16_console_video_data(run16_console_video *video, uint32_t serial,
    uint32_t offset, const void *data, uint32_t bytes)
{
    if (!video || !data) return ERROR_INVALID_PARAMETER;
    if (!video->pending || !serial || serial != video->pending_serial ||
        offset != video->received || !bytes ||
        bytes > video->pending_description.bytes - video->received) {
        discard_pending(video);
        return ERROR_INVALID_DATA;
    }
    memcpy(video->pending + offset, data, bytes);
    video->received += bytes;
    if (video->received == video->pending_description.bytes) {
        if(video->pending_description.kind==CONSOLE_VIDEO_TEXT_FRAME) {
            const console_text_style *style=(const console_text_style *)video->pending;
            if(!style->font_height || style->font_height>32 ||
                style->attribute_font_select>1 || style->cursor_visible>1 ||
                style->cursor_height<0 || style->cursor_height>32 ||
                style->cursor_height1<0 || style->cursor_height1>32 ||
                style->cursor_start < -32 || style->cursor_start>31 ||
                style->cursor_start1 < -32 || style->cursor_start1>31 ||
                video->pending_description.height>768/style->font_height) {
                discard_pending(video);return ERROR_INVALID_DATA;
            }
        }
        /* Publish only after the complete payload. The last complete frame
         * survives a partial transfer until TEXT, disposal or replacement. */
        if (video->pixels) HeapFree(GetProcessHeap(), 0, video->pixels);
        video->pixels = video->pending;
        video->description = video->pending_description;
        video->published_serial = serial;
        video->pending = NULL;
        discard_pending(video);
    }
    return ERROR_SUCCESS;
}

DWORD run16_console_video_text(run16_console_video *video, uint32_t serial)
{
    if (!video) return ERROR_INVALID_PARAMETER;
    if (!serial || serial <= video->serial) return ERROR_INVALID_DATA;
    run16_console_video_dispose(video);
    video->serial = serial;
    return ERROR_SUCCESS;
}
