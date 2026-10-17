#include "wow_button_resources.h"

/* OPENNT-HOST-113 resource boundary. Original inctlpan.c owns the state
 * table; native GDI owns objects. The derived layout is scoped, not a second
 * button state engine or a view of the host USER atlas. */
#define BUTTON_IMAGE_SIZE 13
#define BUTTON_ATLAS_HEIGHT ((OBI_COUNT - OBI_RADIOMASK) * BUTTON_IMAGE_SIZE)

BOOL WINAPI wow_button_resources_initialize(wow_button_resources *resources)
{
    UINT i;
    if (!resources) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (resources->dc || resources->bitmap || resources->previous_bitmap) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    ZeroMemory(resources->bm, sizeof(resources->bm));
    for (i = OBI_RADIOMASK; i < OBI_COUNT; ++i) {
        OEMBITMAPINFO *image = &resources->bm[i];
        if (i >= OBI_3STATE && !(sysBmpStyles[i][1] & DFCS_CHECKED)) {
            /* Registered correction: derive the offset, not stale8. */
            *image = resources->bm[i - (OBI_3STATE - OBI_CHECK)];
        } else {
            image->x = 0;
            image->y = (i - OBI_RADIOMASK) * BUTTON_IMAGE_SIZE;
            image->cx = image->cy = BUTTON_IMAGE_SIZE;
        }
    }
    return TRUE;
}

void WINAPI wow_button_resources_release(wow_button_resources *resources)
{
    DWORD error = GetLastError();
    if (resources) {
        if (resources->dc) {
            if (resources->previous_bitmap)
                SelectObject(resources->dc, resources->previous_bitmap);
            DeleteDC(resources->dc);
        }
        if (resources->bitmap) DeleteObject(resources->bitmap);
        resources->dc = NULL;
        resources->bitmap = NULL;
        resources->previous_bitmap = NULL;
    }
    SetLastError(error);
}

BOOL WINAPI wow_button_resources_ready(wow_button_resources *resources)
{
    wow_button_resources pending = {0};
    DWORD error = GetLastError();
    HDC screen = NULL;
    UINT i;
    if (!resources || resources->bm[OBI_RADIOMASK].cx != BUTTON_IMAGE_SIZE) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (resources->dc && resources->bitmap && resources->previous_bitmap)
        return TRUE;
    if (resources->dc || resources->bitmap || resources->previous_bitmap) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    __try {
        screen = GetDC(NULL);
        if (!screen) return FALSE;
        pending.dc = CreateCompatibleDC(screen);
        if (pending.dc)
            pending.bitmap = CreateCompatibleBitmap(screen, BUTTON_IMAGE_SIZE,
                BUTTON_ATLAS_HEIGHT);
        ReleaseDC(NULL, screen);
        screen = NULL;
        if (!pending.dc || !pending.bitmap) goto failed;
        pending.previous_bitmap = SelectObject(pending.dc, pending.bitmap);
        if (!pending.previous_bitmap || pending.previous_bitmap == HGDI_ERROR) {
            pending.previous_bitmap = NULL;
            goto failed;
        }
        for (i = OBI_RADIOMASK; i < OBI_COUNT; ++i) {
            OEMBITMAPINFO *image = &resources->bm[i];
            RECT rect = {image->x, image->y, image->x + image->cx, image->y + image->cy};
            if (i >= OBI_3STATE && !(sysBmpStyles[i][1] & DFCS_CHECKED))
                continue; /* Alias already rendered by its earlier owner. */
            if (!DrawFrameControl(pending.dc, &rect, sysBmpStyles[i][0],
                    sysBmpStyles[i][1])) goto failed;
        }
        resources->dc = pending.dc;
        resources->bitmap = pending.bitmap;
        resources->previous_bitmap = pending.previous_bitmap;
        pending.dc = NULL;
        pending.bitmap = NULL;
        pending.previous_bitmap = NULL;
        SetLastError(error);
        return TRUE;
    failed:
        SetLastError(ERROR_GEN_FAILURE);
        return FALSE;
    } __finally {
        DWORD cleanup_error = GetLastError();
        if (screen) ReleaseDC(NULL, screen);
        wow_button_resources_release(&pending);
        SetLastError(cleanup_error);
    }
}

BOOL WINAPI wow_button_resources_blit(wow_button_resources *resources,
    HDC destination, int x, int y, int width, int height,
    int source_x, int source_y, DWORD operation)
{
    if (!destination || source_x < 0 || source_y < 0 || width <= 0 || height <= 0 ||
            source_x > BUTTON_IMAGE_SIZE || source_y > BUTTON_ATLAS_HEIGHT ||
            width > BUTTON_IMAGE_SIZE - source_x ||
            height > BUTTON_ATLAS_HEIGHT - source_y ||
            (operation != SRCCOPY && operation != SRCAND && operation != SRCINVERT)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!wow_button_resources_ready(resources)) return FALSE;
    return BitBlt(destination, x, y, width, height,
        resources->dc, source_x, source_y, operation);
}
