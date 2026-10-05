#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <gmi.h>
#include <gfx_upd.h>
#include <gvi.h>
#include <egagraph.h>
#include <egacpu.h>
#include <egaports.h>
#include <conapi.h>
#include <nt_graph.h>
#include <nt_uis.h>
#include <host_rrr.h>
#include "mvdm_softpc_text_video.h"
#include "ntvdm-exe/win32/console_text.h"
#include "ntvdm-exe/win32/console_client.h"
#include <string.h>

/* Original nt_graph.c::textResize dimensions used by nt_cga_text clipping.
 * RegisterConsoleVDM's backing capacity is not the visible screen extent. */
extern int now_width, now_height;
extern void disable_stream_io(void);
extern void nt_cursor_size_changed(int, int);
extern BOOL ConsoleInitialised,ConsoleNoUpdates;

int mvdm_softpc_text_video_local(void)
{ return sc.ScreenState==FULLSCREEN && sc.ModeType==TEXT; }
void mvdm_softpc_text_video_flush(void (*publish)(void))
{
    if(sc.ModeType==TEXT && ConsoleInitialised && !ConsoleNoUpdates && !get_mode_change_required()) {
        /* Mode retirement can quiesce copied sends between ordinary ticks.
         * Mouse flush must rearm locally, never fall back to IRQ transport. */
        if(!ntvdm_console_video_async(TRUE)) {
            DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);return;
        }
        (void)(*update_alg.calc_update)();publish();
    }
}
void mvdm_softpc_text_video_pause(void)
{
    if(!ntvdm_console_video_async(FALSE))
        DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
}
void mvdm_softpc_text_video_resume(void)
{
    if(!ntvdm_console_video_async(sc.ScreenState==FULLSCREEN))
        DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
}

/* CCPU's software VGA remains active in either presentation route. Never
 * request hardware fullscreen, map physical regen or change guest video mode
 * merely because the frontend switches its visible surface. */
int mvdm_softpc_text_video_sync_route(void)
{
    BOOL requested=FALSE;
    DWORD desired;
    if(!NtvdmConsoleTextRequested(&requested))return 0;
    if(requested && sc.ScreenState==STREAM_IO)disable_stream_io();
    if(sc.ScreenState==STREAM_IO)return 1;
    /* Graphics already requires the software Window even when the user's
       text display preference remains Console. */
    desired=(requested || sc.ModeType==GRAPHICS) ? FULLSCREEN : WINDOWED;
    if(!ntvdm_console_video_async(desired==FULLSCREEN))return 0;
    if(sc.ScreenState!=desired) {
        sc.ScreenState=desired;
        nt_mark_screen_refresh();
        nt_cursor_size_changed(0,0);
    }
    return 1;
}

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
