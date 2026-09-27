#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <gmi.h>
#include <gfx_upd.h>
#include <gvi.h>
#include <egagraph.h>
#include <egacpu.h>
#include <egaports.h>
#include "mvdm_softpc_text_video.h"
#include <string.h>

/* Original nt_graph.c::textResize dimensions used by nt_cga_text clipping.
 * RegisterConsoleVDM's backing capacity is not the visible screen extent. */
extern int now_width, now_height;

/* Original ega_vide.c::load_ega_fonts stores each glyph in a 32-byte slot,
 * using this original eight-bank ordering. C-video interleaves four planes;
 * plane two contains fonts (egaports.h::FONT_BASE_ADDR). Unlike the retired
 * fixed-ROM snapshot, this preserves downloaded fonts and both active banks.
 * This is a copied presentation boundary, not a second font selector/loader. */
int mvdm_softpc_text_video_copy(mvdm_softpc_text_video *copy)
{
    static const unsigned offsets[8] = {
        0, 0x4000, 0x8000, 0xc000, 0x2000, 0x6000, 0xa000, 0xe000
    };
    int selected[2], height, bank;
    unsigned glyph, scanline;
    if (!copy || !EGA_planes || get_display_disabled() || get_mode_change_required())
        return 0;
    height = get_char_height();
    selected[0] = get_prim_font_index();
    selected[1] = get_sec_font_index();
    if (height <= 0 || height > FONT_MAX_HEIGHT || selected[0] < 0 ||
        selected[0] > 7 || selected[1] < 0 || selected[1] > 7) return 0;
    memset(copy, 0, sizeof(*copy));
    copy->font_height = (unsigned)height;
    copy->columns = now_width > 0 ? (unsigned)now_width : 0;
    copy->rows = now_height > 0 ? (unsigned)now_height : 0;
    copy->attribute_font_select = get_attrib_font_select();
    copy->cursor_column = get_cur_x();
    copy->cursor_row = get_cur_y();
    copy->cursor_start = get_cursor_start();
    copy->cursor_height = get_cursor_height();
    copy->cursor_start1 = get_cursor_start1();
    copy->cursor_height1 = get_cursor_height1();
    copy->cursor_visible = is_cursor_visible();
    for (bank = 0; bank < 2; ++bank)
        for (glyph = 0; glyph < 256; ++glyph)
            for (scanline = 0; scanline < (unsigned)height; ++scanline)
                copy->fonts[bank][glyph][scanline] = EGA_planes[FONT_BASE_ADDR +
                    4u * (offsets[selected[bank]] + FONT_MAX_HEIGHT * glyph + scanline)];
    return 1;
}
