#ifndef WOW_BUTTON_RESOURCES_H
#define WOW_BUTTON_RESOURCES_H
#include <windows.h>
#define WOW_ORIGINAL_BUTTON_RESOURCES
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_BUTTON_RESOURCES

/* Caller-owned derived resources. No HWND, private host pointer or cache. */
typedef struct wow_button_resources {
    UINT BitCount; /* Scoped host display input, not atlas allocation state. */
    OEMBITMAPINFO bm[OBI_COUNT];
    HDC dc;
    HBITMAP bitmap;
    HGDIOBJ previous_bitmap;
} wow_button_resources;
extern CONST WORD sysBmpStyles[OBI_COUNT][2];
/* Start with a zeroed scope. Release before reinitializing it. */
BOOL WINAPI wow_button_resources_initialize(wow_button_resources *);
BOOL WINAPI wow_button_resources_ready(wow_button_resources *);
void WINAPI wow_button_resources_release(wow_button_resources *);
BOOL WINAPI wow_button_resources_blit(wow_button_resources *, HDC,
    int, int, int, int, int, int, DWORD);
#endif
