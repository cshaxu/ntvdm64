#ifndef WOW_BUTTON_RESOURCES_H
#define WOW_BUTTON_RESOURCES_H
#include <windows.h>
#include <winternl.h>
struct tagOEMBITMAPINFO;
/* Immutable C-owned geometry; B owns only the GDI resources below. */
typedef struct wow_button_resource_provider {
    struct tagOEMBITMAPINFO *images;
    int width, height;
    BOOL (WINAPI *render)(HDC);
} wow_button_resource_provider;
BOOL WINAPI wow_button_resources_bind(const wow_button_resource_provider *);

/* Caller-owned derived resources. No HWND, private host pointer or cache. */
typedef struct wow_button_resources {
    UINT BitCount; /* Scoped host display input, not atlas allocation state. */
    struct tagOEMBITMAPINFO *bm;
    HDC dc;
    HBITMAP bitmap;
    HGDIOBJ previous_bitmap;
} wow_button_resources;
/* Start with a zeroed scope. Release before reinitializing it. */
BOOL WINAPI wow_button_resources_initialize(wow_button_resources *);
BOOL WINAPI wow_button_resources_ready(wow_button_resources *);
void WINAPI wow_button_resources_release(wow_button_resources *);
BOOL WINAPI wow_button_resources_blit(wow_button_resources *, HDC,
    int, int, int, int, int, int, DWORD);
#endif
