#include "wow_button_resources.h"
#include "button-resources/leaves.h"
#include <stdio.h>
#include <string.h>

/* Real GDI resources and original table; selected leaves inject failure.
 * This proves the resource boundary, not ButtonWndProc or guest behavior. */
static unsigned failures, live_screen, live_dc, live_bitmap, draws;
static unsigned fail_step, throw_step;
static BOOL selecting_bitmap;
#define RESOURCE_EXCEPTION 0xe0477113u
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
static BOOL injected(unsigned step)
{
    if (throw_step == step) RaiseException(RESOURCE_EXCEPTION, 0, 0, NULL);
    if (fail_step != step) return FALSE;
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return TRUE;
}
HDC WINAPI button_test_GetDC(HWND window)
{
    HDC dc;
    CHECK(window == NULL);
    if (injected(1)) return NULL;
    dc = GetDC(window);
    if (dc) ++live_screen;
    return dc;
}
int WINAPI button_test_ReleaseDC(HWND window, HDC dc)
{
    int result = ReleaseDC(window, dc);
    CHECK(result && live_screen);
    if (result && live_screen) --live_screen;
    return result;
}
HDC WINAPI button_test_CreateCompatibleDC(HDC source)
{
    HDC dc;
    if (injected(2)) return NULL;
    dc = CreateCompatibleDC(source);
    if (dc) ++live_dc;
    return dc;
}
HBITMAP WINAPI button_test_CreateCompatibleBitmap(HDC dc, int width, int height)
{
    HBITMAP bitmap;
    if (injected(3)) return NULL;
    bitmap = CreateCompatibleBitmap(dc, width, height);
    if (bitmap) ++live_bitmap;
    selecting_bitmap = TRUE;
    return bitmap;
}
HGDIOBJ WINAPI button_test_SelectObject(HDC dc, HGDIOBJ object)
{
    if (selecting_bitmap) {
        selecting_bitmap = FALSE;
        if (injected(4)) return HGDI_ERROR;
    }
    return SelectObject(dc, object);
}
BOOL WINAPI button_test_DeleteDC(HDC dc)
{
    BOOL result = DeleteDC(dc);
    CHECK(result && live_dc);
    if (result && live_dc) --live_dc;
    return result;
}
BOOL WINAPI button_test_DeleteObject(HGDIOBJ object)
{
    BOOL result = DeleteObject(object);
    CHECK(result && live_bitmap);
    if (result && live_bitmap) --live_bitmap;
    return result;
}
BOOL WINAPI button_test_DrawFrameControl(HDC dc, LPRECT rect, UINT type, UINT state)
{
    ++draws;
    if (injected(4 + draws)) return FALSE;
    return DrawFrameControl(dc, rect, type, state);
}
static void empty(const wow_button_resources *resources)
{
    CHECK(!resources->dc && !resources->bitmap && !resources->previous_bitmap);
    CHECK(!live_dc && !live_bitmap && !live_screen);
}
static void metadata(void)
{
    wow_button_resources resources = {0};
    unsigned i;
    CHECK(!wow_button_resources_initialize(NULL));
    CHECK(!wow_button_resources_ready(&resources));
    CHECK(wow_button_resources_initialize(&resources));
    for (i = 0; i < OBI_RADIOMASK; ++i)
        CHECK(!resources.bm[i].cx && !resources.bm[i].cy);
    for (i = OBI_RADIOMASK; i < OBI_COUNT; ++i) {
        unsigned owner = i;
        if (i == OBI_3STATE) owner = OBI_CHECK;
        if (i == OBI_3STATE_D) owner = OBI_CHECK_D;
        CHECK(resources.bm[i].x == 0 && resources.bm[i].cx == 13 && resources.bm[i].cy == 13);
        CHECK(resources.bm[i].y == (int)(owner - OBI_RADIOMASK) * 13);
    }
    empty(&resources);
}
static void failure_cleanup(void)
{
    unsigned mode, step;
    for (mode = 0; mode < 2; ++mode) {
        /* Acquisition, select, and every one of fourteen render calls. */
        for (step = 1; step <= 18; ++step) {
            wow_button_resources resources = {0};
            BOOL caught = FALSE;
            CHECK(wow_button_resources_initialize(&resources));
            draws = 0;
            selecting_bitmap = FALSE;
            fail_step = mode ? 0 : step;
            throw_step = mode ? step : 0;
            __try { CHECK(!wow_button_resources_ready(&resources)); }
            __except (GetExceptionCode() == RESOURCE_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
                caught = TRUE;
            }
            CHECK(caught == (BOOL)mode);
            empty(&resources);
            fail_step = throw_step = 0;
            selecting_bitmap = FALSE;
            draws = 0;
            CHECK(wow_button_resources_ready(&resources));
            CHECK(draws == 14 && live_dc == 1 && live_bitmap == 1 && !live_screen);
            SetLastError(0x47113);
            wow_button_resources_release(&resources);
            CHECK(GetLastError() == 0x47113);
            wow_button_resources_release(&resources);
            empty(&resources);
        }
    }
}
static void native_images(void)
{
    wow_button_resources first = {0}, second = {0};
    HDC screen = GetDC(NULL), target = CreateCompatibleDC(screen);
    HDC reference = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 13, 13);
    HBITMAP reference_bitmap = CreateCompatibleBitmap(screen, 13, 13);
    HGDIOBJ previous = SelectObject(target, bitmap);
    HGDIOBJ reference_previous = SelectObject(reference, reference_bitmap);
    unsigned i;
    int x, y;
    CHECK(screen && target && reference && bitmap && reference_bitmap && previous && reference_previous);
    ReleaseDC(NULL, screen);
    CHECK(wow_button_resources_initialize(&first));
    CHECK(wow_button_resources_initialize(&second));
    draws = 0;
    CHECK(wow_button_resources_ready(&first));
    CHECK(draws == 14);
    CHECK(wow_button_resources_ready(&first) && draws == 14);
    CHECK(!wow_button_resources_initialize(&first) && GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(wow_button_resources_ready(&second));
    CHECK(first.dc != second.dc && first.bitmap != second.bitmap);
    CHECK(live_dc == 2 && live_bitmap == 2);
    for (i = OBI_RADIOMASK; i < OBI_COUNT; ++i) {
        RECT rect = {0, 0, 13, 13};
        unsigned owner = i;
        if (i == OBI_3STATE) owner = OBI_CHECK;
        if (i == OBI_3STATE_D) owner = OBI_CHECK_D;
        CHECK(DrawFrameControl(reference, &rect, sysBmpStyles[owner][0], sysBmpStyles[owner][1]));
        CHECK(wow_button_resources_blit(&first, target, 0, 0, 13, 13,
            first.bm[i].x, first.bm[i].y, SRCCOPY));
        for (y = 0; y < 13; ++y) for (x = 0; x < 13; ++x)
            CHECK(GetPixel(target, x, y) == GetPixel(reference, x, y));
    }
    CHECK(!wow_button_resources_blit(&first, target, 0, 0, 14, 13, 0, 0, SRCCOPY));
    CHECK(!wow_button_resources_blit(&first, target, 0, 0, 13, 13, -1, 0, SRCCOPY));
    CHECK(!wow_button_resources_blit(&first, target, 0, 0, 13, 13, 0, 208, SRCCOPY));
    CHECK(!wow_button_resources_blit(&first, target, 0, 0, 13, 13, 0, 0, PATCOPY));
    wow_button_resources_release(&second);
    CHECK(live_dc == 1 && live_bitmap == 1 && first.dc && first.bitmap);
    /* Radio mask/image composition preserves the caller's background. */
    {
        RECT rect = {0, 0, 13, 13};
        HBRUSH background = CreateSolidBrush(RGB(43, 71, 101));
        FillRect(target, &rect, background);
        FillRect(reference, &rect, background);
        CHECK(wow_button_resources_blit(&first, target, 0, 0, 13, 13, 0,
            first.bm[OBI_RADIOMASK].y, SRCAND));
        CHECK(wow_button_resources_blit(&first, target, 0, 0, 13, 13, 0,
            first.bm[OBI_RADIO_C].y, SRCINVERT));
        CHECK(DrawFrameControl(reference, &rect, DFC_BUTTON, DFCS_BUTTONRADIO | DFCS_CHECKED));
        for (y = 0; y < 13; ++y) for (x = 0; x < 13; ++x)
            CHECK(GetPixel(target, x, y) == GetPixel(reference, x, y));
        DeleteObject(background);
    }
    wow_button_resources_release(&first);
    empty(&first);
    SelectObject(target, previous);
    SelectObject(reference, reference_previous);
    DeleteDC(target); DeleteDC(reference);
    DeleteObject(bitmap); DeleteObject(reference_bitmap);
}
int __cdecl main(void)
{
    metadata();
    failure_cleanup();
    native_images();
    printf("button_resource_failures=%u\n", failures);
    return failures ? 1 : 0;
}
