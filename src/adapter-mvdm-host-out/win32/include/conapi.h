/*
 * Same-shaped modern binding for the historical OpenNT conapi.h include.
 *
 * DIVERGENCE: the selected OpenNT declaration carrier predates the modern
 * Windows SDK and redeclares console structures already supplied by
 * windows.h.  Original SoftPC callers retain #include <conapi.h>; selecting
 * this adapter header preserves that source-facing include shape while the
 * public SDK remains the single definition owner.
 */
#ifndef MVDM_ADAPTER_CONAPI_H
#define MVDM_ADAPTER_CONAPI_H

#include <windows.h>

/* The public SDK no longer exposes this NT4 VDM graphics-buffer descriptor.
 * Preserve the original layout because reached SoftPC screen state embeds it;
 * the display/console provider, not this declaration carrier, owns behavior. */
#ifndef NOGDI
typedef struct _CONSOLE_GRAPHICS_BUFFER_INFO {
    DWORD dwBitMapInfoLength;
    LPBITMAPINFO lpBitMapInfo;
    DWORD dwUsage;
    HANDLE hMutex;
    PVOID lpBitMap;
} CONSOLE_GRAPHICS_BUFFER_INFO, *PCONSOLE_GRAPHICS_BUFFER_INFO;
#endif

#endif /* MVDM_ADAPTER_CONAPI_H */
