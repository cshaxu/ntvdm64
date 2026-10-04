#ifndef CONSOLE_GEOMETRY_FIXTURE_H
#define CONSOLE_GEOMETRY_FIXTURE_H
#include "ntcon-exe/console_frontend.h"
#include "ntvdm-exe/win32/console_geometry.h"
/* Test-only composition of the real NTVDM selector and real frontend storage
 * primitive. Never selected as a production provider or protocol fallback. */
static DWORD test_prepare_vga(HANDLE output,SMALL_RECT *window)
{
    if(!window)return ERROR_INVALID_PARAMETER;
    return frontend_console_prepare_text(output,window,(COORD){80,
        ntvdm_console_return_height(window->Bottom-window->Top+1)});
}
#endif
