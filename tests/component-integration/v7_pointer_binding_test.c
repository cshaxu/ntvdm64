#include <windows.h>
#include <stdio.h>

static unsigned char planes[0x80000];
unsigned char *EGA_planes = planes;
void (*paint_v7ptr)();
void (*clear_v7ptr)();
static const BYTE *last_pattern;
static unsigned last_x, last_y, calls;
static BOOL last_paint;

void ntvdm_console_graphics_v7_pointer(const BYTE *pattern, unsigned x,
                                       unsigned y, BOOL paint)
{
    last_pattern=pattern; last_x=x; last_y=y; last_paint=paint; ++calls;
}

void mvdm_softpc_v7_pointer_attach(void);

int main(void)
{
    mvdm_softpc_v7_pointer_attach();
    if (!paint_v7ptr || !clear_v7ptr) return 1;
    paint_v7ptr(0x7ff00ul, 11, 17);
    if (calls != 1 || last_pattern != planes + 0x7ff00 || !last_paint ||
        last_x != 11 || last_y != 17) return 2;
    paint_v7ptr(0x7ff01ul, 0, 0);
    if (calls != 1) return 3;
    EGA_planes = NULL;
    paint_v7ptr(0ul, 0, 0);
    if (calls != 1) return 4;
    clear_v7ptr(11, 17);
    if (calls != 2 || last_pattern != NULL || last_paint) return 5;
    puts("PASS original V7 pointer ABI, plane boundary and callback binding");
    return 0;
}
