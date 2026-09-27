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
#include "mvdm_softpc_text_video.h"
#include <stdio.h>
#include <string.h>

DISPLAY_GLOBS PCDisplay;
struct EGA_GLOBALS EGA_GRAPH;
byte *EGA_planes;
int now_width,now_height;
static byte planes[4*65536];
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
    puts("PASS production text-state copy: eight EGA banks, 1..32 scanlines, downloaded glyph, dimensions, split cursor and refusal without mutation");
    return 0;
}
