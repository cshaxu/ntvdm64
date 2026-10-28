/* Adapted from SoftPC compat/v7_pointer.c, SHA-256
 * 1C0F8893DAC407575D00D40191A50CEBF2E267E1C75539FFB59CCF8E78B1F5B2.
 * Original V7 registers select the 32x32 AND/XOR mask; this module only
 * composites it into an already reconstructed 8-bit indexed surface.
 * Surface lifetime, locking and dirty publication belong to the caller. */
#include "v7_pointer.h"
#include <limits.h>
#define V7_WIDTH 32u
#define V7_MASK_BYTES 128u

static BOOL surface_valid(const BYTE *pixels, unsigned width, unsigned height,
    unsigned stride)
{
    return pixels && width && height && width <= SHRT_MAX && height <= SHRT_MAX &&
        stride >= width && (size_t)stride <= SIZE_MAX / height;
}

BOOL mvdm_v7_pointer_clear(mvdm_v7_pointer *state, BYTE *pixels,
    unsigned width, unsigned height, unsigned stride, SMALL_RECT *dirty)
{
    int x, y;
    if (!state || !dirty || !surface_valid(pixels,width,height,stride)) return FALSE;
    if (!state->visible) return FALSE;
    if (state->left < 0 || state->top < 0 || state->right < state->left ||
        state->bottom < state->top || (unsigned)state->right >= width ||
        (unsigned)state->bottom >= height ||
        (unsigned)(state->right-state->left) >= V7_WIDTH ||
        (unsigned)(state->bottom-state->top) >= V7_WIDTH) {
        state->visible=FALSE; return FALSE;
    }
    for (y=state->top; y<=state->bottom; ++y)
        for (x=state->left; x<=state->right; ++x)
            pixels[(size_t)y*stride+(unsigned)x] =
                state->background[(y-state->top)*V7_WIDTH+(x-state->left)];
    dirty->Left=(SHORT)state->left; dirty->Top=(SHORT)state->top;
    dirty->Right=(SHORT)state->right; dirty->Bottom=(SHORT)state->bottom;
    state->visible=FALSE;
    return TRUE;
}

BOOL mvdm_v7_pointer_paint(mvdm_v7_pointer *state, BYTE *pixels,
    unsigned width, unsigned height, unsigned stride, const BYTE *pattern,
    size_t pattern_size, unsigned left, unsigned top, SMALL_RECT *dirty)
{
    unsigned row, column;
    SMALL_RECT previous;
    BOOL cleared;
    if (!state || !dirty || !pattern || pattern_size < 2u*V7_MASK_BYTES ||
        !surface_valid(pixels,width,height,stride)) return FALSE;
    cleared=mvdm_v7_pointer_clear(state,pixels,width,height,stride,&previous);
    if (left>=width || top>=height) {
        if (cleared) *dirty=previous;
        return cleared;
    }
    state->left=(int)left; state->top=(int)top;
    state->right=(int)(left + ((width-left)<V7_WIDTH ? width-left : V7_WIDTH)-1);
    state->bottom=(int)(top + ((height-top)<V7_WIDTH ? height-top : V7_WIDTH)-1);
    for (row=0; row<=(unsigned)(state->bottom-state->top); ++row) {
        for (column=0; column<=(unsigned)(state->right-state->left); ++column) {
            unsigned bit=7u-(column&7u), index=row*4u+(column>>3u);
            BYTE and_mask=(pattern[index] & (1u<<bit)) ? 0xff : 0;
            BYTE xor_mask=(pattern[V7_MASK_BYTES+index] & (1u<<bit)) ? 0xff : 0;
            BYTE *pixel=pixels+(size_t)(top+row)*stride+left+column;
            state->background[row*V7_WIDTH+column]=*pixel;
            *pixel=(BYTE)((*pixel & and_mask)^xor_mask);
        }
    }
    dirty->Left=(SHORT)state->left; dirty->Top=(SHORT)state->top;
    dirty->Right=(SHORT)state->right; dirty->Bottom=(SHORT)state->bottom;
    if (cleared) {
        if (previous.Left<dirty->Left) dirty->Left=previous.Left;
        if (previous.Top<dirty->Top) dirty->Top=previous.Top;
        if (previous.Right>dirty->Right) dirty->Right=previous.Right;
        if (previous.Bottom>dirty->Bottom) dirty->Bottom=previous.Bottom;
    }
    state->visible=TRUE;
    return TRUE;
}
