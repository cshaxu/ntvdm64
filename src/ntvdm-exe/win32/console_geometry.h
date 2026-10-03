#ifndef NTVDM_CONSOLE_GEOMETRY_H
#define NTVDM_CONSOLE_GEOMETRY_H
#include <windows.h>

/* Project-owned binding of OpenNT nt_fulsc.c calcScreenParams/MID_VAL.
 * The original mirror still owns actual VGA state and resume. Only NTVDM
 * selects this capability; the frontend applies explicit copied dimensions. */
static SHORT ntvdm_console_return_height(SHORT height)
{
    if(height<=22+(25-22)/2)return 22;
    if(height<=25+(28-25)/2)return 25;
    if(height<=28+(43-28)/2)return 28;
    if(height<=43+(50-43)/2)return 43;
    return 50;
}
#endif
