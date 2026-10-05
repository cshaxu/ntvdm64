/* Unit evidence for the production copied-state reader, not guest execution.
 * Use original headers/layouts and a synthetic EGA plane; no Console or UI. */
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
#include "mvdm_softpc_text_video.h"
#include <stdio.h>
#include <string.h>

DISPLAY_GLOBS PCDisplay;
struct EGA_GLOBALS EGA_GRAPH;
byte *EGA_planes;
int now_width,now_height;
static byte planes[4*65536];
/* Native boundary substitutes only: exercise the production adapter without
 * importing a guest executor or inventing a different snapshot algorithm. */
SCREEN_DESCRIPTION sc;
UPDATE_ALG update_alg;
BOOL ConsoleInitialised,ConsoleNoUpdates;
static BOOL requested,async_active;
static unsigned painted,published,refreshed,shaped,stream_disabled;
BOOL NtvdmConsoleTextRequested(BOOL *value){*value=requested;return TRUE;}
BOOL ntvdm_console_video_async(BOOL value){async_active=value;return TRUE;}
void disable_stream_io(void){++stream_disabled;sc.ScreenState=WINDOWED;}
void nt_mark_screen_refresh(void){++refreshed;}
void nt_cursor_size_changed(int x,int y){(void)x;(void)y;++shaped;}
int DisplayErrorTerm(int code,DWORD error,char *file,int line)
{(void)code;(void)error;(void)file;(void)line;return 0;}
static void paint(void){++painted;}
static void publish(void){++published;}
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d\n",__LINE__); return 1; } } while (0)

int main(void)
{
    static const unsigned offsets[8]={0,0x4000,0x8000,0xc000,0x2000,0x6000,0xa000,0xe000};
    mvdm_softpc_text_video copy,previous;
    unsigned bank,glyph,line,height;
    CHECK(!mvdm_softpc_text_video_copy(NULL));
    CHECK(!mvdm_softpc_text_video_copy(&copy));
    EGA_planes=planes;now_width=80;now_height=50;
    for (bank=0;bank<8;++bank)
        for (glyph=0;glyph<256;++glyph)
            for (line=0;line<32;++line)
                planes[FONT_BASE_ADDR+4*(offsets[bank]+32*glyph+line)]=(byte)(bank*31+glyph+line);
    for (bank=0;bank<8;++bank) for (height=1;height<=32;++height) {
        EGA_GRAPH.prim_font_index=bank;EGA_GRAPH.sec_font_index=7-bank;
        EGA_GRAPH.attrib_font_select=1;PCDisplay.char_height=height;
        PCDisplay.cur_x=79;PCDisplay.cur_y=49;PCDisplay.PC_cursor_visible=1;
        PCDisplay.cursor_start=1;PCDisplay.cursor_height=2;
        PCDisplay.cursor_start1=5;PCDisplay.cursor_height1=3;
        CHECK(mvdm_softpc_text_video_copy(&copy));
        CHECK(copy.columns==80 && copy.rows==50 && copy.font_height==height);
        CHECK(copy.attribute_font_select && copy.cursor_column==79 && copy.cursor_row==49);
        CHECK(copy.cursor_visible && copy.cursor_start==1 && copy.cursor_height==2 &&
            copy.cursor_start1==5 && copy.cursor_height1==3);
        for (glyph=0;glyph<256;++glyph) for (line=0;line<32;++line) {
            CHECK(copy.fonts[0][glyph][line]==(line<height ? (byte)(bank*31+glyph+line) : 0));
            CHECK(copy.fonts[1][glyph][line]==(line<height ? (byte)((7-bank)*31+glyph+line) : 0));
        }
    }
    previous=copy;
    PCDisplay.display_disabled=1;CHECK(!mvdm_softpc_text_video_copy(&copy));
    PCDisplay.display_disabled=0;PCDisplay.mode_change_required=1;
    CHECK(!mvdm_softpc_text_video_copy(&copy));PCDisplay.mode_change_required=0;
    PCDisplay.char_height=33;CHECK(!mvdm_softpc_text_video_copy(&copy));
    PCDisplay.char_height=0;CHECK(!mvdm_softpc_text_video_copy(&copy));
    PCDisplay.char_height=8;EGA_GRAPH.prim_font_index=-1;
    CHECK(!mvdm_softpc_text_video_copy(&copy));EGA_GRAPH.prim_font_index=0;
    EGA_GRAPH.sec_font_index=8;CHECK(!mvdm_softpc_text_video_copy(&copy));
    CHECK(!memcmp(&copy,&previous,sizeof(copy)));
    EGA_GRAPH.sec_font_index=0;now_height=43;
    planes[FONT_BASE_ADDR+4*(32*65+7)]=0xa5;
    CHECK(mvdm_softpc_text_video_copy(&copy));
    CHECK(copy.rows==43 && copy.fonts[0][65][7]==0xa5 && copy.fonts[1][65][7]==0xa5);
    sc.ModeType=TEXT;sc.ScreenState=WINDOWED;ConsoleInitialised=TRUE;
    update_alg.calc_update=paint;requested=TRUE;
    CHECK(mvdm_softpc_text_video_sync_route() && async_active);
    CHECK(sc.ScreenState==FULLSCREEN && refreshed==1 && shaped==1);
    CHECK(mvdm_softpc_text_video_local());
    mvdm_softpc_text_video_flush(publish);CHECK(painted==1 && published==1);
    ConsoleNoUpdates=TRUE;mvdm_softpc_text_video_flush(publish);
    CHECK(painted==1 && published==1);ConsoleNoUpdates=FALSE;
    PCDisplay.mode_change_required=1;mvdm_softpc_text_video_flush(publish);
    CHECK(painted==1);PCDisplay.mode_change_required=0;
    mvdm_softpc_text_video_pause();CHECK(!async_active);
    mvdm_softpc_text_video_resume();CHECK(async_active);
    async_active=FALSE;mvdm_softpc_text_video_flush(publish);
    CHECK(async_active && painted==2 && published==2);
    requested=FALSE;CHECK(mvdm_softpc_text_video_sync_route() && !async_active);
    CHECK(sc.ScreenState==WINDOWED && !mvdm_softpc_text_video_local());
    sc.ModeType=GRAPHICS;CHECK(mvdm_softpc_text_video_sync_route() && async_active);
    mvdm_softpc_text_video_flush(publish);CHECK(painted==2);
    sc.ModeType=TEXT;sc.ScreenState=STREAM_IO;requested=TRUE;
    CHECK(mvdm_softpc_text_video_sync_route() && stream_disabled==1 && async_active);
    puts("PASS production copy/fonts/split cursor and local flush, mode-settle, block/resume, Console/Window/graphics routes");
    return 0;
}
