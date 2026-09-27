#ifndef NTVDM_CONSOLE_TEXT_H
#define NTVDM_CONSOLE_TEXT_H
#include <windows.h>
/* Original video-update owner only. Copies, never frontend/guest pointers. */
BOOL NtvdmConsoleTextRequested(BOOL *);
void NtvdmConsoleTextColours(const PALETTEENTRY *);
BOOL NtvdmConsoleUpdateText(HPALETTE);
#endif
