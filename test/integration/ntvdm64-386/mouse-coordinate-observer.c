/* Diagnostic pass-through: record the real coordinate service, never alter it. */
#define EmulateCoordinates observed_coordinates
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_guest.c"
#undef EmulateCoordinates
extern IU8 Currently_emulated_video_mode;
extern void EmulateCoordinates(half_word, IS16, IS16, IS16 *, IS16 *);
extern void test_mouse_coordinates(unsigned, int, int, int, int, int, int);
extern void test_mouse_coordinate_state(unsigned, unsigned, unsigned, unsigned);
void observed_coordinates(half_word mode, IS16 dx, IS16 dy, IS16 *x, IS16 *y)
{
    int old_x = *x, old_y = *y;
    test_mouse_coordinate_state(Currently_emulated_video_mode,
        current_video_mode, cursor_position.x, cursor_position.y);
    EmulateCoordinates(mode, dx, dy, x, y);
    test_mouse_coordinates(mode, dx, dy, old_x, old_y, *x, *y);
}
