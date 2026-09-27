/* Unchanged software text cursor bodies with bounded test memory/geometry.
 * This does not assert that NTVDM currently selects those bodies at runtime. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef NDEBUG
#error This fixture requires assertions.
#endif
#define GLOBAL
#define IFN0() (void)
#define TRUE 1
#define VID_ADDR 0x44e
typedef struct { int x,y; } POINT_VALUE;
typedef struct { POINT_VALUE top_left,bottom_right; } MOUSE_AREA;
typedef uint32_t MOUSE_BYTE_ADDRESS;
static struct { POINT_VALUE position; } cursor_status;
static POINT_VALUE cursor_grid={8,8},save_position;
static MOUSE_AREA virtual_screen={{0,0},{640,200}},black_hole={{-16,-16},{-8,-8}};
static struct { uint16_t screen,cursor; } software_text_cursor={0xffff,0x7700};
static uint16_t text_cursor_background,video_offset;
static int save_area_in_use;
static uint16_t cells[8192];
static unsigned writes;
static void point_copy(const POINT_VALUE *a,POINT_VALUE *b) { *b=*a; }
static void point_translate(POINT_VALUE *a,const POINT_VALUE *b) { a->x+=b->x;a->y+=b->y; }
static int area_is_intersected_by_area(const MOUSE_AREA *a,const MOUSE_AREA *b)
{ return a->top_left.x<b->bottom_right.x && b->top_left.x<a->bottom_right.x &&
    a->top_left.y<b->bottom_right.y && b->top_left.y<a->bottom_right.y; }
static unsigned point_as_text_cell_address(const POINT_VALUE *p)
{ assert(p->x>=0 && p->y>=0);return (unsigned)((p->y/8)*80+p->x/8)*2; }
static uint16_t sas_w_at(unsigned address) { assert(address==VID_ADDR);return video_offset; }
static void sas_loadw(unsigned address,uint16_t *value)
{ assert(address>=0xb8000 && address<0xbc000 && !(address&1));*value=cells[(address-0xb8000)/2]; }
static void sas_storew(unsigned address,uint16_t value)
{ assert(address>=0xb8000 && address<0xbc000 && !(address&1));cells[(address-0xb8000)/2]=value;++writes; }
#include "original_mouse_text_cursor.h"
int main(void)
{
    unsigned slot=2*80+3;
    cursor_status.position=(POINT_VALUE){24,16};cells[slot]=0x0741;
    software_text_cursor_display();
    assert(save_area_in_use && text_cursor_background==0x0741);
    assert(cells[slot]==(0x0741^0x7700) && writes==1);
    /* Undisplay uses saved position, not a subsequently moved cursor. */
    cursor_status.position=(POINT_VALUE){32,24};
    software_text_cursor_undisplay();assert(cells[slot]==0x0741 && writes==2);
    video_offset=4000;cells[2000+244]=0x1f42;
    software_text_cursor.screen=0x0fff;software_text_cursor.cursor=0x4000;
    software_text_cursor_display();
    assert(cells[2244]==((0x1f42&0x0fff)^0x4000) && writes==3);
    software_text_cursor_undisplay();assert(cells[2244]==0x1f42 && writes==4);
    black_hole=(MOUSE_AREA){{32,24},{40,32}};
    software_text_cursor_display();assert(writes==4);
    cursor_status.position=(POINT_VALUE){640,200};
    software_text_cursor_display();assert(writes==4);
    puts("PASS original software text cursor: masks, saved-position restore, page offset, conditional exclusion, off-screen exclusion; memory/geometry mocked");
    return 0;
}
