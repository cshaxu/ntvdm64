/* Diagnostic pass-through for the original video-to-mouse notification. */
#define mouse_video_mode_changed observed_original_mouse_video_mode_changed
#include "mvdm-host/softpc.new/base/keymouse/mouse_io.c"
#undef mouse_video_mode_changed
extern void test_mouse_mode(unsigned, unsigned, unsigned, unsigned, unsigned);
void mouse_video_mode_changed(int mode)
{
    test_mouse_mode(0, mode, current_video_mode, getCS(), getIP());
    observed_original_mouse_video_mode_changed(mode);
    test_mouse_mode(1, mode, current_video_mode, getCS(), getIP());
}
