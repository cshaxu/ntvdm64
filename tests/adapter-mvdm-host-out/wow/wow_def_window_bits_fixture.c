#include "wow_private_user_compat.h"
#include <stdio.h>
#include <string.h>

/* Host-boundary test only. It does not claim a guest window was created. */
int main(void)
{
    BYTE storage[103];
    BYTE *bits = storage + 1;
    WORD max_message;
    unsigned index;
    const WORD required[] = { WM_CLOSE, WM_GETTEXT, WM_SETTEXT, WM_PAINT,
        WM_NCCREATE, WM_NCCALCSIZE, WM_NCHITTEST, WM_SYSCOMMAND,
        WM_WINDOWPOSCHANGED, WM_CTLCOLORSTATIC, WM_MOUSEWHEEL, WM_PRINT,
        0x003b, 0x022a, 0x022b, 0x0313 };
    const WORD absent[] = { WM_NULL, WM_CREATE, WM_DESTROY, WM_USER };

    memset(storage, 0xa5, sizeof(storage));
    if (wow_private_user_get_def_window_proc_bits(NULL, 101) != 0) return 1;
    if (wow_private_user_get_def_window_proc_bits(bits, WM_PRINT / 8) != 0) return 2;
    for (index = 0; index < sizeof(storage); ++index)
        if (storage[index] != 0xa5) return 3;
    memset(bits, 0, 101);
    max_message = wow_private_user_get_def_window_proc_bits(bits, 101);
    if (max_message != WM_PRINT) return 4;
    for (index = 0; index < ARRAYSIZE(required); ++index)
        if (!(bits[required[index] >> 3] & (1u << (required[index] & 7))))
            return 5;
    for (index = 0; index < ARRAYSIZE(absent); ++index)
        if (absent[index] <= max_message &&
            (bits[absent[index] >> 3] & (1u << (absent[index] & 7)))) return 6;
    if (storage[0] != 0xa5 || storage[102] != 0xa5) return 7;
    puts("WOW_DEF_WINDOW_BITS_BOUNDARY_OK");
    return 0;
}
