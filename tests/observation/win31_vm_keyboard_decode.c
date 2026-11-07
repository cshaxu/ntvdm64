/* S6 input-boundary witness: real shared decoder, no window or guest mutation. */
#include <windows.h>
#include <stdio.h>
#include "lib-nxvm/base/input.h"

int main(void)
{
    static const struct { const char *name; WORD key, scan; lib_u32 neutral; } cases[] = {
        { "ordinary-A", 'A', 0x1e, 'A' },
        { "Pause", VK_PAUSE, 0x45, BASE_KEY_PAUSE },
        { "Ctrl-Break", VK_CANCEL, 0x46, BASE_KEY_BREAK },
        { "right-Control", VK_RCONTROL, 0x1d, BASE_KEY_CONTROL }
    };
    unsigned i;
    int failed = 0;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        lib_u16 scan = 0;
        lib_u32 key = 0;
        lib_bool accepted = base_keyboard_platform_transition(cases[i].scan,
            cases[i].key, &scan, &key);
        printf("%s native=%u scan=%u accepted=%u neutral=%u output-scan=%u\n",
            cases[i].name, cases[i].key, cases[i].scan,
            (unsigned)accepted, (unsigned)key, (unsigned)scan);
        if (!accepted || key != cases[i].neutral || scan != cases[i].scan) failed = 1;
    }
    return failed;
}
