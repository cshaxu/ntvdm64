/* Production cursor bodies; only VGA state, memory and graphics restorers are mocked. */
#define main original_text_cursor_tests
#include "original_mouse_text_cursor_test.c"
#undef main
#define LOCAL static
#define NTVDM 1
#define EGG 1
#define VGG 1
#define FALSE 0
#define EGA 1
#define VGA 2
#define MOUSE_TEXT_CURSOR_TYPE_SOFTWARE 0
#define vd_video_mode 0x449
#define in_text_mode() (bios_mode < 4 || bios_mode == 7)
#define alpha_num_mode() in_text_mode()
#define is_it_text() hardware_text
#define jap_mouse (!is_it_text() && (sas_hw_at(vd_video_mode) != Currently_emulated_video_mode) && alpha_num_mode())
typedef unsigned char UTINY;
static int cursor_EM_disabled, text_cursor_type, video_adapter=VGA;
static int hardware_text=1, bios_mode=3, Currently_emulated_video_mode=0x67;
static unsigned ega_restores, other_restores;
static unsigned char sas_hw_at(unsigned address) { assert(address==vd_video_mode); return (unsigned char)bios_mode; }
static void EGA_graphics_cursor_undisplay(void) { ++ega_restores; }
static void VGA_graphics_cursor_undisplay(void) { ++other_restores; }
static void graphics_cursor_undisplay(void) { ++other_restores; }
static void hardware_text_cursor_undisplay(void) { ++other_restores; }
#include "original_mouse_restore_dispatch.h"
int main(void)
{
    const unsigned slot=163, other=2163;
    original_text_cursor_tests();
    memset(cells,0,sizeof(cells)); writes=0;
    black_hole=(MOUSE_AREA){{-16,-16},{-8,-8}};
    software_text_cursor.screen=0xffff; software_text_cursor.cursor=0x7700;
    cursor_status.position=(POINT_VALUE){24,16}; video_offset=0;
    cells[slot]=0x0741; cells[other]=0x1f42;
    software_text_cursor_display(); cursor_undisplay();
    assert(cells[slot]==0x0741 && !save_area_in_use && ega_restores==0);
    software_text_cursor_display(); hardware_text=0;
    cursor_undisplay();
    assert(cells[slot]==0x0741 && !save_area_in_use && ega_restores==0);
    hardware_text=1; software_text_cursor_display(); video_offset=4000;
    cursor_undisplay();
    assert(cells[slot]==0x0741 && cells[other]==0x1f42);
    { unsigned previous=writes; cursor_undisplay(); assert(writes==previous); }
    /* Existing graphics ownership remains on the graphics branch. */
    save_area_in_use=1; saved_cursor_kind=MOUSE_SAVED_EGA; hardware_text=1;
    cursor_undisplay();
    assert(ega_restores==1 && other_restores==0 && !save_area_in_use);
    save_area_in_use=1; saved_cursor_kind=MOUSE_SAVED_VGA; cursor_undisplay();
    assert(other_restores==1);
    save_area_in_use=1; saved_cursor_kind=MOUSE_SAVED_CGA; cursor_undisplay();
    assert(other_restores==2);
    puts("PASS saved text owner: graphics transition, page transition, single consumption; graphics dispatch retained");
    return 0;
}
