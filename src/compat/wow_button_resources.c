#include "wow_button_resources.h"
/* C owns immutable original geometry and style interpretation. The same
 * image-lifetime provider is bound before any control can create resources. */
static const wow_button_resource_provider *provider;

BOOL WINAPI wow_button_resources_bind(const wow_button_resource_provider *value)
{
    const wow_button_resource_provider *previous;
    if (!value || !value->images || !value->render || value->width <= 0 || value->height <= 0) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider, (PVOID)value, NULL);
    if (previous && previous != value) { SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE; }
    return TRUE;
}

BOOL WINAPI wow_button_resources_initialize(wow_button_resources *resources)
{
    if (!resources) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!provider) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    if (resources->dc || resources->bitmap || resources->previous_bitmap) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    resources->bm = provider->images;
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
    if (!provider || !resources || resources->bm != provider->images) {
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
            pending.bitmap = CreateCompatibleBitmap(screen, provider->width,
                provider->height);
        ReleaseDC(NULL, screen);
        screen = NULL;
        if (!pending.dc || !pending.bitmap) goto failed;
        pending.previous_bitmap = SelectObject(pending.dc, pending.bitmap);
        if (!pending.previous_bitmap || pending.previous_bitmap == HGDI_ERROR) {
            pending.previous_bitmap = NULL;
            goto failed;
        }
        if (!provider->render(pending.dc)) goto failed;
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
    if (!provider || !destination || source_x < 0 || source_y < 0 || width <= 0 || height <= 0 ||
            source_x > provider->width || source_y > provider->height ||
            width > provider->width - source_x ||
            height > provider->height - source_y ||
            (operation != SRCCOPY && operation != SRCAND && operation != SRCINVERT)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!wow_button_resources_ready(resources)) return FALSE;
    return BitBlt(destination, x, y, width, height,
        resources->dc, source_x, source_y, operation);
}
