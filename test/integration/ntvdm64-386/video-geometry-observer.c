/* Diagnostic link only: retain the selected original BIOS calculation and
 * observe its actual display/BDA inputs. No guest or device state is changed. */
#include "mvdm-host/softpc.new/base/video/ega_vide.c"
extern void test_video_geometry(unsigned, unsigned, unsigned, unsigned);
void test_video_geometry_snapshot(unsigned after, unsigned function)
{
    half_word index, low, overflow, maximum;
    io_addr port;
    test_video_geometry(after, function, get_screen_height() + 1,
        sas_hw_at_no_check(vd_rows_on_screen) + 1);
    if (function != 0x1112) return;
    port = sas_w_at_no_check(0x463);
    inb(port, &index);
    outb(port, 0x12); inb(port + 1, &low);
    outb(port, 7); inb(port + 1, &overflow);
    outb(port, 9); inb(port + 1, &maximum);
    outb(port, index);
    test_video_geometry(after + 2, function,
        low | ((overflow & 2) << 7) | ((overflow & 0x40) << 3), maximum);
}
