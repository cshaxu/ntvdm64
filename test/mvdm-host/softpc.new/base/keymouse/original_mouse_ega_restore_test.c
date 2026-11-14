/* Exercise the actual original EGA cursor body with isolated geometry/video stubs. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define LOCAL static
#define IFN0() (void)
#define TRUE 1
#define MOUSE_GRAPHICS_CURSOR_WIDTH 16
#define EGG 1
#define V7VGA 1
#define vd_video_mode 0
#define text(x) x
typedef int16_t MOUSE_SCALAR;
typedef int boolean;
typedef unsigned int MOUSE_BIT_ADDRESS;
typedef unsigned int MOUSE_BYTE_ADDRESS;
typedef uint32_t IU32;
typedef struct { MOUSE_SCALAR x,y; } point;
typedef struct { point top_left,bottom_right; } MOUSE_AREA;
static struct {point position;} cursor_status;
static MOUSE_AREA virtual_screen={{0,0},{640,200}},save_area,black_hole={{-20,-20},{-10,-10}};
static point save_position;
static struct {point size,hot_spot; IU32 screen_lo[16],screen_hi[16],cursor_lo[16],cursor_hi[16];} graphics_cursor={{16,16},{-1,-1}};
static IU32 ega_backgrnd_lo[16],ega_backgrnd_mid[16],ega_backgrnd_hi[16],planes[8192];
static unsigned char *EGA_planes=(unsigned char *)planes;
static int jap_mouse=1,save_area_in_use,change_mode_during_draw;
static MOUSE_BYTE_ADDRESS saved_text_address;
enum { MOUSE_SAVED_TEXT, MOUSE_SAVED_EGA, MOUSE_SAVED_VGA, MOUSE_SAVED_CGA };
static int saved_cursor_kind;
static void point_copy(const point*a,point*b){*b=*a;}
static void point_translate(point*a,const point*b){a->x+=b->x;a->y+=b->y;}
static void point_translate_back(point*a,const point*b){a->x-=b->x;a->y-=b->y;}
static int area_is_intersected_by_area(const MOUSE_AREA*a,const MOUSE_AREA*b){return a->top_left.x<b->bottom_right.x&&b->top_left.x<a->bottom_right.x&&a->top_left.y<b->bottom_right.y&&b->top_left.y<a->bottom_right.y;}
static void area_coerce_to_area(MOUSE_AREA*a,const MOUSE_AREA*b){if(a->top_left.x<b->top_left.x)a->top_left.x=b->top_left.x;if(a->top_left.y<b->top_left.y)a->top_left.y=b->top_left.y;if(a->bottom_right.x>b->bottom_right.x)a->bottom_right.x=b->bottom_right.x;if(a->bottom_right.y>b->bottom_right.y)a->bottom_right.y=b->bottom_right.y;}
static int area_depth(const MOUSE_AREA*a){return a->bottom_right.y-a->top_left.y;}
static int area_width(const MOUSE_AREA*a){return a->bottom_right.x-a->top_left.x;}
static unsigned ega_point_as_graphics_cell_address(const point*p){(void)p;return 0;}
static void mark_string(unsigned a,unsigned b){(void)a;(void)b;}
static struct {void(*mark_string)(unsigned,unsigned);} update_alg={mark_string};
/* Production SAS may reenter guest via bios_read_byte -> biosDoInst.
 * This models that boundary, not a mode-changing dirty callback. */
static unsigned sas_hw_at(unsigned a){(void)a;if(change_mode_during_draw)jap_mouse=0;return 3;}
static unsigned get_bytes_per_line(void){return 1;}
static unsigned get_actual_offset_per_line(void){return 1;}
#include "original_mouse_ega_restore.h"
int main(void)
{
    int i;
    cursor_status.position = (point){700, 100};
    for (i = 0; i < 7; ++i) {
        EGA_graphics_cursor_display();
        assert(virtual_screen.bottom_right.y == 200);
        assert(cursor_status.position.y == 100);
    }
    cursor_status.position = (point){10, 10};
    black_hole = (MOUSE_AREA){{0, 0}, {100, 100}};
    EGA_graphics_cursor_display();
    assert(virtual_screen.bottom_right.y == 200 && cursor_status.position.y == 10);
    black_hole = (MOUSE_AREA){{-20, -20}, {-10, -10}};
    change_mode_during_draw = 1;
    EGA_graphics_cursor_display();
    assert(save_area_in_use && saved_cursor_kind == MOUSE_SAVED_EGA && virtual_screen.bottom_right.y == 200 &&
           cursor_status.position.y == 10 && jap_mouse == 0);
    change_mode_during_draw = 0;
    EGA_graphics_cursor_display();
    assert(virtual_screen.bottom_right.y == 200 && cursor_status.position.y == 10);
    puts("PASS EGA cursor temporary geometry restored: off-screen, excluded, drawn, mode change, non-virtual");
    return 0;
}
